/*
************************************************************************************************************************
*                                                      FontEngine
*
*                             Copyright(C), 2006-2008, SoftWinner Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : FontEngine.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.10.13
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.10.13       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __FontEngine_h
#define  __FontEngine_h

#include "egon2.h"
#include "sfte.h"

typedef struct _FE_FONT_t
{
	SFTE_Face  face;
	int             bbox_ymin;     // unit : pixel
	unsigned  int   line_distance; //
	unsigned  int   base_width;
	unsigned  char  *base_addr;
	unsigned  int   base_psize;

}FE_FONT_t, *FE_FONT;


extern int     open_font( const char *font_file, int pixel_size,  unsigned int width, unsigned char *addr);
extern int     close_font( void );
extern int     check_change_line(unsigned int x, unsigned char ch);
extern int     draw_bmp_ulc( __u32 ori_x, __u32 ori_y , __u32 color);



#endif     //  ifndef __FontEngine_h

/* end of FontEngine.h  */
