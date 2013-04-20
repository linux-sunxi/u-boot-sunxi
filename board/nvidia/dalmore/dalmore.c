/*
 * Copyright (c) 2010-2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/gp_padctrl.h>
#include "pinmux-config-dalmore.h"
#include <i2c.h>

#define BAT_I2C_ADDRESS		0x48	/* TPS65090 charger */
#define PMU_I2C_ADDRESS		0x58	/* TPS65913 PMU */

/*
 * Routine: pinmux_init
 * Description: Do individual peripheral pinmux configs
 */
void pinmux_init(void)
{
	pinmux_config_table(tegra114_pinmux_set_nontristate,
		ARRAY_SIZE(tegra114_pinmux_set_nontristate));

	pinmux_config_table(tegra114_pinmux_common,
		ARRAY_SIZE(tegra114_pinmux_common));

	pinmux_config_table(unused_pins_lowpower,
		ARRAY_SIZE(unused_pins_lowpower));

	/* Initialize any non-default pad configs (APB_MISC_GP regs) */
	padgrp_config_table(dalmore_padctrl, ARRAY_SIZE(dalmore_padctrl));
}

#if defined(CONFIG_TEGRA_MMC)
/*
 * Do I2C/PMU writes to bring up SD card bus power
 *
 */
void board_sdmmc_voltage_init(void)
{
	uchar reg, data_buffer[1];
	int ret;

	ret = i2c_set_bus_num(0);/* PMU is on bus 0 */
	if (ret)
		printf("%s: i2c_set_bus_num returned %d\n", __func__, ret);

	/* TPS65913: LDO9_VOLTAGE = 3.3V */
	data_buffer[0] = 0x31;
	reg = 0x61;

	ret = i2c_write(PMU_I2C_ADDRESS, reg, 1, data_buffer, 1);
	if (ret)
		printf("%s: PMU i2c_write %02X<-%02X returned %d\n",
			__func__, reg, data_buffer[0], ret);

	/* TPS65913: LDO9_CTRL = Active */
	data_buffer[0] = 0x01;
	reg = 0x60;

	ret = i2c_write(PMU_I2C_ADDRESS, reg, 1, data_buffer, 1);
	if (ret)
		printf("%s: PMU i2c_write %02X<-%02X returned %d\n",
			__func__, reg, data_buffer[0], ret);

	/* TPS65090: FET6_CTRL = enable output auto discharge, enable FET6 */
	data_buffer[0] = 0x03;
	reg = 0x14;

	ret = i2c_write(BAT_I2C_ADDRESS, reg, 1, data_buffer, 1);
	if (ret)
		printf("%s: BAT i2c_write %02X<-%02X returned %d\n",
			__func__, reg, data_buffer[0], ret);
}

/*
 * Routine: pin_mux_mmc
 * Description: setup the MMC muxes, power rails, etc.
 */
void pin_mux_mmc(void)
{
	/*
	 * NOTE: We don't do mmc-specific pin muxes here.
	 * They were done globally in pinmux_init().
	 */

	/* Bring up the SDIO3 power rail */
	board_sdmmc_voltage_init();
}
#endif /* MMC */
