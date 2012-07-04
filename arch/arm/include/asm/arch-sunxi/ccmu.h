/**
 * ccmu.h - definition for registers of ccmu controller
 * date:    2012-2-13 8:42:56
 * modify:  2012-2-29 10:22:06, add register 0x164~0x16c, 0x0270
 * author:  Aaron<leafy.myeh@allwinnertech.com>
 * history: V0.1
 */
#ifndef __CCMU_H
#define __CCMU_H
#include "type.h"
#include "platform.h"

#define CCM_PLL1_CPUX_CTRL  	(CCM_BASE+0x000)
#define CCM_PLL2_AUDIO_CTRL 	(CCM_BASE+0x008)
#define CCM_PLL3_VIDEO_CTRL    	(CCM_BASE+0x010)
#define CCM_PLL4_VE_CTRL     	(CCM_BASE+0x018)
#define CCM_PLL5_DDR_CTRL  		(CCM_BASE+0x020)
#define CCM_PLL6_MOD_CTRL  		(CCM_BASE+0x028)
#define CCM_PLL7_VIDEO1_CTRL	(CCM_BASE+0x030)
#define CCM_PLL8_GPU_CTRL  		(CCM_BASE+0x038)
#define CCM_MIPI_PLL_CTRL		(CCM_BASE+0x040)

#define CCM_CPU_L2_AXI_CTRL		(CCM_BASE+0x050)
#define CCM_AHB1_APB1_CTRL		(CCM_BASE+0x054)
#define CCM_APB2_CLK_CTRL		(CCM_BASE+0x058)
#define CCM_AXI_GATE_CTRL		(CCM_BASE+0x05c)
#define CCM_AHB1_GATE0_CTRL		(CCM_BASE+0x060)
#define CCM_AHB1_GATE1_CTRL		(CCM_BASE+0x064)
#define CCM_APB1_GATE0_CTRL		(CCM_BASE+0x068)
#define CCM_APB2_GATE0_CTRL		(CCM_BASE+0x06C)

#define CCM_NAND0_SCLK_CTRL		(CCM_BASE+0x080)
#define CCM_NAND1_SCLK_CTRL		(CCM_BASE+0x084)

#define CCM_SDC0_SCLK_CTRL		(CCM_BASE+0x088)
#define CCM_SDC1_SCLK_CTRL		(CCM_BASE+0x08c)
#define CCM_SDC2_SCLK_CTRL		(CCM_BASE+0x090)
#define CCM_SDC3_SCLK_CTRL		(CCM_BASE+0x094)
#define CCM_TS_SCLK_CTRL		(CCM_BASE+0x098)
#define CCM_SS_SCLK_CTRL		(CCM_BASE+0x09c)
#define CCM_SPI0_SCLK_CTRL		(CCM_BASE+0x0a0)
#define CCM_SPI1_SCLK_CTRL		(CCM_BASE+0x0a4)
#define CCM_SPI2_SCLK_CTRL		(CCM_BASE+0x0a8)
#define CCM_SPI3_SCLK_CTRL		(CCM_BASE+0x0ac)
#define CCM_I2S0_SCLK_CTRL		(CCM_BASE+0x0b0)
#define CCM_I2S1_SCLK_CTRL		(CCM_BASE+0x0b4)

#define CCM_SPDIF_SCLK_CTRL		(CCM_BASE+0x0c0)

#define CCM_USBPHY_SCLK_CTRL	(CCM_BASE+0x0cc)

#define CCM_MDFS_CLK_CTRL		(CCM_BASE+0x0f0)
#define CCM_DRAMCLK_CFG_CTRL	(CCM_BASE+0x0f4)
#define CCM_DRAMCLK_GATE_CTRL	(CCM_BASE+0x0100)
#define CCM_BE0_SCLK_CTRL		(CCM_BASE+0x0104)
#define CCM_BE1_SCLK_CTRL		(CCM_BASE+0x0108)
#define CCM_FE0_SCLK_CTRL		(CCM_BASE+0x010c)
#define CCM_FE1_SCLK_CTRL		(CCM_BASE+0x0110)
#define CCM_MP_SCLK_CTRL		(CCM_BASE+0x0114)
#define CCM_LCD0C0_SCLK_CTRL	(CCM_BASE+0x0118)
#define CCM_LCD1C0_SCLK_CTRL	(CCM_BASE+0x011c)

#define CCM_LCD0C1_SCLK_CTRL	(CCM_BASE+0x012c)
#define CCM_LCD1C1_SCLK_CTRL	(CCM_BASE+0x0130)
#define CCM_CSI0_SCLK_CTRL		(CCM_BASE+0x0134)
#define CCM_CSI1_SCLK_CTRL		(CCM_BASE+0x0138)
#define CCM_VE_SCLK_CTRL		(CCM_BASE+0x013c)
#define CCM_CODEC_SCLK_CTRL		(CCM_BASE+0x0140)
#define CCM_AVS_SCLK_CTRL		(CCM_BASE+0x0144)

#define CCM_HDMI_SCLK_CTRL		(CCM_BASE+0x0150)

#define CCM_MBUS_SCLK_CTRL0		(CCM_BASE+0x015c)
#define CCM_MBUS_SCLK_CTRL1		(CCM_BASE+0x0160)

#define CCM_MIPIDSI_SCLK_CTRL	(CCM_BASE+0x0168)
#define CCM_MIPICSI0_SCLK_CTRL	(CCM_BASE+0x016c)

#define CCM_DRC0_SCLK_CTRL		(CCM_BASE+0x0180)
#define CCM_DRC1_SCLK_CTRL		(CCM_BASE+0x0184)
#define CCM_DEU0_SCLK_CTRL		(CCM_BASE+0x0188)
#define CCM_DEU1_SCLK_CTRL		(CCM_BASE+0x018c)

#define CCM_GPU_CORECLK_CTRL	(CCM_BASE+0x01A0)
#define CCM_GPU_MEMCLK_CTRL		(CCM_BASE+0x01A4)
#define CCM_GPU_HYDCLK_CTRL		(CCM_BASE+0x01A8)

#define CCM_PLL_STABLE_REG		(CCM_BASE+0x0200)
#define CCM_MCLK_STABLE_REG		(CCM_BASE+0x0204)

#define CCM_PPL1_BIAS_REG		(CCM_BASE+0x0220)
#define CCM_PPL2_BIAS_REG		(CCM_BASE+0x0224)
#define CCM_PPL3_BIAS_REG		(CCM_BASE+0x0228)
#define CCM_PPL4_BIAS_REG		(CCM_BASE+0x022C)
#define CCM_PPL5_BIAS_REG		(CCM_BASE+0x0230)
#define CCM_PPL6_BIAS_REG		(CCM_BASE+0x0234)
#define CCM_PPL7_BIAS_REG		(CCM_BASE+0x0238)
#define CCM_PPL8_BIAS_REG		(CCM_BASE+0x023C)
#define CCM_MIPIPLL_BIAS_REG	(CCM_BASE+0x0240)

#define CCM_PPL1_TUNE_REG		(CCM_BASE+0x0250)

#define CCM_PPL5_TUNE_REG		(CCM_BASE+0x0260)

#define CCM_MIPIPLL_TUNE_REG	(CCM_BASE+0x0270)

#define CCM_PPL1_PAT_REG		(CCM_BASE+0x0280)
#define CCM_PPL2_PAT_REG		(CCM_BASE+0x0284)
#define CCM_PPL3_PAT_REG		(CCM_BASE+0x0288)
#define CCM_PPL4_PAT_REG		(CCM_BASE+0x028C)
#define CCM_PPL5_PAT_REG		(CCM_BASE+0x0290)

#define CCM_PPL7_PAT_REG		(CCM_BASE+0x0298)
#define CCM_PPL8_PAT_REG		(CCM_BASE+0x029C)

#define CCM_MIPIPLL_PAT_REG		(CCM_BASE+0x02A0)

#define CCM_AHB1_RST_REG0		(CCM_BASE+0x02C0)
#define CCM_AHB1_RST_REG1		(CCM_BASE+0x02C4)
#define CCM_AHB1_RST_REG2		(CCM_BASE+0x02C8)

#define CCM_APB1_RST_REG		(CCM_BASE+0x02D0)
#define CCM_APB2_RST_REG		(CCM_BASE+0x02D8)

#define CCM_CLK_OUTA_REG		(CCM_BASE+0x0300)
#define CCM_CLK_OUTB_REG		(CCM_BASE+0x0304)
#define CCM_CLK_OUTC_REG		(CCM_BASE+0x0308)

/* cmmu pll ctrl bit field */
#define CCM_PLL_STABLE_FLAG		(1U << 28)

/* clock ID */
#define AXI_BUS		(0)
#define AHB1_BUS0	(1)
#define AHB1_BUS1	(2)
#define AHB1_LVDS	(3)
#define APB1_BUS0	(4)
#define APB2_BUS0	(5)
/* axi */
#define DRAMMEM_CKID	((AXI_BUS << 8) | 0)
/* ahb1 branc0 */
#define USBOHCI2_CKID	((AHB1_BUS0 << 8) | 31)
#define USBOHCI1_CKID	((AHB1_BUS0 << 8) | 30)
#define USBOHCI0_CKID	((AHB1_BUS0 << 8) | 29)
#define USBEHCI2_CKID	((AHB1_BUS0 << 8) | 28)
#define USBEHCI1_CKID	((AHB1_BUS0 << 8) | 27)
#define USBEHCI0_CKID	((AHB1_BUS0 << 8) | 26)
#define USB_OTG_CKID	((AHB1_BUS0 << 8) | 24)
#define SPI3_CKID		((AHB1_BUS0 << 8) | 23)
#define SPI2_CKID		((AHB1_BUS0 << 8) | 22)
#define SPI1_CKID		((AHB1_BUS0 << 8) | 21)
#define SPI0_CKID		((AHB1_BUS0 << 8) | 20)
#define HSTMR_CKID		((AHB1_BUS0 << 8) | 19)
#define TS_CKID			((AHB1_BUS0 << 8) | 18)
#define GMAC_CKID		((AHB1_BUS0 << 8) | 17)
#define DRAMREG_CKID	((AHB1_BUS0 << 8) | 14)
#define NAND0_CKID		((AHB1_BUS0 << 8) | 13)
#define NAND1_CKID		((AHB1_BUS0 << 8) | 12)
#define SDC3_CKID		((AHB1_BUS0 << 8) | 11)
#define SDC2_CKID		((AHB1_BUS0 << 8) | 10)
#define SDC1_CKID		((AHB1_BUS0 << 8) | 9 )
#define SDC0_CKID		((AHB1_BUS0 << 8) | 8 )
#define DMA_CKID		((AHB1_BUS0 << 8) | 6 )
#define SS_CKID			((AHB1_BUS0 << 8) | 5 )
#define MIPIDSI_CKID	((AHB1_BUS0 << 8) | 1 )
#define MIPICSI_CKID	((AHB1_BUS0 << 8) | 0 )
/* ahb1 branc1 */
#define DRC1_CKID		((AHB1_BUS1 << 8) | 26)
#define DRC0_CKID		((AHB1_BUS1 << 8) | 25)
#define DEU1_CKID		((AHB1_BUS1 << 8) | 24)
#define DEU0_CKID		((AHB1_BUS1 << 8) | 23)
#define SPINLOCK_CKID	((AHB1_BUS1 << 8) | 22)
#define MSGBOX_CKID		((AHB1_BUS1 << 8) | 21)
#define GPU_CKID		((AHB1_BUS1 << 8) | 20)
#define MP_CKID			((AHB1_BUS1 << 8) | 18)
#define FE1_CKID		((AHB1_BUS1 << 8) | 15)
#define FE0_CKID		((AHB1_BUS1 << 8) | 14)
#define BE1_CKID		((AHB1_BUS1 << 8) | 13)
#define BE0_CKID		((AHB1_BUS1 << 8) | 12)
#define HDMI_CKID		((AHB1_BUS1 << 8) | 11)
#define CSI1_CKID		((AHB1_BUS1 << 8) | 9)
#define CSI0_CKID		((AHB1_BUS1 << 8) | 8)
#define LCD1_CKID		((AHB1_BUS1 << 8) | 5)
#define LCD0_CKID		((AHB1_BUS1 << 8) | 4)
#define VE_CKID			((AHB1_BUS1 << 8) | 0)
/* ahb1 special for lvds */
#define LVDS_CKID		((AHB1_LVDS << 8) | 0)

/* apb1  */
#define IIS1_CKID		((APB1_BUS0 << 8) | 13)
#define IIS0_CKID		((APB1_BUS0 << 8) | 12)
#define KP_CKID			((APB1_BUS0 << 8) | 10)
#define GPADC_CKID		((APB1_BUS0 << 8) | 8)
#define PIO_CKID		((APB1_BUS0 << 8) | 5)
#define SPDIF_CKID		((APB1_BUS0 << 8) | 1)
#define CODEC_CKID		((APB1_BUS0 << 8) | 0)
/* apb2  */
#define UART4_CKID		((APB2_BUS0 << 8) | 20)
#define UART3_CKID		((APB2_BUS0 << 8) | 19)
#define UART2_CKID		((APB2_BUS0 << 8) | 18)
#define UART1_CKID		((APB2_BUS0 << 8) | 17)
#define UART0_CKID		((APB2_BUS0 << 8) | 16)
#define STWI_CKID		((APB2_BUS0 << 8) | 4)
#define TWI3_CKID		((APB2_BUS0 << 8) | 3)
#define TWI2_CKID		((APB2_BUS0 << 8) | 2)
#define TWI1_CKID		((APB2_BUS0 << 8) | 1)
#define TWI0_CKID		((APB2_BUS0 << 8) | 0)

#define CPUCLK_SRC_32K		(0)
#define CPUCLK_SRC_24M		(1)
#define CPUCLK_SRC_PLL1		(2)

#define AHB1CLK_SRC_LOSC	(0)
#define AHB1CLK_SRC_24M		(1)
#define AHB1CLK_SRC_AXI		(2)
#define AHB1CLK_SRC_PLL6D	(3)

#define APB2CLK_SRC_LOSC	(0)
#define APB2CLK_SRC_24M		(1)
#define APB2CLK_SRC_PLL6	(2)

#define MBUSCLK_SRC_24M		(0)
#define MBUSCLK_SRC_PLL6	(1)
#define MBUSCLK_SRC_PLL5	(2)

void ccm_clock_enable(u32 clk_id);
void ccm_clock_disable(u32 clk_id);
void ccm_module_enable(u32 clk_id);
void ccm_module_disable(u32 clk_id);
void ccm_module_reset(u32 clk_id);
void ccm_clock_disable_all(void);
void ccm_reset_all_module(void);

u32 ccm_setup_pll1_cpux_clk(u32 pll1_clk);
u32 ccm_get_pll1_cpux_clk(void);
u32 ccm_setup_pll3_video0_clk(u32 mode_sel, u32 pll_clk);
u32 ccm_get_pll3_video0_clk(void);
u32 ccm_setup_pll4_ve_clk(u32 mode_sel, u32 pll_clk);
u32 ccm_get_pll4_ve_clk(void);
u32 ccm_setup_pll5_ddr_clk(u32 pll5_clk);
u32 ccm_get_pll5_ddr_clk(void);
u32 ccm_setup_pll6_dev_clk(u32 pll6_clk);
u32 ccm_get_pll6_dev_clk(void);  
u32 ccm_setup_pll7_video0_clk(u32 mode_sel, u32 pll_clk);
u32 ccm_get_pll7_video0_clk(void);
u32 ccm_setup_pll8_gpu_clk(u32 mode_sel, u32 pll_clk);
u32 ccm_get_pll8_gpu_clk(void);

void ccm_set_cpu_clk_src(u32 src);
u32  ccm_set_mbus0_clk(u32 src, u32 clk);
u32  ccm_set_mbus1_clk(u32 src, u32 clk);
void ccm_set_cpu_l2_axi_div(u32 periph_div, u32 l2_div, u32 axi_div);
void ccm_set_ahb1_clk_src(u32 src);
void ccm_set_ahb1_apb1_div(u32 prediv, u32 ahb1_div, u32 apb1_div);
s32  ccm_set_apb2_clk(u32 src);
void ccm_set_pll_stable_time(u32 time);
void ccm_set_mclk_stable_time(u32 time);
u32  ccm_get_axi_clk(void);
u32  ccm_get_ahb1_clk(void);
u32  ccm_get_apb1_clk(void);
u32  ccm_get_apb2_clk(void);

#endif
