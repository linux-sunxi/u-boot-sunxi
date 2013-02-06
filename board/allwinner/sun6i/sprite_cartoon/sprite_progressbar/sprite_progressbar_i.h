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
#ifndef  __SPRITE_PROGRESSBAR_I_H__
#define  __SPRITE_PROGRESSBAR_I_H__


typedef struct  _progressbar_t
{
	int  x1;					//左上角x坐标
	int  y1;					//左上角y坐标
	int  x2;					//右下角x坐标
	int  y2;					//右下角y坐标
	int  st_x;					//内部起始点的x坐标
	int  st_y;					//内部起始点的y坐标
	int  pr_x;					//当前进度所在的x坐标
	int  pr_y;					//无效
	int  thick;					//框的厚度，边缘厚度
	int  width;					//整体宽度
	int  height;				//整体高度
	int  frame_color;			//边框颜色
	int  progress_color;		//内部颜色
	int  progress_ratio;		//当前进度百分比
}
progressbar_t;




#endif   //__SPRITE_PROGRESSBAR_I_H__

