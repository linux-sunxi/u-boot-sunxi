/*
 * board.c
 *
 * Board functions for Phytec phyCORE-AM335x (pcm051) based boards
 *
 * Copyright (C) 2013 Lemonage Software GmbH
 * Author Lars Poeschel <poeschel@lemonage.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <errno.h>
#include <spl.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/omap.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sys_proto.h>
#include <asm/io.h>
#include <asm/emif.h>
#include <asm/gpio.h>
#include <i2c.h>
#include <miiphy.h>
#include <cpsw.h>
#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

static struct wd_timer *wdtimer = (struct wd_timer *)WDT_BASE;
#ifdef CONFIG_SPL_BUILD
static struct uart_sys *uart_base = (struct uart_sys *)DEFAULT_UART_BASE;
#endif

/* MII mode defines */
#define MII_MODE_ENABLE		0x0
#define RGMII_MODE_ENABLE	0xA
#define RMII_RGMII2_MODE_ENABLE	0x49

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

/* UART defines */
#ifdef CONFIG_SPL_BUILD
#define UART_RESET		(0x1 << 1)
#define UART_CLK_RUNNING_MASK	0x1
#define UART_SMART_IDLE_EN	(0x1 << 0x3)

/* DDR RAM defines */
#define DDR_CLK_MHZ		303 /* DDR_DPLL_MULT value */

static void rtc32k_enable(void)
{
	struct rtc_regs *rtc = (struct rtc_regs *)RTC_BASE;

	/*
	 * Unlock the RTC's registers.  For more details please see the
	 * RTC_SS section of the TRM.  In order to unlock we need to
	 * write these specific values (keys) in this order.
	 */
	writel(0x83e70b13, &rtc->kick0r);
	writel(0x95a4f1e0, &rtc->kick1r);

	/* Enable the RTC 32K OSC by setting bits 3 and 6. */
	writel((1 << 3) | (1 << 6), &rtc->osc);
}

static const struct ddr_data ddr3_data = {
	.datardsratio0 = MT41J256M8HX15E_RD_DQS,
	.datawdsratio0 = MT41J256M8HX15E_WR_DQS,
	.datafwsratio0 = MT41J256M8HX15E_PHY_FIFO_WE,
	.datawrsratio0 = MT41J256M8HX15E_PHY_WR_DATA,
	.datadldiff0 = PHY_DLL_LOCK_DIFF,
};

static const struct cmd_control ddr3_cmd_ctrl_data = {
	.cmd0csratio = MT41J256M8HX15E_RATIO,
	.cmd0dldiff = MT41J256M8HX15E_DLL_LOCK_DIFF,
	.cmd0iclkout = MT41J256M8HX15E_INVERT_CLKOUT,

	.cmd1csratio = MT41J256M8HX15E_RATIO,
	.cmd1dldiff = MT41J256M8HX15E_DLL_LOCK_DIFF,
	.cmd1iclkout = MT41J256M8HX15E_INVERT_CLKOUT,

	.cmd2csratio = MT41J256M8HX15E_RATIO,
	.cmd2dldiff = MT41J256M8HX15E_DLL_LOCK_DIFF,
	.cmd2iclkout = MT41J256M8HX15E_INVERT_CLKOUT,
};

static struct emif_regs ddr3_emif_reg_data = {
	.sdram_config = MT41J256M8HX15E_EMIF_SDCFG,
	.ref_ctrl = MT41J256M8HX15E_EMIF_SDREF,
	.sdram_tim1 = MT41J256M8HX15E_EMIF_TIM1,
	.sdram_tim2 = MT41J256M8HX15E_EMIF_TIM2,
	.sdram_tim3 = MT41J256M8HX15E_EMIF_TIM3,
	.zq_config = MT41J256M8HX15E_ZQ_CFG,
	.emif_ddr_phy_ctlr_1 = MT41J256M8HX15E_EMIF_READ_LATENCY |
				PHY_EN_DYN_PWRDN,
};
#endif

/*
 * early system init of muxing and clocks.
 */
void s_init(void)
{
	/*
	 * WDT1 is already running when the bootloader gets control
	 * Disable it to avoid "random" resets
	 */
	writel(0xAAAA, &wdtimer->wdtwspr);
	while (readl(&wdtimer->wdtwwps) != 0x0)
		;
	writel(0x5555, &wdtimer->wdtwspr);
	while (readl(&wdtimer->wdtwwps) != 0x0)
		;

#ifdef CONFIG_SPL_BUILD
	/* Setup the PLLs and the clocks for the peripherals */
	pll_init();

	/* Enable RTC32K clock */
	rtc32k_enable();

	/* UART softreset */
	u32 regval;

	enable_uart0_pin_mux();

	regval = readl(&uart_base->uartsyscfg);
	regval |= UART_RESET;
	writel(regval, &uart_base->uartsyscfg);
	while ((readl(&uart_base->uartsyssts) &	UART_CLK_RUNNING_MASK)
		!= UART_CLK_RUNNING_MASK)
		;

	/* Disable smart idle */
	regval = readl(&uart_base->uartsyscfg);
	regval |= UART_SMART_IDLE_EN;
	writel(regval, &uart_base->uartsyscfg);

	gd = &gdata;

	preloader_console_init();

	/* Initalize the board header */
	enable_i2c0_pin_mux();
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	enable_board_pin_mux();

	config_ddr(DDR_CLK_MHZ, MT41J256M8HX15E_IOCTRL_VALUE, &ddr3_data,
			&ddr3_cmd_ctrl_data, &ddr3_emif_reg_data, 0);
#endif
}

/*
 * Basic board specific setup.  Pinmux has been handled already.
 */
int board_init(void)
{
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	gd->bd->bi_boot_params = PHYS_DRAM_1 + 0x100;

	return 0;
}

#ifdef CONFIG_DRIVER_TI_CPSW
static void cpsw_control(int enabled)
{
	/* VTP can be added here */

	return;
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_id		= 0,
		.phy_if		= PHY_INTERFACE_MODE_RGMII,
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,
		.phy_id		= 1,
		.phy_if		= PHY_INTERFACE_MODE_RGMII,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 1,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};
#endif

#if defined(CONFIG_DRIVER_TI_CPSW) || \
	(defined(CONFIG_USB_ETHER) && defined(CONFIG_MUSB_GADGET))
int board_eth_init(bd_t *bis)
{
	int rv, n = 0;
#ifdef CONFIG_DRIVER_TI_CPSW
	uint8_t mac_addr[6];
	uint32_t mac_hi, mac_lo;

	if (!eth_getenv_enetaddr("ethaddr", mac_addr)) {
		printf("<ethaddr> not set. Reading from E-fuse\n");
		/* try reading mac address from efuse */
		mac_lo = readl(&cdev->macid0l);
		mac_hi = readl(&cdev->macid0h);
		mac_addr[0] = mac_hi & 0xFF;
		mac_addr[1] = (mac_hi & 0xFF00) >> 8;
		mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
		mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
		mac_addr[4] = mac_lo & 0xFF;
		mac_addr[5] = (mac_lo & 0xFF00) >> 8;

		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("ethaddr", mac_addr);
		else
			goto try_usbether;
	}

	writel(RMII_RGMII2_MODE_ENABLE, &cdev->miisel);

	rv = cpsw_register(&cpsw_data);
	if (rv < 0)
		printf("Error %d registering CPSW switch\n", rv);
	else
		n += rv;
try_usbether:
#endif

#if defined(CONFIG_USB_ETHER) && !defined(CONFIG_SPL_BUILD)
	rv = usb_eth_initialize(bis);
	if (rv < 0)
		printf("Error %d registering USB_ETHER\n", rv);
	else
		n += rv;
#endif
	return n;
}
#endif
