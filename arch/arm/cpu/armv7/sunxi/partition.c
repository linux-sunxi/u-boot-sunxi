/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Allwinner private partition format definition.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <mmc.h>
#include <asm/arch/partition.h>

typedef struct crc32_data
{
	u32 crc;
	u32 table[256];
}crc32_data_t;

static char mbr_buf[1024];

static u32 calc_crc32(void * buffer, u32 length)
{
	u32 i, j;
	crc32_data_t crc32;
	u32 result = 0xffffffff; //设置初始值

	crc32.crc = 0;

	for( i = 0; i < 256; ++i)//用++i以提高效率
	{
		crc32.crc = i;
		for( j = 0; j < 8 ; ++j)
		{
			//这个循环实际上就是用"计算法"来求取crc的校验码
			if(crc32.crc & 1)
				crc32.crc = (crc32.crc >> 1) ^ 0xEDB88320;
			else //0xEDB88320就是crc-32多项表达式的值
				crc32.crc >>= 1;
		}
		crc32.table[i] = crc32.crc;
	}

    for( i = 0; i < length; ++i)
    {
        result = crc32.table[(result^((unsigned char*)buffer)[i]) & 0xff] ^ (result>>8);
    }

	return result^0xffffffff;
}

u32 sunxi_partition_get_total_num(void)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;

	return mbr->PartCount;
}

int sunxi_partition_get_name(int index, char *buf)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;

	strncpy(buf, (const char *)mbr->array[index].name, 12);
	return 0;
}

u64 sunxi_partition_get_offset(int part_index)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;

	if(part_index >= mbr->PartCount)
	{
		return -1;
	}

	return mbr->array[part_index].addrlo;
}

u64 sunxi_partition_get_size(int part_index)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;

	if(part_index >= mbr->PartCount)
	{
		return -1;
	}

	return mbr->array[part_index].lenlo;
}

u64 sunxi_partition_get_offset_byname(const char *part_name)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;
	int			i;

	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)mbr->array[i].name))
		{
			return mbr->array[i].addrlo;
		}
	}

	return -1;
}

u64 sunxi_partition_get_size_byname(const char *part_name)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;
	int			i;

	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)mbr->array[i].name))
		{
			return mbr->array[i].lenlo;
		}
	}

	return -1;
}

/* get the partition info, offset and size
 * input: partition name
 * output: part_offset and part_size (in byte)
 */
int sunxi_partition_get_info_byname(const char *part_name, loff_t *part_offset, loff_t *part_size)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;
	int			i;

	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)mbr->array[i].name))
		{
			*part_offset = mbr->array[i].addrlo;
			*part_size = mbr->array[i].lenlo;
			return 0;
		}
	}

	return -1;
}

int sunxi_partition_init(void)
{
    int i, part_index = 0;
    sunxi_mbr_t    *mbr;

    if(sunxi_flash_read(0, MBR_SIZE >> 9, mbr_buf)){
		printf("read flash error\n");

		return 0;
	}

	mbr = (sunxi_mbr_t*)mbr_buf;
   	if(!strncmp((const char*)mbr->magic, MBR_MAGIC, 8))
   	{
        int crc = 0;

        crc = calc_crc32(&mbr->version, MBR_SIZE-4);
        if(crc == mbr->crc32)
        {
            if(mbr->PartCount < 32)
            {
                PARTITION* pe = mbr->array;

                for(i=0; i<mbr->PartCount && i < MBR_MAX_PART_COUNT; i++, pe++)
                {
                    unsigned long long  len, addr;

                    addr = pe->addrhi;
                    addr = (addr << 32) | pe->addrlo;
                    len  = pe->lenhi;
                    len  = (len << 32) | pe->lenlo;

					if(!len)
					{
						len = sunxi_flash_get_size() - addr;
						pe->lenlo = len & 0xffffffff;
					}
                    part_index ++;
                }
            }
        }
   	}

	return part_index;
}
