/*
 * (C) Copyright 2012
 *     wangflord@allwinnertech.com
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

#ifndef _PMU_H_
#define _PMU_H_

#define PMU_SHORT_KEY_PRESSED      (1<<1)
#define PMU_LONG_KEY_PRESSED       (1<<0)


extern int axp_set_power_supply_output(void);
extern int axp_set_dcdc2(int set_vol);
extern int axp_set_dcdc3(int set_vol);
extern int axp_set_dcdc4(int set_vol);
extern int axp_set_dcdc5(int set_vol);

extern int axp_set_eldo3(int on_off);
extern int axp_set_dldo3(int on_off);

extern int axp_set_dc1sw(int on_off);

extern int axp_set_hardware_poweroff_vol(void);
extern int axp_set_hardware_poweron_vol(void);
extern int axp_probe_startup_cause(void);
extern int axp_probe_power_supply_condition(void);
extern int axp_set_power_off(void);
extern int axp_set_next_poweron_status(int data);


extern int axp_probe_dcin_exist(void);
extern int axp_probe_battery_exist(void);
extern int axp_probe_battery_vol(void);
extern int axp_probe_rest_battery_capacity(void);

extern int axp_probe_charge_current(void);
extern int axp_set_charge_control(void);
extern int axp_set_charge_current(int current);
extern int axp_set_vbus_cur_limit(int current);
extern int axp_set_vbus_vol_limit(int vol);

extern int axp_probe_key(void);

#endif	/* _PMU_H_ */
