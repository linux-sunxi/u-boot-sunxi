
#ifndef __DISP_LCD_H__
#define __DISP_LCD_H__

#include "disp_display_i.h"


__s32 Disp_lcdc_init(__u32 sel);
__s32 Disp_lcdc_exit(__u32 sel);

#ifdef __LINUX_OSAL__
__s32 Disp_lcdc_event_proc(__s32 irq, void *parg);
#else
__s32 Disp_lcdc_event_proc(void *parg);
#endif
__s32 Disp_lcdc_pin_cfg(__u32 sel, __disp_output_type_t out_type, __u32 bon);
__u32 Disp_get_screen_scan_mode(__disp_tv_mode_t tv_mode);
__s32 TCON_get_cur_line(__u32 sel, __u32 tcon_index);
__s32 TCON_get_start_delay(__u32 sel, __u32 tcon_index);



__u32 tv_mode_to_width(__disp_tv_mode_t mode);
__u32 tv_mode_to_height(__disp_tv_mode_t mode);
__u32 vga_mode_to_width(__disp_vga_mode_t mode);
__u32 vga_mode_to_height(__disp_vga_mode_t mode);

void LCD_delay_ms(__u32 ms) ;
void LCD_delay_us(__u32 ns);

__s32 LCD_GPIO_request(__u32 sel, __u32 io_index);
__s32 LCD_GPIO_release(__u32 sel,__u32 io_index);
__s32 LCD_GPIO_set_attr(__u32 sel,__u32 io_index, __bool b_output);
__s32 LCD_GPIO_read(__u32 sel,__u32 io_index);
__s32 LCD_GPIO_write(__u32 sel,__u32 io_index, __u32 data);

extern void LCD_get_panel_funs_0(__lcd_panel_fun_t * fun);
extern void LCD_get_panel_funs_1(__lcd_panel_fun_t * fun);

#endif
