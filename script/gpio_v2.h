/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm boot sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : gpiov2.h
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
#ifndef	  __GPIO_V2_H__
#define	  __GPIO_V2_H__

#define   EGPIO_FAIL             (-1)
#define   EGPIO_SUCCESS          (0)

typedef enum
{
	PIN_PULL_DEFAULT 	= 	0xFF,
	PIN_PULL_DISABLE 	=	0x00,
	PIN_PULL_UP			=	0x01,
	PIN_PULL_DOWN		=	0x02,
	PIN_PULL_RESERVED	=	0x03
}pin_pull_level_t;



typedef	enum
{
	PIN_MULTI_DRIVING_DEFAULT	=	0xFF,
	PIN_MULTI_DRIVING_0			=	0x00,
	PIN_MULTI_DRIVING_1			=	0x01,
	PIN_MULTI_DRIVING_2			=	0x02,
	PIN_MULTI_DRIVING_3			=	0x03
}pin_drive_level_t;

typedef enum
{
    PIN_DATA_LOW    ,
    PIN_DATA_HIGH   ,
    PIN_DATA_DEFAULT = 0XFF
}pin_data_t;


#define	PIN_PHY_GROUP_A			0x00
#define	PIN_PHY_GROUP_B			0x01
#define	PIN_PHY_GROUP_C			0x02
#define	PIN_PHY_GROUP_D			0x03
#define	PIN_PHY_GROUP_E			0x04
#define	PIN_PHY_GROUP_F			0x05
#define	PIN_PHY_GROUP_G			0x06
#define	PIN_PHY_GROUP_H			0x07
#define	PIN_PHY_GROUP_I			0x08
#define	PIN_PHY_GROUP_J			0x09



#endif	//__GPIO_V2_H__

