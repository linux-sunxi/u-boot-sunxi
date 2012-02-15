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
#include <asm/arch/dram.h>
#include <asm/arch/sys_proto.h>

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

	u32 i;
	static struct sunxi_gpio *gpio_c =
		&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[SUNXI_GPIO_C];

	/* set nand controller pin */
	for(i=0; i < 4; i++) {
		writel(0x22222222, &gpio_c->cfg[i]);
	}
	writel(0x55555555, &gpio_c->drv[0]);
	writel(0x15555, &gpio_c->drv[1]);

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

	if (get_base() >= 4)
		return 1;	/* in SDRAM */

	return 0;		/* running in SRAM */
}

/* do some early init */
void s_init(void) {

	int in_sdram = is_running_in_sdram();

	watchdog_init();
	sunxi_key_init();
	clock_init();
	gpio_init();

	if (!in_sdram)
		sunxi_dram_init();
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
