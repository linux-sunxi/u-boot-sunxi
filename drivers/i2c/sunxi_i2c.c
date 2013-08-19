/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
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
#include <asm/arch/clock.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include <asm/arch/i2c.h>

static struct i2c __attribute__ ((section(".data"))) *i2c_base =
	(struct i2c *)0x1c2ac00;

void i2c_init(int speed, int slaveaddr)
{
	int timeout = 0x2ff;

	sunxi_gpio_set_cfgpin(SUNXI_GPB(0), 2);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(1), 2);
	clock_twi_onoff(0, 1);

	/* Enable the i2c bus */
	writel(TWI_CTL_BUSEN, &i2c_base->ctl);

	/* 400KHz operation M=2, N=1, 24MHz APB clock */
	writel(TWI_CLK_DIV(2, 1), &i2c_base->clkr);
	writel(TWI_SRST_SRST, &i2c_base->reset);

	while ((readl(&i2c_base->reset) & TWI_SRST_SRST) && timeout--);
}

int i2c_probe(uchar chip)
{
	return -1;
}

static int i2c_wait_ctl(int mask, int state)
{
	int timeout = 0x2ff;
	int value = state ? mask : 0;

	debug("i2c_wait_ctl(%x == %x), ctl=%x, status=%x\n", mask, value,
	      i2c_base->ctl, i2c_base->status);

	while (((readl(&i2c_base->ctl) & mask) != value) && timeout-- > 0);

	debug("i2c_wait_ctl(), timeout=%d, ctl=%x, status=%x\n", timeout,
	      i2c_base->ctl, i2c_base->status);

	if (timeout != 0)
		return 0;
	else
		return -1;
}

static void i2c_clear_irq(void)
{
	writel(readl(&i2c_base->ctl) & ~TWI_CTL_INTFLG, &i2c_base->ctl);
}

static int i2c_wait_irq(void)
{
	return i2c_wait_ctl(TWI_CTL_INTFLG, 1);
}

static int i2c_wait_status(int status)
{
	int timeout = 0x2ff;

	while (readl(&i2c_base->status) != status && timeout-- > 0);

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

	while (readl(&i2c_base->lctl) != 0x3a && timeout-- > 0);

	if (timeout != 0)
		return 0;
	else
		return -1;
}

static int i2c_stop(void)
{
	u32 ctl;

	ctl = readl(&i2c_base->ctl) & 0xc0;
	ctl |= TWI_CTL_STP;

	writel(ctl, &i2c_base->ctl);

	/* dummy to delay one I/O operation to make sure it's started */
	(void)readl(&i2c_base->ctl);

	if (i2c_wait_ctl(TWI_CTL_STP, 0) != 0)
		return -1;
	if (i2c_wait_status(TWI_STAT_IDLE))
		return -1;
	if (i2c_wait_bus_idle() != 0)
		return -1;

	return 0;
}

static int i2c_send_data(u8 data, u8 status)
{
	debug("i2c_write(%02x, %x), ctl=%x, status=%x\n", data, status,
	      i2c_base->ctl, i2c_base->status);

	writel(data, &i2c_base->data);
	i2c_clear_irq();

	if (i2c_wait_irq_status(status) != 0)
		return -1;

	return 0;
}

static int i2c_start(int status)
{
	u32 ctl;

	debug("i2c_start(%x), ctl=%x, status=%x\n", status, i2c_base->ctl,
	      i2c_base->status);
	/* Check that the controller is idle */
	if (status == TWI_STAT_TX_STA &&
	    readl(&i2c_base->status) != TWI_STAT_IDLE) {
		return -1;
	}

	writel(0, &i2c_base->efr);

	/* Send start */
	ctl = readl(&i2c_base->ctl);
	ctl |= TWI_CTL_STA;	/* Set start bit */
	ctl &= ~TWI_CTL_INTFLG;	/* Clear int flag */
	writel(ctl, &i2c_base->ctl);

	if (i2c_wait_ctl(TWI_CTL_STA, 0) != 0)
		return -1;
	if (i2c_wait_irq_status(status) != 0)
		return -1;

	return 0;
}

int i2c_do_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	u32 status;
	u32 ctl;

	if (i2c_start(TWI_STAT_TX_STA) != 0)
		return -1;

	/* Send chip address */
	if (i2c_send_data(chip << 1 | 0, TWI_STAT_TX_AW_ACK) != 0)
		return -1;

	/* Send data address */
	if (i2c_send_data(addr, TWI_STAT_TXD_ACK) != 0)
		return -1;

	/* Send restart for read */
	if (i2c_start(TWI_STAT_TX_RESTA) != 0)
		return -1;

	/* Send chip address */
	if (i2c_send_data(chip << 1 | 1, TWI_STAT_TX_AR_ACK) != 0)
		return -1;

	/* Set ACK mode */
	ctl = readl(&i2c_base->ctl);
	ctl |= TWI_CTL_ACK;
	writel(ctl, &i2c_base->ctl);
	status = TWI_STAT_RXD_ACK;

	/* Read data */
	while (len > 0) {
		if (len == 1) {
			/* Set NACK mode (last byte) */
			ctl = readl(&i2c_base->ctl);
			ctl &= ~TWI_CTL_ACK;
			writel(ctl, &i2c_base->ctl);
			status = TWI_STAT_RXD_NAK;
		}

		i2c_clear_irq();
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

static int i2c_do_write(uchar chip, uint addr, int alen, uchar *buffer,
			int len)
{
	if (i2c_start(TWI_STAT_TX_STA) != 0)
		return -1;

	/* Send chip address */
	if (i2c_send_data(chip << 1 | 0, TWI_STAT_TX_AW_ACK) != 0)
		return -1;

	/* Send data address */
	if (i2c_send_data(addr, TWI_STAT_TXD_ACK) != 0)
		return -1;

	/* Send data */
	while (len > 0) {
		if (i2c_send_data(*buffer++, TWI_STAT_TXD_ACK) != 0)
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
