/*
 * (C) Copyright 2012
 *     wangflord@allwinnerstech.com
 *
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
 * along with this program;
 *
 */
#ifndef   __AXP_H__
#define   __AXP_H__

#include <i2c.h>

#define  AXP_POWER_ON_BY_POWER_KEY       0
#define  AXP_POWER_ON_BY_POWER_TRIGGER   1


#define  AXP_POWER_NULL					 0
#define  AXP_POWER_PMU209				 1



static inline int axp_i2c_read(unsigned char chip, unsigned int addr, unsigned char *buffer)
{
	return i2c_read(chip, addr, 1, buffer, 1);
}

static inline int axp_i2c_write(unsigned char chip, unsigned int addr, unsigned char data)
{
	return i2c_write(chip, addr, 1, &data, 1);
}

static inline int abs(int x)
{
	return x>0?x:(-x);
}


extern int axp_clear_data_buffer(void);
extern int axp_reboot_coulomb(void);
extern int axp_probe_dcin_exist(void);
extern int axp_probe_battery_exist(void);
extern int axp_probe_battery_vol(void);
extern int axp_probe_shortkey(void);
extern int axp_probe_longkey(void);
extern int axp_probe_last_poweron_status(void);
extern int axp_set_power_off(void);
extern int axp_probe_bat_coulomb_count(void);
extern int axp_probe_poweron_cause(void);
extern int axp_probe_charge_current(void);
extern int axp_probe_charge_status(void);
extern int axp_probe_rest_battery_capacity(void);
extern int axp_set_charge_control(void);
extern int axp_probe_buttery_resistance_record(void);
extern int axp_set_next_poweron_status(int data);
extern int axp_set_dcdc2(int set_vol);
extern int axp_set_dcdc3(int set_vol);
extern int axp_set_dcdc4(int set_vol);
extern int axp_set_ldo2(int set_vol);
extern int axp_set_ldo3(int set_vol);
extern int axp_set_ldo4(int set_vol);
extern int axp_set_poweroff_vol(int set_vol);
extern int axp_set_charge_current(int current);
extern int axp_set_vbus_cur_limit(int current);
extern int axp_set_vbus_vol_limit(int vol);



#endif /* __AXP_H__ */
