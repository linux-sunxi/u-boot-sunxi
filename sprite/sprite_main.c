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
#include <asm/arch/partition.h>
#include "sprite_queue.h"
#include "sprite_privatedata.h"

download_info  dl_map;
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
int sunxi_sprite_download_init(int workmode)
{
	//初始化队列，用于读取数据
    if(sunxi_queue_init())
    {
    	printf("sunxi queue init fail\n");

    	return -1;
    }
	//这里区分量产模式，卡量产还是usb量产
	if(workmode == WORK_MODE_CARD_PRODUCT)
	{
		//卡量产模式
		//初始化卡量产中所需要用到的硬件，比如LED，CODEC
		//;
	}
	else if(workmode == WORK_MODE_USB_PRODUCT)
	{
		//usb量产模式
		//初始化USB硬件，此处弹开USB控制器，表示fel结束，然后开始接收数据
//		if(sprite_usb_hardware_init())
//		{
//			printf("sunxi sprite: usb hardware init fail\n");
//
//			return -1;
//		}
//		//获取dl_map文件，用于指引下载的数据
//	    if(sprite_usb_fetch_download_map(&dl_map))
//	    {
//	    	return -1;
//	    }
//	    sunxi_sprite_probe_part_data_size = sunxi_sprite_usb_probe_part_data_size;
	}
	else
	{
		//其它模式，无视掉
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
int sunxi_sprite_erase_flash(void)
{
	int need_erase_flash;
    char buf[1024 * 1024];
    char *tmp_buf;
    sunxi_mbr_t  *local_mbr;
    int i, ret;
	//获取擦除信息，查看是否需要擦除flash
    ret = script_parser_fetch("nand_para", "erase_flash", &need_erase_flash, 1);
    if((!ret) && (need_erase_flash))
    {
    	//check if need to protect part data
    	//初始化flash，如果是量产卡去略过
    	//当初始化失败的时候，直接擦除，不处理私有数据
//    	if(sunxi_sprite_init())
    	{
    		//sunxi_sprite_erase();
    		return 0;
    	}
    	//读出量产介质上的MBR
    	if(!sunxi_sprite_read(0, 1024 * 1024/512, buf))
    	{
    		printf("read local mbr fail\n");

    		return 0;
    	}
    	//校验MBR
    	tmp_buf = buf;
    	for(i=0;i<MBR_MAX_PART_COUNT;i++)
        {
        	local_mbr = (sunxi_mbr_t *)tmp_buf;
        	if(crc32(0, tmp_buf + 4, MBR_SIZE - 4) != local_mbr->crc32)
        	{
        		printf("bad mbr table\n");
        		tmp_buf += MBR_SIZE;
        	}
	        else
	        {
	        	sunx_sprite_store_part_data(local_mbr);
	            break;
	        }
	    }
//	    sprite_flash_erase();
        if(i < MBR_MAX_PART_COUNT)
	    {
	    	sunx_sprite_restore_part_data(local_mbr);
	    }
		sprite_flash_exit();
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
int sunxi_sprite_hardware_scan(void *buffer)
{
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
int sunxi_sprite_mode(int workmode)
{
	int production_meida;
	char  storage_info[512];

	//获取当前是量产介质是nand或者卡
	production_meida = uboot_spare_head.boot_data.storage_type;
	//初始化各自硬件
	if(sunxi_sprite_download_init(workmode))
	{
		return -1;
	}
	//擦除flash
	sunxi_sprite_erase_flash();
	sunxi_sprite_hardware_scan((void*)storage_info);
	//烧写分区数据
	//这里区分量产模式，卡量产还是usb量产
	if(workmode == WORK_MODE_CARD_PRODUCT)
	{
		//测试卡上固件的合法性
		if(sprite_card_firmware_probe())
	    {
	    	printf("sunxi sprite firmware probe fail\n");

	    	return -1;
	    }
	    //获取dl_map文件，用于指引下载的数据
	    if(sprite_card_fetch_download_map(&dl_map))
	    {
	    	return -1;
	    }
	    sunxi_sprite_card_download_part(&dl_map);
	}
	else if(workmode == WORK_MODE_USB_PRODUCT)
	{
		//usb量产模式
		//初始化USB硬件，此处弹开USB控制器，表示fel结束，然后开始接收数据
//		if(sprite_usb_hardware_init())
//		{
//			printf("sunxi sprite: usb hardware init fail\n");
//
//			return -1;
//		}
//		//获取dl_map文件，用于指引下载的数据
//	    if(sprite_usb_fetch_download_map(&dl_map))
//	    {
//	    	return -1;
//	    }
//	    sunxi_sprite_probe_part_data_size = sunxi_sprite_usb_probe_part_data_size;
	}
	else
	{
		//其它模式，无视掉
		return -1;
	}
	//烧写结束

	return 0;
}

