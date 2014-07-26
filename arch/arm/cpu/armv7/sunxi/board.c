/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some init for sunxi platform.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <netdev.h>
#include <miiphy.h>
#include <serial.h>
#ifdef CONFIG_SPL_BUILD
#include <spl.h>
#endif
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/timer.h>
#include <asm/arch/watchdog.h>

#ifdef CONFIG_SPL_BUILD
/* Pointer to the global data structure for SPL */
DECLARE_GLOBAL_DATA_PTR;

/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nand flash, mmc2.
 * Unfortunately we can't check how SPL was loaded so assume
 * it's always the first SD/MMC controller
 */
u32 spl_boot_device(void)
{
	return BOOT_DEVICE_MMC1;
}

/* No confirmation data available in SPL yet. Hardcode bootmode */
u32 spl_boot_mode(void)
{
	return MMCSD_MODE_RAW;
}
#endif

int gpio_init(void)
{
#if CONFIG_CONS_INDEX == 1 && defined(CONFIG_UART0_PORT_F)
#if defined(CONFIG_SUN4I) || defined(CONFIG_SUN7I)
	/* disable GPB22,23 as uart0 tx,rx to avoid conflict */
	sunxi_gpio_set_cfgpin(SUNXI_GPB(22), SUNXI_GPIO_INPUT);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(23), SUNXI_GPIO_INPUT);
#endif
	sunxi_gpio_set_cfgpin(SUNXI_GPF(2), SUNXI_GPF2_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPF(4), SUNXI_GPF4_UART0_RX);
	sunxi_gpio_set_pull(SUNXI_GPF(4), 1);
#elif CONFIG_CONS_INDEX == 1 && (defined(CONFIG_SUN4I) || defined(CONFIG_SUN7I))
	sunxi_gpio_set_cfgpin(SUNXI_GPB(22), SUN4I_GPB22_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(23), SUN4I_GPB23_UART0_RX);
	sunxi_gpio_set_pull(SUNXI_GPB(23), 1);
#elif CONFIG_CONS_INDEX == 1 && defined(CONFIG_SUN6I)
	sunxi_gpio_set_cfgpin(SUNXI_GPH(20), 2);
	sunxi_gpio_set_cfgpin(SUNXI_GPH(21), 2);
	sunxi_gpio_set_pull(SUNXI_GPH(21), 1);
#elif CONFIG_CONS_INDEX == 1 && defined(CONFIG_SUN5I)
	sunxi_gpio_set_cfgpin(SUNXI_GPB(19), SUN5I_GPB19_UART0_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPB(20), SUN5I_GPB20_UART0_RX);
	sunxi_gpio_set_pull(SUNXI_GPB(20), 1);
#elif CONFIG_CONS_INDEX == 2 && defined(CONFIG_SUN5I)
	sunxi_gpio_set_cfgpin(SUNXI_GPG(3), SUN5I_GPG3_UART1_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPG(4), SUN5I_GPG4_UART1_RX);
	sunxi_gpio_set_pull(SUNXI_GPG(4), 1);
#elif CONFIG_CONS_INDEX == 5 && defined(CONFIG_SUN8I)
	sunxi_gpio_set_cfgpin(SUNXI_GPL(2), SUN8I_GPL2_R_UART_TX);
	sunxi_gpio_set_cfgpin(SUNXI_GPL(3), SUN8I_GPL3_R_UART_RX);
	sunxi_gpio_set_pull(SUNXI_GPL(3), 1);
#else
#error Unsupported console port number. Please fix pin mux settings in board.c
#endif

	return 0;
}

void reset_cpu(ulong addr)
{
	watchdog_set(0);
	while (1);
}

/* do some early init */
void s_init(void)
{
#if !defined CONFIG_SPL_BUILD && (defined CONFIG_SUN7I || defined CONFIG_SUN6I)
	/* Enable SMP mode for CPU0, by setting bit 6 of Auxiliary Ctl reg */
	asm volatile(
		"mrc p15, 0, r0, c1, c0, 1\n"
		"orr r0, r0, #1 << 6\n"
		"mcr p15, 0, r0, c1, c0, 1\n");
#endif

	watchdog_init();
	clock_init();
	timer_init();
	gpio_init();
	i2c_init_board();

#ifdef CONFIG_SPL_BUILD
	gd = &gdata;
	preloader_console_init();

#ifdef CONFIG_SPL_I2C_SUPPORT
	/* Needed early by sunxi_board_init if PMU is enabled */
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif
#endif
/* No SPL on sun6i, so we do sunxi_board_init() from non spl there */
#if defined(CONFIG_SPL_BUILD) || defined(CONFIG_SUN6I) || defined(CONFIG_SUN8I)
	sunxi_board_init();
#endif
}

#ifndef CONFIG_SYS_DCACHE_OFF
void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
	dcache_enable();
}
#endif

#ifdef CONFIG_CMD_NET
/*
 * Initializes on-chip ethernet controllers.
 * to override, implement board_eth_init()
 */
int cpu_eth_init(bd_t *bis)
{
	__maybe_unused int rc;

#ifdef CONFIG_MACPWR
	gpio_direction_output(CONFIG_MACPWR, 1);
	mdelay(200);
#endif

#ifdef CONFIG_SUNXI_EMAC
	rc = sunxi_emac_initialize(bis);
	if (rc < 0) {
		printf("sunxi: failed to initialize emac\n");
		return rc;
	}
#endif

#ifdef CONFIG_SUNXI_GMAC
	rc = sunxi_gmac_initialize(bis);
	if (rc < 0) {
		printf("sunxi: failed to initialize gmac\n");
		return rc;
	}
#endif

	return 0;
}
#endif
