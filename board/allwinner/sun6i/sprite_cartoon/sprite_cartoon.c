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
#include "sprite_cartoon.h"
#include "sprite_cartoon_i.h"
#include "sprite_cartoon_color.h"
#include <asm/arch/drv_display.h>

sprite_cartoon_source  sprite_source;

/*
************************************************************************************************************
*
*                                             function
*
*    name          :	sprite_cartoon_screen_set
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
int sprite_cartoon_screen_set(void)
{
	uint arg[4] = { 0 };
	/* 初始化图形参数 */
	sprite_source.screen_width  = disp_ioctl(NULL, DISP_CMD_SCN_GET_WIDTH, (void*)arg);
	sprite_source.screen_height = disp_ioctl(NULL, DISP_CMD_SCN_GET_HEIGHT, (void*)arg);
	sprite_source.screen_size   = sprite_source.screen_width * sprite_source.screen_height * 4;
    sprite_source.screen_buf 	= malloc(sprite_source.screen_size);

    if(!sprite_source.screen_buf)
    {
        return -1;
    }
    memset(sprite_source.screen_buf, 0, sprite_source.screen_size);

	board_display_framebuffer_set(sprite_source.screen_width, sprite_source.screen_height, 32, (void *)sprite_source.screen_buf);

    return 0;
}



int sprite_cartoon_test(void)
{
	int i;
	uint progressbar_hd;
	int screen_width, screen_height;
	uint arg[4] = { 0 };
	int x1, x2, y1, y2;

	sprite_cartoon_screen_set();
	board_display_show_until_lcd_open(0);

	screen_width  = disp_ioctl(NULL, DISP_CMD_SCN_GET_WIDTH, (void*)arg);
	screen_height = disp_ioctl(NULL, DISP_CMD_SCN_GET_HEIGHT, (void*)arg);

	printf("screen_width = %d\n", screen_width);
    printf("screen_height = %d\n", screen_height);

	x1 = screen_width/4;
	x2 = x1 * 3;

	y1 = screen_height/2 - 40;
	y2 = screen_height/2 + 40;

	printf("bar x1: %d y1: %d\n", x1, y1);
	printf("bar x2: %d y2: %d\n", x2, y2);

	progressbar_hd = sprite_cartoon_progressbar_create(x1, y1, x2, y2);
	sprite_cartoon_progressbar_config(progressbar_hd, SPRITE_CARTOON_GUI_RED, SPRITE_CARTOON_GUI_GREEN, 2);
	sprite_cartoon_progressbar_active(progressbar_hd);

	do
	{
		for(i=0;i<100;i++)
		{
			sprite_cartoon_progressbar_upgrate(progressbar_hd, i);
			__msdelay(500);
		}
		for(i=99;i>0;i--)
		{
			sprite_cartoon_progressbar_upgrate(progressbar_hd, i);
			__msdelay(500);
		}
	}
	while(1);
}


