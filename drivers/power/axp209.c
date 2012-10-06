/*
 * (C) Copyright 2012
 * Henrik Nordstrom <henrik@henriknordstrom.net>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <i2c.h>

enum {
	AXP209_CHIP_VERSION = 0x21,
	AXP209_DCDC2_VOLTAGE = 0x23,
	AXP209_DCDC3_VOLTAGE = 0x27,
	AXP209_SHUTDOWN = 0x32,
} axp209_reg;

int axp209_write(u8 reg, u8 val)
{
	return i2c_write(0x34, reg, 1, &val, 1);
}

int axp209_read(u8 reg, u8 *val)
{
	return i2c_read(0x34, reg, 1, val, 1);
}

void axp209_set_dcdc2(int mvol)
{
	int target = (mvol - 700) / 25;
	int rc;
	u8 current;
	if (target < 0)
		target = 0;
	if (target > (1<<6)-1)
		target = (1<<6)-1;
	/* Do we really need to be this gentle? It has built-in voltage slope */
	while ((rc = axp209_read(AXP209_DCDC2_VOLTAGE, &current)) == 0 && current != target)
	{
		if (current < target)
			current++;
		else
			current--;
		axp209_write(AXP209_DCDC2_VOLTAGE, current);
	}
	return;
}

void axp209_set_dcdc3(int mvol)
{
	int target = (mvol - 700) / 25;
	int rc;
	u8 current;
	if (target < 0)
		target = 0;
	if (target > (1<<7)-1)
		target = (1<<7)-1;
	axp209_write(AXP209_DCDC3_VOLTAGE, &current);
	axp209_read(AXP209_DCDC3_VOLTAGE, current);
	return;
}

void axp209_poweroff(void)
{
	u8 val;

	if (axp209_read(AXP209_SHUTDOWN, &val) != 0)
		return;
	val |= 1 << 7;
	axp209_write(AXP209_SHUTDOWN, val);
	udelay(10000);	/* wait for power to drain */
}

void axp209_init(void)
{
}
