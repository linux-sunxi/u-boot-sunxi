/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC driver for allwinner sunxi platform.
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

#ifndef _SUNXI_MMC_H
#define _SUNXI_MMC_H

#define SUNXI_SDMMC0_BASE	0x01C0F000
#define SUNXI_SDMMC1_BASE	0x01C10000
#define SUNXI_SDMMC2_BASE	0x01C11000
#define SUNXI_SDMMC3_BASE	0x01C12000

struct sunxi_mmc {
	unsigned int 	 gctrl;			//(0x00) SMC Global Control Register
	unsigned int 	 clkcr;         //(0x04) SMC Clock Control Register
	unsigned int 	 timeout;       //(0x08) SMC Time Out Register
	unsigned int 	 width;         //(0x0C) SMC Bus Width Register
	unsigned int 	 blksz;         //(0x10) SMC Block Size Register
	unsigned int 	 bytecnt;       //(0x14) SMC Byte Count Register
	unsigned int 	 cmd;           //(0x18) SMC Command Register
	unsigned int 	 arg;           //(0x1C) SMC Argument Register
	unsigned int 	 resp0;         //(0x20) SMC Response Register 0
	unsigned int 	 resp1;         //(0x24) SMC Response Register 1
	unsigned int 	 resp2;         //(0x28) SMC Response Register 2
	unsigned int 	 resp3;         //(0x2C) SMC Response Register 3
	unsigned int 	 imask;         //(0x30) SMC Interrupt Mask Register
	unsigned int 	 mint;          //(0x34) SMC Masked Interrupt Status Register
	unsigned int 	 rint;          //(0x38) SMC Raw Interrupt Status Register
	unsigned int 	 status;        //(0x3C) SMC Status Register
	unsigned int 	 ftrglevel;     //(0x40) SMC FIFO Threshold Watermark Register
	unsigned int 	 funcsel;       //(0x44) SMC Function Select Register
	unsigned int 	 cbcr;          //(0x48) SMC CIU Byte Count Register
	unsigned int 	 bbcr;          //(0x4C) SMC BIU Byte Count Register
	unsigned int 	 dbgc;          //(0x50) SMC Debug Enable Register
#if 0
	unsigned int 	 res0[11];		//(0x54~0x7c)
	unsigned int 	 dmac;          //(0x80) SMC IDMAC Control Register
	unsigned int 	 dlba;          //(0x84) SMC IDMAC Descriptor List Base Address Register
	unsigned int 	 idst;          //(0x88) SMC IDMAC Status Register
	unsigned int 	 idie;          //(0x8C) SMC IDMAC Interrupt Enable Register
	unsigned int 	 chda;          //(0x90)
	unsigned int 	 cbda;          //(0x94)
	unsigned int 	 res0[26];		//(0x98~0xff)
	unsigned int 	 fifo;          //(0x100) SMC FIFO Access Address
#endif
};

#endif /* _SUNXI_MMC_H */
