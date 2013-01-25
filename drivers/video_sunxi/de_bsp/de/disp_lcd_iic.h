#include "disp_display_i.h"
#include "disp_lcd.h"
#include "disp_display.h"

/**********************************************************************
 *
 *  iic function
 *
 **********************************************************************/
/*
#define IIC_SCLB_LOW()	        LCD_GPIO_write(0, 0, 0)
#define IIC_SCLB_HIGH()	        LCD_GPIO_write(0, 0, 1)		

#define IIC_SDAB_INPUT_SETUP()	LCD_GPIO_set_attr(0, 1, 0)
#define IIC_SDAB_OUTPUT_SETUP()	LCD_GPIO_set_attr(0, 1, 1)
#define CHECK_SDAB_HIGH()       LCD_GPIO_read(0, 1)	 
#define IIC_SDAB_LOW()          LCD_GPIO_write(0, 1, 0)
#define IIC_SDAB_HIGH()	        LCD_GPIO_write(0, 1, 1)	
*/


//---------------------------------------------------------
__s32 lcd_iic_write(__u8 slave_addr, __u8 sub_addr, __u8 value);
__s32 lcd_iic_read(__u8 slave_addr, __u8 sub_addr, __u8* value);

