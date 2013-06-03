/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>

#if 0
int sunxi_gpio_set_cfgpin(u32 pin, u32 val) {

	u32 cfg;
	u32 bank = GPIO_BANK(pin);
	u32 index = GPIO_CFG_INDEX(pin);
	u32 offset = GPIO_CFG_OFFSET(pin);

	struct sunxi_gpio *pio =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];

	cfg = readl(&pio->cfg[0] + index);
	cfg &= ~(0xf << offset);
	cfg |= val << offset;

	writel(cfg, &pio->cfg[0] + index);

	return 0;
}

int sunxi_gpio_get_cfgpin(u32 pin) {

	u32 cfg;
	u32 bank = GPIO_BANK(pin);
	u32 index = GPIO_CFG_INDEX(pin);
	u32 offset = GPIO_CFG_OFFSET(pin);

	struct sunxi_gpio *pio =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];

	cfg = readl(&pio->cfg[0] + index);
	cfg >>= offset;

	return (cfg & 0xf);
}

int sunxi_gpio_output(u32 pin, u32 val) {

	u32 dat;
	u32 bank = GPIO_BANK(pin);
	u32 num = GPIO_NUM(pin);

	struct sunxi_gpio *pio =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];

	dat = readl(&pio->dat);
	if(val)
		dat |= 1 << num;
	else
		dat &= ~(1 << num);

	writel(dat, &pio->dat);

	return 0;
}

int sunxi_gpio_input(u32 pin) {

	u32 dat;
	u32 bank = GPIO_BANK(pin);
	u32 num = GPIO_NUM(pin);

	struct sunxi_gpio *pio =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];

	dat = readl(&pio->dat);
	dat >>= num;

	return (dat & 0x1);
}
/*
int gpio_request(unsigned gpio, const char *label) {

	return 0;
}
*/
int gpio_free(unsigned gpio) {

	return 0;
}

int gpio_direction_input(unsigned gpio) {

	sunxi_gpio_set_cfgpin(gpio, SUNXI_GPIO_INPUT);
	return sunxi_gpio_input(gpio);
}

int gpio_direction_output(unsigned gpio, int value) {

	sunxi_gpio_set_cfgpin(gpio, SUNXI_GPIO_OUTPUT);
	return sunxi_gpio_output(gpio, value);
}

int gpio_get_value(unsigned gpio) {

	return sunxi_gpio_input(gpio);
}

int gpio_set_value(unsigned gpio, int value) {

	return sunxi_gpio_output(gpio, value);
}

#ifdef DEBUG
int do_gpio_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {

	int i;

	for(i = 0; i < 18; i++) {
		gpio_direction_input(SUNXI_GPA(i));
		gpio_direction_output(SUNXI_GPA(i), 1);
	}
}

U_BOOT_CMD(
	gpio_test, 1, 0,	do_gpio_test,
	"Test gpio",
	""
);
#endif

#endif

