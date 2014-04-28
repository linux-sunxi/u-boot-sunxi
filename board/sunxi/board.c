/*
 * (C) Copyright 2012-2013 Henrik Nordstrom <henrik@henriknordstrom.net>
 * (C) Copyright 2013 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some board init for the Allwinner A10-evb board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#ifdef CONFIG_AXP152_POWER
#include <axp152.h>
#endif
#ifdef CONFIG_AXP209_POWER
#include <axp209.h>
#endif
#ifdef CONFIG_AXP221_POWER
#include <axp221.h>
#endif
#include <asm/arch/clock.h>
#include <asm/arch/dram.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>

DECLARE_GLOBAL_DATA_PTR;

/* add board specific code here */
int board_init(void)
{
	int id_pfr1;

	gd->bd->bi_boot_params = (PHYS_SDRAM_0 + 0x100);

	asm volatile("mrc p15, 0, %0, c0, c1, 1" : "=r"(id_pfr1));
	debug("id_pfr1: 0x%08x\n", id_pfr1);
	/* Generic Timer Extension available? */
	if ((id_pfr1 >> 16) & 0xf) {
		debug("Setting CNTFRQ\n");
		/* CNTFRQ == 24 MHz */
		asm volatile("mcr p15, 0, %0, c14, c0, 0" : : "r"(24000000));
	}

#ifdef CONFIG_STATUS_LED
	status_led_set(STATUS_LED_BOOT, STATUS_LED_ON);
#endif
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("Board: %s\n", CONFIG_SYS_BOARD_NAME);

	return 0;
}
#endif

int dram_init(void)
{
	gd->ram_size = get_ram_size((unsigned long *)PHYS_SDRAM_0, PHYS_SDRAM_0_SIZE);

	return 0;
}

#ifdef CONFIG_GENERIC_MMC
static void mmc_pinmux_setup(int sdc)
{
	unsigned int pin;

	switch (sdc) {
	case 0:
		/* D1-PF0, D0-PF1, CLK-PF2, CMD-PF3, D3-PF4, D4-PF5 */
		for (pin = SUNXI_GPF(0); pin <= SUNXI_GPF(5); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPF0_SDC0);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
		break;

	case 1:
#if CONFIG_MMC1_PG
		/* PG0-CMD, PG1-CLK, PG2~5-D0~3 : 4 */
		for (pin = SUNXI_GPG(0); pin <= SUNXI_GPG(5); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUN4I_GPG0_SDC1);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#else
		/* PH22-CMD, PH23-CLK, PH24~27-D0~D3 : 5 */
		for (pin = SUNXI_GPH(22); pin <= SUNXI_GPH(27); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUN4I_GPH22_SDC1);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#endif
		break;

	case 2:
		/* CMD-PC6, CLK-PC7, D0-PC8, D1-PC9, D2-PC10, D3-PC11 */
		for (pin = SUNXI_GPC(6); pin <= SUNXI_GPC(11); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPC6_SDC2);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
		break;

	case 3:
		/* PI4-CMD, PI5-CLK, PI6~9-D0~D3 : 2 */
		for (pin = SUNXI_GPI(4); pin <= SUNXI_GPI(9); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUN4I_GPI4_SDC3);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
		break;

	default:
		printf("sunxi: invalid MMC slot %d for pinmux setup\n", sdc);
		break;
	}
}

int board_mmc_init(bd_t *bis)
{
	mmc_pinmux_setup(CONFIG_MMC_SUNXI_SLOT);
	sunxi_mmc_init(CONFIG_MMC_SUNXI_SLOT);
#if !defined (CONFIG_SPL_BUILD) && defined (CONFIG_MMC_SUNXI_SLOT_EXTRA)
	mmc_pinmux_setup(CONFIG_MMC_SUNXI_SLOT_EXTRA);
	sunxi_mmc_init(CONFIG_MMC_SUNXI_SLOT_EXTRA);
#endif

	return 0;
}
#endif

#if defined(CONFIG_SPL_BUILD) || defined(CONFIG_SUN6I) || defined(CONFIG_SUN8I)
void sunxi_board_init(void)
{
	int power_failed = 0;
#if !defined(CONFIG_SUN6I) && !defined(CONFIG_SUN8I)
	unsigned long ramsize;

	printf("DRAM:");
	ramsize = sunxi_dram_init();
	printf(" %lu MiB\n", ramsize >> 20);
	if (!ramsize)
		hang();
#endif

#ifdef CONFIG_AXP152_POWER
	power_failed = axp152_init();
	power_failed |= axp152_set_dcdc2(1400);
	power_failed |= axp152_set_dcdc3(1500);
	power_failed |= axp152_set_dcdc4(1250);
	power_failed |= axp152_set_ldo2(3000);
#endif
#ifdef CONFIG_AXP209_POWER
	power_failed |= axp209_init();
	power_failed |= axp209_set_dcdc2(1400);
#ifdef CONFIG_FAST_MBUS
	power_failed |= axp209_set_dcdc3(1300);
#else
	power_failed |= axp209_set_dcdc3(1250);
#endif
	power_failed |= axp209_set_ldo2(3000);
	power_failed |= axp209_set_ldo3(2800);
	power_failed |= axp209_set_ldo4(2800);
#endif
#ifdef CONFIG_AXP221_POWER
	power_failed = axp221_init();
	power_failed |= axp221_set_dcdc1(3300);
	power_failed |= axp221_set_dcdc2(1200);
	power_failed |= axp221_set_dcdc3(1260);
	power_failed |= axp221_set_dcdc4(1200);
	power_failed |= axp221_set_dcdc5(1500);
#ifdef CONFIG_ENABLE_DLDO1_POWER
	power_failed |= axp221_set_dldo1(3300);
#endif
#endif

#if !defined(CONFIG_SUN6I) && !defined(CONFIG_SUN8I)
	/*
	 * Only clock up the CPU to full speed if we are reasonably
	 * assured it's being powered with suitable core voltage
	 */
	if (!power_failed)
#ifdef CONFIG_SUN7I
		clock_set_pll1(912000000);
#else
		clock_set_pll1(1008000000);
#endif
	else
		printf("Failed to set core voltage! Can't set CPU frequency\n");
#endif
}
#endif

#if defined(CONFIG_SPL_OS_BOOT) && defined(CONFIG_AXP209_POWER)
int spl_start_uboot(void)
{
	if (axp209_poweron_by_dc())
		return 0;
	axp209_power_button(); /* Clear any pending button event */
	mdelay(100);
	return axp209_power_button();
}
#endif

#ifdef CONFIG_SPL_DISPLAY_PRINT
void spl_display_print(void)
{
	printf("Board: %s\n", CONFIG_SYS_BOARD_NAME);
}
#endif
