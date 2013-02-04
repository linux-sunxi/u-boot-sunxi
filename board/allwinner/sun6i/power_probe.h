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
#ifndef  __SUNXI_POWER_PROBE_H__
#define  __SUNXI_POWER_PROBE_H__



extern int usb_detect_enter(void);

extern int usb_detect_exit(void);

extern void usb_detect_for_charge(int detect_time);

extern void power_limit_detect_enter(void);

extern void power_limit_detect_exit(void);


#endif   //__SUNXI_POWER_PROBE_H__



