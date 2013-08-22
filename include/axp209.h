/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
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

extern int axp209_set_dcdc2(int mvolt);
extern int axp209_set_dcdc3(int mvolt);
extern int axp209_set_ldo2(int mvolt);
extern int axp209_set_ldo3(int mvolt);
extern int axp209_set_ldo4(int mvolt);
extern void axp209_poweroff(void);
extern int axp209_init(void);
extern int axp209_poweron_by_dc(void);
extern int axp209_power_button(void);
