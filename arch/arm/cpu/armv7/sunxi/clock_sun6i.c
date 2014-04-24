/*
 * sun6i specific clock code
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
#include <asm/arch/prcm.h>
#include <asm/arch/sys_proto.h>

#ifdef CONFIG_SPL_BUILD
void clock_init_safe(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_prcm_reg * const prcm =
		(struct sunxi_prcm_reg *)SUNXI_PRCM_BASE;

	/* Set PLL ldo voltage without this PLL6 does not work properly */
	writel(PRCM_PLL_CTRL_LDO_DIGITAL_EN | PRCM_PLL_CTRL_LDO_ANALOG_EN |
		PRCM_PLL_CTRL_EXT_OSC_EN | PRCM_PLL_CTRL_LDO_OUT_L(1140) |
		PRCM_PLL_CTRL_LDO_KEY, &prcm->pll_ctrl1);
	writel(PRCM_PLL_CTRL_LDO_DIGITAL_EN | PRCM_PLL_CTRL_LDO_ANALOG_EN |
		PRCM_PLL_CTRL_EXT_OSC_EN | PRCM_PLL_CTRL_LDO_OUT_L(1140) |
		PRCM_PLL_CTRL_LDO_KEY, &prcm->pll_ctrl1);
	writel(PRCM_PLL_CTRL_LDO_DIGITAL_EN | PRCM_PLL_CTRL_LDO_ANALOG_EN |
		PRCM_PLL_CTRL_EXT_OSC_EN | PRCM_PLL_CTRL_LDO_OUT_L(1140),
		&prcm->pll_ctrl1);

	/* AXI and PLL1 settings from boot0 / boot1, PLL1 set to 486 Mhz */
	writel(AXI_DIV_3 << AXI_DIV_SHIFT |
	       ATB_DIV_2 << ATB_DIV_SHIFT |
	       CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT,
	       &ccm->cpu_axi_cfg);
	writel(PLL1_CFG_DEFAULT, &ccm->pll1_cfg);
	sdelay(200);
	writel(AXI_DIV_3 << AXI_DIV_SHIFT |
	       ATB_DIV_2 << ATB_DIV_SHIFT |
	       CPU_CLK_SRC_PLL1 << CPU_CLK_SRC_SHIFT,
	       &ccm->cpu_axi_cfg);

	writel(PLL6_CFG_DEFAULT, &ccm->pll6_cfg);
}
#endif

void clock_init_uart(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

#if CONFIG_CONS_INDEX < 5
	/* uart clock source is apb2 */
	writel(APB2_CLK_SRC_OSC24M|
	       APB2_CLK_RATE_N_1|
	       APB2_CLK_RATE_M(1),
	       &ccm->apb2_div);

	/* open the clock for uart */
	setbits_le32(&ccm->apb2_gate,
		CLK_GATE_OPEN << (APB2_GATE_UART_SHIFT+CONFIG_CONS_INDEX-1));

	/* deassert uart reset */
	setbits_le32(&ccm->apb2_reset_cfg,
		1 << (APB2_RESET_UART_SHIFT+CONFIG_CONS_INDEX-1));
#else
	/* enable R_PIO and R_UART clocks, and de-assert resets */
	prcm_apb0_enable(PRCM_APB0_GATE_PIO | PRCM_APB0_GATE_UART);
#endif

	/* Dup with clock_init_safe(), drop once sun6i SPL support lands */
	writel(PLL6_CFG_DEFAULT, &ccm->pll6_cfg);
}

int clock_twi_onoff(int port, int state)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	if (port > 3)
		return -1;

	/* set the apb clock gate for twi */
	if (state)
		setbits_le32(&ccm->apb2_gate,
			     CLK_GATE_OPEN << (APB2_GATE_TWI_SHIFT+port));
	else
		clrbits_le32(&ccm->apb2_gate,
			     CLK_GATE_OPEN << (APB2_GATE_TWI_SHIFT+port));

	return 0;
}

unsigned int clock_get_pll6(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll6_cfg);
	int n = ((rval & CCM_PLL6_CTRL_N_MASK) >> CCM_PLL6_CTRL_N_SHIFT) + 1;
	int k = ((rval & CCM_PLL6_CTRL_K_MASK) >> CCM_PLL6_CTRL_K_SHIFT) + 1;
	return 24000000 * n * k / 2;
}
