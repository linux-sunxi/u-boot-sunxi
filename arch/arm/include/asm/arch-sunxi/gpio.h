/*
 * (C) Copyright 2007-2012
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SUNXI_GPIO_H
#define _SUNXI_GPIO_H

#include <linux/types.h>

/*
 * sunxi has 9 banks of gpio, they are:
 * PA0 - PA17 | PB0 - PB23 | PC0 - PC24
 * PD0 - PD27 | PE0 - PE31 | PF0 - PF5
 * PG0 - PG9  | PH0 - PH27 | PI0 - PI12
 */

#define SUNXI_GPIO_A	0
#define SUNXI_GPIO_B	1
#define SUNXI_GPIO_C	2
#define SUNXI_GPIO_D	3
#define SUNXI_GPIO_E	4
#define SUNXI_GPIO_F	5
#define SUNXI_GPIO_G	6
#define SUNXI_GPIO_H	7
#define SUNXI_GPIO_I	8

struct sunxi_gpio {
	u32 cfg[4];
	u32 dat;
	u32 drv[2];
	u32 pull[2];
};

/* gpio interrupt control */
struct sunxi_gpio_int {
	u32 cfg[3];
	u32 ctl;
	u32 sta;
	u32 deb;		/* interrupt debounce */
};

struct sunxi_gpio_reg {
	struct sunxi_gpio gpio_bank[9];
	u8 res[0xbc];
	struct sunxi_gpio_int gpio_int;
};

#define GPIO_BANK(pin)		((pin) >> 5)
#define GPIO_NUM(pin)		((pin) & 0x1f)

#define GPIO_CFG_INDEX(pin)	(((pin) & 0x1f) >> 3)
#define GPIO_CFG_OFFSET(pin)	((((pin) & 0x1f) & 0x7) << 2)

#define GPIO_PULL_INDEX(pin)	(((pin) & 0x1f) >> 4)
#define GPIO_PULL_OFFSET(pin)	((((pin) & 0x1f) & 0xf) << 1)

/* GPIO bank sizes */
#define SUNXI_GPIO_A_NR		32
#define SUNXI_GPIO_B_NR		32
#define SUNXI_GPIO_C_NR		32
#define SUNXI_GPIO_D_NR		32
#define SUNXI_GPIO_E_NR		32
#define SUNXI_GPIO_F_NR		32
#define SUNXI_GPIO_G_NR		32
#define SUNXI_GPIO_H_NR		32
#define SUNXI_GPIO_I_NR		32

#define SUNXI_GPIO_NEXT(__gpio) \
	((__gpio##_START) + (__gpio##_NR) + 0)

enum sunxi_gpio_number {
	SUNXI_GPIO_A_START = 0,
	SUNXI_GPIO_B_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_A),
	SUNXI_GPIO_C_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_B),
	SUNXI_GPIO_D_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_C),
	SUNXI_GPIO_E_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_D),
	SUNXI_GPIO_F_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_E),
	SUNXI_GPIO_G_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_F),
	SUNXI_GPIO_H_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_G),
	SUNXI_GPIO_I_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_H),
};

/* SUNXI GPIO number definitions */
#define SUNXI_GPA(_nr)	(SUNXI_GPIO_A_START + (_nr))
#define SUNXI_GPB(_nr)	(SUNXI_GPIO_B_START + (_nr))
#define SUNXI_GPC(_nr)	(SUNXI_GPIO_C_START + (_nr))
#define SUNXI_GPD(_nr)	(SUNXI_GPIO_D_START + (_nr))
#define SUNXI_GPE(_nr)	(SUNXI_GPIO_E_START + (_nr))
#define SUNXI_GPF(_nr)	(SUNXI_GPIO_F_START + (_nr))
#define SUNXI_GPG(_nr)	(SUNXI_GPIO_G_START + (_nr))
#define SUNXI_GPH(_nr)	(SUNXI_GPIO_H_START + (_nr))
#define SUNXI_GPI(_nr)	(SUNXI_GPIO_I_START + (_nr))

/* GPIO pin function config */
#define SUNXI_GPIO_INPUT	0
#define SUNXI_GPIO_OUTPUT	1

#define SUNXI_GPA0_ERXD3	2
#define SUNXI_GPA0_SPI1_CS0	3
#define SUNXI_GPA0_UART2_RTS	4

#define SUNXI_GPA1_ERXD2	2
#define SUNXI_GPA1_SPI1_CLK	3
#define SUNXI_GPA1_UART2_CTS	4

#define SUNXI_GPA2_ERXD1	2
#define SUNXI_GPA2_SPI1_MOSI	3
#define SUNXI_GPA2_UART2_TX	4

#define SUNXI_GPA10_UART1_TX	4
#define SUNXI_GPA11_UART1_RX	4

#define SUN4I_GPB22_UART0_TX	2
#define SUN4I_GPB23_UART0_RX	2

#define SUN5I_GPB19_UART0_TX	2
#define SUN5I_GPB20_UART0_RX	2

#define SUN5I_GPG3_UART0_TX	4
#define SUN5I_GPG4_UART0_RX	4

#define SUNXI_GPC2_NCLE		2
#define SUNXI_GPC2_SPI0_CLK	3

#define SUNXI_GPC6_NRB0		2
#define SUNXI_GPC6_SDC2_CMD	3

#define SUNXI_GPC7_NRB1		2
#define SUNXI_GPC7_SDC2_CLK	3

#define SUNXI_GPC8_NDQ0		2
#define SUNXI_GPC8_SDC2_D0	3

#define SUNXI_GPC9_NDQ1		2
#define SUNXI_GPC9_SDC2_D1	3

#define SUNXI_GPC10_NDQ2	2
#define SUNXI_GPC10_SDC2_D2	3

#define SUNXI_GPC11_NDQ3	2
#define SUNXI_GPC11_SDC2_D3	3

#define SUNXI_GPF2_SDC0_CLK	2
#define SUNXI_GPF2_UART0_TX	4

#define SUNXI_GPF4_SDC0_D3	2
#define SUNXI_GPF4_UART0_RX	4

int sunxi_gpio_set_cfgpin(u32 pin, u32 val);
int sunxi_gpio_get_cfgpin(u32 pin);
int sunxi_gpio_set_pull(u32 pin, u32 val);
int name_to_gpio(const char *name);
#define name_to_gpio	name_to_gpio

#endif /* _SUNXI_GPIO_H */
