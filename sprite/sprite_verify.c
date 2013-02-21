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

#define  VERIFY_ONCE_BYTES    (512 * 1024)
#define  VERIFY_ONCE_SECTORS  (VERIFY_ONCE_BYTES/512)
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
uint add_sum(void *buffer, uint length)
{
	unsigned int *buf;
	unsigned int count;
	unsigned int sum;

	count = length >> 2;                         // 以 字（4bytes）为单位计数
	sum = 0;
	buf = (unsigned int *)buffer;
	while(count--)
	{
		sum += *buf++;                         // 依次累加，求得校验和
	};

	switch(length & 0x03)
	{
		case 0:
			return sum;
		case 1:
			sum += (*buf & 0x000000ff);
			break;
		case 2:
			sum += (*buf & 0x0000ffff);
			break;
		case 3:
			sum += (*buf & 0x00ffffff);
			break;
	}

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
uint sunxi_sprite_part_rawdata_verify(uint base_start, long long base_bytes)
{
	uint checksum;
	uint tail_bytes, last_time_bytes;
	uint tmp_sectors, start;
	char tmp_buf[VERIFY_ONCE_BYTES];

	tail_bytes  = base_bytes & 0x1ff;
	tmp_sectors = base_bytes/512;
    if(tail_bytes)
    {
    	tmp_sectors ++;
    }
    start   = base_start;
	debug("read total sectors %d\n", tmp_sectors);
	debug("read part start %d\n", base_start);
    checksum = 0;
    while(tmp_sectors > VERIFY_ONCE_SECTORS)
    {
    	if(sunxi_sprite_read(start, VERIFY_ONCE_SECTORS, tmp_buf) != VERIFY_ONCE_SECTORS)
    	{
    		printf("sunxi sprite: read flash error when verify\n");

    		return 0;
    	}
    	start       += VERIFY_ONCE_SECTORS;
    	tmp_sectors -= VERIFY_ONCE_SECTORS;
    	checksum    += add_sum(tmp_buf, VERIFY_ONCE_BYTES);
    	debug("check sum = 0x%x\n", checksum);
    }
    if(tmp_sectors)
    {
    	if(sunxi_sprite_read(start, tmp_sectors, tmp_buf) != tmp_sectors)
    	{
    		printf("sunxi sprite: read flash error when verify\n");

    		return 0;
    	}
    	if(tail_bytes)
    	{
    		last_time_bytes = (tmp_sectors - 1) * 512 + tail_bytes;
    	}
    	else
    	{
    		last_time_bytes = tmp_sectors * 512;
    	}
    	checksum += add_sum(tmp_buf, last_time_bytes);
		debug("check sum = 0x%x\n", checksum);
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


