/*
*************************************************************************************
*                         			eGon
*					        Application Of eGon2.0
*
*				(c) Copyright 2006-2010, All winners Co,Ld.
*							All	Rights Reserved
*
* File Name 	: ui.c
*
* Author 		: javen
*
* Description 	: 图形显示输出操作函数集
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	   2010-09-10          1.0            create this file
*
*************************************************************************************
*/
#ifndef  __UI_CHAR_I_H__
#define  __UI_CHAR_I_H__


#include  "types.h"


typedef struct ui_char_info_t
{
    char   *crt_addr;                         //当前用于显示的地址
    __u32   rest_screen_height;               //剩余的存储屏幕高度，剩余总高度, 字符单位，行
    __u32   rest_screen_width;                //剩余的存储屏幕宽度, 剩余总宽度, 字符单位，行
    __u32   rest_display_height;              //剩余的显示高度
    __u32   total_height;                     //用于显示总的高度
    __u32   current_height;                   //当前已经使用的高度
    __u32   x;                                //显示位置的x坐标
    __u32   y;                                //显示位置的y坐标
    int     word_size;						  //字符大小
}
_ui_char_info_t;




#endif   //__UI_CHAR_I_H__

