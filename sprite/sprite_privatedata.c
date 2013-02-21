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
#include <asm/arch/sunxi_mbr.h>

#define  SUNXI_SPRITE_PROTECT_DATA_BASE   (0x41000000)
#define  SUNXI_SPRITE_PROTECT_DATA_MAX    (16)

struct sunxi_private_data_store
{
	char  *addr;
	char   name[16];
	uint   sectors;
	int    reserved[2];
};

static struct sunxi_private_data_store   pdata[SUNXI_SPRITE_PROTECT_DATA_MAX];
static int  pdata_count = 0;
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
int sunxi_sprite_store_part_data(sunxi_mbr_t  *mbr)
{
	int i,index;

	index = 1;
	memset(pdata, 0, SUNXI_SPRITE_PROTECT_DATA_MAX * sizeof(struct sunxi_private_data_store));
	pdata[0].addr = (char *)SUNXI_SPRITE_PROTECT_DATA_BASE;

	for(i=0;i<mbr->PartCount;i++)
	{
		if((mbr->array[i].ro) || (!strcmp((const char *)mbr->array[i].name, "private")))
		{
			if(!sunxi_sprite_read(mbr->array[i].addrlo, mbr->array[i].lenlo, pdata[index].addr))
			{
				printf("sunxi sprite error : read private data error\n");

				return -1;
			}
			strncpy(pdata[index].name, (const char *)mbr->array[i].name, 16);
			pdata[index].sectors = mbr->array[i].lenlo;
			index ++;
			pdata[index].addr = pdata[index-1].addr + pdata[index-1].sectors*512;
			if(index > 15)
			{
				printf("sunxi sprite error : too mauch protect data. much than 15 parts\n");

				return -1;
			}
		}
	}
	pdata_count = index;

	return 0;
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
int sunxi_sprite_restore_part_data(sunxi_mbr_t  *mbr)
{
	int i,j;

	if(pdata_count <= 0)
	{
		return 0;
	}
	for(i=0;i<mbr->PartCount;i++)
	{
		if(mbr->array[i].ro)
		{
			for(j=0;j<pdata_count;j++)
			{
				if(!strcmp((const char*)pdata[j].name, (const char*)mbr->array[i].name))
				{
					if(!sunxi_flash_write(mbr->array[i].addrlo, pdata[j].sectors, pdata[j].addr))
					{
						printf("sunxi sprite error : write private data error\n");

						return -1;
					}
					break;
				}
			}
		}
	}

	return 0;
}



