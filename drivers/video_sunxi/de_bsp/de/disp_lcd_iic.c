#include "disp_lcd_iic.h"

/**********************************************************************
 *
 *  iic function
 *
 **********************************************************************/

/*
#define IIC_SCLB_LOW()				lcd_gpio0_set_value(0)
#define IIC_SCLB_HIGH()				lcd_gpio0_set_value(1)

#define IIC_SDAB_INPUT_SETUP()		lcd_gpio1_set_input()
#define IIC_SDAB_OUTPUT_SETUP()		lcd_gpio1_set_output()
#define CHECK_SDAB_HIGH()		 	lcd_gpio1_get_value()
#define IIC_SDAB_LOW()				lcd_gpio1_set_value(0)
#define IIC_SDAB_HIGH()				lcd_gpio1_set_value(1)

*/
inline void IIC_SCLB_LOW(void)
{
    LCD_GPIO_set_attr(0, LCD_GPIO_SCL, 1);
    LCD_GPIO_write(0, LCD_GPIO_SCL, 0);
}

inline void IIC_SCLB_HIGH(void)
{
    LCD_GPIO_set_attr(0, LCD_GPIO_SCL, 0);
}

inline void IIC_SDAB_LOW(void)
{
    LCD_GPIO_set_attr(0, LCD_GPIO_SDA, 1);
    LCD_GPIO_write(0, LCD_GPIO_SDA, 0);
}

inline void IIC_SDAB_HIGH(void)
{
    LCD_GPIO_set_attr(0, LCD_GPIO_SDA, 0);
}

inline __u32 CHECK_SDAB_HIGH(void)
{
    LCD_GPIO_set_attr(0, LCD_GPIO_SDA, 0);
    return LCD_GPIO_read(0, LCD_GPIO_SDA);
}
    
__bool i2cB_clock( void )
{
	__bool sample = 0;
    
	LCD_delay_us(5); 
	IIC_SCLB_HIGH();
	LCD_delay_us(5); 
	IIC_SCLB_LOW();
	return (sample);
}

__bool i2cB_ack(void)
{
	LCD_delay_us(5);
	IIC_SCLB_HIGH();
	LCD_delay_us(5);
	if(CHECK_SDAB_HIGH())
	{
		LCD_delay_us(5); 
		IIC_SCLB_LOW();
		LCD_delay_us(5); 
		IIC_SDAB_HIGH();
		LCD_delay_us(5); 
		return(1);
	}
	else
	{
//		LCD_delay_us(5);
		IIC_SCLB_LOW();
		LCD_delay_us(5);
		IIC_SDAB_HIGH();
		return(0);
	}
}

//---------------------------------------------------------
void i2cBStartA( void )
{
	IIC_SCLB_HIGH();
	IIC_SDAB_HIGH();
	LCD_delay_us(5); 
	IIC_SDAB_LOW();
	LCD_delay_us(5); 
	IIC_SCLB_LOW();
}

__bool i2cBStart( void )
{
	IIC_SDAB_HIGH();
	IIC_SCLB_HIGH();
	LCD_delay_us(5); 
	if(CHECK_SDAB_HIGH())
	{
		{
			i2cBStartA();
			return(1);
		}
	}
	return(0);
}


void i2cBStop(void)
{
   IIC_SDAB_LOW();
   LCD_delay_us(5); 
   IIC_SCLB_HIGH();
   LCD_delay_us(5); 
   IIC_SDAB_HIGH();
   LCD_delay_us(5); 
}
//---------------------------------------------------------
__bool i2cBTransmit(__u8 value)
{
	register __u8 i ;
	for ( i=0 ; i<8 ; i++ )
	{
		if((value&0x80)==0x80)
		{
			IIC_SDAB_HIGH();
		}
		else
		{
			IIC_SDAB_LOW();
		}
		value = value << 1;
		i2cB_clock();
	}
	return(!i2cB_ack());
}

__bool i2cBLocateSubAddr(__u8 slave_addr, __u8 sub_addr)
{
//	register __u8 i;
//	for (i=0; i<3; i++)
	{
		//Start I2C
		if (i2cBStart())
		{
			//Slave address
			if (i2cBTransmit(slave_addr))
			{
				if (i2cBTransmit(sub_addr))
					return(1);
			}
		}
		i2cBStop();
	}
	return(0);
}

//---------------------------------------------------------
__bool i2cBReceive(__u8* value)
{
	register __u8 i ;
	*value = 0;
	for ( i=0 ; i<8 ; i++ )
	{
		IIC_SCLB_HIGH();
		LCD_delay_us(5); 
		if(CHECK_SDAB_HIGH())
		{
			*value |= (1<<(7-i));
		}
		IIC_SCLB_LOW();
		LCD_delay_us(5);
	}
	IIC_SDAB_HIGH();
	IIC_SCLB_HIGH();
	LCD_delay_us(5);
	IIC_SCLB_LOW();
	IIC_SDAB_HIGH();
	LCD_delay_us(5);
	return(1);
}

__bool i2cBLocateSubDataR(__u8 slave_addr, __u8* value)
{
	register __u8 i;
	for (i=0; i<3; i++)
	{
		//Start I2C
		if (i2cBStart())
		{
			//Slave address
			if (i2cBTransmit(slave_addr|1))
			{
				if (i2cBReceive(value))
					return(1);
			}
		}
		i2cBStop();
	}
	return(0);
}
//---------------------------------------------------------
__s32 lcd_iic_write(__u8 slave_addr, __u8 sub_addr, __u8 value)
{
	if (i2cBLocateSubAddr(slave_addr, sub_addr))
	{
		//value
		if (i2cBTransmit(value))
		{
			i2cBStop();
			return(1);
		}
	}
	i2cBStop();
	return(0);
}


__s32 lcd_iic_read(__u8 slave_addr, __u8 sub_addr, __u8* value)
{
	if (i2cBLocateSubAddr(slave_addr, sub_addr))
		i2cBStop();
	LCD_delay_us(10);
	if (i2cBLocateSubDataR(slave_addr,value))
		i2cBStop();
	return(1);
}

