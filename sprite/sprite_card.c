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
#include <malloc.h>
#include "sparse/sparse.h"
#include <asm/arch/queue.h>
#include <asm/arch/sunxi_mbr.h>
#include "encrypt/encrypt.h"
#include "sprite_queue.h"
#include "sprite_download.h"
#include "sprite_verify.h"
#include "firmware/imgdecode.h"

#define  SPRITE_CARD_ONCE_DATA_DEAL    (512 * 1024)
#define  SPRITE_CARD_ONCE_SECTOR_DEAL  (SPRITE_CARD_ONCE_DATA_DEAL/512)

static void *imghd = NULL;
static void *imgitemhd = NULL;
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
uint sprite_card_firmware_start(void)
{
	return sunxi_partition_get_offset(1);
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
int sprite_card_firmware_probe(void)
{
	imghd = Img_Open("c:/imgfile");
	if(!imghd)
	{
		return -1;
	}

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
int sprite_card_fetch_download_map(sunxi_download_info  *dl_map)
{
	imgitemhd = Img_OpenItem(imghd, "12345678", "1234567890dlinfo");
	if(!imgitemhd)
	{
		return -1;
	}
	debug("try to read item dl map\n");
	if(!Img_ReadItem(imghd, imgitemhd, (void *)dl_map, sizeof(sunxi_download_info)))
	{
		printf("sunxi sprite error : read dl map failed\n");

		return -1;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

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

uint (* sunxi_sprite_decode)(void * ibuf, uint len);

int  (* sunxi_sprite_download_data)(void *this_buffer, void *next_buffer, uint len);

void *(* sunxi_sprite_next_buffer)(queue_data *qdata, void *buffer);

static uint decode_null(void * ibuf, uint len)
{
	debug("deocde null\n");
	return (uint)ibuf;
}

static uint decode_need(void * ibuf, uint len)
{
	char buffer[SPRITE_CARD_ONCE_DATA_DEAL];

	debug("deocde need\n");
	decode(ibuf, buffer, len);
	memcpy(ibuf, buffer, len);

	return (uint)ibuf;
}

static void *sunxi_sprite_decode_next_buffer(queue_data *qdata, void *buffer)
{
	return buffer;
}

static void *sunxi_sprite_nodecode_next_buffer(queue_data *qdata, void *buffer)
{
	return qdata->data;
}
#if 0
//int sunxi_sprite_card_download_part(sunxi_download_info *dl_map)
//{
//	long long part_datasize;      //分区镜像的大小(字节单位)
//	long long part_tmpsize;       //分区镜像的大小，用于递减，到零时表示处理完成
//    int  i = 0;
//    int  imgfile_start;
//    int  data_format;                //格式
//    uint base_part_start, part_flash_size;    //分区起始扇区和大小(扇区单位)
//    queue_data  *qdata, *next_qdata; //用于取出一个buffer
//    uint origin_verify, active_verify; //校验数据
//    char  *base_buffer, *half_buffer;
//    char  *decode_buffer;
//    char  *next_buffer;
//
//	base_buffer = malloc(SPRITE_CARD_ONE_DATA_DEAL);
//	if(!base_buffer)
//	{
//		printf("sunxi sprite: fail to malloc memory for post-treatment\n");
//
//		return -1;
//	}
//	half_buffer = base_buffer + 512 * 1024;
//	qdata = next_qdata = NULL;
//
//    for(i=0;i<dl_map->download_count;i++)
//    {
//    	//任意一个分区，需要烧写的数据量
//    	//清空所有buffer
//    	//sunxi_queue_reset();
//    	//USB量产，要求送入名为dl_filename的数据
//    	base_part_start = dl_map->one_part_info[i].addrlo;
//        part_flash_size = dl_map->one_part_info[i].lenlo;
//        part_tmpsize    = (long long)part_flash_size * 9;
//    	//卡量产，直接读取
//    	//二者都将采用中断自动执行的方式，不停的填充buffer空间
//    	//当填充完成，没有新buffer可用，则启动定时中断查询是否有新空间
//		imgitemhd = Img_OpenItem(imghd, "RFSFAT16", (char *)dl_map->one_part_info[i].dl_filename);
//		if(!imgitemhd)
//		{
//			printf("sprite error: open part %s failed\n", dl_map->one_part_info[i].dl_filename);
//
//			goto _card_download_error;
//		}
//    	part_datasize = Img_GetItemSize(imghd, imgitemhd);
//        if(part_datasize > part_tmpsize)      //检查分区大小是否合法
//        {
//        	//data is larger than part size
//        	printf("sunxi sprite: data size is larger than part %s size\n", dl_map->one_part_info[i].dl_filename);
//
//        	goto _card_download_error;
//        }
//        //开始获取分区数据
//		imgfile_start = Img_GetItemStart(imghd, imgitemhd);
//		if(!imgfile_start)
//		{
//			printf("sunxi sprite: cant get part data imgfile_start\n");
//
//			goto _card_download_error;
//		}
//		//通知卡驱动，现在准备从imgfile_start处连续读取part_datasize长度的数据
//		//sunxi_sprite_card_read_sequence(imgfile_start, (uint)((part_datasize + 511)/512));
//		//开始读取
////		if(sunxi_sprite_card_read_sequence())
////		{
////		    printf("sunxi sprite: cant read data from sdcard\n");
////
////		    goto _card_download_error;
////		}
//		//开始循环处理所有数据
//		part_tmpsize = part_datasize;
//		if(dl_map->one_part_info[i].encrypt)
//		{
//			//需要解密
//			sunxi_sprite_decode = decode;
//			sunxi_sprite_next_buffer = sunxi_sprite_decode_next_buffer;
//			init_code();
//		}
//		else
//		{
//			//不需要解密
//			sunxi_sprite_decode = decode_null;
//			sunxi_sprite_next_buffer = sunxi_sprite_nodecode_next_buffer;
//		}
//		//首先获取一个buffer的数据，用于判断数据格式
//		while(sunxi_queue_isempty());       	//如果buffer队列是空的则等待，直到有buffer可用
//		sunxi_outqueue_query(qdata, next_qdata);		//取出一个buffer
//		decode_buffer = (char *)sunxi_sprite_decode((void *)qdata->data, (void *)half_buffer, qdata->len);
//		data_format = unsparse_probe((char *)decode_buffer, qdata->len, base_part_start);		//判断数据格式
//		if(data_format == ANDROID_FORMAT_DETECT)
//		{
//			sunxi_sprite_download_data = unsparse_direct_write;
//		}
//		else if(data_format == ANDROID_FORMAT_BAD)
//		{
//			sunxi_sprite_download_raw_init(base_part_start);
//			sunxi_sprite_download_data = sunxi_sprite_download_raw;
//		}
//		//把第一笔数据写下去
//		part_tmpsize -= qdata->len;
//		//数据格式判断，如果是sparse格式
//		next_buffer = sunxi_sprite_next_buffer(next_qdata, half_buffer);
//		if(sunxi_sprite_download_data((void *)decode_buffer, (void *)next_buffer, qdata->len))
//		{
//			printf("sunxi sprite error: download data error\n");
//
//			goto _card_download_error;
//		}
//		sunxi_queue_out();
//		while(part_tmpsize)
//		{
//			if(!sunxi_queue_isfull())     //如果buffer队列不是满的，则可以用于继续存放固件数据
//			{
//				if(0 == sunxi_sprite_card_sequence_status()) //如果卡处于idle状态，则可以读固件数据
//				{
//					//现在，卡处于idle状态，同时有buffer可用，则调用卡函数继续读取数据
//					if(sunxi_sprite_card_read_sequence())         //如果卡无法读取数据，则返回失败
//					{
//				    	printf("sunxi sprite: cant read sdcard by sequence\n");
//
//				    	goto _card_download_error;
//					}
//				}
//			}
//			//现在处理已经完成的数据，准备解密和烧录
//			if(!sunxi_outqueue_query(qdata, next_qdata))    //如果buffer队列中可以取出一个有效buffer
//			{
//				//首先执行解密函数,不论有没有加密，函数都需要调用
//				//当需要解密时，real_dest_buffer指向half_buffer
//				//当不需要解密时，real_dest_buffer指向qdata.data
//				decode_buffer = (char *)sunxi_sprite_decode((void *)qdata->data, (void *)half_buffer, qdata->len);
//				//数据格式判断，如果是sparse格式
//				next_buffer = sunxi_sprite_next_buffer(next_qdata, half_buffer);
//				if(sunxi_sprite_download_data((void *)decode_buffer, (void *)next_buffer, qdata->len))
//				{
//					printf("sunxi sprite error: download data error\n");
//
//					goto _card_download_error;
//				}
//				sunxi_queue_out();
//			}
//		}
//		exit_code();
//		Img_CloseItem(imghd, imgitemhd);
//		//校验数据
//        if(dl_map->one_part_info[i].vf_filename)
//        {
//        	imgitemhd = Img_OpenItem(imghd, "RFSFAT16", (char *)dl_map->one_part_info[i].vf_filename);
//			if(!imgitemhd)
//			{
//				printf("sprite error: open part %s failed\n", dl_map->one_part_info[i].vf_filename);
//				Img_CloseItem(imghd, imgitemhd);
//
//				continue;
//			}
//        	if(!Img_ReadItem(imghd, imgitemhd, (void *)&origin_verify, sizeof(int)))   //读出数据
//	        {
//	            printf("sprite update warning: fail to read data from %s\n", dl_map->one_part_info[i].vf_filename);
//				//Img_CloseItem(imghd, imgitemhd);
//	            continue;
//	        }
//        	if(data_format == ANDROID_FORMAT_BAD)
//        	{
//                active_verify = sunxi_sprite_part_rawdata_verify(base_part_start, part_datasize);
//            }
//            else
//            {
//            	active_verify = sunxi_sprite_part_sparsedata_verify();
//            }
//            if(origin_verify != active_verify)
//            {
//            	printf("sunxi sprite: part %s verify error\n", dl_map->one_part_info[i].dl_filename);
//            	printf("origin checksum=%x, active checksum=%x\n", origin_verify, active_verify);
//            	Img_CloseItem(imghd, imgitemhd);
//
//            	return -1;
//            }
//            Img_CloseItem(imghd, imgitemhd);
//        }
//        else
//        {
//        	printf("sunxi sprite: part %s not need to verify\n", dl_map->one_part_info[i].dl_filename);
//        }
//    }
//
//    return 0;
//_card_download_error:
//	return -1;
//}
#else
int sunxi_sprite_card_download_part(sunxi_download_info *dl_map)
{
	uint part_datasectors;   // 需要烧写的分区的扇区个数
	uint part_tmpsectors;    // 递减值，为0时表示烧写完成
    int  i = 0;
    uint imgfile_start, tmp_file_start;
    uint read_one_sectors;
    int  data_format;                //格式
    uint base_part_start, part_flash_size;    //分区起始扇区和大小(扇区单位)
    uint origin_verify, active_verify; //校验数据
    char  *base_buffer = NULL, *half_buffer;
    char  *decode_buffer;
    int  ret = -1;

	base_buffer = (char *)malloc(SPRITE_CARD_ONCE_DATA_DEAL * 2);
	if(!base_buffer)
	{
		printf("sunxi sprite: fail to malloc memory for post-treatment\n");

		return -1;
	}
	half_buffer = base_buffer + SPRITE_CARD_ONCE_DATA_DEAL;
	//qdata = next_qdata = NULL;
	debug("total download part %d\n", dl_map->download_count);

    for(i=0;i<dl_map->download_count;i++)
    {
    	//任意一个分区，需要烧写的数据量
    	//清空所有buffer
    	//sunxi_queue_reset();
    	//USB量产，要求送入名为dl_filename的数据
    	debug("download %d part %s\n", i, dl_map->one_part_info[i].dl_filename);
    	base_part_start = dl_map->one_part_info[i].addrlo;
        part_flash_size = dl_map->one_part_info[i].lenlo;
        //卡量产，直接读取
    	//二者都将采用中断自动执行的方式，不停的填充buffer空间
    	//当填充完成，没有新buffer可用，则启动定时中断查询是否有新空间
    	imgitemhd = Img_OpenItem(imghd, "RFSFAT16", (char *)dl_map->one_part_info[i].dl_filename);
		if(!imgitemhd)
		{
			printf("sprite error: open part %s failed\n", dl_map->one_part_info[i].dl_filename);

			goto _card_download_error;
		}
		part_datasectors = Img_GetItemSize(imghd, imgitemhd)>>9;
    	debug("part size=%d\n", (uint)part_flash_size);
		debug("part data=%d\n", (uint)part_datasectors);
        if(part_datasectors > part_flash_size)      //检查分区大小是否合法
        {
        	//data is larger than part size
        	printf("sunxi sprite: data size is larger than part %s size\n", dl_map->one_part_info[i].dl_filename);

        	goto _card_download_error;
        }
        //开始获取分区数据
		imgfile_start = Img_GetItemStart(imghd, imgitemhd);
		if(!imgfile_start)
		{
			printf("sunxi sprite: cant get part data imgfile_start\n");

			goto _card_download_error;
		}
		debug("part data start sector=%d\n", imgfile_start);
		tmp_file_start = imgfile_start;
		//通知卡驱动，现在准备从imgfile_start处连续读取part_datasize长度的数据
		//sunxi_sprite_card_read_sequence(imgfile_start, (uint)((part_datasize + 511)/512));
		//开始读取
//		if(sunxi_sprite_card_read_sequence())
//		{
//		    printf("sunxi sprite: cant read data from sdcard\n");
//
//		    goto _card_download_error;
//		}
		//开始循环处理所有数据
		part_tmpsectors = part_datasectors;
		if(dl_map->one_part_info[i].encrypt)
		{
			//需要解密
			sunxi_sprite_decode = decode_need;
//			sunxi_sprite_next_buffer = sunxi_sprite_decode_next_buffer;
			init_code();
		}
		else
		{
			//不需要解密
			sunxi_sprite_decode = decode_null;
//			sunxi_sprite_next_buffer = sunxi_sprite_nodecode_next_buffer;
		}
		if(part_tmpsectors >= SPRITE_CARD_ONCE_DATA_DEAL)
		{
			read_one_sectors = SPRITE_CARD_ONCE_SECTOR_DEAL;
		}
		else
		{
			read_one_sectors = part_tmpsectors;
		}
		if(sunxi_flash_read(tmp_file_start, read_one_sectors, half_buffer) != read_one_sectors)
		{
			printf("sunxi sprite error : read sdcard block %d, total %d failed\n", tmp_file_start, read_one_sectors);

			goto _card_download_error;
		}
		//首先获取一个buffer的数据，用于判断数据格式
		decode_buffer = (char *)sunxi_sprite_decode(half_buffer, read_one_sectors * 512);
		data_format = unsparse_probe((char *)decode_buffer, read_one_sectors, base_part_start);		//判断数据格式
		if(data_format == ANDROID_FORMAT_DETECT)
		{
			sunxi_sprite_download_data = unsparse_direct_write;
		}
		else if(data_format == ANDROID_FORMAT_BAD)
		{
			sunxi_sprite_download_raw_init(base_part_start);
			sunxi_sprite_download_data = sunxi_sprite_download_raw;
		}
		//把第一笔数据写下去
		part_tmpsectors -= read_one_sectors;
		tmp_file_start += read_one_sectors;
		//数据格式判断，如果是sparse格式
		debug("write first data\n");
		if(sunxi_sprite_download_data((void *)decode_buffer, (void *)decode_buffer, read_one_sectors))
		{
			printf("sunxi sprite error: download data error\n");

			goto _card_download_error;
		}
		debug("first data write ok\n");
		read_one_sectors = SPRITE_CARD_ONCE_SECTOR_DEAL;
		while(part_tmpsectors >= SPRITE_CARD_ONCE_DATA_DEAL)
		{
			debug("rest part sectors=%d, once sectors=%d\n", part_tmpsectors, SPRITE_CARD_ONCE_DATA_DEAL);
			if(sunxi_flash_read(tmp_file_start, read_one_sectors, half_buffer) != read_one_sectors)
			{
				printf("sunxi sprite error : read sdcard block %d, total %d failed\n", tmp_file_start, read_one_sectors);

				goto _card_download_error;
			}
			decode_buffer = (char *)sunxi_sprite_decode(half_buffer, read_one_sectors * 512);
			//现在处理已经完成的数据，准备解密和烧录
				//数据格式判断，如果是sparse格式
			if(sunxi_sprite_download_data((void *)decode_buffer, (void *)decode_buffer, read_one_sectors))
			{
				printf("sunxi sprite error: download data error\n");

				goto _card_download_error;
			}
		}
		exit_code();
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
		//校验数据
		debug("part data download finish\n");
#if 1
        debug("try to read verify file %s\n", dl_map->one_part_info[i].vf_filename);
        if(dl_map->one_part_info[i].vf_filename)
        {
        	imgitemhd = Img_OpenItem(imghd, "RFSFAT16", (char *)dl_map->one_part_info[i].vf_filename);
			if(!imgitemhd)
			{
				printf("sprite error: open part %s failed\n", dl_map->one_part_info[i].vf_filename);
				Img_CloseItem(imghd, imgitemhd);
				imgitemhd = NULL;

				continue;
			}
			debug("img open ok\n");
        	if(!Img_ReadItem(imghd, imgitemhd, (void *)&origin_verify, sizeof(int)))   //读出数据
	        {
	            printf("sprite update warning: fail to read data from %s\n", dl_map->one_part_info[i].vf_filename);
				Img_CloseItem(imghd, imgitemhd);
				imgitemhd = NULL;

	            continue;
	        }
	        debug("img data read ok\n");
        	if(data_format == ANDROID_FORMAT_BAD)
        	{
                active_verify = sunxi_sprite_part_rawdata_verify(base_part_start, part_datasectors);
            }
            else
            {
            	active_verify = sunxi_sprite_part_sparsedata_verify();
            }
            if(origin_verify != active_verify)
            {
            	printf("origin checksum=%x, active checksum=%x\n", origin_verify, active_verify);
            	printf("sunxi sprite: part %s verify error\n", dl_map->one_part_info[i].dl_filename);
            	Img_CloseItem(imghd, imgitemhd);
            	imgitemhd = NULL;

            	goto _card_download_error;
            }
            Img_CloseItem(imghd, imgitemhd);
            imgitemhd = NULL;
        }
        else
        {
        	printf("sunxi sprite: part %s not need to verify\n", dl_map->one_part_info[i].dl_filename);
        }
#endif
    }
	debug("all download part ok\n");
	ret = 0;

_card_download_error:
	if(base_buffer)
	{
		free(base_buffer);
	}
	if(imgitemhd)
	{
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
	}

	return ret;
}
#endif
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
int sunxi_sprite_deal_uboot(int production_media)
{
	char buffer[1024 * 1024];
	uint item_original_size;

    imgitemhd = Img_OpenItem(imghd, "BOOT    ", "UBOOT_0000000000");
    if(!imgitemhd)
    {
        printf("sprite update error: fail to open uboot item\n");
        return -1;
    }
    //uboot长度
    item_original_size = Img_GetItemSize(imghd, imgitemhd);
    {
        printf("sprite update error: fail to get uboot item size\n");
        return -1;
    }
    /*获取uboot的数据*/
    if(!Img_ReadItem(imghd, imgitemhd, (void *)buffer, item_original_size))
    {
        printf("update error: fail to read data from for uboot\n");
        return -1;
    }
    Img_CloseItem(imghd, imgitemhd);
    imgitemhd = NULL;

    if(sunxi_sprite_download_uboot(buffer, production_media))
    {
    	printf("update error: fail to write uboot\n");
        return -1;
    }

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
int sunxi_sprite_deal_boot0(int production_media)
{
	char buffer[1024 * 1024];
	uint item_original_size;

	if(production_media == 0)
	{
		imgitemhd = Img_OpenItem(imghd, "BOOT    ", "NANDBOOT0_000000");
	}
	else
	{
		imgitemhd = Img_OpenItem(imghd, "BOOT    ", "CARDBOOT0_000000");
	}
    if(!imgitemhd)
    {
        printf("sprite update error: fail to open boot0 item\n");
        return -1;
    }
    //boot0长度
    item_original_size = Img_GetItemSize(imghd, imgitemhd);
    if(!item_original_size)
    {
        printf("sprite update error: fail to get boot0 item size\n");
        return -1;
    }
    /*获取boot0的数据*/
    if(!Img_ReadItem(imghd, imgitemhd, (void *)buffer, item_original_size))
    {
        printf("update error: fail to read data from for boot0\n");
        return -1;
    }
    Img_CloseItem(imghd, imgitemhd);
    imgitemhd = NULL;

    if(sunxi_sprite_download_boot0(buffer, production_media))
    {
    	printf("update error: fail to write boot0\n");
        return -1;
    }

    return 0;
}

