/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some init for sunxi platform.
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
#include <asm/arch/timer.h>
#include <asm/arch/gpio.h>
#include <asm/arch/key.h>
#include <asm/arch/sys_proto.h>

int watchdog_init(void)
{
	struct sunxi_wdog *wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;
	/* disable watchdog */
	writel(0, &(wdog->mode));

	return 0;
}

int clock_init(void)
{
	/* set clock source to OSC24M */
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 16, 2, CPU_CLK_SRC_OSC24M);		/* CPU_CLK_SRC_SEL [17:16] */

	/* set the pll1 factors, pll1 out = 24MHz*n*k/m/p */	

	sr32(SUNXI_CCM_PLL1_CFG, 8, 5, PLL1_FACTOR_N);		/* PLL1_FACTOR_N [12:8] */
	sr32(SUNXI_CCM_PLL1_CFG, 4, 2, PLL1_FACTOR_K);		/* PLL1_FACTOR_K [5:4] */
	sr32(SUNXI_CCM_PLL1_CFG, 0, 2, PLL1_FACTOR_M);		/* PLL1_FACTOR_M [1:0] */
	sr32(SUNXI_CCM_PLL1_CFG, 16, 2, PLL1_FACTOR_P);		/* PLL1_FACTOR_P [17:16] */

	/* wait for clock to be stable*/	
	sdelay(0x4000);
	/* set clock divider, cpu:axi:ahb:apb0 = 8:4:2:1 */
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 0, 2, AXI_DIV);	/* AXI_CLK_DIV_RATIO [1:0] */
#ifdef CONFIG_SUN5I
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 6, 2, AHB_CLK_SRC_AXI);/* AHB_CLK_SRC [7:6] */
#endif
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 4, 2, AHB_DIV);	/* AHB_CLK_DIV_RATIO [5:4] */
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 9, 2, APB0_DIV);	/* APB0_CLK_DIV_RATIO [9:8] */

	/* change cpu clock source to pll1 */
	sr32(SUNXI_CCM_CPU_AHB_APB0_CFG, 16, 2, CPU_CLK_SRC_PLL1);/* CPU_CLK_SRC_SEL [17:16] */
	/* 
	 * if the clock source is changed,
	 * at most wait for 8 present running clock cycles
	 */
	sdelay(10);

	/* config apb1 clock */
	sr32(SUNXI_CCM_APB1_CLK_DIV, 24, 2, APB1_CLK_SRC_OSC24M);
	sr32(SUNXI_CCM_APB1_CLK_DIV, 16, 2, APB1_FACTOR_N);
	sr32(SUNXI_CCM_APB1_CLK_DIV, 0, 5, APB1_FACTOR_M);

	/* open the clock for uart0 */
	sr32(SUNXI_CCM_APB1_GATING, 16, 1, CLK_GATE_OPEN);

	/* config nand clock */
	sr32(SUNXI_CCM_NAND_SCLK_CFG, 24, 2, NAND_CLK_SRC_OSC24);
	sr32(SUNXI_CCM_NAND_SCLK_CFG, 16, 2, NAND_CLK_DIV_N);
	sr32(SUNXI_CCM_NAND_SCLK_CFG, 0, 4, NAND_CLK_DIV_M);
	sr32(SUNXI_CCM_NAND_SCLK_CFG, 31, 1, CLK_GATE_OPEN);
	/* open clock for nand */
	sr32(SUNXI_CCM_AHB_GATING0, 13, 1, CLK_GATE_OPEN);

	return 0;
}

void gpio_init()
{
	u32 i;
	static struct sunxi_gpio *gpio_c =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[SUNXI_GPIO_C];

	/* set nand controller pin */
	for(i=0; i < 4; i++) {
		writel(0x22222222, &gpio_c->cfg[i]);
	}
	writel(0x55555555, &gpio_c->drv[0]);
	writel(0x15555, &gpio_c->drv[1]);
#ifdef UART_PINS_TO_SD
	/* disable GPB22,23 as uart0 tx,rx */
	sunxi_gpio_set_cfgpin(SUNXI_GPB(22), SUNXI_GPIO_INPUT);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(23), SUNXI_GPIO_INPUT);
	/* set GPF2,4 as uart0 tx,rx */
	sunxi_gpio_set_cfgpin(SUNXI_GPF(2), SUNXI_GPF2_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPF(4), SUNXI_GPF4_UART0_RX);
#endif
}

/* do some early init */
void s_init(void)
{
	watchdog_init();
	sunxi_key_init();
	clock_init();
	gpio_init();
}

void reset_cpu(ulong addr)
{
	sunxi_nand_flush_opts();
	sunxi_reset();
}

#ifndef CONFIG_SYS_DCACHE_OFF
void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
	dcache_enable();
}
#endif
