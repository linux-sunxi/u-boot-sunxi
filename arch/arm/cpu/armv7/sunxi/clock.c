/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Author :
 * 		Tom Cubie <tangliang@allwinnertech.com>
 * Derived from aw1623 boot1 code by
 * 		Jerry Wang <wangflord@allwinnertech.com>
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>

/* get core clock(pll1, clock for cpu) */
static ulong sunxi_get_core_clock(void)
{
	u32 pll1_cfg;
	u32 p, n, k, m;
	u32 clock;

	pll1_cfg = readl(SUNXI_CCM_PLL1_CFG);
	p = SUNXI_GET_BITS(pll1_cfg, 16, 2);
	n = SUNXI_GET_BITS(pll1_cfg, 8, 5);
	k = SUNXI_GET_BITS(pll1_cfg, 4, 2) + 1;
	k = SUNXI_GET_BITS(pll1_cfg, 0, 2) + 1;

	/* PLL1 output=(24MHz * N * K)/(M * P) */
	clock = 24 * 1000 * 1000 * n * k / p / m;
	
	return clock;
}

/* return axi clock */
static ulong sunxi_get_axi_clock(void)
{
	u32 cpu_ahb_apb0_cfg;
	u32 clock_src, div_ratio;
	u32 clock;

	cpu_ahb_apb0_cfg = readl(SUNXI_CCM_CPU_AHB_APB0_CFG);
	clock_src = SUNXI_GET_BITS(cpu_ahb_apb0_cfg, 16, 2);
	div_ratio = SUNXI_GET_BITS(cpu_ahb_apb0_cfg, 0, 12) + 1;

	switch(clock_src)
	{
		case 0:					/* 32KHz OSC(internal)  */
			clock = 32 * 1000;
			break;
		case 1:					/* OSC24M */
			clock = 24 * 1000 * 1000;
			break;
		case 2:					/* PLL1 */
			clock = sunxi_get_core_clock();
			break;
		case 3:					/* 200MHz(source from PLL6) */
			clock = 200 * 1000 * 1000;
		default:
			return 0;
	}

	return clock/div_ratio;
}

/* return ahb clock */
static ulong sunxi_get_ahb_clock(void)
{
	u32 cpu_ahb_apb0_cfg;
	u32 div_ratio;
	u32 clock;

	cpu_ahb_apb0_cfg = readl(SUNXI_CCM_CPU_AHB_APB0_CFG);
	div_ratio = SUNXI_GET_BITS(cpu_ahb_apb0_cfg, 4, 2);
	clock = sunxi_get_axi_clock();
	clock >>= div_ratio;

	return clock;
}

/* return ahb0 clock */
static ulong sunxi_get_ahb0_clock(void)
{
	u32 cpu_ahb_apb0_cfg;
	u32 div_ratio;
	u32 clock;

	cpu_ahb_apb0_cfg = readl(SUNXI_CCM_CPU_AHB_APB0_CFG);
	div_ratio = SUNXI_GET_BITS(cpu_ahb_apb0_cfg, 8, 2);
	clock = sunxi_get_ahb_clock();
	if(div_ratio)
		clock >>= div_ratio;
	else
		clock >>= 1;

	return clock;
}
 /* return ahb1 clock */
static ulong sunxi_get_ahb1_clock(void)
{
	u32 ahb1_div_reg;
	u32 clock_src;
	u32 clock = 0;

	ahb1_div_reg = readl(SUNXI_CCM_APB1_CLK_DIV);
	clock_src = SUNXI_GET_BITS(ahb1_div_reg, 24, 2);
	
	if(clock_src == 0) /* OSC24M */
		clock = 24 * 1000 * 1000;

	return clock;
}

/* set the cpu core clock, 4:2:1 */
ulong sunxi_core_clock_set(ulong clock_frequency)
{
	u32 n, k;
	u32 axi_div;
	u32 clk_freq;

	clk_freq = clock_frequency / 1000000;	/* handle in MHz */

	if(!clk_freq || clk_freq > 2000) {
		printf("illegal clock freqency, set to default clock 384MHz\n");
		clk_freq = 384;
	}
	else if(clk_freq < 24) {
		/* if less than 24MHz, set to 24MHz */
		clk_freq = 24;
	}
	
	/* set clock source to OSC24M */
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 16, 2, 0x01);

	if(clk_freq == 24) {
		return 24 * 1000 * 1000;
	}
	else if(clk_freq <= 744) {
		k = 0;
		axi_div = 0;
	}else if(clk_freq <= 1488) {
		k = 1;
		axi_div = 1;
		clk_freq >>= 1;
	}else if(clk_freq <= 2000) {
		k = 2;
		axi_div = 2;
		clk_freq >>= 2;
	}

	n = (clk_freq/24) & 0x1F;
	
	sr32(SUNXI_CCM_PLL1_CFG, 8, 5, n);				/* PLL1_FACTOR_N [12:8] */
	sr32(SUNXI_CCM_PLL1_CFG, 4, 2, k);				/* PLL1_FACTOR_K [5:4] */
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 0, 2, axi_div);	/* AXI_CLK_DIV_RATIO [1:0] */
	
	/* wait for clock to be stable*/	
	sdelay(0x4000);
	/* change cpu clock source to pll1 */
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 16, 2, 2);		/* CPU_CLK_SRC_SEL [17:16] */
	
	return 24 * 1000 * 1000 * n * (k + 1);
}
