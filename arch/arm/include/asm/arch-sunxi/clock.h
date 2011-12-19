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
 
#ifndef _SUNXI_CLOCK_H
#define _SUNXI_CLOCK_H

/* clock control module regs definition */

#define SUNXI_CCM_PLL1_CFG			(SUNXI_CCM_BASE + 0x00)		/* PLL1 control */
#define SUNXI_CCM_PLL1_TUN			(SUNXI_CCM_BASE + 0x04)		/* PLL1 tuning */
#define SUNXI_CCM_PLL2_CFG			(SUNXI_CCM_BASE + 0x08)
#define SUNXI_CCM_PLL2_TUN			(SUNXI_CCM_BASE + 0x0C)
#define SUNXI_CCM_PLL3_CFG			(SUNXI_CCM_BASE + 0x10)

#define SUNXI_CCM_PLL4_CFG			(SUNXI_CCM_BASE + 0x18)

#define SUNXI_CCM_PLL5_CFG			(SUNXI_CCM_BASE + 0x20)
#define SUNXI_CCM_PLL5_TUN			(SUNXI_CCM_BASE + 0x24)
#define SUNXI_CCM_PLL6_CFG			(SUNXI_CCM_BASE + 0x28)

#define SUNXI_CCM_PLL7_CFG			(SUNXI_CCM_BASE + 0x30)

#define SUNXI_CCM_PLL1_TUN2			(SUNXI_CCM_BASE + 0x38)
#define SUNXI_CCM_PLL5_TUN2			(SUNXI_CCM_BASE + 0x3C)

#define SUNXI_CCM_PLL_LOCK_DBG		(SUNXI_CCM_BASE + 0x4C)

#define SUNXI_CCM_OSC24M_CFG		(SUNXI_CCM_BASE + 0x50)		/* osc 24M control */
#define SUNXI_CCM_CPU_AHB_APB0_CFG	(SUNXI_CCM_BASE + 0x54)		/* cpu ahb apb0 divide ratio */
#define SUNXI_CCM_APB1_CLK_DIV		(SUNXI_CCM_BASE + 0x58)
#define SUNXI_CCM_AXI_GATING		(SUNXI_CCM_BASE + 0x5C)
#define SUNXI_CCM_AHB_GATING0		(SUNXI_CCM_BASE + 0x60)
#define SUNXI_CCM_AHB_GATING1		(SUNXI_CCM_BASE + 0x64)
#define SUNXI_CCM_APB0_GATING		(SUNXI_CCM_BASE + 0x68)
#define SUNXI_CCM_APB1_GATING		(SUNXI_CCM_BASE + 0x6C)

#define SUNXI_CCM_NAND_SCLK_CFG		(SUNXI_CCM_BASE + 0x80)

#define SUNXI_CCM_MMC0_SCLK_CFG		(SUNXI_CCM_BASE + 0x88)
#define SUNXI_CCM_MMC1_SCLK_CFG		(SUNXI_CCM_BASE + 0x8C)
#define SUNXI_CCM_MMC2_SCLK_CFG		(SUNXI_CCM_BASE + 0x90)
#define SUNXI_CCM_MMC3_SCLK_CFG		(SUNXI_CCM_BASE + 0x94)

/* pll1 factors */
#define PLL1_FACTOR_N			21
#define PLL1_FACTOR_K			1
#define PLL1_FACTOR_M			0
#define PLL1_FACTOR_P			0

/* apb1 bit field */
#define APB1_CLK_SRC_OSC24M		0
#define APB1_FACTOR_M			0
#define APB1_FACTOR_N			0

/* clock divide */
#define CPU_CLK_SRC_OSC24M		1
#define CPU_CLK_SRC_PLL1		2
#define AXI_DIV					1
#define AHB_DIV					1
#define APB0_DIV				1
#ifdef SUN5I
#define AHB_CLK_SRC_AXI			0
#endif


#define CLK_GATE_OPEN			0x1
#define CLK_GATE_CLOSE			0x0

/* nand clock */
#define NAND_CLK_SRC_OSC24		0
#define NAND_CLK_DIV_N			0
#define NAND_CLK_DIV_M			0




#endif /* _SUNXI_CLOCK_H */
