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
#include "sparse/sparse.h"

#define  VERIFY_ONCE_DATA_LENGTH    (512 * 1024)
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
uint sunxi_sprite_part_rawdata_verify(uint base_start, long long base_length)
{
	uint checksum;
	uint sectors, start;
	uint onetime_sectors;
	uint rest_bytes;
	char tmp_buf[VERIFY_ONCE_DATA_LENGTH];
	uint *data;
	int  tmp_length;

    start  = base_start;
    sectors = base_length/512;
    rest_bytes = base_length%512;
    onetime_sectors = VERIFY_ONCE_DATA_LENGTH/512;
    if(sectors & 0x1ff)
    {
    	sectors ++;
    }
    checksum = 0;
    while(sectors >= onetime_sectors)
    {
    	if(!sunxi_sprite_read(start, onetime_sectors, tmp_buf))
    	{
    		printf("sunxi sprite: read flash error when verify\n");

    		return 0;
    	}
    	start  += onetime_sectors;
    	sectors -= onetime_sectors;
    	tmp_length = VERIFY_ONCE_DATA_LENGTH/4;
    	data = (uint *)tmp_buf;
    	while(tmp_length--)
    	{
    		checksum += *data++;
    	}
    }
    if(sectors)
    {
    	if(!sunxi_sprite_read(start, sectors, tmp_buf))
    	{
    		printf("sunxi sprite: read flash error when verify\n");

    		return 0;
    	}
    	tmp_length = sectors * 512/4 - rest_bytes;
    	data = (uint *)tmp_buf;
    	while(tmp_length--)
    	{
    		checksum += *data++;
    	}
	    switch(rest_bytes & 0x03)
		{
			case 1:
				checksum += (*data & 0x000000ff);
				break;
			case 2:
				checksum += (*data & 0x0000ffff);
				break;
			case 3:
				checksum += (*data & 0x00ffffff);
				break;
		}
    }

    return checksum;
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
uint sunxi_sprite_part_sparsedata_verify(void)
{
	return unsparse_checksum();
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
uint sunxi_sprite_generate_checksum(void *buffer, uint length, uint src_sum)
{
	uint           		  *buf;
	uint            	   loop;
	uint            	   i;
	uint            	   sum;

	sum = 0;
	if( ( length & 0x3 ) != 0 )                   // must 4-byte-aligned
		return 0;
	buf = (uint *)buffer;
	loop = length >> 2;
    /* 计算当前文件内容的“校验和”*/
    for( i = 0, sum = 0;  i < loop;  i++ )
    	sum += buf[i];

    sum = sum - src_sum + STAMP_VALUE;

    return sum;
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
int sunxi_sprite_verify_checksum(void *buffer, uint length, uint src_sum)
{
	uint *buf;
	uint count;
	uint sum;

	/* 生成校验和 */
	count = length >> 2;                       // 以 字（4bytes）为单位计数
	sum = 0;
	buf = (__u32 *)buffer;
	do
	{
		sum += *buf++;                         // 依次累加，求得校验和
		sum += *buf++;                         // 依次累加，求得校验和
		sum += *buf++;                         // 依次累加，求得校验和
		sum += *buf++;                         // 依次累加，求得校验和
	}while( ( count -= 4 ) > (4-1) );

	while( count-- > 0 )
		sum += *buf++;

	sum = sum - src_sum + STAMP_VALUE;

	debug("src sum=%x, check sum=%x\n", src_sum, sum);
	if( sum == src_sum )
		return 0;               // 校验成功
	else
		return -1;              // 校验失败
}


