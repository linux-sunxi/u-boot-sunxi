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

#ifdef  __SPRITE_PRIVATEDATA_H__
#define __SPRITE_PRIVATEDATA_H__

#include <asm/arch/partition.h>

extern int sunx_sprite_store_part_data(sunxi_mbr_t  *mbr);

extern int sunx_sprite_restore_part_data(sunxi_mbr_t  *mbr);


#endif





