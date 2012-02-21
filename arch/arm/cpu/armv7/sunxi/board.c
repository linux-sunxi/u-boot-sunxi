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
#include <asm/arch/dram.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/early_print.h>

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
sunxi_boot_type_t boot_from(void) {

	u32 cfg;

	cfg = sunxi_gpio_get_cfgpin(SUNXI_GPC(7));
	if( cfg == SUNXI_GPC7_SDC2_CLK )
		return SUNXI_BOOT_TYPE_MMC2;

	cfg = sunxi_gpio_get_cfgpin(SUNXI_GPC(2));
	if( cfg == SUNXI_GPC2_NCLE )
		return SUNXI_BOOT_TYPE_NAND;

	cfg = sunxi_gpio_get_cfgpin(SUNXI_GPF(2));
	if( cfg == SUNXI_GPF2_SDC0_CLK )
		return SUNXI_BOOT_TYPE_MMC2;

	/* if we are here, something goes wrong */
	return SUNXI_BOOT_TYPE_NULL;
}

int watchdog_init(void) {

	struct sunxi_wdog *wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;
	/* disable watchdog */
	writel(0, &(wdog->mode));

	return 0;
}

int gpio_init(void) {

	/* config uart pin */
	sunxi_gpio_set_cfgpin(SUNXI_GPB(22), SUNXI_GPB22_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(23), SUNXI_GPB23_UART0_RX);

	return 0;
}

u32 get_base(void) {

	u32 val;

	__asm__ __volatile__("mov %0, pc \n":"=r"(val)::"memory");
	val &= 0xF0000000;
	val >>= 28;
	return val;
}

u32 is_running_in_sram(void) {

	if (get_base() == 0)
		return 1;	/* in SRAM */

	return 0;		/* running in SDRAM */
}

/* do some early init */
void s_init(void) {

	int in_sram = is_running_in_sram();

	watchdog_init();
#ifndef CONFIG_SPL_BUILD
	sunxi_key_init();
#endif
	clock_init();
	gpio_init();
	uart0_init();

	if (in_sram) {
		uart0_puts("init dram\n");
		sunxi_dram_init();
	}
}

extern int sunxi_reset(void);
void reset_cpu(ulong addr) {

	sunxi_reset();
}

#ifndef CONFIG_SYS_DCACHE_OFF
void enable_caches(void) {

	/* Enable D-cache. I-cache is already enabled in start.S */
	dcache_enable();
}
#endif

#ifdef CONFIG_SPL_BUILD
void save_boot_params(u32 r0, u32 r1, u32 r2, u32 r3) {}

void board_init_f(unsigned long bootflag)
{
	/*
	 * We call relocate_code() with relocation target same as the
	 * CONFIG_SYS_SPL_TEXT_BASE. This will result in relocation getting
	 * skipped. Instead, only .bss initialization will happen. That's
	 * all we need
	 */
	uart0_puts("board_init_f\n");
	relocate_code(CONFIG_SPL_STACK, &gdata, CONFIG_SPL_TEXT_BASE);
}

/* Place Holders */
void board_init_r(gd_t *id, ulong dest_addr)
{
	gd = &gdata;
	gd->bd = &bdata;
	gd->flags |= GD_FLG_RELOC;

	uart0_puts("board_init_r\n");
}
#endif
