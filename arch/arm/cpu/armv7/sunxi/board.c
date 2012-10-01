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
#include <version.h>
#include <mmc.h>
#include <fat.h>

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
		return SUNXI_BOOT_TYPE_MMC0;

	/* if we are here, something goes wrong */
	return SUNXI_BOOT_TYPE_NULL;
}

#define UART0_PINS_TO_SD 0

int gpio_init(void) {
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
void s_init(void) {
#ifdef CONFIG_WATCHDOG
	watchdog_init();
#endif
	clock_init();
	gpio_init();

#ifdef CONFIG_SPL_BUILD
	sunxi_dram_init();
#endif
}

extern void sunxi_reset(void);
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

inline void hang(void)
{
	puts("\n### ERROR ### Please RESET the board ###\n");
	for (;;)
		;
}

void board_init_f(unsigned long bootflag)
{
	/*
	 * We call relocate_code() with relocation target same as the
	 * CONFIG_SYS_SPL_TEXT_BASE. This will result in relocation getting
	 * skipped. Instead, only .bss initialization will happen. That's
	 * all we need
	 */
	relocate_code(CONFIG_SPL_STACK, &gdata, CONFIG_SPL_TEXT_BASE);
}

void board_init_r(gd_t *id, ulong dest_addr)
{
	__attribute__((noreturn)) void (*uboot)(void);
	struct mmc *mmc;
	int err;

	gd = &gdata;
	gd->bd = &bdata;
	gd->flags |= GD_FLG_RELOC;
	gd->baudrate = CONFIG_BAUDRATE;

	timer_init();
	serial_init();

	gd->have_console = 1;

	printf("\nU-Boot SPL %s (%s - %s)\n", PLAIN_VERSION, U_BOOT_DATE,
		U_BOOT_TIME);

	puts("MMC:   ");
	mmc_initialize(gd->bd);
	/* We register only one device. So, the dev id is always 0 */
	mmc = find_mmc_device(0);
	if (!mmc) {
		puts("spl: mmc device not found!!\n");
		hang();
	}

	err = mmc_init(mmc);
	if (err) {
		printf("spl: mmc init failed: err - %d\n", err);
		hang();
	}

	puts("Loading U-Boot...   ");

	err = mmc->block_dev.block_read(CONFIG_MMC_SUNXI_SLOT,
			CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR,
			CONFIG_SYS_U_BOOT_MAX_SIZE_SECTORS,
			(uchar *)CONFIG_SYS_TEXT_BASE);

	if(err == CONFIG_SYS_U_BOOT_MAX_SIZE_SECTORS) {
		puts("OK!\n");
	} else {
		hang();
	}

	puts("Jumping to U-Boot...\n");
	/* Jump to U-Boot image */
	uboot = (void *)CONFIG_SYS_TEXT_BASE;
	(*uboot)();
	/* Never returns Here */
}
#endif
