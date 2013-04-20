/*
 * Copyright (c) 2010-2012, NVIDIA CORPORATION.  All rights reserved.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/tegra.h>
#include <asm/arch/pinmux.h>
#include <asm/gpio.h>

#ifdef CONFIG_TEGRA_MMC
/*
 * Routine: pin_mux_mmc
 * Description: setup the pin muxes/tristate values for the SDMMC(s)
 */
void pin_mux_mmc(void)
{
	/* SDMMC4: config 3, x8 on 2nd set of pins */
	pinmux_set_func(PINGRP_ATB, PMUX_FUNC_SDIO4);
	pinmux_set_func(PINGRP_GMA, PMUX_FUNC_SDIO4);
	pinmux_set_func(PINGRP_GME, PMUX_FUNC_SDIO4);

	pinmux_tristate_disable(PINGRP_ATB);
	pinmux_tristate_disable(PINGRP_GMA);
	pinmux_tristate_disable(PINGRP_GME);

	/* SDIO1: SDIO1_CLK, SDIO1_CMD, SDIO1_DAT[3:0] */
	pinmux_set_func(PINGRP_SDIO1, PMUX_FUNC_SDIO1);

	pinmux_tristate_disable(PINGRP_SDIO1);

	/* For power GPIO PV1 */
	pinmux_tristate_disable(PINGRP_UAC);
	/* For CD GPIO PV5 */
	pinmux_tristate_disable(PINGRP_GPV);
}
#endif

#ifdef CONFIG_LCD
/* this is a weak define that we are overriding */
void pin_mux_display(void)
{
	debug("init display pinmux\n");

	/* EN_VDD_PANEL GPIO A4 */
	pinmux_tristate_disable(PINGRP_DAP2);
}
#endif
