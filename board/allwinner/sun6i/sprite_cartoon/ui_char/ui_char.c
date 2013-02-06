/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
*File Name : debug_display.c
*
*Author : Gary.Wang
*
*Version : 1.1.0
*
*Date : 2008.04.29
*
*Description :
*
*Others : None at present.
*
*
* History :
*
*  <Author>         <time>      <version>     <description>
*
* Wangyugang      2008.04.29      1.1.0        build the file
*
*******************************************************************************************************************
*/
#include <stdarg.h>
#include  "ui_main_i.h"
#include  "ui_char_i.h"
#include  "sfte/FontEngine.h"


_ui_char_info_t  ui_char_info;

static  __u32 change_line = 0;
/*
**********************************************************************************************************************
*                                               _change_to_new_line
*
* Description:
*
* Arguments  :
*
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int boot_ui_char_init(int char_size)
{
	char  font_name[] = "c:\\font24.sft";

	memset(&ui_char_info, 0, sizeof(_ui_char_info_t));

	ui_char_info.word_size = 24;
	if(char_size == 32)
	{
		font_name[7] = '3';
    	font_name[8] = '2';
    	ui_char_info.word_size = 32;
	}
	else if(char_size == 24)
	{
		;
	}
	else
	{
		if(ui_source.screen_width < 400)
		{
			font_name[7] = '3';
    		font_name[8] = '2';
    		ui_char_info.word_size = 32;
		}
	}
    if(open_font((const char *)font_name, ui_char_info.word_size, ui_source.screen_width,  (__u8 *)ui_source.screen_buf))   //打开字库
    {
    	__inf("boot_ui_char: open font failed\n");
    	return -1;
    }

    ui_char_info.crt_addr 	  = ui_source.screen_buf;
    ui_char_info.total_height = ((ui_source.screen_size / 4) / (ui_source.screen_width)) / (ui_char_info.word_size);   //总的高度，可以显示的行数

    ui_char_info.rest_screen_height  = ui_source.screen_height/(ui_char_info.word_size);  	//记录屏幕的剩余高度，行数, 剩余1行不用
    ui_char_info.rest_display_height = ui_char_info.total_height;    						//记录显示的剩余高度，行数，剩余1行不用
    ui_char_info.rest_screen_width   = ui_source.screen_width;                        		//剩余宽度等于显示宽度, 像素单位
    ui_char_info.current_height 	 = 0;
    ui_char_info.x              	 = 0;
    ui_char_info.y              	 = 0;

    return 0;
}
/*
**********************************************************************************************************************
*                                               _change_to_new_line
*
* Description:
*
* Arguments  :
*
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static int uichar_change_newline(void)
{
	int ret = 0;

    if(ui_char_info.rest_display_height <= 0)               //剩余宽度不够了，需要切换回到第一个屏幕
    {
		__inf("boot ui char: not enough space to printf\n");

		ret = -1;
	}
	else
	{
//	    __u32  tmp_size;

	    ui_char_info.rest_screen_width   = ui_source.screen_width;   //作为新的一行的长度,像素单位

	    ui_char_info.rest_screen_height  -= 1;           //剩余的屏幕高度, 行数
	    ui_char_info.rest_display_height -= 1;           //剩余的显示高度，行数
	    ui_char_info.current_height      += 1;           //当前高度变多一行
	    ui_char_info.x                    = 0;           //
	    ui_char_info.y                    = ui_char_info.current_height * ui_char_info.word_size;

//	    tmp_size = 4 * display_info.screen_width * display_info.y;
//        memset(display_info.screen_buf + tmp_size,  0,  4 * display_info.screen_width * WORD_SIZE);       //清除掉下一行的数据
//        display_info.crt_addr = display_info.screen_buf + tmp_size;                           //当前显示内容地址变换
	}
//        tmp_size = (4 * display_info.screen_width) * (display_info.screen_height - WORD_SIZE);
//
//        display_info.rest_screen_height = display_info.total_height - (display_info.screen_height/WORD_SIZE);          //剩余高度行数
//        memcpy(display_info.screen_buf, display_info.fb_addr + 4 * display_info.screen_width * WORD_SIZE,  tmp_size);
//        display_info.crt_addr = display_info.screen_buf + tmp_size;
//        memset(display_info.screen_buf + tmp_size,  0,  4 * 2 * display_info.screen_width * WORD_SIZE);
//        display_info.current_height = display_info.screen_height/WORD_SIZE - 2;
//        display_info.rest_display_height  = 1;
//        display_info.x                    = 0;           //
//        display_info.y                    = display_info.current_height * WORD_SIZE;
//
//        display_info.fb_addr = display_info.screen_buf;    //屏幕显示buffer地址
//        LCD_Change_frame_buffer(display_info.fb_addr);
//    }
//    else if(display_info.rest_display_height <= 0)                         //当前窗口显示不下，需要向下滑动
//    {
//        tmp_size = 4 * display_info.screen_width * display_info.current_height * WORD_SIZE;
//
//        memset(display_info.screen_buf + tmp_size,  0,  8 * display_info.screen_width * WORD_SIZE);       //清除掉下一行的数据
//        display_info.crt_addr = display_info.screen_buf + tmp_size;                           //当前显示内容地址变换
//        display_info.rest_display_height = 1;                                               //记录显示的剩余高度，行数
//
//        display_info.fb_addr += 4 * display_info.screen_width * WORD_SIZE;                              //切换到下一行
//        LCD_Change_frame_buffer(display_info.fb_addr);
//    }
//    else
//    {
//        tmp_size = 4 * display_info.screen_width * display_info.y;
//        memset(display_info.screen_buf + tmp_size,  0,  4 * display_info.screen_width * WORD_SIZE);       //清除掉下一行的数据
//        display_info.crt_addr = display_info.screen_buf + tmp_size;                           //当前显示内容地址变换
//    }

    return ret;
}

/*
**********************************************************************************************************************
*                                               _debug_display_putchar
*
* Description:
*
* Arguments  :  ch         :  需要打印的字符
*               rest_width :  当前行剩余的宽度
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static int uichar_putchar(__u8 ch)
{
    __s32 ret, width;

    ret = check_change_line(ui_char_info.x, ch);
    if(ret == -1)             //访问失败，当前字符不处理
    {
        return 0;
    }
    else if(ret == 0)        //访问成功，当前字符处理，但是不需要换行
    {
        ;
    }
    else if(ret == 1)        //访问成功，当前字符处理，需要换行
    {
        if(uichar_change_newline( ))
        {
        	ret = -1;
        }
    }

    width = draw_bmp_ulc(ui_char_info.x, ui_char_info.y, ui_source.color);    //显示字符,返回当前显示字符的宽度，像素单位
    ui_char_info.x += width;
    ui_char_info.rest_screen_width -= width;                        //记录当前行剩余的像素
    //调用打印函数

	return ret;
}
/*
**********************************************************************************************************************
*                                               debug_display_putstr
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static int uichar_putstr(const char * str, int length)
{
	int count = 0;

	while( *str != '\0' )
	{
		if(*str == '\n')
		{
            //需要换行的时候，自动切换到下一行开始进行显示
            //调用换行函数
            if(uichar_change_newline( ))
	        {
	        	return -1;
	        }
		}
		else
		{
			if(uichar_putchar(*str))
			{
				return -1;
			}
		}
		str++;
		count ++;
		if(count >= length)
		{
			if(uichar_change_newline( ))
	        {
	        	return -1;
	        }
		}
	}

	return 0;
}
/*
******************************************************************************************************************
*
*Function Name : UART_printf
*
*Description : This function is to formatedly output through UART, similar to ANSI C function printf().
*                This function can support and only support the following Conversion Specifiers:
*              %d		Signed decimal integer.
*              %u		Unsigned decimal integer.
*              %x		Unsigned hexadecimal integer, using hex digits 0f.
*              %X		Unsigned hexadecimal integer, using hex digits 0F.
*              %c		Single character.
*              %s		Character string.
*              %p		A pointer.
*
*Input : refer to ANSI C function printf().
*
*Output : void, different form ANSI C function printf().
*
*call for : void int_to_string_dec( __s32 input, char * str ), defined in format_transformed.c.
*           void int_to_string_hex( __s32 input, char * str );  defined in format_transformed.c.
*           void Uint_to_string_dec( __u32 input, char * str );  defined in format_transformed.c.
*           void UART_putchar( __s32 ch); defined in boot loader.
*           void UART_puts( const char * string ); defined in boot loader.
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void  boot_ui_printf_null(const char * str, ...)
{
	return ;
}


void boot_ui_printf( const char * str, ...)
{
	int  base_color;
	char string[32];
	char *p;
	__s32 hex_flag ;
	va_list argp;

	va_start( argp, str );

	base_color = boot_ui_get_color();
	base_color &= 0xffffff;
	boot_ui_set_color(base_color);
    if(change_line == 1)
    {
        if(uichar_change_newline())
        {
        	__inf("boot ui char: unable to change to one new line\n");

        	goto boot_ui_print_err;
        }
        wBoot_timer_delay(10);
        change_line = 0;
    }

	while( *str )
	{
		if( *str == '%' )
		{
			++str;
			p = string;
			hex_flag = 'X';
			switch( *str )
			{
			    case 'u':
				case 'd':
				{
					int_to_string_dec( va_arg( argp,  __s32 ), string );
                    if(uichar_putstr( p, 32 ))
                    {
                      	goto boot_ui_print_err;
                    }
					++str;
					break;
				}
				case 'x': hex_flag = 'x';	         // jump to " case 'X' "
				case 'p':
				case 'X':
				{
					int_to_string_hex( va_arg( argp,  __s32 ), string, hex_flag );
					if(uichar_putstr( p , 32))
					{
						goto boot_ui_print_err;
					}
                    ++str;
					break;
				}
				case 'c':
				{
					if(uichar_putchar( va_arg( argp,  __s32 ) ))
					{
						goto boot_ui_print_err;
					}
					++str;
					break;
				}
				case 's':
				{
					if(uichar_putstr( va_arg( argp, char * ), 32 ))
					{
						goto boot_ui_print_err;
					}
					++str;
					break;
				}
				default :
				{
					if(uichar_putchar( '%' ))          // if current character is not Conversion Specifiers 'dxpXucs',
					{
						goto boot_ui_print_err;
					}
					if(uichar_putchar( *str ))         // output directly '%' and current character, and then
					{
						goto boot_ui_print_err;
					}
					++str;                        // let 'str' point to next character.
				}
			}
		}
		else
		{
			if( *str == '\n' )                      // if current character is '\n', insert and output '\r'
				change_line = 1;

			if(uichar_putchar( *str++ ))
			{
				goto boot_ui_print_err;
			}
		}
	}

	va_end( argp );

boot_ui_print_err:
	base_color |= 0xff000000;
	boot_ui_set_color(base_color);

	return;
}



