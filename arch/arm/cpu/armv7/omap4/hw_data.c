/*
 *
 * HW data initialization for OMAP4
 *
 * (C) Copyright 2013
 * Texas Instruments, <www.ti.com>
 *
 * Sricharan R <r.sricharan@ti.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/arch/omap.h>
#include <asm/arch/sys_proto.h>
#include <asm/omap_common.h>
#include <asm/arch/clocks.h>
#include <asm/omap_gpio.h>
#include <asm/io.h>

struct prcm_regs const **prcm =
			(struct prcm_regs const **) OMAP_SRAM_SCRATCH_PRCM_PTR;
struct dplls const **dplls_data =
			(struct dplls const **) OMAP_SRAM_SCRATCH_DPLLS_PTR;
struct vcores_data const **omap_vcores =
		(struct vcores_data const **) OMAP_SRAM_SCRATCH_VCORES_PTR;
struct omap_sys_ctrl_regs const **ctrl =
	(struct omap_sys_ctrl_regs const **)OMAP4_SRAM_SCRATCH_SYS_CTRL;

/*
 * The M & N values in the following tables are created using the
 * following tool:
 * tools/omap/clocks_get_m_n.c
 * Please use this tool for creating the table for any new frequency.
 */

/*
 * dpll locked at 1400 MHz MPU clk at 700 MHz(OPP100) - DCC OFF
 * OMAP4460 OPP_NOM frequency
 */
static const struct dpll_params mpu_dpll_params_1400mhz[NUM_SYS_CLKS] = {
	{175, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 12 MHz   */
	{700, 12, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 13 MHz   */
	{125, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 16.8 MHz */
	{401, 10, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{350, 12, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz   */
	{700, 26, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 27 MHz   */
	{638, 34, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1}	/* 38.4 MHz */
};

/*
 * dpll locked at 1600 MHz - MPU clk at 800 MHz(OPP Turbo 4430)
 * OMAP4430 OPP_TURBO frequency
 */
static const struct dpll_params mpu_dpll_params_1600mhz[NUM_SYS_CLKS] = {
	{200, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 12 MHz   */
	{800, 12, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 13 MHz   */
	{619, 12, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 16.8 MHz */
	{125, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{400, 12, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz   */
	{800, 26, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 27 MHz   */
	{125, 5, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

/*
 * dpll locked at 1200 MHz - MPU clk at 600 MHz
 * OMAP4430 OPP_NOM frequency
 */
static const struct dpll_params mpu_dpll_params_1200mhz[NUM_SYS_CLKS] = {
	{50, 0, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz   */
	{600, 12, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 13 MHz   */
	{250, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 16.8 MHz */
	{125, 3, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{300, 12, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz   */
	{200, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 27 MHz   */
	{125, 7, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

/* OMAP4460 OPP_NOM frequency */
static const struct dpll_params core_dpll_params_1600mhz[NUM_SYS_CLKS] = {
	{200, 2, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 12 MHz   */
	{800, 12, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 13 MHz   */
	{619, 12, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 16.8 MHz */
	{125, 2, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 19.2 MHz */
	{400, 12, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 26 MHz   */
	{800, 26, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 27 MHz   */
	{125, 5, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1}	/* 38.4 MHz */
};

/* OMAP4430 ES1 OPP_NOM frequency */
static const struct dpll_params core_dpll_params_es1_1524mhz[NUM_SYS_CLKS] = {
	{127, 1, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 12 MHz   */
	{762, 12, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 13 MHz   */
	{635, 13, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 16.8 MHz */
	{635, 15, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 19.2 MHz */
	{381, 12, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 26 MHz   */
	{254, 8, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 27 MHz   */
	{496, 24, 1, 5, 8, 4, 6, 5, -1, -1, -1, -1}	/* 38.4 MHz */
};

/* OMAP4430 ES2.X OPP_NOM frequency */
static const struct dpll_params
		core_dpll_params_es2_1600mhz_ddr200mhz[NUM_SYS_CLKS] = {
	{200, 2, 2, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 12 MHz   */
	{800, 12, 2, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 13 MHz   */
	{619, 12, 2, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 16.8 MHz */
	{125, 2, 2, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 19.2 MHz */
	{400, 12, 2, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 26 MHz   */
	{800, 26, 2, 5, 8, 4, 6, 5, -1, -1, -1, -1},	/* 27 MHz   */
	{125, 5, 2, 5, 8, 4, 6, 5, -1, -1, -1, -1}	/* 38.4 MHz */
};

static const struct dpll_params per_dpll_params_1536mhz[NUM_SYS_CLKS] = {
	{64, 0, 8, 6, 12, 9, 4, 5, -1, -1, -1, -1},	/* 12 MHz   */
	{768, 12, 8, 6, 12, 9, 4, 5, -1, -1, -1, -1},	/* 13 MHz   */
	{320, 6, 8, 6, 12, 9, 4, 5, -1, -1, -1, -1},	/* 16.8 MHz */
	{40, 0, 8, 6, 12, 9, 4, 5, -1, -1, -1, -1},	/* 19.2 MHz */
	{384, 12, 8, 6, 12, 9, 4, 5, -1, -1, -1, -1},	/* 26 MHz   */
	{256, 8, 8, 6, 12, 9, 4, 5, -1, -1, -1, -1},	/* 27 MHz   */
	{20, 0, 8, 6, 12, 9, 4, 5, -1, -1, -1, -1}	/* 38.4 MHz */
};

static const struct dpll_params iva_dpll_params_1862mhz[NUM_SYS_CLKS] = {
	{931, 11, -1, -1, 4, 7, -1, -1, -1, -1, -1, -1},	/* 12 MHz   */
	{931, 12, -1, -1, 4, 7, -1, -1, -1, -1, -1, -1},	/* 13 MHz   */
	{665, 11, -1, -1, 4, 7, -1, -1, -1, -1, -1, -1},	/* 16.8 MHz */
	{727, 14, -1, -1, 4, 7, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{931, 25, -1, -1, 4, 7, -1, -1, -1, -1, -1, -1},	/* 26 MHz   */
	{931, 26, -1, -1, 4, 7, -1, -1, -1, -1, -1, -1},	/* 27 MHz   */
	{291, 11, -1, -1, 4, 7, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

/* ABE M & N values with sys_clk as source */
static const struct dpll_params
		abe_dpll_params_sysclk_196608khz[NUM_SYS_CLKS] = {
	{49, 5, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 12 MHz   */
	{68, 8, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 13 MHz   */
	{35, 5, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 16.8 MHz */
	{46, 8, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{34, 8, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz   */
	{29, 7, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 27 MHz   */
	{64, 24, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1}	/* 38.4 MHz */
};

/* ABE M & N values with 32K clock as source */
static const struct dpll_params abe_dpll_params_32k_196608khz = {
	750, 0, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1
};

static const struct dpll_params usb_dpll_params_1920mhz[NUM_SYS_CLKS] = {
	{80, 0, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz   */
	{960, 12, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 13 MHz   */
	{400, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 16.8 MHz */
	{50, 0, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 19.2 MHz */
	{480, 12, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz   */
	{320, 8, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 27 MHz   */
	{25, 0, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

struct dplls omap4430_dplls_es1 = {
	.mpu = mpu_dpll_params_1200mhz,
	.core = core_dpll_params_es1_1524mhz,
	.per = per_dpll_params_1536mhz,
	.iva = iva_dpll_params_1862mhz,
#ifdef CONFIG_SYS_OMAP_ABE_SYSCK
	.abe = abe_dpll_params_sysclk_196608khz,
#else
	.abe = &abe_dpll_params_32k_196608khz,
#endif
	.usb = usb_dpll_params_1920mhz,
	.ddr = NULL
};

struct dplls omap4430_dplls = {
	.mpu = mpu_dpll_params_1200mhz,
	.core = core_dpll_params_1600mhz,
	.per = per_dpll_params_1536mhz,
	.iva = iva_dpll_params_1862mhz,
#ifdef CONFIG_SYS_OMAP_ABE_SYSCK
	.abe = abe_dpll_params_sysclk_196608khz,
#else
	.abe = &abe_dpll_params_32k_196608khz,
#endif
	.usb = usb_dpll_params_1920mhz,
	.ddr = NULL
};

struct dplls omap4460_dplls = {
	.mpu = mpu_dpll_params_1400mhz,
	.core = core_dpll_params_1600mhz,
	.per = per_dpll_params_1536mhz,
	.iva = iva_dpll_params_1862mhz,
#ifdef CONFIG_SYS_OMAP_ABE_SYSCK
	.abe = abe_dpll_params_sysclk_196608khz,
#else
	.abe = &abe_dpll_params_32k_196608khz,
#endif
	.usb = usb_dpll_params_1920mhz,
	.ddr = NULL
};

struct pmic_data twl6030_4430es1 = {
	.base_offset = PHOENIX_SMPS_BASE_VOLT_STD_MODE_UV,
	.step = 12660, /* 12.66 mV represented in uV */
	/* The code starts at 1 not 0 */
	.start_code = 1,
};

struct pmic_data twl6030 = {
	.base_offset = PHOENIX_SMPS_BASE_VOLT_STD_MODE_WITH_OFFSET_UV,
	.step = 12660, /* 12.66 mV represented in uV */
	/* The code starts at 1 not 0 */
	.start_code = 1,
};

struct pmic_data tps62361 = {
	.base_offset = TPS62361_BASE_VOLT_MV,
	.step = 10000, /* 10 mV represented in uV */
	.start_code = 0,
	.gpio = TPS62361_VSEL0_GPIO,
	.gpio_en = 1
};

struct vcores_data omap4430_volts_es1 = {
	.mpu.value = 1325,
	.mpu.addr = SMPS_REG_ADDR_VCORE1,
	.mpu.pmic = &twl6030_4430es1,

	.core.value = 1200,
	.core.addr = SMPS_REG_ADDR_VCORE3,
	.core.pmic = &twl6030_4430es1,

	.mm.value = 1200,
	.mm.addr = SMPS_REG_ADDR_VCORE2,
	.mm.pmic = &twl6030_4430es1,
};

struct vcores_data omap4430_volts = {
	.mpu.value = 1325,
	.mpu.addr = SMPS_REG_ADDR_VCORE1,
	.mpu.pmic = &twl6030,

	.core.value = 1200,
	.core.addr = SMPS_REG_ADDR_VCORE3,
	.core.pmic = &twl6030,

	.mm.value = 1200,
	.mm.addr = SMPS_REG_ADDR_VCORE2,
	.mm.pmic = &twl6030,
};

struct vcores_data omap4460_volts = {
	.mpu.value = 1203,
	.mpu.addr = TPS62361_REG_ADDR_SET1,
	.mpu.pmic = &tps62361,

	.core.value = 1200,
	.core.addr = SMPS_REG_ADDR_VCORE1,
	.core.pmic = &twl6030,

	.mm.value = 1200,
	.mm.addr = SMPS_REG_ADDR_VCORE2,
	.mm.pmic = &twl6030,
};

/*
 * Enable essential clock domains, modules and
 * do some additional special settings needed
 */
void enable_basic_clocks(void)
{
	u32 const clk_domains_essential[] = {
		(*prcm)->cm_l4per_clkstctrl,
		(*prcm)->cm_l3init_clkstctrl,
		(*prcm)->cm_memif_clkstctrl,
		(*prcm)->cm_l4cfg_clkstctrl,
		0
	};

	u32 const clk_modules_hw_auto_essential[] = {
		(*prcm)->cm_l3_gpmc_clkctrl,
		(*prcm)->cm_memif_emif_1_clkctrl,
		(*prcm)->cm_memif_emif_2_clkctrl,
		(*prcm)->cm_l4cfg_l4_cfg_clkctrl,
		(*prcm)->cm_wkup_gpio1_clkctrl,
		(*prcm)->cm_l4per_gpio2_clkctrl,
		(*prcm)->cm_l4per_gpio3_clkctrl,
		(*prcm)->cm_l4per_gpio4_clkctrl,
		(*prcm)->cm_l4per_gpio5_clkctrl,
		(*prcm)->cm_l4per_gpio6_clkctrl,
		0
	};

	u32 const clk_modules_explicit_en_essential[] = {
		(*prcm)->cm_wkup_gptimer1_clkctrl,
		(*prcm)->cm_l3init_hsmmc1_clkctrl,
		(*prcm)->cm_l3init_hsmmc2_clkctrl,
		(*prcm)->cm_l4per_gptimer2_clkctrl,
		(*prcm)->cm_wkup_wdtimer2_clkctrl,
		(*prcm)->cm_l4per_uart3_clkctrl,
		0
	};

	/* Enable optional additional functional clock for GPIO4 */
	setbits_le32((*prcm)->cm_l4per_gpio4_clkctrl,
			GPIO4_CLKCTRL_OPTFCLKEN_MASK);

	/* Enable 96 MHz clock for MMC1 & MMC2 */
	setbits_le32((*prcm)->cm_l3init_hsmmc1_clkctrl,
			HSMMC_CLKCTRL_CLKSEL_MASK);
	setbits_le32((*prcm)->cm_l3init_hsmmc2_clkctrl,
			HSMMC_CLKCTRL_CLKSEL_MASK);

	/* Select 32KHz clock as the source of GPTIMER1 */
	setbits_le32((*prcm)->cm_wkup_gptimer1_clkctrl,
			GPTIMER1_CLKCTRL_CLKSEL_MASK);

	/* Enable optional 48M functional clock for USB  PHY */
	setbits_le32((*prcm)->cm_l3init_usbphy_clkctrl,
			USBPHY_CLKCTRL_OPTFCLKEN_PHY_48M_MASK);

	do_enable_clocks(clk_domains_essential,
			 clk_modules_hw_auto_essential,
			 clk_modules_explicit_en_essential,
			 1);
}

void enable_basic_uboot_clocks(void)
{
	u32 const clk_domains_essential[] = {
		0
	};

	u32 const clk_modules_hw_auto_essential[] = {
		(*prcm)->cm_l3init_hsusbotg_clkctrl,
		(*prcm)->cm_l3init_usbphy_clkctrl,
		(*prcm)->cm_l3init_usbphy_clkctrl,
		(*prcm)->cm_clksel_usb_60mhz,
		(*prcm)->cm_l3init_hsusbtll_clkctrl,
		0
	};

	u32 const clk_modules_explicit_en_essential[] = {
		(*prcm)->cm_l4per_mcspi1_clkctrl,
		(*prcm)->cm_l4per_i2c1_clkctrl,
		(*prcm)->cm_l4per_i2c2_clkctrl,
		(*prcm)->cm_l4per_i2c3_clkctrl,
		(*prcm)->cm_l4per_i2c4_clkctrl,
		(*prcm)->cm_l3init_hsusbhost_clkctrl,
		0
	};

	do_enable_clocks(clk_domains_essential,
			 clk_modules_hw_auto_essential,
			 clk_modules_explicit_en_essential,
			 1);
}

/*
 * Enable non-essential clock domains, modules and
 * do some additional special settings needed
 */
void enable_non_essential_clocks(void)
{
	u32 const clk_domains_non_essential[] = {
		(*prcm)->cm_mpu_m3_clkstctrl,
		(*prcm)->cm_ivahd_clkstctrl,
		(*prcm)->cm_dsp_clkstctrl,
		(*prcm)->cm_dss_clkstctrl,
		(*prcm)->cm_sgx_clkstctrl,
		(*prcm)->cm1_abe_clkstctrl,
		(*prcm)->cm_c2c_clkstctrl,
		(*prcm)->cm_cam_clkstctrl,
		(*prcm)->cm_dss_clkstctrl,
		(*prcm)->cm_sdma_clkstctrl,
		0
	};

	u32 const clk_modules_hw_auto_non_essential[] = {
		(*prcm)->cm_l3instr_l3_3_clkctrl,
		(*prcm)->cm_l3instr_l3_instr_clkctrl,
		(*prcm)->cm_l3instr_intrconn_wp1_clkctrl,
		(*prcm)->cm_l3init_hsi_clkctrl,
		0
	};

	u32 const clk_modules_explicit_en_non_essential[] = {
		(*prcm)->cm1_abe_aess_clkctrl,
		(*prcm)->cm1_abe_pdm_clkctrl,
		(*prcm)->cm1_abe_dmic_clkctrl,
		(*prcm)->cm1_abe_mcasp_clkctrl,
		(*prcm)->cm1_abe_mcbsp1_clkctrl,
		(*prcm)->cm1_abe_mcbsp2_clkctrl,
		(*prcm)->cm1_abe_mcbsp3_clkctrl,
		(*prcm)->cm1_abe_slimbus_clkctrl,
		(*prcm)->cm1_abe_timer5_clkctrl,
		(*prcm)->cm1_abe_timer6_clkctrl,
		(*prcm)->cm1_abe_timer7_clkctrl,
		(*prcm)->cm1_abe_timer8_clkctrl,
		(*prcm)->cm1_abe_wdt3_clkctrl,
		(*prcm)->cm_l4per_gptimer9_clkctrl,
		(*prcm)->cm_l4per_gptimer10_clkctrl,
		(*prcm)->cm_l4per_gptimer11_clkctrl,
		(*prcm)->cm_l4per_gptimer3_clkctrl,
		(*prcm)->cm_l4per_gptimer4_clkctrl,
		(*prcm)->cm_l4per_hdq1w_clkctrl,
		(*prcm)->cm_l4per_mcbsp4_clkctrl,
		(*prcm)->cm_l4per_mcspi2_clkctrl,
		(*prcm)->cm_l4per_mcspi3_clkctrl,
		(*prcm)->cm_l4per_mcspi4_clkctrl,
		(*prcm)->cm_l4per_mmcsd3_clkctrl,
		(*prcm)->cm_l4per_mmcsd4_clkctrl,
		(*prcm)->cm_l4per_mmcsd5_clkctrl,
		(*prcm)->cm_l4per_uart1_clkctrl,
		(*prcm)->cm_l4per_uart2_clkctrl,
		(*prcm)->cm_l4per_uart4_clkctrl,
		(*prcm)->cm_wkup_keyboard_clkctrl,
		(*prcm)->cm_wkup_wdtimer2_clkctrl,
		(*prcm)->cm_cam_iss_clkctrl,
		(*prcm)->cm_cam_fdif_clkctrl,
		(*prcm)->cm_dss_dss_clkctrl,
		(*prcm)->cm_sgx_sgx_clkctrl,
		0
	};

	/* Enable optional functional clock for ISS */
	setbits_le32((*prcm)->cm_cam_iss_clkctrl, ISS_CLKCTRL_OPTFCLKEN_MASK);

	/* Enable all optional functional clocks of DSS */
	setbits_le32((*prcm)->cm_dss_dss_clkctrl, DSS_CLKCTRL_OPTFCLKEN_MASK);

	do_enable_clocks(clk_domains_non_essential,
			 clk_modules_hw_auto_non_essential,
			 clk_modules_explicit_en_non_essential,
			 0);

	/* Put camera module in no sleep mode */
	clrsetbits_le32((*prcm)->cm_cam_clkstctrl,
			MODULE_CLKCTRL_MODULEMODE_MASK,
			CD_CLKCTRL_CLKTRCTRL_NO_SLEEP <<
			MODULE_CLKCTRL_MODULEMODE_SHIFT);
}

void hw_data_init(void)
{
	u32 omap_rev = omap_revision();

	(*prcm) = &omap4_prcm;

	switch (omap_rev) {

	case OMAP4430_ES1_0:
	*dplls_data = &omap4430_dplls_es1;
	*omap_vcores = &omap4430_volts_es1;
	break;

	case OMAP4430_ES2_0:
	case OMAP4430_ES2_1:
	case OMAP4430_ES2_2:
	case OMAP4430_ES2_3:
	*dplls_data = &omap4430_dplls;
	*omap_vcores = &omap4430_volts;
	break;

	case OMAP4460_ES1_0:
	case OMAP4460_ES1_1:
	*dplls_data = &omap4460_dplls;
	*omap_vcores = &omap4460_volts;
	break;

	default:
		printf("\n INVALID OMAP REVISION ");
	}

	*ctrl = &omap4_ctrl;
}
