#include "lcd_edp_anx6345.h"

/**********************************************************************
 *
 *  anx6345 function
 *
 **********************************************************************/
__u8 SP_TX_I2C_Write_BYTE(__u8 dev_addr, __u8 offset, __u8 d)
{
	lcd_iic_write(dev_addr, offset, d);
	return 0;
}

__u8 SP_TX_I2C_Read_BYTE(__u8 dev_addr, __u8 offset, __u8 *d)
{
	lcd_iic_read(dev_addr, offset, d);
	return 0;
}

__u8 SP_TX_Write_Reg(__u8 dev_addr, __u8 offset, __u8 d)
{
	__u8 cResult;

	//SP_TX_I2C_Write_BYTE( dev_addr,  offset,	d);

	cResult = SP_TX_I2C_Write_BYTE( dev_addr,  offset,	d);

	if(cResult !=0)
	{
		//debug_printf("write eror:result code=%.2x,dev addr= %.2x, offset=%.2x,d=%x\n",(WORD)cResult,(WORD)dev_addr, (WORD)offset,(WORD)d);
	}

	  return 1;

}

__u8 SP_TX_Read_Reg(__u8 dev_addr, __u8 offset, __u8 *d)
{
	__u8 cResult;

	//SP_TX_I2C_Read_BYTE( dev_addr,  offset,  d);

		
	cResult = SP_TX_I2C_Read_BYTE( dev_addr,  offset,  d);

	if ( cResult!= 0 ) 
	{
		//debug_printf("read eror:result code = %.2x,dev addr= %.2x, offset=%.2x,d=%.2x\n",(WORD)cResult,(WORD)dev_addr, (WORD)offset,(WORD)(*d));
	
	}
	
	   return 1;
}


//---------------------------------------------------------
/*

#define lcd_reset_set_output()	LCD_GPIO_set_attr(0, 2, 1)
#define lcd_reset_set_value_0() LCD_GPIO_write(0, 2, 0)
#define lcd_reset_set_value_1() LCD_GPIO_write(0, 2, 1)
*/


void anx6345_init(__panel_para_t * info)
{
		__u8 c;
		__u32 count = 0;
		
	/*   
	   lcd_reset_set_output();
	   lcd_reset_set_value_0();
	   LCD_delay_ms(500);
	   lcd_reset_set_value_1();		   //reset
*/
	
		SP_TX_Write_Reg (0x72, 0x05, 0x00);


		 SP_TX_Read_Reg(0x72, 0x01, &c);

		 if(c==0xaa)
		 	{
		 		OSAL_PRINTF("ANX6345 Chip found\n\n\n");
		 	}
		 else
		 	{
		 		OSAL_PRINTF("ANX6345 Chip not found\n\n\n");
		 	}

	
		 //software reset
		 SP_TX_Read_Reg(SP_TX_PORT2_ADDR, SP_TX_RST_CTRL_REG, &c);
		 SP_TX_Write_Reg(SP_TX_PORT2_ADDR, SP_TX_RST_CTRL_REG, c | SP_TX_RST_SW_RST);
		 SP_TX_Write_Reg(SP_TX_PORT2_ADDR, SP_TX_RST_CTRL_REG, c & ~SP_TX_RST_SW_RST);
	
		 SP_TX_Write_Reg(SP_TX_PORT0_ADDR, SP_TX_EXTRA_ADDR_REG, 0x50);//EDID address for AUX access
		 SP_TX_Write_Reg(SP_TX_PORT0_ADDR, SP_TX_HDCP_CTRL, 0x00);	//disable HDCP polling mode.
		 //SP_TX_Write_Reg(SP_TX_PORT0_ADDR, SP_TX_HDCP_CTRL, 0x02);	//Enable HDCP polling mode.
		 SP_TX_Write_Reg(SP_TX_PORT0_ADDR, SP_TX_LINK_DEBUG_REG, 0x30);//enable M value read out
	
		 //SP_TX_Read_Reg(SP_TX_PORT0_ADDR, SP_TX_DEBUG_REG1, &c);
		 SP_TX_Write_Reg(SP_TX_PORT0_ADDR, SP_TX_DEBUG_REG1, 0x00);//disable polling HPD
	
			 SP_TX_Read_Reg(SP_TX_PORT0_ADDR, SP_TX_HDCP_CONTROL_0_REG, &c);
		 SP_TX_Write_Reg(SP_TX_PORT0_ADDR, SP_TX_HDCP_CONTROL_0_REG, c | 0x03);//set KSV valid
	
		 SP_TX_Read_Reg(SP_TX_PORT0_ADDR, SP_TX_AUX_CTRL_REG2, &c);
		 SP_TX_Write_Reg(SP_TX_PORT0_ADDR, SP_TX_AUX_CTRL_REG2, c|0x08);//set double AUX output
	
		 SP_TX_Write_Reg(SP_TX_PORT2_ADDR, SP_COMMON_INT_MASK1, 0xbf);//unmask pll change int
		 SP_TX_Write_Reg(SP_TX_PORT2_ADDR, SP_COMMON_INT_MASK2, 0xff);//mask all int
		 SP_TX_Write_Reg(SP_TX_PORT2_ADDR, SP_COMMON_INT_MASK3, 0xff);//mask all int
		 SP_TX_Write_Reg(SP_TX_PORT2_ADDR, SP_COMMON_INT_MASK4, 0xff);//mask all int
	
	
		//reset AUX
		SP_TX_Read_Reg(SP_TX_PORT2_ADDR, SP_TX_RST_CTRL2_REG, &c);
		SP_TX_Write_Reg(SP_TX_PORT2_ADDR, SP_TX_RST_CTRL2_REG, c |SP_TX_AUX_RST);
		SP_TX_Write_Reg(SP_TX_PORT2_ADDR, SP_TX_RST_CTRL2_REG, c & (~SP_TX_AUX_RST));
	
		 //Chip initialization
	
		 SP_TX_Write_Reg(0x70, SP_TX_SYS_CTRL1_REG, 0x00);
		 LCD_delay_ms(10);
#if 0
		 for(i=0;i<50;i++)
		{
			SP_TX_Read_Reg(0x70, SP_TX_SYS_CTRL1_REG, &c);
		//	SP_TX_Write_Reg(0x70, SP_TX_SYS_CTRL1_REG, c);
		//	SP_TX_Read_Reg(0x70, SP_TX_SYS_CTRL1_REG, &c);
			if((c&SP_TX_SYS_CTRL1_DET_STA)==0x04)
			{
				//debug_puts("clock is detected.\n");
				break;
			}
	
			LCD_delay_ms(10);
		}
#endif
		   //check whether clock is stable
	
	/*	for(i=0;i<50;i++)
		{
	
			SP_TX_Read_Reg(0x70, SP_TX_SYS_CTRL2_REG, &c);
			if((c&SP_TX_SYS_CTRL2_CHA_STA)==0x0)
			{
				debug_puts("clock is stable.\n");
				break;
			}
			LCD_delay_ms(10);
		}
	*/
	
			//VESA range, 8bits BPC, RGB
		SP_TX_Write_Reg(0x72, SP_TX_VID_CTRL2_REG, 0x10);
	
		//ANX6345 chip analog setting
		SP_TX_Write_Reg(0x70, SP_TX_PLL_CTRL_REG, 0x00);				  //UPDATE: FROM 0X07 TO 0X00
	
	
		//ANX chip analog setting
	//	SP_TX_Write_Reg(0x72, ANALOG_DEBUG_REG1, 0x70); 			  //UPDATE: FROM 0XF0 TO 0X70
		SP_TX_Write_Reg(0x70, SP_TX_LINK_DEBUG_REG, 0x30);
	
		//force HPD
		SP_TX_Write_Reg(0x70, SP_TX_SYS_CTRL3_REG, 0x30);
		//Select 2.7G
		   SP_TX_Write_Reg(0x70, SP_TX_LINK_BW_SET_REG, 0x0a);		//2.7g:0x0a;1.62g:0x06
		//Select 2 lanes
		SP_TX_Write_Reg(0x70, 0xa1, 0x02);
	
		 SP_TX_Write_Reg(0x70, SP_TX_LINK_TRAINING_CTRL_REG, SP_TX_LINK_TRAINING_CTRL_EN);
		LCD_delay_ms(5);
		SP_TX_Read_Reg(0x70, SP_TX_LINK_TRAINING_CTRL_REG, &c);
		while((c&0x80)!=0)																				  //UPDATE: FROM 0X01 TO 0X80
		{
			//debug_puts("Waiting...\n");
			LCD_delay_ms(5);
			count ++;
			if(count > 100)
			{
				OSAL_PRINTF("ANX6345 Link trainning fail...\n");
				break;				
			}
			SP_TX_Read_Reg(0x70, SP_TX_LINK_TRAINING_CTRL_REG, &c);
		}
	
	
	
		SP_TX_Write_Reg(0x72, 0x12, 0x2c);
		SP_TX_Write_Reg(0x72, 0x13, 0x06);
		SP_TX_Write_Reg(0x72, 0x14, 0x00);
		SP_TX_Write_Reg(0x72, 0x15, 0x06);
		SP_TX_Write_Reg(0x72, 0x16, 0x02);
		SP_TX_Write_Reg(0x72, 0x17, 0x04);
		SP_TX_Write_Reg(0x72, 0x18, 0x26);
		SP_TX_Write_Reg(0x72, 0x19, 0x50);
		SP_TX_Write_Reg(0x72, 0x1a, 0x04);
		SP_TX_Write_Reg(0x72, 0x1b, 0x00);
		SP_TX_Write_Reg(0x72, 0x1c, 0x04);
		SP_TX_Write_Reg(0x72, 0x1d, 0x18);
		SP_TX_Write_Reg(0x72, 0x1e, 0x00);
		SP_TX_Write_Reg(0x72, 0x1f, 0x10);
		SP_TX_Write_Reg(0x72, 0x20, 0x00);
		SP_TX_Write_Reg(0x72, 0x21, 0x28);
	
		//SP_TX_Write_Reg(0x72, 0x11, 0x03);
	/*
		SP_TX_Write_Reg(0x72, 0x12, 0xee);
		SP_TX_Write_Reg(0x72, 0x13, 0x02);
		SP_TX_Write_Reg(0x72, 0x14, 0xd0);
		SP_TX_Write_Reg(0x72, 0x15, 0x02);
		SP_TX_Write_Reg(0x72, 0x16, 0x05);
		SP_TX_Write_Reg(0x72, 0x17, 0x05);
		SP_TX_Write_Reg(0x72, 0x18, 0x14);
		SP_TX_Write_Reg(0x72, 0x19, 0x39);
		SP_TX_Write_Reg(0x72, 0x1a, 0x03);
		SP_TX_Write_Reg(0x72, 0x1b, 0x80);
		SP_TX_Write_Reg(0x72, 0x1c, 0x02);
		SP_TX_Write_Reg(0x72, 0x1d, 0x37);
		SP_TX_Write_Reg(0x72, 0x1e, 0x00);
		SP_TX_Write_Reg(0x72, 0x1f, 0x14);
		SP_TX_Write_Reg(0x72, 0x20, 0x00);
		SP_TX_Write_Reg(0x72, 0x21, 0x6e);
	*/
		SP_TX_Write_Reg(0x72, 0x11, 0x00);
	
		   //enable BIST. In normal mode, don't need to config this reg
		//SP_TX_Write_Reg(0x72, 0x0b, 0x09);//colorbar:08,graystep:09
	
		//enable video input, set DDR mode, the input DCLK should be 102.5MHz;
		//In normal mode, set this reg to 0x81, SDR mode, the input DCLK should be 205MHz
		SP_TX_Write_Reg(0x72, 0x08, 0x83);		//SDR:0x81;DDR:0x8f
	
		   //force HPD and stream valid
		SP_TX_Write_Reg(0x70, 0x82, 0x33);


}





