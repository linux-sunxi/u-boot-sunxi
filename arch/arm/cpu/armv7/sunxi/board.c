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
#include <serial.h>
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>
#include <asm/arch/gpio.h>
#include <asm/arch/key.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/boot_type.h>
#include <asm/arch/ccmu.h>
#include <errno.h>
#include <netdev.h>
#ifdef CONFIG_SPL_BUILD
DECLARE_GLOBAL_DATA_PTR;

/* Define global data structure pointer to it*/
static gd_t gdata __attribute__ ((section(".data")));
static bd_t bdata __attribute__ ((section(".data")));
#endif

/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
int storage_type = 0;
int uart_console = 0;
int mmc_card_no  = 2;

#ifdef CONFIG_CMD_NET
extern int gmac_initialize(bd_t *bis);
int cpu_eth_init(bd_t *bis)
{
	int rc = -ENODEV;

	rc = gmac_initialize(bis);

	return rc;
}
#endif
sunxi_boot_type_t boot_from(void) {
	return SUNXI_BOOT_TYPE_NULL;
}

int watchdog_init(void) {

	struct sunxi_wdog *wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;
	/* disable watchdog */
	writel(0, &(wdog->mode));

	return 0;
}

int clock_init(void)
{
	/* set cpu clk and init uart  nand (or dram if possible) clk */
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
int sdram_init(void) {

	return 0;
}
	return 0;
}
/*
void clock_init_u_boot(void)
{
	ccm_setup_pll1_cpux_clk(CLK_PLL1);
	ccm_set_cpu_l2_axi_div(L2_PERIPH_DIV,L2_DIV,AXI_DIV);
	ccm_set_ahb1_clk_src(AHB1_CLK_SRC);
	ccm_set_apb2_clk(APB2_CLK);
	ccm_clock_enable(UART0_CKID);
	ccm_module_enable(UART0_CKID);	
}
*/
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
