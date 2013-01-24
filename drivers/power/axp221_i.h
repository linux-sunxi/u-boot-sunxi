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
#ifndef   __AXP221_I_H__
#define   __AXP221_I_H__

extern	int axp221_set_charge_control(void);
extern	int axp221_clear_data_buffer(void);
extern  int axp221_probe_dcin_exist(void);
extern  int axp221_probe_battery_ratio(void);
extern  int axp221_probe_battery_exist(void);
extern  int axp221_probe_battery_vol(void);
extern  int axp221_probe_buttery_resistance_record(void);
extern  int axp221_probe_key(void);
extern  int axp221_probe_last_poweron_status(void);
extern  int axp221_set_next_poweron_status(int data);
extern  int axp221_write_data_buffer(int buffer, uchar value);
extern  int axp221_probe_poweron_cause(void);

extern  int axp221_set_dc1sw(int on_off);

extern  int axp221_set_dcdc1(int set_vol);
extern  int axp221_set_dcdc2(int set_vol);
extern  int axp221_set_dcdc3(int set_vol);
extern  int axp221_set_dcdc4(int set_vol);
extern  int axp221_set_dcdc5(int set_vol);
extern  int axp221_set_aldo1(int set_vol);
extern  int axp221_set_aldo2(int set_vol);
extern  int axp221_set_aldo3(int set_vol);

extern  int axp221_set_dldo1(int on_off);
extern  int axp221_set_dldo2(int on_off);
extern  int axp221_set_dldo3(int on_off);
extern  int axp221_set_dldo4(int on_off);

extern  int axp221_set_eldo1(int on_off);
extern  int axp221_set_eldo2(int on_off);
extern  int axp221_set_eldo3(int on_off);

extern  int axp221_set_gpio1ldo(int onoff, int set_vol);

extern  int axp221_set_power_off(void);
extern  int axp221_set_poweroff_vol(int set_vol);
extern  int axp221_set_charge_current(int current);
extern  int axp221_probe_charge_current(void);
extern  int axp221_probe_charge_status(void);
extern  int axp221_set_vbus_cur_limit(int current);
extern  int axp221_set_vbus_vol_limit(int vol);
extern  int axp221_set_chgcur(int cur);
extern  int axp221_probe_rest_battery_capacity(void);
extern  int axp221_int_query(unsigned char *addr);
extern  int axp221_int_enable_read(unsigned char *addr);
extern  int axp221_int_enable_write(unsigned char *addr);



#endif /* __AXP221_I_H__ */
