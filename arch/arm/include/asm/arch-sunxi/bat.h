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
#ifndef __SUNXI_BAT_H__
#define __SUNXI_BAT_H__

#define   BATTERY_RATIO_DEFAULT                          0
#define   BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN             1
#define   BATTERY_RATIO_TOO_LOW_WITH_DCIN                2
#define   BATTERY_RATIO_ENOUGH                           3


typedef struct
{
	int x;
	int y;
	int bit;
	void *buffer;
}
sunxi_bmp_store_t;

extern int sunxi_bmp_decode(unsigned long addr, sunxi_bmp_store_t *bmp_info);

#endif  /* __SUNXI_BAT_H__ */

