/*
 * clock_ti814x.c
 *
 * Clocks for TI814X based boards
 *
 * Copyright (C) 2013, Texas Instruments, Incorporated
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>

/* PRCM */
#define PRCM_MOD_EN		0x2

/* CLK_SRC */
#define OSC_SRC0		0
#define OSC_SRC1		1

#define L3_OSC_SRC		OSC_SRC0

#define OSC_0_FREQ		20

#define DCO_HS2_MIN		500
#define DCO_HS2_MAX		1000
#define DCO_HS1_MIN		1000
#define DCO_HS1_MAX		2000

#define SELFREQDCO_HS2		0x00000801
#define SELFREQDCO_HS1		0x00001001

#define MPU_N			0x1
#define MPU_M			0x3C
#define MPU_M2			1
#define MPU_CLKCTRL		0x1

#define L3_N			19
#define L3_M			880
#define L3_M2			4
#define L3_CLKCTRL		0x801

#define DDR_N			19
#define DDR_M			666
#define DDR_M2			2
#define DDR_CLKCTRL		0x801

/* ADPLLJ register values */
#define ADPLLJ_CLKCTRL_HS2	0x00000801 /* HS2 mode, TINT2 = 1 */
#define ADPLLJ_CLKCTRL_HS1	0x00001001 /* HS1 mode, TINT2 = 1 */
#define ADPLLJ_CLKCTRL_CLKDCOLDOEN	(1 << 29)
#define ADPLLJ_CLKCTRL_IDLE		(1 << 23)
#define ADPLLJ_CLKCTRL_CLKOUTEN		(1 << 20)
#define ADPLLJ_CLKCTRL_CLKOUTLDOEN	(1 << 19)
#define ADPLLJ_CLKCTRL_CLKDCOLDOPWDNZ	(1 << 17)
#define ADPLLJ_CLKCTRL_LPMODE		(1 << 12)
#define ADPLLJ_CLKCTRL_DRIFTGUARDIAN	(1 << 11)
#define ADPLLJ_CLKCTRL_REGM4XEN		(1 << 10)
#define ADPLLJ_CLKCTRL_TINITZ		(1 << 0)
#define ADPLLJ_CLKCTRL_CLKDCO		(ADPLLJ_CLKCTRL_CLKDCOLDOEN | \
					 ADPLLJ_CLKCTRL_CLKOUTEN | \
					 ADPLLJ_CLKCTRL_CLKOUTLDOEN | \
					 ADPLLJ_CLKCTRL_CLKDCOLDOPWDNZ)

#define ADPLLJ_STATUS_PHASELOCK		(1 << 10)
#define ADPLLJ_STATUS_FREQLOCK		(1 << 9)
#define ADPLLJ_STATUS_PHSFRQLOCK	(ADPLLJ_STATUS_PHASELOCK | \
					 ADPLLJ_STATUS_FREQLOCK)
#define ADPLLJ_STATUS_BYPASSACK		(1 << 8)
#define ADPLLJ_STATUS_BYPASS		(1 << 0)
#define ADPLLJ_STATUS_BYPASSANDACK	(ADPLLJ_STATUS_BYPASSACK | \
					 ADPLLJ_STATUS_BYPASS)

#define ADPLLJ_TENABLE_ENB		(1 << 0)
#define ADPLLJ_TENABLEDIV_ENB		(1 << 0)

#define ADPLLJ_M2NDIV_M2SHIFT		16

#define MPU_PLL_BASE			(PLL_SUBSYS_BASE + 0x048)
#define L3_PLL_BASE			(PLL_SUBSYS_BASE + 0x110)
#define DDR_PLL_BASE			(PLL_SUBSYS_BASE + 0x290)

struct ad_pll {
	unsigned int pwrctrl;
	unsigned int clkctrl;
	unsigned int tenable;
	unsigned int tenablediv;
	unsigned int m2ndiv;
	unsigned int mn2div;
	unsigned int fracdiv;
	unsigned int bwctrl;
	unsigned int fracctrl;
	unsigned int status;
	unsigned int m3div;
	unsigned int rampctrl;
};

#define OSC_SRC_CTRL			(PLL_SUBSYS_BASE + 0x2C0)

/* PRCM */
#define CM_DEFAULT_BASE			(PRCM_BASE + 0x0500)

struct cm_def {
	unsigned int resv0[2];
	unsigned int l3fastclkstctrl;
	unsigned int resv1[1];
	unsigned int pciclkstctrl;
	unsigned int resv2[1];
	unsigned int ducaticlkstctrl;
	unsigned int resv3[1];
	unsigned int emif0clkctrl;
	unsigned int emif1clkctrl;
	unsigned int dmmclkctrl;
	unsigned int fwclkctrl;
	unsigned int resv4[10];
	unsigned int usbclkctrl;
	unsigned int resv5[1];
	unsigned int sataclkctrl;
	unsigned int resv6[4];
	unsigned int ducaticlkctrl;
	unsigned int pciclkctrl;
};

#define CM_ALWON_BASE			(PRCM_BASE + 0x1400)

struct cm_alwon {
	unsigned int l3slowclkstctrl;
	unsigned int ethclkstctrl;
	unsigned int l3medclkstctrl;
	unsigned int mmu_clkstctrl;
	unsigned int mmucfg_clkstctrl;
	unsigned int ocmc0clkstctrl;
	unsigned int vcpclkstctrl;
	unsigned int mpuclkstctrl;
	unsigned int sysclk4clkstctrl;
	unsigned int sysclk5clkstctrl;
	unsigned int sysclk6clkstctrl;
	unsigned int rtcclkstctrl;
	unsigned int l3fastclkstctrl;
	unsigned int resv0[67];
	unsigned int mcasp0clkctrl;
	unsigned int mcasp1clkctrl;
	unsigned int mcasp2clkctrl;
	unsigned int mcbspclkctrl;
	unsigned int uart0clkctrl;
	unsigned int uart1clkctrl;
	unsigned int uart2clkctrl;
	unsigned int gpio0clkctrl;
	unsigned int gpio1clkctrl;
	unsigned int i2c0clkctrl;
	unsigned int i2c1clkctrl;
	unsigned int mcasp345clkctrl;
	unsigned int atlclkctrl;
	unsigned int mlbclkctrl;
	unsigned int pataclkctrl;
	unsigned int resv1[1];
	unsigned int uart3clkctrl;
	unsigned int uart4clkctrl;
	unsigned int uart5clkctrl;
	unsigned int wdtimerclkctrl;
	unsigned int spiclkctrl;
	unsigned int mailboxclkctrl;
	unsigned int spinboxclkctrl;
	unsigned int mmudataclkctrl;
	unsigned int resv2[2];
	unsigned int mmucfgclkctrl;
	unsigned int resv3[2];
	unsigned int ocmc0clkctrl;
	unsigned int vcpclkctrl;
	unsigned int resv4[2];
	unsigned int controlclkctrl;
	unsigned int resv5[2];
	unsigned int gpmcclkctrl;
	unsigned int ethernet0clkctrl;
	unsigned int resv6[1];
	unsigned int mpuclkctrl;
	unsigned int debugssclkctrl;
	unsigned int l3clkctrl;
	unsigned int l4hsclkctrl;
	unsigned int l4lsclkctrl;
	unsigned int rtcclkctrl;
	unsigned int tpccclkctrl;
	unsigned int tptc0clkctrl;
	unsigned int tptc1clkctrl;
	unsigned int tptc2clkctrl;
	unsigned int tptc3clkctrl;
	unsigned int resv7[4];
	unsigned int dcan01clkctrl;
	unsigned int mmchs0clkctrl;
	unsigned int mmchs1clkctrl;
	unsigned int mmchs2clkctrl;
	unsigned int custefuseclkctrl;
};


const struct cm_alwon *cmalwon = (struct cm_alwon *)CM_ALWON_BASE;
const struct cm_def *cmdef = (struct cm_def *)CM_DEFAULT_BASE;

/*
 * Enable the peripheral clock for required peripherals
 */
static void enable_per_clocks(void)
{
	/* UART0 */
	writel(PRCM_MOD_EN, &cmalwon->uart0clkctrl);
	while (readl(&cmalwon->uart0clkctrl) != PRCM_MOD_EN)
		;

	/* HSMMC1 */
	writel(PRCM_MOD_EN, &cmalwon->mmchs1clkctrl);
	while (readl(&cmalwon->mmchs1clkctrl) != PRCM_MOD_EN)
		;
}

/*
 * select the HS1 or HS2 for DCO Freq
 * return : CLKCTRL
 */
static u32 pll_dco_freq_sel(u32 clkout_dco)
{
	if (clkout_dco >= DCO_HS2_MIN && clkout_dco < DCO_HS2_MAX)
		return SELFREQDCO_HS2;
	else if (clkout_dco >= DCO_HS1_MIN && clkout_dco < DCO_HS1_MAX)
		return SELFREQDCO_HS1;
	else
		return -1;
}

/*
 * select the sigma delta config
 * return: sigma delta val
 */
static u32 pll_sigma_delta_val(u32 clkout_dco)
{
	u32 sig_val = 0;
	float frac_div;

	frac_div = (float) clkout_dco / 250;
	frac_div = frac_div + 0.90;
	sig_val = (int)frac_div;
	sig_val = sig_val << 24;

	return sig_val;
}

/*
 * configure individual ADPLLJ
 */
static void pll_config(u32 base, u32 n, u32 m, u32 m2,
		       u32 clkctrl_val, int adpllj)
{
	const struct ad_pll *adpll = (struct ad_pll *)base;
	u32 m2nval, mn2val, read_clkctrl = 0, clkout_dco = 0;
	u32 sig_val = 0, hs_mod = 0;

	m2nval = (m2 << ADPLLJ_M2NDIV_M2SHIFT) | n;
	mn2val = m;

	/* calculate clkout_dco */
	clkout_dco = ((OSC_0_FREQ / (n+1)) * m);

	/* sigma delta & Hs mode selection skip for ADPLLS*/
	if (adpllj) {
		sig_val = pll_sigma_delta_val(clkout_dco);
		hs_mod = pll_dco_freq_sel(clkout_dco);
	}

	/* by-pass pll */
	read_clkctrl = readl(&adpll->clkctrl);
	writel((read_clkctrl | ADPLLJ_CLKCTRL_IDLE), &adpll->clkctrl);
	while ((readl(&adpll->status) & ADPLLJ_STATUS_BYPASSANDACK)
		!= ADPLLJ_STATUS_BYPASSANDACK)
		;

	/* clear TINITZ */
	read_clkctrl = readl(&adpll->clkctrl);
	writel((read_clkctrl & ~ADPLLJ_CLKCTRL_TINITZ), &adpll->clkctrl);

	/*
	 * ref_clk = 20/(n + 1);
	 * clkout_dco = ref_clk * m;
	 * clk_out = clkout_dco/m2;
	*/
	read_clkctrl = readl(&adpll->clkctrl) &
			     ~(ADPLLJ_CLKCTRL_LPMODE |
			     ADPLLJ_CLKCTRL_DRIFTGUARDIAN |
			     ADPLLJ_CLKCTRL_REGM4XEN);
	writel(m2nval, &adpll->m2ndiv);
	writel(mn2val, &adpll->mn2div);

	/* Skip for modena(ADPLLS) */
	if (adpllj) {
		writel(sig_val, &adpll->fracdiv);
		writel((read_clkctrl | hs_mod), &adpll->clkctrl);
	}

	/* Load M2, N2 dividers of ADPLL */
	writel(ADPLLJ_TENABLEDIV_ENB, &adpll->tenablediv);
	writel(~ADPLLJ_TENABLEDIV_ENB, &adpll->tenablediv);

	/* Load M, N dividers of ADPLL */
	writel(ADPLLJ_TENABLE_ENB, &adpll->tenable);
	writel(~ADPLLJ_TENABLE_ENB, &adpll->tenable);

	/* Configure CLKDCOLDOEN,CLKOUTLDOEN,CLKOUT Enable BITS */
	read_clkctrl = readl(&adpll->clkctrl) & ~ADPLLJ_CLKCTRL_CLKDCO;
	if (adpllj)
		writel((read_clkctrl | ADPLLJ_CLKCTRL_CLKDCO),
						&adpll->clkctrl);

	/* Enable TINTZ and disable IDLE(PLL in Active & Locked Mode */
	read_clkctrl = readl(&adpll->clkctrl) & ~ADPLLJ_CLKCTRL_IDLE;
	writel((read_clkctrl | ADPLLJ_CLKCTRL_TINITZ), &adpll->clkctrl);

	/* Wait for phase and freq lock */
	while ((readl(&adpll->status) & ADPLLJ_STATUS_PHSFRQLOCK) !=
	       ADPLLJ_STATUS_PHSFRQLOCK)
		;
}

static void unlock_pll_control_mmr(void)
{
	/* TRM 2.10.1.4 and 3.2.7-3.2.11 */
	writel(0x1EDA4C3D, 0x481C5040);
	writel(0x2FF1AC2B, 0x48140060);
	writel(0xF757FDC0, 0x48140064);
	writel(0xE2BC3A6D, 0x48140068);
	writel(0x1EBF131D, 0x4814006c);
	writel(0x6F361E05, 0x48140070);
}

static void mpu_pll_config(void)
{
	pll_config(MPU_PLL_BASE, MPU_N, MPU_M, MPU_M2, MPU_CLKCTRL, 0);
}

static void l3_pll_config(void)
{
	u32 l3_osc_src, rd_osc_src = 0;

	l3_osc_src = L3_OSC_SRC;
	rd_osc_src = readl(OSC_SRC_CTRL);

	if (OSC_SRC0 == l3_osc_src)
		writel((rd_osc_src & 0xfffffffe)|0x0, OSC_SRC_CTRL);
	else
		writel((rd_osc_src & 0xfffffffe)|0x1, OSC_SRC_CTRL);

	pll_config(L3_PLL_BASE, L3_N, L3_M, L3_M2, L3_CLKCTRL, 1);
}

void ddr_pll_config(unsigned int ddrpll_m)
{
	pll_config(DDR_PLL_BASE, DDR_N, DDR_M, DDR_M2, DDR_CLKCTRL, 1);
}

void enable_emif_clocks(void) {};

void enable_dmm_clocks(void)
{
	writel(PRCM_MOD_EN, &cmdef->fwclkctrl);
	writel(PRCM_MOD_EN, &cmdef->l3fastclkstctrl);
	writel(PRCM_MOD_EN, &cmdef->emif0clkctrl);
	while ((readl(&cmdef->emif0clkctrl)) != PRCM_MOD_EN)
		;
	writel(PRCM_MOD_EN, &cmdef->emif1clkctrl);
	while ((readl(&cmdef->emif1clkctrl)) != PRCM_MOD_EN)
		;
	while ((readl(&cmdef->l3fastclkstctrl) & 0x300) != 0x300)
		;
	writel(PRCM_MOD_EN, &cmdef->dmmclkctrl);
	while ((readl(&cmdef->dmmclkctrl)) != PRCM_MOD_EN)
		;
	writel(PRCM_MOD_EN, &cmalwon->l3slowclkstctrl);
	while ((readl(&cmalwon->l3slowclkstctrl) & 0x2100) != 0x2100)
		;
}

/*
 * Configure the PLL/PRCM for necessary peripherals
 */
void pll_init()
{
	unlock_pll_control_mmr();

	/* Enable the control module */
	writel(PRCM_MOD_EN, &cmalwon->controlclkctrl);

	mpu_pll_config();

	l3_pll_config();

	/* Enable the required peripherals */
	enable_per_clocks();
}
