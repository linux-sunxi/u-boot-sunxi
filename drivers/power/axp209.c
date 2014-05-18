/*
 * (C) Copyright 2012
 * Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <axp209.h>

enum axp209_reg {
	AXP209_POWER_STATUS = 0x00,
	AXP209_CHIP_VERSION = 0x03,
	AXP209_DCDC2_VOLTAGE = 0x23,
	AXP209_DCDC3_VOLTAGE = 0x27,
	AXP209_LDO24_VOLTAGE = 0x28,
	AXP209_LDO3_VOLTAGE = 0x29,
	AXP209_IRQ_STATUS3 = 0x4a,
	AXP209_IRQ_STATUS5 = 0x4c,
	AXP209_SHUTDOWN = 0x32,
};

#define AXP209_POWER_STATUS_ON_BY_DC	(1<<0)

#define AXP209_IRQ3_PEK_SHORT		(1<<1)
#define AXP209_IRQ3_PEK_LONG		(1<<0)

#define AXP209_IRQ5_PEK_UP		(1<<6)
#define AXP209_IRQ5_PEK_DOWN		(1<<5)

int axp209_write(enum axp209_reg reg, u8 val)
{
	return i2c_write(0x34, reg, 1, &val, 1);
}

int axp209_read(enum axp209_reg reg, u8 *val)
{
	return i2c_read(0x34, reg, 1, val, 1);
}

int axp209_set_dcdc2(int mvolt)
{
	int cfg = (mvolt - 700) / 25;
	int rc;
	u8 current;

	if (cfg < 0)
		cfg = 0;
	if (cfg > (1 << 6) - 1)
		cfg = (1 << 6) - 1;

	/* Do we really need to be this gentle? It has built-in voltage slope */
	while ((rc = axp209_read(AXP209_DCDC2_VOLTAGE, &current)) == 0 &&
	       current != cfg) {
		if (current < cfg)
			current++;
		else
			current--;

		rc = axp209_write(AXP209_DCDC2_VOLTAGE, current);
		if (rc)
			break;
	}

	return rc;
}

int axp209_set_dcdc3(int mvolt)
{
	int cfg = (mvolt - 700) / 25;
	u8 reg;
	int rc;

	if (cfg < 0)
		cfg = 0;
	if (cfg > (1 << 7) - 1)
		cfg = (1 << 7) - 1;

	rc = axp209_write(AXP209_DCDC3_VOLTAGE, cfg);
	rc |= axp209_read(AXP209_DCDC3_VOLTAGE, &reg);

	return rc;
}

int axp209_set_ldo2(int mvolt)
{
	int cfg = (mvolt - 1800) / 100;
	int rc;
	u8 reg;

	if (cfg < 0)
		cfg = 0;
	if (cfg > 15)
		cfg = 15;

	rc = axp209_read(AXP209_LDO24_VOLTAGE, &reg);
	if (rc)
		return rc;

	reg = (reg & 0x0f) | (cfg << 4);
	rc = axp209_write(AXP209_LDO24_VOLTAGE, reg);
	if (rc)
		return rc;

	return 0;
}

int axp209_set_ldo3(int mvolt)
{
	int cfg = (mvolt - 700) / 25;

	if (cfg < 0)
		cfg = 0;
	if (cfg > 127)
		cfg = 127;
	if (mvolt == -1)
		cfg = 0x80;	/* detemined by LDO3IN pin */

	return axp209_write(AXP209_LDO3_VOLTAGE, cfg);
}

int axp209_set_ldo4(int mvolt)
{
	int cfg, rc;
	static const int vindex[] = {
		1250, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2500,
		2700, 2800, 3000, 3100, 3200, 3300
	};
	u8 reg;

	/* Translate mvolt to register cfg value, requested <= selected */
	for (cfg = 15; vindex[cfg] > mvolt && cfg > 0; cfg--);

	rc = axp209_read(AXP209_LDO24_VOLTAGE, &reg);
	if (rc)
		return rc;

	/* LDO4 configuration is in lower 4 bits */
	reg = (reg & 0xf0) | (cfg << 0);
	rc = axp209_write(AXP209_LDO24_VOLTAGE, reg);
	if (rc)
		return rc;

	return 0;
}

void axp209_poweroff(void)
{
	u8 val;

	if (axp209_read(AXP209_SHUTDOWN, &val) != 0)
		return;

	val |= 1 << 7;

	if (axp209_write(AXP209_SHUTDOWN, val) != 0)
		return;

	udelay(10000);		/* wait for power to drain */
}

int axp209_init(void)
{
	u8 ver;
	int rc;

	rc = axp209_read(AXP209_CHIP_VERSION, &ver);
	if (rc)
		return rc;

	/* Low 4 bits is chip version */
	ver &= 0x0f;

	if (ver != 0x1)
		return -1;

	return 0;
}

int axp209_poweron_by_dc(void)
{
	u8 v;

	if (axp209_read(AXP209_POWER_STATUS, &v))
		return 0;
	return (v & AXP209_POWER_STATUS_ON_BY_DC);
}

int axp209_power_button(void)
{
	u8 v;

	if (axp209_read(AXP209_IRQ_STATUS5, &v))
		return 0;
	axp209_write(AXP209_IRQ_STATUS5, AXP209_IRQ5_PEK_DOWN);
	return v & AXP209_IRQ5_PEK_DOWN;
}
