/*
 * sun4i, sun5i and sun7i specific clock code
 *
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * (C) Copyright 2013 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sys_proto.h>

#ifdef CONFIG_SPL_BUILD
void clock_init_safe(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* Set safe defaults until PMU is configured */
	writel(AXI_DIV_1 << AXI_DIV_SHIFT |
	       AHB_DIV_2 << AHB_DIV_SHIFT |
	       APB0_DIV_1 << APB0_DIV_SHIFT |
	       CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT,
	       &ccm->cpu_ahb_apb0_cfg);
	writel(PLL1_CFG_DEFAULT, &ccm->pll1_cfg);
	sdelay(200);
	writel(AXI_DIV_1 << AXI_DIV_SHIFT |
	       AHB_DIV_2 << AHB_DIV_SHIFT |
	       APB0_DIV_1 << APB0_DIV_SHIFT |
	       CPU_CLK_SRC_PLL1 << CPU_CLK_SRC_SHIFT,
	       &ccm->cpu_ahb_apb0_cfg);
#ifdef CONFIG_SUN7I
	writel(0x1 << AHB_GATE_OFFSET_DMA | readl(&ccm->ahb_gate0),
	       &ccm->ahb_gate0);
#endif
	writel(PLL6_CFG_DEFAULT, &ccm->pll6_cfg);
}
#endif

void clock_init_uart(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* uart clock source is apb1 */
	writel(APB1_CLK_SRC_OSC24M|
	       APB1_CLK_RATE_N_1|
	       APB1_CLK_RATE_M(1),
	       &ccm->apb1_clk_div_cfg);

	/* open the clock for uart */
	setbits_le32(&ccm->apb1_gate,
		CLK_GATE_OPEN << (APB1_GATE_UART_SHIFT+CONFIG_CONS_INDEX-1));
}

#ifdef CONFIG_NAND_SUNXI

void clock_init_nand()
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	/* nand clock source is osc24m */
	sr32(&ccm->nand_sclk_cfg, 24, 2, NAND_CLK_SRC_OSC24);
	sr32(&ccm->nand_sclk_cfg, 16, 2, NAND_CLK_DIV_N);
	sr32(&ccm->nand_sclk_cfg, 0, 4, NAND_CLK_DIV_M);
	sr32(&ccm->nand_sclk_cfg, 31, 1, CLK_GATE_OPEN);
	/* open clock for nand */
	sr32(&ccm->ahb_gate0, AHB_GATE_OFFSET_NAND, 1, CLK_GATE_OPEN);
	
}
#endif

int clock_twi_onoff(int port, int state)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	if (port > 2)
		return -1;

	/* set the apb clock gate for twi */
	if (state)
		setbits_le32(&ccm->apb1_gate,
			     CLK_GATE_OPEN << (APB1_GATE_TWI_SHIFT+port));
	else
		clrbits_le32(&ccm->apb1_gate,
			     CLK_GATE_OPEN << (APB1_GATE_TWI_SHIFT+port));

	return 0;
}

/* Return PLL5 frequency in Hz
 * Note: Assumes PLL5 reference is 24MHz clock
 */
unsigned int clock_get_pll5(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll5_cfg);
	int n = (rval >> 8) & 0x1f;
	int k = ((rval >> 4) & 3) + 1;
	int p = 1 << ((rval >> 16) & 3);
	return 24000000 * n * k / p;
}

#ifdef CONFIG_SPL_BUILD
#define PLL1_CFG(N, K, M, P)	( 1 << CCM_PLL1_CFG_ENABLE_SHIFT | \
				  0 << CCM_PLL1_CFG_VCO_RST_SHIFT |  \
				  8 << CCM_PLL1_CFG_VCO_BIAS_SHIFT | \
				  0 << CCM_PLL1_CFG_PLL4_EXCH_SHIFT | \
				 16 << CCM_PLL1_CFG_BIAS_CUR_SHIFT | \
				 (P)<< CCM_PLL1_CFG_DIVP_SHIFT | \
				  2 << CCM_PLL1_CFG_LCK_TMR_SHIFT | \
				 (N)<< CCM_PLL1_CFG_FACTOR_N_SHIFT | \
				 (K)<< CCM_PLL1_CFG_FACTOR_K_SHIFT | \
				  0 << CCM_PLL1_CFG_SIG_DELT_PAT_IN_SHIFT | \
				  0 << CCM_PLL1_CFG_SIG_DELT_PAT_EN_SHIFT | \
				 (M)<< CCM_PLL1_CFG_FACTOR_M_SHIFT)

static struct {
	u32 pll1_cfg;
	unsigned int freq;
} pll1_para[] = {
	/* This array must be ordered by frequency. */
	{ PLL1_CFG(16, 0, 0, 0), 384000000 },
	{ PLL1_CFG(16, 1, 0, 0), 768000000 },
	{ PLL1_CFG(20, 1, 0, 0), 960000000 },
	{ PLL1_CFG(21, 1, 0, 0), 1008000000},
	{ PLL1_CFG(22, 1, 0, 0), 1056000000},
	{ PLL1_CFG(23, 1, 0, 0), 1104000000},
	{ PLL1_CFG(24, 1, 0, 0), 1152000000},
	{ PLL1_CFG(25, 1, 0, 0), 1200000000},
	{ PLL1_CFG(26, 1, 0, 0), 1248000000},
	{ PLL1_CFG(27, 1, 0, 0), 1296000000},
	{ PLL1_CFG(28, 1, 0, 0), 1344000000},
	{ PLL1_CFG(29, 1, 0, 0), 1392000000},
	{ PLL1_CFG(30, 1, 0, 0), 1440000000},
	{ PLL1_CFG(31, 1, 0, 0), 1488000000},
	/* Final catchall entry */
	{ PLL1_CFG(31, 1, 0, 0), ~0},
};

void clock_set_pll1(unsigned int hz)
{
	int i = 0;
	int axi, ahb, apb0;
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* Find target frequency */
	while (pll1_para[i].freq < hz)
		i++;

	hz = pll1_para[i].freq;

	/* Calculate system clock divisors */
	axi = DIV_ROUND_UP(hz, 432000000);	/* Max 450MHz */
	ahb = DIV_ROUND_UP(hz/axi, 204000000);	/* Max 250MHz */
	apb0 = 2;				/* Max 150MHz */

	printf("CPU: %uHz, AXI/AHB/APB: %d/%d/%d\n", hz, axi, ahb, apb0);

	/* Map divisors to register values */
	axi = axi - 1;
	if (ahb > 4)
		ahb = 3;
	else if (ahb > 2)
		ahb = 2;
	else if (ahb > 1)
		ahb = 1;
	else
		ahb = 0;

	apb0 = apb0 - 1;

	/* Switch to 24MHz clock while changing PLL1 */
	writel(AXI_DIV_1 << AXI_DIV_SHIFT |
	       AHB_DIV_2 << AHB_DIV_SHIFT |
	       APB0_DIV_1 << APB0_DIV_SHIFT |
	       CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT,
	       &ccm->cpu_ahb_apb0_cfg);
	sdelay(20);

	/* Configure sys clock divisors */
	writel(axi << AXI_DIV_SHIFT |
	       ahb << AHB_DIV_SHIFT |
	       apb0 << APB0_DIV_SHIFT |
	       CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT,
	       &ccm->cpu_ahb_apb0_cfg);

	/* Configure PLL1 at the desired frequency */
	writel(pll1_para[i].pll1_cfg, &ccm->pll1_cfg);
	sdelay(200);

	/* Switch CPU to PLL1 */
	writel(axi << AXI_DIV_SHIFT |
	       ahb << AHB_DIV_SHIFT |
	       apb0 << APB0_DIV_SHIFT |
	       CPU_CLK_SRC_PLL1 << CPU_CLK_SRC_SHIFT,
	       &ccm->cpu_ahb_apb0_cfg);
	sdelay(20);
}
#endif

unsigned int clock_get_pll6(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll6_cfg);
	int n = ((rval & CCM_PLL6_CTRL_N_MASK) >> CCM_PLL6_CTRL_N_SHIFT);
	int k = ((rval & CCM_PLL6_CTRL_K_MASK) >> CCM_PLL6_CTRL_K_SHIFT) + 1;
	return 24000000 * n * k / 2;
}

