/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC register definition for allwinner sunxi platform.
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

struct sunxi_mmc {
	u32 gctrl;         /* (0x00) SMC Global Control Register */
	u32 clkcr;         /* (0x04) SMC Clock Control Register */
	u32 timeout;       /* (0x08) SMC Time Out Register */
	u32 width;         /* (0x0C) SMC Bus Width Register */
	u32 blksz;         /* (0x10) SMC Block Size Register */
	u32 bytecnt;       /* (0x14) SMC Byte Count Register */
	u32 cmd;           /* (0x18) SMC Command Register */
	u32 arg;           /* (0x1C) SMC Argument Register */
	u32 resp0;         /* (0x20) SMC Response Register 0 */
	u32 resp1;         /* (0x24) SMC Response Register 1 */
	u32 resp2;         /* (0x28) SMC Response Register 2 */
	u32 resp3;         /* (0x2C) SMC Response Register 3 */
	u32 imask;         /* (0x30) SMC Interrupt Mask Register */
	u32 mint;          /* (0x34) SMC Masked Interrupt Status Register */
	u32 rint;          /* (0x38) SMC Raw Interrupt Status Register */
	u32 status;        /* (0x3C) SMC Status Register */
	u32 ftrglevel;     /* (0x40) SMC FIFO Threshold Watermark Register */
	u32 funcsel;       /* (0x44) SMC Function Select Register */
	u32 cbcr;          /* (0x48) SMC CIU Byte Count Register */
	u32 bbcr;          /* (0x4C) SMC BIU Byte Count Register */
	u32 dbgc;          /* (0x50) SMC Debug Enable Register */
	u32 res0[11];      /* (0x54~0x7c) */
	u32 dmac;          /* (0x80) SMC IDMAC Control Register */
	u32 dlba;          /* (0x84) SMC IDMAC Descriptor List Base Address Register */
	u32 idst;          /* (0x88) SMC IDMAC Status Register */
	u32 idie;          /* (0x8C) SMC IDMAC Interrupt Enable Register */
	u32 chda;          /* (0x90) */
	u32 cbda;          /* (0x94) */
	u32 res1[26];      /* (0x98~0xff) */
	u32 fifo;          /* (0x100) SMC FIFO Access Address */
};

#endif /* _SUNXI_MMC_H */
