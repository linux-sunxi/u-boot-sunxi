/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC register definition for allwinner sunxi platform.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_MMC_H
#define _SUNXI_MMC_H

#include <linux/types.h>

struct sunxi_mmc {
	u32 gctrl;		/* (0x00) SMC Global Control Register */
	u32 clkcr;		/* (0x04) SMC Clock Control Register */
	u32 timeout;		/* (0x08) SMC Time Out Register */
	u32 width;		/* (0x0c) SMC Bus Width Register */
	u32 blksz;		/* (0x10) SMC Block Size Register */
	u32 bytecnt;		/* (0x14) SMC Byte Count Register */
	u32 cmd;		/* (0x18) SMC Command Register */
	u32 arg;		/* (0x1c) SMC Argument Register */
	u32 resp0;		/* (0x20) SMC Response Register 0 */
	u32 resp1;		/* (0x24) SMC Response Register 1 */
	u32 resp2;		/* (0x28) SMC Response Register 2 */
	u32 resp3;		/* (0x2c) SMC Response Register 3 */
	u32 imask;		/* (0x30) SMC Interrupt Mask Register */
	u32 mint;		/* (0x34) SMC Masked Interrupt Status Reg */
	u32 rint;		/* (0x38) SMC Raw Interrupt Status Register */
	u32 status;		/* (0x3c) SMC Status Register */
	u32 ftrglevel;		/* (0x40) SMC FIFO Threshold Watermark Reg */
	u32 funcsel;		/* (0x44) SMC Function Select Register */
	u32 cbcr;		/* (0x48) SMC CIU Byte Count Register */
	u32 bbcr;		/* (0x4c) SMC BIU Byte Count Register */
	u32 dbgc;		/* (0x50) SMC Debug Enable Register */
	u32 res0[11];		/* (0x54~0x7c) */
	u32 dmac;		/* (0x80) SMC IDMAC Control Register */
	u32 dlba;		/* (0x84) SMC IDMAC Descr List Base Addr Reg */
	u32 idst;		/* (0x88) SMC IDMAC Status Register */
	u32 idie;		/* (0x8c) SMC IDMAC Interrupt Enable Register */
	u32 chda;		/* (0x90) */
	u32 cbda;		/* (0x94) */
	u32 res1[26];		/* (0x98~0xff) */
	u32 fifo;		/* (0x100) SMC FIFO Access Address */
};

#define SUNXI_MMC_CLK_POWERSAVE		(0x1 << 17)
#define SUNXI_MMC_CLK_ENABLE		(0x1 << 16)
#define SUNXI_MMC_CLK_DIVIDER_MASK 	(0xff)

#define SUNXI_MMC_GCTRL_SOFT_RESET	(0x1 << 0)
#define SUNXI_MMC_GCTRL_FIFO_RESET	(0x1 << 1)
#define SUNXI_MMC_GCTRL_DMA_RESET	(0x1 << 2)
#define SUNXI_MMC_GCTRL_RESET		(SUNXI_MMC_GCTRL_SOFT_RESET|\
					 SUNXI_MMC_GCTRL_FIFO_RESET|\
					 SUNXI_MMC_GCTRL_DMA_RESET)
#define SUNXI_MMC_GCTRL_DMA_ENABLE	(0x1 << 5)
#define SUNXI_MMC_GCTRL_ACCESS_BY_AHB   (0x1 << 31)

#define SUNXI_MMC_CMD_RESP_EXPIRE	(0x1 << 6)
#define SUNXI_MMC_CMD_LONG_RESPONSE	(0x1 << 7)
#define SUNXI_MMC_CMD_CHK_RESPONSE_CRC	(0x1 << 8)
#define SUNXI_MMC_CMD_DATA_EXPIRE	(0x1 << 9)
#define SUNXI_MMC_CMD_WRITE		(0x1 << 10)
#define SUNXI_MMC_CMD_AUTO_STOP		(0x1 << 12)
#define SUNXI_MMC_CMD_WAIT_PRE_OVER	(0x1 << 13)
#define SUNXI_MMC_CMD_SEND_INIT_SEQ	(0x1 << 15)
#define SUNXI_MMC_CMD_UPCLK_ONLY	(0x1 << 21)
#define SUNXI_MMC_CMD_START		(0x1 << 31)

#define SUNXI_MMC_IDMAC_RESET		(0x1 << 0)
#define SUNXI_MMC_IDMAC_FIXBURST	(0x1 << 1)
#define SUNXI_MMC_IDMAC_ENABLE		(0x1 << 7)

#define SUNXI_MMC_IDIE_TXIRQ		(0x1 << 0)
#define SUNXI_MMC_IDIE_RXIRQ		(0x1 << 1)

int sunxi_mmc_init(int sdc_no);
#endif /* _SUNXI_MMC_H */
