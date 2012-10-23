/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * (C) Copyright 2011
 * Texas Instruments, <www.ti.com>
 * Author: Vikram Pandita <vikram.pandita@ti.com>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <config.h>
#include <common.h>
#include <sparse_format.h>
#include "sparse.h"

#define   SPARSE_FORMAT_TYPE_TOTAL_HEAD       0xff00
#define   SPARSE_FORMAT_TYPE_CHUNK_HEAD       0xff01
#define   SPARSE_FORMAT_TYPE_CHUNK_DATA       0xff02


static uint  android_format_checksum;
static uint  sparse_format_type;
static uint  chunk_count;
static int  last_rest_size;
static int  chunk_length;
static uint  flash_start;
static sparse_header_t globl_header;
/*
************************************************************************************************************
*
*                                             unsparse_probe
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static uint add_sum(void *pbuf, uint len)
{
	//确保len是4字节对齐的，不然就需要重新计算
	uint sum = 0;
	uint *addr = (uint *)pbuf;
	uint data_len = len;
	uint rest;

    rest = len & 0x03;
	data_len = (len & (~0x03))>>2;

	while(data_len --)
	{
		sum += *addr ++;
	}

    if(rest == 0)
    {
    	;
    }
	else if(rest == 1)
	{
		sum += (*addr) & 0xff;
	}
	else if(rest == 2)
	{
		sum += (*addr) & 0xffff;
	}
	else if(rest == 3)
	{
		sum += (*addr) & 0xffffff;
	}

	return sum;
}
/*
************************************************************************************************************
*
*                                             unsparse_probe
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int unsparse_probe(char *source, uint length, uint android_format_flash_start)
{
	sparse_header_t *header = (sparse_header_t*) source;

	if (header->magic != SPARSE_HEADER_MAGIC)
	{
		printf("sparse: bad magic\n");

		return -1;
	}

	if ((header->major_version != SPARSE_HEADER_MAJOR_VER) ||
	    (header->file_hdr_sz != sizeof(sparse_header_t)) ||
	    (header->chunk_hdr_sz != sizeof(chunk_header_t)))
	{
		printf("sparse: incompatible format\n");

		return ANDROID_FORMAT_BAD;
	}
	android_format_checksum  = 0;
	last_rest_size = 0;
	chunk_count = 0;
	chunk_length = 0;
	sparse_format_type = SPARSE_FORMAT_TYPE_TOTAL_HEAD;
	flash_start = android_format_flash_start;

	return ANDROID_FORMAT_DETECT;
}
/*
************************************************************************************************************
*
*                                             DRAM_Write
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int  unsparse_direct_write(void *pbuf, void *next_buf, uint length)
{
	int   unenough_length;
	int   this_rest_size;
	int   tmp_down_size;
	char *tmp_buf, *tmp_dest_buf;
	chunk_header_t   *chunk;

    //首先计算传进的数据的校验和
	android_format_checksum += add_sum(pbuf, length);

    this_rest_size = last_rest_size + length;
    tmp_buf = (char *)pbuf - last_rest_size;
	last_rest_size = 0;

    while(this_rest_size > 0)
    {
		switch(sparse_format_type)
		{
			case SPARSE_FORMAT_TYPE_TOTAL_HEAD:
			{
				memcpy(&globl_header, tmp_buf, sizeof(sparse_header_t));
            	this_rest_size -= sizeof(sparse_header_t);
            	tmp_buf += sizeof(sparse_header_t);

                sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_HEAD;

				break;
			}
			case SPARSE_FORMAT_TYPE_CHUNK_HEAD:
			{
				if(this_rest_size < sizeof(chunk_header_t))
				{
					printf("sparse: chunk head data is not enough\n");
					last_rest_size = this_rest_size;
					tmp_dest_buf = (char *)next_buf - this_rest_size;
		    		memcpy(tmp_dest_buf, tmp_buf, this_rest_size);
					this_rest_size = 0;

		    		break;
				}
				chunk = (chunk_header_t *)tmp_buf;
				/* move to next chunk */
				tmp_buf += sizeof(chunk_header_t);        //此时tmp_buf已经指向下一个chunk或者data起始地址
				this_rest_size -= sizeof(chunk_header_t); //剩余的数据长度
				chunk_length = chunk->chunk_sz * globl_header.blk_sz;   //当前数据块需要写入的数据长度
				printf("chunk index = %d\n", chunk_count ++);

				switch (chunk->chunk_type)
				{
					case CHUNK_TYPE_RAW:

						if (chunk->total_sz != (chunk_length + sizeof(chunk_header_t)))
						{
							printf("sparse: bad chunk size for chunk %d, type Raw\n", chunk_count);

							return -1;
						}
						//这里不处理数据部分，转到下一个状态
						sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_DATA;

						break;

					case CHUNK_TYPE_DONT_CARE:
						if (chunk->total_sz != sizeof(chunk_header_t))
						{
							printf("sparse: bogus DONT CARE chunk\n");

							return -1;
						}
						flash_start += (chunk_length>>9);
						sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_HEAD;

						break;

					default:
						printf("sparse: unknown chunk ID %x\n", chunk->chunk_type);

						return -1;
				}
				break;
			}
			case SPARSE_FORMAT_TYPE_CHUNK_DATA:
			{
				//首先判断数据是否足够当前chunk所需,如果不足，则计算出还需要的数据长度
				unenough_length = (chunk_length >= this_rest_size)? (chunk_length - this_rest_size):0;
				if(!unenough_length)
				{
					//数据足够，直接写入
					if(!sunxi_sprite_write(flash_start, chunk_length>>9, tmp_buf))
					{
						printf("sparse: flash write failed\n");

						return -1;
					}
					if(chunk_length & 511)
					{
						printf("data is not sector align 0\n");

						return -1;
					}
					flash_start += (chunk_length>>9);
					tmp_buf += chunk_length;
					this_rest_size -= chunk_length;
					chunk_length = 0;

					sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_HEAD;
				}
				else  //存在缺失数据的情况
				{
					if(this_rest_size < 8 * 1024) //先看已有数据是否不足8k
					{
						//当不足时，把这笔数据放到下一笔数据的前部，等待下一次处理
						tmp_dest_buf = (char *)next_buf - this_rest_size;
						memcpy(tmp_dest_buf, tmp_buf, this_rest_size);
                        last_rest_size = this_rest_size;
						this_rest_size = 0;

						break;
					}
					//当已有数据超过16k时
					//当缺失数据长度不足4k时,可能只缺几十个字节
					if(unenough_length < 4 * 1024)
					{
						//采用拼接方法，先烧写部分已有数据，然后在下一次把未烧写的已有数据和缺失数据一起烧录
						tmp_down_size = this_rest_size + unenough_length - 4 * 1024;
					}
					else //这里处理缺失数据超过8k(包含)的情况,同时已有数据也超过16k
					{
						//直接烧录当前全部数据;
						tmp_down_size = this_rest_size & (~(512 -1));  //扇区对齐
					}
					if(!sunxi_sprite_write(flash_start, tmp_down_size>>9, tmp_buf))
					{
						printf("sparse: flash write failed\n");

						return -1;
					}
					if(tmp_down_size & 511)
					{
						printf("data is not sector align 1\n");

						return -1;
					}
					tmp_buf += tmp_down_size;
					flash_start += (tmp_down_size>>9);
					chunk_length -= tmp_down_size;
					this_rest_size -= tmp_down_size;
					tmp_dest_buf = (char *)next_buf - this_rest_size;
					memcpy(tmp_dest_buf, tmp_buf, this_rest_size);
					last_rest_size = this_rest_size;
					this_rest_size = 0;

					sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_DATA;
				}

				break;
			}

			default:
			{
				printf("sparse: unknown status\n");

				return -1;
			}
		}
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             unsparse_checksum
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
uint unsparse_checksum(void)
{
	//return verify_sum(android_format_dram_base, android_format_size);
	return android_format_checksum;
}


