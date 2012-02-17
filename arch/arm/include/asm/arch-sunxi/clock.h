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

struct sunxi_ccm_reg {

	u32 pll1_cfg;             /* 0x00 pll1 control */
	u32 pll1_tun;             /* 0x04 pll1 tuning */
	u32 pll2_cfg;             /* 0x08 pll2 control */
	u32 pll2_tun;             /* 0x0c pll2 tuning */
	u32 pll3_cfg;             /* 0x10 pll3 control */
	u8  res0[0x4];
	u32 pll4_cfg;             /* 0x18 pll4 control */
	u8  res1[0x4];
	u32 pll5_cfg;             /* 0x20 pll5 control */
	u32 pll5_tun;             /* 0x24 pll5 tuning */
	u32 pll6_cfg;             /* 0x28 pll6 control */
	u32 pll6_tun;             /* 0x2c pll6 tuning */
	u32 pll7_cfg;             /* 0x30 pll7 control */
	u32 pll1_tun2;            /* 0x34 pll5 tuning2 */
	u8  res2[0x4];
	u32 pll5_tun2;            /* 0x3c pll5 tuning2 */
	u8  res3[0xc];
	u32 pll_lock_dbg;         /* 0x4c pll lock time debug */
	u32 osc24m_cfg;           /* 0x50 osc24m control */
	u32 cpu_ahb_apb0_cfg;     /* 0x54 cpu,ahb and apb0 divide ratio */
	u32 apb1_clk_div_cfg;     /* 0x58 apb1 clock dividor */
	u32 axi_gate;             /* 0x5c axi module clock gating */
	u32 ahb_gate0;            /* 0x60 ahb module clock gating 0 */
	u32 ahb_gate1;            /* 0x64 ahb module clock gating 1 */
	u32 apb0_gate;            /* 0x68 apb0 module clock gating */
	u32 apb1_gate;            /* 0x6c apb1 module clock gating */
	u8  res4[0x10];
	u32 nand_sclk_cfg;        /* 0x80 nand sub clock control */
	u32 ms_sclk_cfg;          /* 0x84 memory stick sub clock control */
	u32 sd0_clk_cfg;          /* 0x88 sd0 clock control */
	u32 sd1_clk_cfg;          /* 0x8c sd1 clock control */
	u32 sd2_clk_cfg;          /* 0x90 sd2 clock control */
	u32 sd3_clk_cfg;          /* 0x94 sd3 clock control */
	u32 ts_clk_cfg;           /* 0x98 transport stream clock control */
	u32 ss_clk_cfg;           /* 0x9c */
	u32 spi0_clk_cfg;         /* 0xa0 */
	u32 spi1_clk_cfg;         /* 0xa4 */
	u32 spi2_clk_cfg;         /* 0xa8 */
	u32 pata_clk_cfg;         /* 0xac */
	u32 ir0_clk_cfg;          /* 0xb0 */
	u32 ir1_clk_cfg;          /* 0xb4 */
	u32 iis_clk_cfg;          /* 0xb8 */
	u32 ac97_clk_cfg;         /* 0xbc */
	u32 spdif_clk_cfg;        /* 0xc0 */
	u32 keypad_clk_cfg;       /* 0xc4 */
	u32 sata_clk_cfg;         /* 0xc8 */
	u32 usb_clk_cfg;          /* 0xcc */
	u32 gps_clk_cfg;          /* 0xd0 */
	u32 spi3_clk_cfg;         /* 0xd4 */
	u8  res5[0x28];
	u32 dram_clk_cfg;         /* 0x100 */
	u32 be0_clk_cfg;          /* 0x104 */
	u32 be1_clk_cfg;          /* 0x108 */
	u32 fe0_clk_cfg;          /* 0x10c */
	u32 fe1_clk_cfg;          /* 0x110 */
	u32 mp_clk_cfg;           /* 0x114 */
	u32 lcd0_ch0_clk_cfg;     /* 0x118 */
	u32 lcd1_ch0_clk_cfg;     /* 0x11c */
	u32 csi_isp_clk_cfg;      /* 0x120 */
	u8  res6[0x4];
	u32 tvd_clk_reg;          /* 0x128 */
	u32 lcd0_ch1_clk_cfg;     /* 0x12c */
	u32 lcd1_ch1_clk_cfg;     /* 0x130 */
	u32 csi0_clk_cfg;         /* 0x134 */
	u32 csi1_clk_cfg;         /* 0x138 */
	u32 ve_clk_cfg;           /* 0x13c */
	u32 audio_codec_clk_cfg;  /* 0x140 */
	u32 avs_clk_cfg;          /* 0x144 */
	u32 ace_clk_cfg;          /* 0x148 */
	u32 lvds_clk_cfg;         /* 0x14c */
	u32 hdmi_clk_cfg;         /* 0x150 */
	u32 mali_clk_cfg;         /* 0x154 */
};
/* pll1 factors */
#define PLL1_FACTOR_N			21
#define PLL1_FACTOR_K			1
#define PLL1_FACTOR_M			0
#define PLL1_FACTOR_P			0
#define PLL1_ENABLE				1

/* apb1 bit field */
#define APB1_CLK_SRC_OSC24M		0
#define APB1_FACTOR_M			0
#define APB1_FACTOR_N			0

/* pll5(for ddr) bit field */
#define DDR_CLK_HZ				(360 * 1024 * 1024)
#define OSC24M_CLK_HZ			(24 * 1024 * 1024)

#define PLL5_FACTOR_N			(DDR_CLK_HZ / OSC24M_CLK_HZ)
#define PLL5_FACTOR_K			1
#define PLL5_FACTOR_M			1
#define PLL5_OUT_DIV_P			1
#define PLL5_ENABLE				1
#define DDR_CLK_OUT_ENABLE		1
#define DDR_CLK_OUT_DISABLE		0

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

/* gps clock */
#define GPS_SCLK_GATING_OFF		0
#define GPS_RESET				0

/* ahb clock gate bit offset */
#define AHB_GATE_OFFSET_GPS			26
#define AHB_GATE_OFFSET_SATA		25
#define AHB_GATE_OFFSET_PATA		24
#define AHB_GATE_OFFSET_SPI3		23
#define AHB_GATE_OFFSET_SPI2		22
#define AHB_GATE_OFFSET_SPI1		21
#define AHB_GATE_OFFSET_SPI0		20
#define AHB_GATE_OFFSET_TS0			18
#define AHB_GATE_OFFSET_EMAC		17
#define AHB_GATE_OFFSET_ACE			16
#define AHB_GATE_OFFSET_SDRAM		14
#define AHB_GATE_OFFSET_NAND		13
#define AHB_GATE_OFFSET_MS			12
#define AHB_GATE_OFFSET_MMC3		11
#define AHB_GATE_OFFSET_MMC2		10
#define AHB_GATE_OFFSET_MMC1		9
#define AHB_GATE_OFFSET_MMC0		8
#define AHB_GATE_OFFSET_BIST		7
#define AHB_GATE_OFFSET_DMA			6
#define AHB_GATE_OFFSET_SS			5
#define AHB_GATE_OFFSET_USB_OHCI1	4
#define AHB_GATE_OFFSET_USB_EHCI1	3
#define AHB_GATE_OFFSET_USB_OHCI0	2
#define AHB_GATE_OFFSET_USB_EHCI0	1
#define AHB_GATE_OFFSET_USB			0

#ifndef __ASSEMBLY__
int clock_init(void);
#endif

#endif /* _SUNXI_CLOCK_H */
