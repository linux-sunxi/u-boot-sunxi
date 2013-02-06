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
#ifndef  __SPRITE_PROGRESSBAR_H__
#define  __SPRITE_PROGRESSBAR_H__


extern  unsigned	sprite_cartoon_progressbar_create(int x1, int y1, int x2, int y2);
extern  int       	sprite_cartoon_progressbar_config(unsigned p, int frame_color, int progress_color, int thickness);

extern  int 		sprite_cartoon_progressbar_active(unsigned p);
extern  int 		sprite_cartoon_progressbar_destroy(unsigned p);
extern  int 		sprite_cartoon_progressbar_upgrate(unsigned p, int rate);



#endif   //__SPRITE_PROGRESSBAR_H__

