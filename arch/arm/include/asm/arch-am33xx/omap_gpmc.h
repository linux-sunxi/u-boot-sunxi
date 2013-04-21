/*
 * (C) Copyright 2004-2008 Texas Instruments, <www.ti.com>
 * Rohit Choraria <rohitkc@ti.com>
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
#ifndef __ASM_ARCH_OMAP_GPMC_H
#define __ASM_ARCH_OMAP_GPMC_H

/* These GPMC_NAND_HW_BCHx_ECC_LAYOUT defines are based on AM33xx ELM */
#define GPMC_NAND_HW_BCH4_ECC_LAYOUT {\
	.eccbytes = 32,\
	.eccpos = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,\
				16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,\
				28, 29, 30, 31, 32, 33},\
	.oobfree = {\
		{.offset = 34,\
		 .length = 30 } } \
}

#define GPMC_NAND_HW_BCH8_ECC_LAYOUT {\
	.eccbytes = 56,\
	.eccpos = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,\
				16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,\
				28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,\
				40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,\
				52, 53, 54, 55, 56, 57},\
	.oobfree = {\
		{.offset = 58,\
		 .length = 6 } } \
}

#define GPMC_NAND_HW_BCH16_ECC_LAYOUT {\
	.eccbytes = 104,\
	.eccpos = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,\
				16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,\
				28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,\
				40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,\
				52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,\
				64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,\
				76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,\
				88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,\
				100, 101, 102, 103, 104, 105},\
	.oobfree = {\
		{.offset = 106,\
		 .length = 8 } } \
}
#endif /* __ASM_ARCH_OMAP_GPMC_H */
