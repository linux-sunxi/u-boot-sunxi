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

	struct sunxi_ccm_reg *ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

#ifdef CONFIG_SPL_BUILD
	/* Hardcode clock config for now */
	ccm->cpu_ahb_apb0_cfg = 0x00010010;
	ccm->pll1_cfg = 0xa1005000;
	sdelay(200);
	sr32(&ccm->cpu_ahb_apb0_cfg, 16, 2, CPU_CLK_SRC_PLL1);/* CPU_CLK_SRC_SEL [17:16] */

	/* dram clock setup is in dram.c */
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
