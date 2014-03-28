/*
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

int clock_init(void)
{
#ifdef CONFIG_SPL_BUILD
	clock_init_safe();
#endif
	clock_init_uart();

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

unsigned int clock_get_pll6(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll6_cfg);
	int n = (rval >> 8) & 0x1f;
	int k = ((rval >> 4) & 3) + 1;
	return 24000000 * n * k / 2;
}
