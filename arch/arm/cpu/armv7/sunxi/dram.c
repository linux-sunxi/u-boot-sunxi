/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Berg Xing <bergxing@allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Sunxi platform dram controller init.
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
#include <asm/arch/dram.h>
#include <asm/arch/sys_proto.h>

/*
 * Get DRAM Size in MB unit;
 */
unsigned dramc_get_dram_size(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)DRAMC_IO_BASE;
	u32 reg_val;
	u32 dram_size;
	u32 chip_den;

	reg_val = readl(&dram->dcr);
	chip_den = (reg_val >> 3) & 0x7;

	dram_size = min(1024, 32 << chip_den);

	if (((reg_val >> 1) & 0x3) == 0x1)
		dram_size <<= 1;
	if (((reg_val >> 6) & 0x7) == 0x3)
		dram_size <<= 1;
	if (((reg_val >> 10) & 0x3) == 0x1)
		dram_size <<= 1;

	return dram_size;
}
