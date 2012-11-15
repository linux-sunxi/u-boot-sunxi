/*
 * (C) Copyright 2007-2012
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
#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sys_proto.h>

int clock_init(void) {
#ifndef CONFIG_SUN6I_FPGA
	struct sunxi_ccm_reg *ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

/* pll1
 *       \          2:1           2:1           2:1
 *         cpu-clk ----> axi-clk ----> ahb-clk ----> apb0-clk
 *       /
 * osc24m
 */

	/* set cpu clock source to OSC24M */
	sr32(&ccm->cpu_ahb_apb0_cfg, 16, 2, CPU_CLK_SRC_OSC24M);		/* CPU_CLK_SRC_SEL [17:16] */

	/* set the pll1 factors, pll1 out = 24MHz*n*k/m/p */
	sr32(&ccm->pll1_cfg, 8, 5, PLL1_FACTOR_N);		/* PLL1_FACTOR_N [12:8] */
	sr32(&ccm->pll1_cfg, 4, 2, PLL1_FACTOR_K);		/* PLL1_FACTOR_K [5:4] */
	sr32(&ccm->pll1_cfg, 0, 2, PLL1_FACTOR_M);		/* PLL1_FACTOR_M [1:0] */
	sr32(&ccm->pll1_cfg, 16, 2, PLL1_FACTOR_P);		/* PLL1_FACTOR_P [17:16] */

	/* set clock divider, cpu:axi:ahb:apb0 = 8:4:2:1 */
	sr32(&ccm->cpu_ahb_apb0_cfg, 0, 2, AXI_DIV);	/* AXI_CLK_DIV_RATIO [1:0] */
#ifdef CONFIG_SUN5I
	sr32(&ccm->cpu_ahb_apb0_cfg, 6, 2, AHB_CLK_SRC_AXI);/* AHB_CLK_SRC [7:6] */
#endif
	sr32(&ccm->cpu_ahb_apb0_cfg, 4, 2, AHB_DIV);	/* AHB_CLK_DIV_RATIO [5:4] */
	sr32(&ccm->cpu_ahb_apb0_cfg, 9, 2, APB0_DIV);	/* APB0_CLK_DIV_RATIO [9:8] */

	/* enable pll1 */
	sr32(&ccm->pll1_cfg, 31, 1, PLL1_ENABLE);		/* PLL1_ENABLE [31] */
	sdelay(0x1000);

	/* change cpu clock source to pll1 */
	sr32(&ccm->cpu_ahb_apb0_cfg, 16, 2, CPU_CLK_SRC_PLL1);/* CPU_CLK_SRC_SEL [17:16] */
	/*
	 * if the clock source is changed,
	 * at most wait for 8 present running clock cycles
	 */
	sdelay(10);

	/* uart clock source is apb1 */
	sr32(&ccm->apb1_clk_div_cfg, 24, 2, APB1_CLK_SRC_OSC24M);
	sr32(&ccm->apb1_clk_div_cfg, 16, 2, APB1_FACTOR_N);
	sr32(&ccm->apb1_clk_div_cfg, 0, 5, APB1_FACTOR_M);
	/* open the clock for uart0 */
	sr32(&ccm->apb1_gate, 16, 1, CLK_GATE_OPEN);


#ifdef CONFIG_SPL_BUILD
	/* ddr clock source is pll5 */
	sr32(&ccm->pll5_cfg, 29, 1, DDR_CLK_OUT_DISABLE);
	sr32(&ccm->pll5_cfg, 0, 2, PLL5_FACTOR_M);
	sr32(&ccm->pll5_cfg, 4, 2, PLL5_FACTOR_K);
	sr32(&ccm->pll5_cfg, 8, 5, PLL5_FACTOR_N);
	sr32(&ccm->pll5_cfg, 16, 2, PLL5_OUT_DIV_P);
	sr32(&ccm->pll5_cfg, 31, 1, PLL5_ENABLE);
	sdelay(0x100000);
	sr32(&ccm->pll5_cfg, 29, 1, DDR_CLK_OUT_ENABLE);

	/* if we don't reset the gps module, it will access sdram
	 * but sdram is not ready, and the system will die...
	 */
	sr32(&ccm->gps_clk_cfg, 0, 1, GPS_SCLK_GATING_OFF);
	sr32(&ccm->gps_clk_cfg, 1, 1, GPS_RESET);
	sr32(&ccm->ahb_gate0, AHB_GATE_OFFSET_GPS, 1, CLK_GATE_OPEN);
	sdelay(0x100);
	sr32(&ccm->ahb_gate0, AHB_GATE_OFFSET_GPS, 1, CLK_GATE_CLOSE);

	sr32(&ccm->ahb_gate0, AHB_GATE_OFFSET_SDRAM, 1, CLK_GATE_CLOSE);
	sdelay(0x1000);
	sr32(&ccm->ahb_gate0, AHB_GATE_OFFSET_SDRAM, 1, CLK_GATE_OPEN);
	sdelay(0x1000);
#endif

#if 0
	/* nand clock source is osc24m */
	sr32(&ccm->nand_sclk_cfg, 24, 2, NAND_CLK_SRC_OSC24);
	sr32(&ccm->nand_sclk_cfg, 16, 2, NAND_CLK_DIV_N);
	sr32(&ccm->nand_sclk_cfg, 0, 4, NAND_CLK_DIV_M);
	sr32(&ccm->nand_sclk_cfg, 31, 1, CLK_GATE_OPEN);
	/* open clock for nand */
	sr32(&ccm->ahb_gate0, AHB_GATE_OFFSET_NAND, 1, CLK_GATE_OPEN);
#endif
#endif  /* CONFIG_SUN6I_FPGA */
	return 0;
}
