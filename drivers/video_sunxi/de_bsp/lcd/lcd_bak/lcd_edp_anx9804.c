#include "lcd_edp_anx9804.h"


void anx9804_init(__panel_para_t * info)
{
	__u8 c;
  __s32 i;
  __u32 count = 0;

	//HW reset
	lcd_iic_write(0x72, DP_TX_RST_CTRL_REG, DP_TX_RST_HW_RST);
	LCD_delay_ms(100);
	lcd_iic_write(0x72, DP_TX_RST_CTRL_REG, 0x00);
	//Power on total and select DP mode
  lcd_iic_write(0x72, DP_POWERD_CTRL_REG, 0x00 );
	
	//get chip ID. Make sure I2C is OK
	lcd_iic_read(0x72, DP_TX_DEV_IDH_REG , &c);
	if(c==0x98)
	{
		OSAL_PRINTF("ANX9804 Chip found\n");
	}
	else
	{
		OSAL_PRINTF("ANX9804 Chip not found\n");
	}
#if 0
	//for clock detect
	for(i=0;i<50;i++)
	{
		lcd_iic_read(0x70, DP_TX_SYS_CTRL1_REG, &c);
		lcd_iic_write(0x70, DP_TX_SYS_CTRL1_REG, c);
		lcd_iic_read(0x70, DP_TX_SYS_CTRL1_REG, &c);
		if((c&DP_TX_SYS_CTRL1_DET_STA)!=0)
		{
			OSAL_PRINTF("ANX9804 clock is detected.\n");
			break;
		}

		LCD_delay_ms(10);
	}
#endif
       //check whether clock is stable
	for(i=0;i<50;i++)
	{
		lcd_iic_read(0x70, DP_TX_SYS_CTRL2_REG, &c);
		lcd_iic_write(0x70, DP_TX_SYS_CTRL2_REG, c);
		lcd_iic_read(0x70, DP_TX_SYS_CTRL2_REG, &c);
		if((c&DP_TX_SYS_CTRL2_CHA_STA)==0)
		{
			OSAL_PRINTF("ANX9804 clock is stable.\n");
			break;
		}
		LCD_delay_ms(10);
	}

	//VESA range, 8bits BPC, RGB 
	lcd_iic_write(0x72, DP_TX_VID_CTRL2_REG, 0x10);
	
	//ANX9804 chip analog setting
	lcd_iic_write(0x70, DP_TX_PLL_CTRL_REG, 0x07); 
	lcd_iic_write(0x72, DP_TX_PLL_FILTER_CTRL3, 0x19); 
	lcd_iic_write(0x72, DP_TX_PLL_CTRL3, 0xd9); 
	
	//lcd_iic_write(0x7a, 0x38, 0x10); 
	//lcd_iic_write(0x7a, 0x39, 0x20); 
	//lcd_iic_write(0x7a, 0x65, 0x00); 
	
	//Select AC mode
	lcd_iic_write(0x72, DP_TX_RST_CTRL2_REG, 0x40); 
	
	//lcd_iic_write(0x7a, 0x61, 0x10); 
	//lcd_iic_write(0x7a, 0x62, 0x10); 
	//lcd_iic_write(0x7a, 0x63, 0x10); 
	//lcd_iic_write(0x7a, 0x64, 0x10); 

	//ANX9804 chip analog setting
	lcd_iic_write(0x72, ANALOG_DEBUG_REG1, 0xf0);
	lcd_iic_write(0x72, ANALOG_DEBUG_REG3, 0x99);
	lcd_iic_write(0x72, DP_TX_PLL_FILTER_CTRL1, 0x7b);
	lcd_iic_write(0x70, DP_TX_LINK_DEBUG_REG, 0x30);
	lcd_iic_write(0x72, DP_TX_PLL_FILTER_CTRL, 0x06);

	//force HPD
	lcd_iic_write(0x70, DP_TX_SYS_CTRL3_REG, 0x30);
	//power on 4 lanes
	lcd_iic_write(0x70, 0xc8, 0x00);
	//lanes setting
	lcd_iic_write(0x70, 0xa3, 0x00);
	lcd_iic_write(0x70, 0xa4, 0x00);
	lcd_iic_write(0x70, 0xa5, 0x00);
	lcd_iic_write(0x70, 0xa6, 0x00);

#if 0
	//step 1: read DPCD 0x00001, the correct value should be 0x0a, or 0x06
	lcd_iic_write(0x70,  0xE4,  0x80);

	//set read cmd and count, read 2 __u8s data, get downstream max_bandwidth and max_lanes
	lcd_iic_write(0x70, 0xE5,  0x19);

	//set aux address19:0
	lcd_iic_write(0x70,  0xE6,  0x01);
	lcd_iic_write(0x70,  0xE7,  0x00);
	lcd_iic_write(0x70,  0xE8,  0x00);

	//Enable Aux
	lcd_iic_write(0x70,  0xE9, 0x01);

	//wait aux finished
	for(i=0; i<50; i++)
	{
	  lcd_iic_read(0x70,  0xE9,  &c);
	  if(c==0x00)
	  {
	    break;
	  }
	}

	//read data from buffer
	lcd_iic_write(  0x70,  0xF0,   &max_bandwidth);
	lcd_iic_write(  0x70,  0xF1,   &max_lanes);
	debug_pr__s32f("max_bandwidth = %.2x, max_lanes = %.2x\n", (WORD)max_bandwidth, (WORD)max_lanes);
#endif

	//reset AUX CH
	lcd_iic_write(0x72,  DP_TX_RST_CTRL2_REG,  0x44);
	lcd_iic_write(0x72,  DP_TX_RST_CTRL2_REG,  0x40);

	//to save power
	lcd_iic_write(0x72, DP_POWERD_CTRL_REG, 0x10 );//audio power down
	lcd_iic_write(0x70, DP_TX_HDCP_CONTROL_0_REG, 0x00 );
	lcd_iic_write(0x70, 0xA7, 0x00 );//Spread spectrum 30 kHz
	//end

	//Select 2.7G
	//lcd_iic_write(0x70, DP_TX_LINK_BW_SET_REG, 0x0a);
	lcd_iic_write(0x70, DP_TX_LINK_BW_SET_REG, 0x06);	//Select 1.62G

	//Select 4 lanes
	lcd_iic_write(0x70, DP_TX_LANE_COUNT_SET_REG, 0x04);
	
	//strart link traing
	//DP_TX_LINK_TRAINING_CTRL_EN is self clear. If link training is OK, it will self cleared.
	lcd_iic_write(0x70, DP_TX_LINK_TRAINING_CTRL_REG, DP_TX_LINK_TRAINING_CTRL_EN);
	LCD_delay_ms(5);
	lcd_iic_read(0x70, DP_TX_LINK_TRAINING_CTRL_REG, &c);
	while((c&0x01)!=0)
	{
		OSAL_PRINTF("ANX9804 Waiting...\n");
		LCD_delay_ms(5);
		count ++;
		if(count > 100)
		{
			OSAL_PRINTF("ANX9804 Link trainning fail...\n");
			break;			
		}
		lcd_iic_read(0x70, DP_TX_LINK_TRAINING_CTRL_REG, &c);
	}
	//lcd_iic_write(0x7a, 0x7c, 0x02);  	

    //BIST MODE: video format. In normal mode, don't need to config these reg from 0x12~0x21
	//lcd_iic_write(0x72, 0x12, 0x2c);
	//lcd_iic_write(0x72, 0x13, 0x06);
	//lcd_iic_write(0x72, 0x14, 0x00);
	//lcd_iic_write(0x72, 0x15, 0x06);
	//lcd_iic_write(0x72, 0x16, 0x02);
	//lcd_iic_write(0x72, 0x17, 0x04);
	//lcd_iic_write(0x72, 0x18, 0x26);
	//lcd_iic_write(0x72, 0x19, 0x50);
	//lcd_iic_write(0x72, 0x1a, 0x04);
	//lcd_iic_write(0x72, 0x1b, 0x00);
	//lcd_iic_write(0x72, 0x1c, 0x04);
	//lcd_iic_write(0x72, 0x1d, 0x18);
	//lcd_iic_write(0x72, 0x1e, 0x00);
	//lcd_iic_write(0x72, 0x1f, 0x10);
	//lcd_iic_write(0x72, 0x20, 0x00);
	//lcd_iic_write(0x72, 0x21, 0x28);

	//lcd_iic_write(0x72, 0x11, 0x03);
	
    //enable BIST. In normal mode, don't need to config this reg
	//lcd_iic_write(0x72, 0x0b, 0x08);
	
	//enable video input, set DDR mode, the input DCLK should be 102.5MHz; 
	//In normal mode, set this reg to 0x81, SDR mode, the input DCLK should be 205MHz
	//lcd_iic_write(0x72, 0x08, 0x8d);
	//lcd_iic_write(0x72, 0x08, 0x81);
	lcd_iic_write(0x72, 0x08, 0x81);
	
    //force HPD and stream valid
	lcd_iic_write(0x70, 0x82, 0x33);
}


