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
#include <asm/arch/boot_type.h>
/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
int storage_type = 0;
int uart_console = 0;
int mmc_card_no  = 2;

int watchdog_init(void) {

	struct sunxi_wdog *wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;
	/* disable watchdog */
	writel(0, &(wdog->mode));

	return 0;
}

int clock_init(void) {
#if 0
	struct sunxi_ccm_reg *ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* set clock source to OSC24M */
	sr32(&ccm->cpu_ahb_apb0_cfg, 16, 2, CPU_CLK_SRC_OSC24M);		/* CPU_CLK_SRC_SEL [17:16] */

	/* set the pll1 factors, pll1 out = 24MHz*n*k/m/p */	
	sr32(&ccm->pll1_cfg, 8, 5, PLL1_FACTOR_N);		/* PLL1_FACTOR_N [12:8] */
	sr32(&ccm->pll1_cfg, 4, 2, PLL1_FACTOR_K);		/* PLL1_FACTOR_K [5:4] */
	sr32(&ccm->pll1_cfg, 0, 2, PLL1_FACTOR_M);		/* PLL1_FACTOR_M [1:0] */
	sr32(&ccm->pll1_cfg, 16, 2, PLL1_FACTOR_P);		/* PLL1_FACTOR_P [17:16] */

	/* wait for clock to be stable*/	
	sdelay(0x4000);
	/* set clock divider, cpu:axi:ahb:apb0 = 8:4:2:1 */
	sr32(&ccm->pll1_cfg, 0, 2, AXI_DIV);	/* AXI_CLK_DIV_RATIO [1:0] */
#ifdef CONFIG_SUN5I
	sr32(&ccm->pll1_cfg, 6, 2, AHB_CLK_SRC_AXI);/* AHB_CLK_SRC [7:6] */
#endif
	sr32(&ccm->pll1_cfg, 4, 2, AHB_DIV);	/* AHB_CLK_DIV_RATIO [5:4] */
	sr32(&ccm->pll1_cfg, 9, 2, APB0_DIV);	/* APB0_CLK_DIV_RATIO [9:8] */

	/* change cpu clock source to pll1 */
	sr32(&ccm->pll1_cfg, 16, 2, CPU_CLK_SRC_PLL1);/* CPU_CLK_SRC_SEL [17:16] */
	/* 
	 * if the clock source is changed,
	 * at most wait for 8 present running clock cycles
	 */
	sdelay(10);

	/* config apb1 clock */
	sr32(&ccm->apb1_clk_div_cfg, 24, 2, APB1_CLK_SRC_OSC24M);
	sr32(&ccm->apb1_clk_div_cfg, 16, 2, APB1_FACTOR_N);
	sr32(&ccm->apb1_clk_div_cfg, 0, 5, APB1_FACTOR_M);

	/* open the clock for uart0 */
	sr32((u32 *)SUNXI_CCM_APB1_GATING, 16, 1, CLK_GATE_OPEN);

	/* config nand clock */
	sr32((u32 *)SUNXI_CCM_NAND_SCLK_CFG, 24, 2, NAND_CLK_SRC_OSC24);
	sr32((u32 *)SUNXI_CCM_NAND_SCLK_CFG, 16, 2, NAND_CLK_DIV_N);
	sr32((u32 *)SUNXI_CCM_NAND_SCLK_CFG, 0, 4, NAND_CLK_DIV_M);
	sr32((u32 *)SUNXI_CCM_NAND_SCLK_CFG, 31, 1, CLK_GATE_OPEN);
	/* open clock for nand */
	sr32((u32 *)SUNXI_CCM_AHB_GATING0, 13, 1, CLK_GATE_OPEN);
#endif
	return 0;
}

void gpio_init(void) {
#if 0
	u32 i;
	static struct sunxi_gpio *gpio_c =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[SUNXI_GPIO_C];

	/* set nand controller pin */
	for(i=0; i < 4; i++) {
		writel(0x22222222, &gpio_c->cfg[i]);
	}
	writel(0x55555555, &gpio_c->drv[0]);
	writel(0x15555, &gpio_c->drv[1]);
#endif
}

int sdram_init(void) {

	return 0;
}

u32 get_base(void) {

	u32 val;

	__asm__ __volatile__("mov %0, pc \n":"=r"(val)::"memory");
	val &= 0xF0000000;
	val >>= 28;
	return val;
}

u32 is_running_in_sdram(void) {

	if (get_base() > 4)
		return 1;	/* in SDRAM */

	return 0;		/* running in SRAM */
}
extern void sw_gpio_init(void);
/* do some early init */
void s_init(void)
{
	int in_sdram = 0;
#ifdef CONFIG_SPL
		in_sdram = is_running_in_sdram();
#endif
	watchdog_init();
	sw_gpio_init();
	clock_init();
	gpio_init();
#ifdef CONFIG_SPL
	if (!in_sdram)
		sdram_init();
#endif
}

extern int sunxi_reset(void);
void reset_cpu(ulong addr) {
	sunxi_flash_exit();
	sunxi_reset();
}

#ifndef CONFIG_SYS_DCACHE_OFF
void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
#ifdef CONFIG_SPL
		dcache_enable();
#endif
}
#endif
