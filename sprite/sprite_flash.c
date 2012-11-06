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

#include <config.h>
#include <common.h>

#define  SUNXI_SPRITE_FORMAT_RAW         0
#define  SUNXI_SPRITE_FORMAT_SPARSE      1

#define  SUNXI_SPRITE_DRAM_OP_BASE       CONFIG_SYS_SDRAM_BASE
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void sunxi_sprite_probe_part_data_format(void *buffer)
{
	int format;

	if(sparse_format_detect(buffer))
	{
		puts("part format: format\n");
		;
	}
	else
	{
		puts("part format: raw\n");
		;
	}

	return ;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/


