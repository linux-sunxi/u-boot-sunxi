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

#ifndef  __SUNXI_SPRITE_VERIFY_H__
#define  __SUNXI_SPRITE_VERIFY_H__

#include <common.h>


uint sunxi_sprite_part_rawdata_verify(uint base_start, uint base_sectors);

uint sunxi_sprite_part_sparsedata_verify(void);

uint sunxi_sprite_generate_checksum(void *buffer, uint length, uint src_sum);

int sunxi_sprite_verify_checksum(void *buffer, uint length, uint src_sum);

#endif

