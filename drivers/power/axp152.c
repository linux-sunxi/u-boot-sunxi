/*
 * (C) Copyright 2012
 * Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <i2c.h>
#include <axp152.h>

enum axp152_reg {
	AXP152_CHIP_VERSION = 0x3,
	AXP152_DCDC2_VOLTAGE = 0x23,
	AXP152_DCDC3_VOLTAGE = 0x27,
	AXP152_DCDC4_VOLTAGE = 0x2B,
	AXP152_LDO2_VOLTAGE = 0x2A,
	AXP152_SHUTDOWN = 0x32,
};

int axp152_write(enum axp152_reg reg, u8 val)
{
	return i2c_write(0x30, reg, 1, &val, 1);
}

int axp152_read(enum axp152_reg reg, u8 *val)
{
	return i2c_read(0x30, reg, 1, val, 1);
}

int axp152_set_dcdc2(int mvolt)
{
	int target = (mvolt - 700) / 25;
	int rc;
	u8 current;

	if (target < 0)
		target = 0;
	if (target > (1<<6)-1)
		target = (1<<6)-1;
	/* Do we really need to be this gentle? It has built-in voltage slope */
	while ((rc = axp152_read(AXP152_DCDC2_VOLTAGE, &current)) == 0 &&
	       current != target) {
		if (current < target)
			current++;
		else
			current--;
		rc = axp152_write(AXP152_DCDC2_VOLTAGE, current);
		if (rc)
			break;
	}
	return rc;
}

int axp152_set_dcdc3(int mvolt)
{
	int target = (mvolt - 700) / 50;
	u8 reg;
	int rc;

	if (target < 0)
		target = 0;
	if (target > (1<<6)-1)
		target = (1<<6)-1;
	rc = axp152_write(AXP152_DCDC3_VOLTAGE, target);
	rc |= axp152_read(AXP152_DCDC3_VOLTAGE, &reg);
	return rc;
}

int axp152_set_dcdc4(int mvolt)
{
	int target = (mvolt - 700) / 25;
	u8 reg;
	int rc;

	if (target < 0)
		target = 0;
	if (target > (1<<7)-1)
		target = (1<<7)-1;
	rc = axp152_write(AXP152_DCDC4_VOLTAGE, target);
	rc |= axp152_read(AXP152_DCDC4_VOLTAGE, &reg);
	return rc;
}

int axp152_set_ldo2(int mvolt)
{
	int target = (mvolt - 700) / 100;
	int rc;
	u8 reg;

	if (target < 0)
		target = 0;
	if (target > 31)
		target = 31;
	rc = axp152_write(AXP152_LDO2_VOLTAGE, target);
	rc |= axp152_read(AXP152_LDO2_VOLTAGE, &reg);
	return rc;
}

void axp152_poweroff(void)
{
	u8 val;

	if (axp152_read(AXP152_SHUTDOWN, &val) != 0)
		return;
	val |= 1 << 7;
	if (axp152_write(AXP152_SHUTDOWN, val) != 0)
		return;
	udelay(10000);	/* wait for power to drain */
}

int axp152_init(void)
{
	u8 ver;
	int rc;

	rc = axp152_read(AXP152_CHIP_VERSION, &ver);
	if (rc)
		return rc;
	if (ver != 0x05)
		return -1;
	return 0;
}
