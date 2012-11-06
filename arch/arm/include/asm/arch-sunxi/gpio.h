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
 
#ifndef _SUNXI_GPIO_H
#define _SUNXI_GPIO_H

#define SUNXI_GPIO_NULL 0
#define SUNXI_GPIO_A    1
#define SUNXI_GPIO_B    2
#define SUNXI_GPIO_C    3
#define SUNXI_GPIO_D    4
#define SUNXI_GPIO_E    5
#define SUNXI_GPIO_F    6
#define SUNXI_GPIO_G    7
#define SUNXI_GPIO_H    8
#define SUNXI_GPIO_I    9

struct sunxi_gpio 
{
	volatile u32 cfg[4];
	volatile u32 dat;
	volatile u32 drv[2];
	volatile u32 pull[2];
};

/* gpio interrupt control */
struct sunxi_gpio_int {
	u32 cfg[3];
	u32 ctl;
	u32 sta;
	u32 deb;			/* interrupt debounce */
};

extern int sunxi_set_gpio_all(void  *user_gpio_list, u32 group_count_max);


#endif /* _SUNXI_GPIO_H */


