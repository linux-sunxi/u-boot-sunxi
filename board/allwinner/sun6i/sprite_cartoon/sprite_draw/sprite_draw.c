/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm boot sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include  "../sprite_cartoon_i.h"
#include  "../sprite_cartoon.h"

#define  ABS(x)   (((x)<0)?(-(x)):(x))
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_draw_hollowrectangle(int x1, int y1, int x2, int y2)
{
	int   end_x, end_y;
	int   start_x, start_y;
	char *base1, *base2;
	int   x, y, tmp;
	int   line_offset;

	end_x   = x1;
	end_y   = y1;
	start_x = x2;
	start_y = y2;

	//确保起点比较小，方便计算
	if(start_y > end_y)
	{
		tmp 	= start_y;
		start_y = end_y;
		end_y   = tmp;
	}
	if(start_x > end_x)
	{
		tmp 	= start_x;
		start_x = end_x;
		end_x   = tmp;
	}
	//开始绘线
	base1  = sprite_source.screen_buf + (sprite_source.screen_width * start_y + start_x)* 4;
	base2  = base1 + (end_y - start_y) * sprite_source.screen_width * 4;
	for(x=start_x;x<=end_x;x++)
	{
		*((int *)base1) = sprite_source.color;
		*((int *)base2) = sprite_source.color;
		base1 += 4;
		base2 += 4;
	}
	base1  = sprite_source.screen_buf + (sprite_source.screen_width * start_y + start_x)* 4;
	base2  = base1 + (end_x - start_x) * 4;
	line_offset = sprite_source.screen_width * 4;
	for(y=start_y;y<=end_y;y++)
	{
		*((int *)base1) = sprite_source.color;
		*((int *)base2) = sprite_source.color;
		base1 += line_offset;
		base2 += line_offset;
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_draw_solidrectangle(int x1, int y1, int x2, int y2)
{
	int   end_x, end_y;
	int   start_x, start_y;
	char *base1, *base2;
	char *base1_1, *base2_1;
	int   x, y, tmp;

	end_x   = x1;
	end_y   = y1;
	start_x = x2;
	start_y = y2;

	//确保起点比较小，方便计算
	if(start_y > end_y)
	{
		tmp 	= start_y;
		start_y = end_y;
		end_y   = tmp;
	}
	if(start_x > end_x)
	{
		tmp 	= start_x;
		start_x = end_x;
		end_x   = tmp;
	}

	base1 = sprite_source.screen_buf + (sprite_source.screen_width * start_y + start_x) * 4;
	base2 = base1 + sprite_source.screen_width * (end_y - start_y) * 4;
	base1_1 = base1;
	base2_1 = base2;
	for(x=end_x;x>=start_x;x--)
	{
		for(y=end_y;y>=start_y;y-=2)
		{
			*((int *)base1_1) = sprite_source.color;
			*((int *)base2_1) = sprite_source.color;
			base1_1 += sprite_source.screen_width * 4;
			base2_1 -= sprite_source.screen_width * 4;
		}
		base1 += 4;
		base2 += 4;
		base1_1 = base1;
		base2_1 = base2;
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_draw_line(int x1, int y1, int x2, int y2)
{
	int   end_x, end_y;
	int   start_x, start_y;
	char *base;
	int   x, y, tmp;
	int   x_tmp, y_tmp;
	int   x_offset, y_offset;
	float a, b;

	end_x   = x1;
	end_y   = y1;
	start_x = x2;
	start_y = y2;

	base   = sprite_source.screen_buf;
	//开始绘制线条
	if(end_x == start_x)
	{
		base += (sprite_source.screen_width * start_y + start_x)* 4;
		if(start_y > end_y)
		{
			tmp 	= start_y;
			start_y = end_y;
			end_y   = tmp;
		}
		for(y=start_y;y<=end_y;y++)
		{
			*((int *)base) = sprite_source.color;
			base += sprite_source.screen_width * 4;
		}
	}
	else if(end_y == start_y)
	{
		base += (sprite_source.screen_width * start_y + start_x)* 4;
		if(start_x > end_x)
		{
			tmp 	= start_x;
			start_x = end_x;
			end_x   = tmp;
		}
		for(x=start_x;x<=end_x;x++)
		{
			*((int *)base) = sprite_source.color;
			base += 4;
		}
	}
	else
	{
		a = (start_y - end_y)*1.0/(start_x - end_x);
		b = (start_x * end_y - end_x * start_y)*1.0/(start_x - end_x);

		if(ABS(end_x - start_x) > ABS(end_y - start_y))
		{
			if(end_x < start_x)
			{
				x_tmp   = end_x;
				end_x   = start_x;
				start_x = x_tmp;

				y_tmp   = end_y;
				end_y   = start_y;
				start_y = y_tmp;
			}
			if(end_y < start_y)
			{
				y_offset = -sprite_source.screen_width * 4;
			}
			else
			{
				y_offset =  sprite_source.screen_width * 4;
			}
			y_tmp = (int)(a * start_x + b);
			base += (sprite_source.screen_width * start_y + start_x) * 4;
			*((int *)base) = sprite_source.color;
			for(x=start_x+1;x<=end_x;x++)
			{
				y = (int)(a * x + b);

				if(y != y_tmp)
				{
					y_tmp = y;
					base += y_offset;
				}
				base += 4;
				*((int *)base) = sprite_source.color;
			}
		}
		else
		{
			if(end_y < start_y)
			{
				y_tmp   = end_y;
				end_y   = start_y;
				start_y = y_tmp;

				x_tmp   = end_x;
				end_x   = start_x;
				start_x = x_tmp;
			}
			if(end_x < start_x)
			{
				x_offset = -4;
			}
			else
			{
				x_offset = 4;
			}
			x_tmp = (int)((start_y - b)/a);
			tmp = sprite_source.screen_width * 4;
			base += (sprite_source.screen_width * start_y + start_x) * 4;
			*((int *)base) = sprite_source.color;
			for(y=start_y+1;y<=end_y;y++)
			{
				x = (int)((y - b)/a);

				if(x != x_tmp)
				{
					x_tmp = x;
					base += x_offset;
				}
				base += tmp;
				*((int *)base) = sprite_source.color;
			}
		}
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_draw_lineto(int px, int py)
{
	int   end_x, end_y;

	end_x   = px;
	end_y   = py;
	sprite_cartoon_ui_draw_line(sprite_source.this_x, sprite_source.this_y, end_x, end_y);
	sprite_source.this_x = end_x;
	sprite_source.this_y = end_y;

	return 0;
}
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_draw_point(int x, int y)
{
	int   scr_x, scr_y;
	char *base;

	scr_x  = x;
	scr_y  = y;

	base  = sprite_source.screen_buf + (sprite_source.screen_width * scr_y + scr_x)* 4;
	*((int *)base) = sprite_source.color;

	sprite_source.this_x = scr_x;
	sprite_source.this_y = scr_y;

	return 0;
}
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_set_color(int color)
{
	sprite_source.color = color;

	return 0;
}
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_get_color(void)
{
	return sprite_source.color;
}
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_moveto(int x, int y)
{
	int   scr_x, scr_y;

	scr_x  = x;
	scr_y  = y;

	sprite_source.this_x = scr_x;
	sprite_source.this_y = scr_y;

	return 0;
}
/*
************************************************************************************************************
*
*                                             ui_show_data
*
*    函数名称：ui_show_data
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：在屏幕的某个位置显示传进的数据
*
*
************************************************************************************************************
*/
int sprite_cartoon_ui_clear(void)
{
	memset(sprite_source.screen_buf, 0, sprite_source.screen_size);

	return 0;
}

