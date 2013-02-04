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
#ifndef  __SUNXI_BAT_CARTOON_H__
#define  __SUNXI_BAT_CARTOON_H__

#define  SUNXI_BAT_BMP_MAX    (11)

extern int battery_charge_cartoon_init(void);

extern int battery_charge_cartoon_exit(void);

extern int battery_charge_cartoon_rate(int rate);

extern int battery_charge_cartoon_reset(void);

extern int battery_charge_cartoon_degrade(int alpha_step);


#endif  //__SUNXI_BAT_CARTOON_H__
