/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * (C) Copyright 2013 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sys_proto.h>

#ifdef DEBUG_PRINT_CCM
void debug_print_ccm(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* 0x00 pll1 control */
	printf("pll1_cfg  = 0x%x\n", readl(&ccm->pll1_cfg));
	/* 0x04 pll1 tuning */
	printf("pll1_tun  = 0x%x\n", readl(&ccm->pll1_tun));
	/* 0x08 pll2 control */
	printf("pll2_cfg  = 0x%x\n", readl(&ccm->pll2_cfg));
	/* 0x0c pll2 tuning */
	printf("pll2_tun  = 0x%x\n", readl(&ccm->pll2_tun));
	/* 0x10 pll3 control */
	printf("pll3_cfg  = 0x%x\n", readl(&ccm->pll3_cfg));
	/* Reserved: u8 res0[0x4]; */
	/* 0x18 pll4 control */
	printf("pll4_cfg  = 0x%x\n", readl(&ccm->pll4_cfg));
	/* Reserved: u8 res1[0x4]; */
	/* 0x20 pll5 control */
	printf("pll5_cfg  = 0x%x\n", readl(&ccm->pll5_cfg));
	/* 0x24 pll5 tuning */
	printf("pll5_tun  = 0x%x\n", readl(&ccm->pll5_tun));
	/* 0x28 pll6 control */
	printf("pll6_cfg  = 0x%x\n", readl(&ccm->pll6_cfg));
	/* 0x2c pll6 tuning */
	printf("pll6_tun  = 0x%x\n", readl(&ccm->pll6_tun));
	/* 0x30 pll7 control */
	printf("pll7_cfg  = 0x%x\n", readl(&ccm->pll7_cfg));
	/* 0x34 pll5 tuning2 */
	printf("pll1_tun2  = 0x%x\n", readl(&ccm->pll1_tun2));
	/* Reserved: u8 res2[0x4]; */
	/* 0x3c pll5 tuning2 */
	printf("pll5_tun2  = 0x%x\n", readl(&ccm->pll5_tun2));
	/* Reserved: u8 res3[0xc]; */
	/* 0x4c pll lock time debug */
	printf("pll_lock_dbg  = 0x%x\n", readl(&ccm->pll_lock_dbg));
	/* 0x50 osc24m control */
	printf("osc24m_cfg  = 0x%x\n", readl(&ccm->osc24m_cfg));
	/* 0x54 cpu,ahb and apb0 divide ratio */
	printf("cpu_ahb_apb0_cfg  = 0x%x\n", readl(&ccm->cpu_ahb_apb0_cfg));
	/* 0x58 apb1 clock dividor */
	printf("apb1_clk_div_cfg  = 0x%x\n", readl(&ccm->apb1_clk_div_cfg));
	/* 0x5c axi module clock gating */
	printf("axi_gate  = 0x%x\n", readl(&ccm->axi_gate));
	/* 0x60 ahb module clock gating 0 */
	printf("ahb_gate0  = 0x%x\n", readl(&ccm->ahb_gate0));
	/* 0x64 ahb module clock gating 1 */
	printf("ahb_gate1  = 0x%x\n", readl(&ccm->ahb_gate1));
	/* 0x68 apb0 module clock gating */
	printf("apb0_gate  = 0x%x\n", readl(&ccm->apb0_gate));
	/* 0x6c apb1 module clock gating */
	printf("apb1_gate  = 0x%x\n", readl(&ccm->apb1_gate));
	/* Reserved: u8 res4[0x10]; */
	/* 0x80 nand sub clock control */
	printf("nand_sclk_cfg  = 0x%x\n", readl(&ccm->nand_sclk_cfg));
	/* 0x84 memory stick sub clock control */
	printf("ms_sclk_cfg  = 0x%x\n", readl(&ccm->ms_sclk_cfg));
	/* 0x88 sd0 clock control */
	printf("sd0_clk_cfg  = 0x%x\n", readl(&ccm->sd0_clk_cfg));
	/* 0x8c sd1 clock control */
	printf("sd1_clk_cfg  = 0x%x\n", readl(&ccm->sd1_clk_cfg));
}
#endif

#ifdef CONFIG_SPL_BUILD
static void clock_init_safe(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* Set safe defaults until PMU is configured */
	writel(AXI_DIV_1 << 0 | AHB_DIV_2 << 4 | APB0_DIV_1 << 8 |
	       CPU_CLK_SRC_OSC24M << 16, &ccm->cpu_ahb_apb0_cfg);
	writel(0xa1005000, &ccm->pll1_cfg);
	sdelay(200);
	writel(AXI_DIV_1 << 0 | AHB_DIV_2 << 4 | APB0_DIV_1 << 8 |
	       CPU_CLK_SRC_PLL1 << 16, &ccm->cpu_ahb_apb0_cfg);
#ifdef CONFIG_SUN7I
	writel(0x1 << 6 | readl(&ccm->ahb_gate0), &ccm->ahb_gate0);
	writel(0x1 << 31 | readl(&ccm->pll6_cfg), &ccm->pll6_cfg);
#endif
}
#endif

int clock_init(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

#ifdef CONFIG_SPL_BUILD
	clock_init_safe();
#endif

	/* uart clock source is apb1 */
	sr32(&ccm->apb1_clk_div_cfg, 24, 2, APB1_CLK_SRC_OSC24M);
	sr32(&ccm->apb1_clk_div_cfg, 16, 2, APB1_FACTOR_N);
	sr32(&ccm->apb1_clk_div_cfg, 0, 5, APB1_FACTOR_M);

	/* open the clock for uart */
	sr32(&ccm->apb1_gate, 16 + CONFIG_CONS_INDEX - 1, 1, CLK_GATE_OPEN);

#ifdef CONFIG_NAND_SUNXI
	/* nand clock source is osc24m */
	sr32(&ccm->nand_sclk_cfg, 24, 2, NAND_CLK_SRC_OSC24);
	sr32(&ccm->nand_sclk_cfg, 16, 2, NAND_CLK_DIV_N);
	sr32(&ccm->nand_sclk_cfg, 0, 4, NAND_CLK_DIV_M);
	sr32(&ccm->nand_sclk_cfg, 31, 1, CLK_GATE_OPEN);
	/* open clock for nand */
	sr32(&ccm->ahb_gate0, AHB_GATE_OFFSET_NAND, 1, CLK_GATE_OPEN);
#endif

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

int clock_twi_onoff(int port, int state)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	if (port > 2)
		return -1;

	/* set the apb1 clock gate for twi */
	sr32(&ccm->apb1_gate, 0 + port, 1, state);

	return 0;
}

#ifdef CONFIG_SPL_BUILD
#define PLL1_CFG(N, K, M, P)	(1 << 31 | 0 << 30 | 8 << 26 | 0 << 25 | \
				 16 << 20 | (P) << 16 | 2 << 13 | (N) << 8 | \
				 (K) << 4 | 0 << 3 | 0 << 2 | (M) << 0)
#define RDIV(a, b)		((a + (b) - 1) / (b))

struct {
	u32 pll1_cfg;
	unsigned int freq;
} pll1_para[] = {
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
	{ PLL1_CFG(31, 1, 0, 0), ~0},
};

void clock_set_pll1(int hz)
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
	axi = RDIV(hz, 432000000);		/* Max 450MHz */
	ahb = RDIV(hz/axi, 204000000);		/* Max 250MHz */
	apb0 = 2;				/* Max 150MHz */

	printf("CPU: %dHz, AXI/AHB/APB: %d/%d/%d\n", hz, axi, ahb, apb0);

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
	writel(AXI_DIV_1 << 0 | AHB_DIV_2 << 4 | APB0_DIV_1 << 8 |
	       CPU_CLK_SRC_OSC24M << 16, &ccm->cpu_ahb_apb0_cfg);
	sdelay(20);

	/* Configure sys clock divisors */
	writel(axi << 0 | ahb << 4 | apb0 << 8 | CPU_CLK_SRC_OSC24M << 16,
	       &ccm->cpu_ahb_apb0_cfg);

	/* Configure PLL1 at the desired frequency */
	writel(pll1_para[i].pll1_cfg, &ccm->pll1_cfg);
	sdelay(200);

	/* Switch CPU to PLL1 */
	writel(axi << 0 | ahb << 4 | apb0 << 8 | CPU_CLK_SRC_PLL1 << 16,
	       &ccm->cpu_ahb_apb0_cfg);
	sdelay(20);
}
#endif
