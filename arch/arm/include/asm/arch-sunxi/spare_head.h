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
#ifndef  __spare_head_h__
#define  __spare_head_h__

#include "nand_bsp.h"
#include "dram.h"
/* work mode */
#define WORK_MODE_PRODUCT      (1<<4)
#define WORK_MODE_UPDATE       (1<<5)

#define WORK_MODE_BOOT			0x00	//正常启动
#define WORK_MODE_USB_PRODUCT	0x10	//用于USB量产
#define WORK_MODE_CARD_PRODUCT	0x11	//用于卡量产
#define WORK_MODE_USB_UPDATE	0x20	//用于USB升级
#define WORK_MODE_OUTER_UPDATE	0x21	//用于外部盘升级

#define UBOOT_MAGIC				"uboot"
#define STAMP_VALUE             0x5F0A6C39
#define ALIGN_SIZE				8 * 1024
#define MAGIC_SIZE              8
#define STORAGE_BUFFER_SIZE     (256)

typedef struct _normal_gpio_cfg
{
    char      port;                       //端口号
    char      port_num;                   //端口内编号
    char      mul_sel;                    //功能编号
    char      pull;                       //电阻状态
    char      drv_level;                  //驱动驱动能力
    char      data;                       //输出电平
    char      reserved[2];                //保留位，保证对齐
}
normal_gpio_cfg;

//SD卡相关数据结构
typedef struct sdcard_spare_info_t
{
	int 			card_no[4];                   //当前启动的卡控制器编号
	int 			speed_mode[4];                //卡的速度模式，0：低速，其它：高速
	int				line_sel[4];                  //卡的线制，0: 1线，其它，4线
	int				line_count[4];                //卡使用线的个数
}
sdcard_spare_info;



#endif


