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

#define  VERIFY_ONCE_DATA_LENGTH    (512 * 1024)

struct sunxi_private_data_store
{
	char  *addr;
	char   name[16];
	uint   sectors;
	int    reserved[2];
};

static struct sunxi_private_data_store   pdata[16];
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
int sunx_sprite_store_part_data(sunxi_mbr_t  *mbr)
{
	int i,index;

	index = 1;
	memset(pdata, 0, 16 * sizeof(struct sunxi_private_data_store));
	pdata[0].addr = (char *)VERIFY_ONCE_DATA_LENGTH;

	for(i=0;i<mbr->PartCount;i++)
	{
		if(mbr->array[i].ro)
		{
			pdata[index].addr = pdata[index -1].addr + pdata[index -1].sectors*512;
			if(!sunxi_flash_read(mbr->array[i].addrlo, mbr->array[i].lenlo, pdata[index].addr))
			{
				printf("sunxi sprite error : read private data error\n");

				return -1;
			}
			strncpy(pdata[index].name, (const char *)mbr->array[i].name, 16);
			pdata[index].sectors = mbr->array[i].lenlo;
			index ++;
			if(index > 15)
			{
				printf("sunxi sprite error : too mauch private parts\n");

				return -1;
			}
		}
	}
	pdata_count = index - 1;

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
int sunx_sprite_restore_part_data(sunxi_mbr_t  *mbr)
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
			for(j=1;j<pdata_count + 1;j++)
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



