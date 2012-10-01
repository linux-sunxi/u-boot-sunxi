/*
 * Copyright (c) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/i2c.h>
#include <asm/arch/gpio.h>
#include <asm/arch/clock.h>

static struct i2c __attribute__((section (".data"))) *i2c_base = (struct i2c *)0x1c2ac00;

void i2c_init(int speed, int slaveaddr)
{
	sunxi_gpio_set_cfgpin(SUNXI_GPB(0), 2);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(1), 2);
	clock_twi_onoff(0, 1);
	/* Enable the bus in master mode */
	writel((1<<6), &i2c_base->ctl);
	/* 400KHz operation */
	writel((2<<3)|(1<<0), &i2c_base->clkr);
	writel(1, &i2c_base->reset);
	while ((readl(&i2c_base->reset) & 1));
}

int i2c_probe(uchar chip)
{
	return -1;
}

static int i2c_wait_ctl(int bit, int state)
{
	int timeout = 0x2ff;
	while((readl(&i2c_base->ctl) & (1<<bit)) != state && timeout-- > 0);
	if (timeout != 0)
		return 0;
	else
		return -1;
}

static void i2c_clear_irq(void)
{
	writel(readl(&i2c_base->ctl) & ~(1<<3), &i2c_base->ctl);
}

static int i2c_wait_irq(void)
{
	i2c_clear_irq();
	return i2c_wait_ctl(3, 0);
}

static int i2c_wait_status(int status)
{
	int timeout = 0x2ff;
	while(readl(&i2c_base->status) != status && timeout-- > 0);
	if (timeout != 0)
		return 0;
	else
		return -1;
}

static int i2c_wait_irq_status(int status)
{
	if (i2c_wait_irq() != 0)
		return -1;
	if (readl(&i2c_base->status) != status)
		return -1;
	return 0;
}

static int i2c_wait_bus_idle(void)
{
	int timeout = 0x2ff;
	while(readl(&i2c_base->lctl) != 0x3a && timeout-- > 0);
	if (timeout != 0)
		return 0;
	else
		return -1;
}

static int i2c_stop(void)
{
	u32 ctl;

	ctl = readl(&i2c_base->ctl) & 0xc0;
	ctl |= 1 << 4;
	writel(ctl, &i2c_base->ctl);
	(void)readl(&i2c_base->ctl);	/* delay one I/O operation to make sure it's started */
	if (i2c_wait_ctl(4, 0) != 0)
		return -1;
	if (i2c_wait_status(0xf8))
		return -1;
	if (i2c_wait_bus_idle() != 0)
		return -1;
	return 0;
}

static int i2c_send_data(u8 data, u8 status)
{
	writel(data, &i2c_base->data);
	if (i2c_wait_irq_status(status) != 0)
		return -1;
	return 0;
}

static int i2c_start(int status)
{
	u32 ctl;

	/* Check that the controller is idle */
	if (readl(&i2c_base->status) != 0xf8) {
		return -1;
	}
	
	writel(0, &i2c_base->efr);

	/* Send start */
	ctl = readl(&i2c_base->ctl);
	ctl |= 1 << 5;
	writel(ctl, &i2c_base->ctl);
	if (i2c_wait_irq_status(status))
		return -1;
	return 0;
}

int i2c_do_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	u32 status;
	u32 ctl;

	if (i2c_start(0x08) != 0)
		return -1;

	/* Send chip address */
	if (i2c_send_data(chip << 1 | 1, 0x18) != 0)
		return -1;

	/* Send data address */
	if (i2c_send_data(addr, 0x28) != 0)
		return -1;

	/* Send restart for read */
	if (i2c_start(0x10) != 0)
		return -1;

	/* Send chip address */
	if (i2c_send_data(chip << 1 | 1, 0x10) != 0)
		return -1;

	/* Set ACK mode */
	ctl = readl(&i2c_base->ctl);
	ctl |= 1 << 2;
	writel(ctl, &i2c_base->ctl);
	status = 0x50;

	/* Read data */
	while(len > 0) {
		if (len == 1) {
			/* Set NACK mode */
			ctl = readl(&i2c_base->ctl);
			ctl &= ~(1 << 2);
			writel(ctl, &i2c_base->ctl);
			status = 0x58;
		}
		if (i2c_wait_irq_status(status) != 0)
			return -1;
		*buffer++ = readl(&i2c_base->data);
		len--;
	}
	return 0;
}

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int rc = i2c_do_read(chip, addr, alen, buffer, len);

	i2c_stop();

	return rc;
}

static int i2c_do_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	u32 ctl;

	if (i2c_start(0x08) != 0)
		return -1;

	/* Send chip address */
	if (i2c_send_data(chip << 1 | 0, 0x18) != 0)
		return -1;

	/* Send data address */
	if (i2c_send_data(addr, 0x28) != 0)
		return -1;

	/* Send data */
	while(len > 0) {
		if (i2c_send_data(*buffer++, 0x28) != 0)
			return -1;
		len--;
	}
	return 0;
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int rc = i2c_do_write(chip, addr, alen, buffer, len);

	i2c_stop();

	return rc;
}
