/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some board init for the Allwinner A10-evb board.
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

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void) {

	puts("Board: Marsboard 1GB\n");
	/* set LED */
	gpio_direction_output(SUNXI_GPB(5),0);
	gpio_direction_output(SUNXI_GPB(6),1);
	gpio_direction_output(SUNXI_GPB(7),1);
	gpio_direction_output(SUNXI_GPB(8),1);

	/* Set pull-up resistor PB23 */
	writel(0x40004000,(SUNXI_PIO_BASE + 0x44));

	return 0;
}
#endif
