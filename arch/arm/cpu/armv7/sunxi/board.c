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
#include <i2c.h>
#include <asm/gpio.h>
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sys_proto.h>
#include <netdev.h>
#ifdef CONFIG_SPL_BUILD
#include <spl.h>
#endif

#ifdef CONFIG_SPL_BUILD
/* Pointer to the global data structure for SPL */
DECLARE_GLOBAL_DATA_PTR;

/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * Unfortunately we can't check how SPL was loaded so assume
 * it's always the first SD/MMC controller
 */
u32 spl_boot_device(void) {
	return BOOT_DEVICE_MMC1;
}

/* No confiration data available in SPL yet. Hardcode bootmode */
u32 spl_boot_mode(void)
{
	return MMCSD_MODE_RAW;
}
#endif

int gpio_init(void)
{
#if CONFIG_CONS_INDEX == 1 && defined(CONFIG_UART0_PORT_F)
#ifdef CONFIG_SUN4I
	/* disable GPB22,23 as uart0 tx,rx to avoid conflict */
	gpio_direction_input(SUNXI_GPB(22));
	gpio_direction_input(SUNXI_GPB(23));
#endif
	sunxi_gpio_set_cfgpin(SUNXI_GPF(2), SUNXI_GPF2_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPF(4), SUNXI_GPF4_UART0_RX);
#elif CONFIG_CONS_INDEX == 1 && defined(CONFIG_SUN4I)
	sunxi_gpio_set_cfgpin(SUNXI_GPB(22), SUN4I_GPB22_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(23), SUN4I_GPB23_UART0_RX);
#elif CONFIG_CONS_INDEX == 2 && defined(CONFIG_SUN5I)
	sunxi_gpio_set_cfgpin(SUNXI_GPG(3), SUN5I_GPG3_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPG(4), SUN5I_GPG4_UART0_RX);
#else
#error Unsupported console port number. Please fix pin mux settings in board.c
#endif

	return 0;
}

/* do some early init */
void s_init(void)
{
#if defined(CONFIG_WATCHDOG) && defined(CONFIG_SUNXI_WATCHDOG)
	watchdog_set(23);	/* max possible timeout */
#endif
	clock_init();
	gpio_init();

#ifdef CONFIG_SPL_BUILD
	gd = &gdata;
	preloader_console_init();

#ifdef CONFIG_SPL_I2C_SUPPORT
	/* Needed early by sunxi_board_init if PMU is enabled */
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif

	sunxi_board_init();
#endif

}

extern void sunxi_reset(void);
void reset_cpu(ulong addr)
{
	sunxi_reset();
}

#ifndef CONFIG_SYS_DCACHE_OFF
void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
	dcache_enable();
}
#endif

#if defined(CONFIG_SUNXI_WEMAC)
/*
 * Initializes on-chip ethernet controllers.
 * to override, implement board_eth_init()
 */
int cpu_eth_init(bd_t *bis)
{
	sunxi_wemac_initialize(bis);

	return 0;
}
#endif
