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
#ifndef  __SUNXI_SPRITE_DOWNLOAD_H__
#define  __SUNXI_SPRITE_DOWNLOAD_H__


#include <config.h>
#include <common.h>

int sunxi_sprite_download_raw_init(uint flash_part_start);

int sunxi_sprite_download_raw(void *buffer, void *next_buffer, uint sectors);

int sunxi_sprite_download_uboot(void *buffer, int production_media, char *storage_info);

int sunxi_sprite_download_boot0(void *buffer, int production_media, char *storage_info);


#endif

