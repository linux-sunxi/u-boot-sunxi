/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Berg Xing <bergxing@allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Sunxi platform dram controller init.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/dram.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>

 
//*****************************************************************************
//	Allwinner Technology, All Right Reserved. 2006-2010 Copyright (c)
//
//	File: 				mctl_hal.c
//
//	Description:  This file implements basic functions for AW1633 DRAM controller
//		 
//	History:
//              2012/02/06      Berg Xing       0.10    Initial version
//              2012/02/24      Berg Xing       0.20    Supprot 2 channel
//              2012/02/27      Berg Xing       0.30    modify mode register access
//				2012/03/01		Berg Xing       0.40    add LPDDR2
//				2012/03/10		Berg Xing       0.50    add mctl_dll_init() function
//*****************************************************************************
void aw_delay (u32 n)
{
	while(n--);
}
uint32 mctl_sys_init(void)
{
	uint32 reg_val;
		
  	//PLL5 enable
	reg_val = mctl_read_w(CCU_PLL5CFG);
  	reg_val |= 0x1U<<31;
  	reg_val |= 0x1U<<20;
  	mctl_write_w(CCU_PLL5CFG, reg_val);
#ifndef SYSTEM_SIMULATION
  	aw_delay(0x20);
#endif
  
  	//MDFS clock enable
	reg_val = mctl_read_w(CCU_MDFS_CLK);
	reg_val |= 0x1U<<31;
  	mctl_write_w(CCU_MDFS_CLK, reg_val);
  	
  	//select DRAM clock
  	reg_val = mctl_read_w(CCU_MDFS_CFG);
	reg_val |= 0x1U<<16;
  	mctl_write_w(CCU_MDFS_CFG, reg_val);
	
  	//release DRAMC register reset
  	reg_val = mctl_read_w(CCU_AHB1_RST);
  	reg_val |= 0x1<<14;
  	mctl_write_w(CCU_AHB1_RST, reg_val);
  	
  	//DRAMC AHB clok on
  	reg_val = mctl_read_w(CCU_AHBGATE0);
  	reg_val |= 0x1<<14;
  	mctl_write_w(CCU_AHBGATE0, reg_val);
  	
  	return (1);
}

uint32 mctl_reset_release(void)
{
	uint32 reg_val;
		
  	reg_val = mctl_read_w(CCU_MDFS_CFG);
  	reg_val |= 0x1U<<31;
  	mctl_write_w(CCU_MDFS_CFG, reg_val);
#ifndef SYSTEM_SIMULATION
  	aw_delay(0x20);
#endif
    	
  	return (1);
}

uint32 mctl_dll_init(uint32 ch_index)
{
	uint32 ch_id;
	
	if(ch_index == 1)
		ch_id = 0x1000;	
	else
		ch_id = 0x0;
	
   //*********************************************** 
   // set dram PHY register
   //*********************************************** 
	//reset dll
	mctl_write_w(ch_id + SDR_ACDLLCR,0x80000000);
	mctl_write_w(ch_id + SDR_DX0DLLCR,0x80000000);
	mctl_write_w(ch_id + SDR_DX1DLLCR,0x80000000);
	mctl_write_w(ch_id + SDR_DX2DLLCR,0x80000000);
	mctl_write_w(ch_id + SDR_DX3DLLCR,0x80000000);
#ifndef SYSTEM_SIMULATION
	aw_delay(0x10);
#endif
	//enable dll
	mctl_write_w(ch_id + SDR_ACDLLCR,0x0);
	mctl_write_w(ch_id + SDR_DX0DLLCR,0x0);
	mctl_write_w(ch_id + SDR_DX1DLLCR,0x0);
	mctl_write_w(ch_id + SDR_DX2DLLCR,0x0);
	mctl_write_w(ch_id + SDR_DX3DLLCR,0x0);
#ifndef SYSTEM_SIMULATION
	aw_delay(0x10);	
#endif
	//release reset dll
	mctl_write_w(ch_id + SDR_ACDLLCR,0x40000000);
	mctl_write_w(ch_id + SDR_DX0DLLCR,0x40000000);
	mctl_write_w(ch_id + SDR_DX1DLLCR,0x40000000);
	mctl_write_w(ch_id + SDR_DX2DLLCR,0x40000000);
	mctl_write_w(ch_id + SDR_DX3DLLCR,0x40000000);
#ifndef SYSTEM_SIMULATION
	aw_delay(0x10);	
#endif
	
	return (1);
}

uint32 mctl_channel_init(uint32 ch_index)
{
	uint32 reg_val;
	uint32 clkmhz;
	uint32 ch_id;
	
	if(ch_index == 1)
		ch_id = 0x1000;	
	else
		ch_id = 0x0;
   
   //set PHY genereral configuration register
   reg_val = 0x01042202;
   mctl_write_w(ch_id + SDR_PGCR, reg_val);
   
   //set mode register
	mctl_write_w(ch_id + SDR_MR0, MCTL_MR0);	
	mctl_write_w(ch_id + SDR_MR1, MCTL_MR1);
	mctl_write_w(ch_id + SDR_MR2, MCTL_MR2);
	mctl_write_w(ch_id + SDR_MR3, MCTL_MR3);   
   
	//set PHY DDR mode
	if(MCTL_DDR_TYPE==2)		//DDR2
		reg_val = 0xa;
	else if(MCTL_DDR_TYPE==3)	//DDR3
		reg_val = 0xb;
	else if(MCTL_DDR_TYPE==5)	//LPDDR
		reg_val = 0x8;
	else						//LPDDR2
		reg_val = 0xc;			
	mctl_write_w(ch_id + SDR_DCR, reg_val);	
	
#ifndef FPGA_PLATFORM	
	//set DDR system general configuration register
	reg_val = 0xd200013b;
	mctl_write_w(ch_id + SDR_DSGCR, reg_val);
	
	//set DATX8 common configuration register
	reg_val = 0x910;
	mctl_write_w(ch_id + SDR_DXCCR, reg_val);	
#endif		

	//set COM sclk enable register
	reg_val = mctl_read_w(SDR_COM_CCR);
	reg_val |= (0x4 | (0x1<<ch_index));
	mctl_write_w(SDR_COM_CCR, reg_val);

   //*********************************************** 
   // check dram PHY status
   //*********************************************** 
	while( (mctl_read_w(ch_id + SDR_PGSR)&0x3)!= 0x3 ) {};	
	
	//init external dram
#ifdef FPGA_PLATFORM
	reg_val = 0x69;	
#else	
	reg_val = 0xe9;
#endif	
	mctl_write_w(ch_id + SDR_PIR, reg_val);
	
#ifndef SYSTEM_SIMULATION
	aw_delay(0x10);
#endif
	
	//wait init done
	while( (mctl_read_w(ch_id + SDR_PGSR)&0x1) == 0x0) {};	

   //*********************************************** 
   // set dram MCTL register
   //*********************************************** 	
	//move to configure state
	reg_val = 0x1;
	mctl_write_w(ch_id + SDR_SCTL, reg_val);	
	while(  (mctl_read_w(ch_id + SDR_SSTAT)&0x7) != 0x1 ) {};	
	
	//set memory timing regitsers	
	clkmhz = MCTL_CLK;
	clkmhz = clkmhz/1000000;
	reg_val = clkmhz;
	mctl_write_w(ch_id + SDR_TOGCNT1U, reg_val);		//1us
	reg_val = clkmhz/10;
	mctl_write_w(ch_id + SDR_TOGCNT100N, reg_val);		//100ns
	mctl_write_w(ch_id + SDR_TREFI	,MCTL_TREFI);
	mctl_write_w(ch_id + SDR_TMRD	,MCTL_TMRD);
	mctl_write_w(ch_id + SDR_TRFC	,MCTL_TRFC);
	mctl_write_w(ch_id + SDR_TRP	,MCTL_TRP|(MCTL_TPREA<<16));
	mctl_write_w(ch_id + SDR_TRTW	,MCTL_TRTW);
	mctl_write_w(ch_id + SDR_TAL	,MCTL_TAL);
	mctl_write_w(ch_id + SDR_TCL	,MCTL_TCL);
	mctl_write_w(ch_id + SDR_TCWL	,MCTL_TCWL);
	mctl_write_w(ch_id + SDR_TRAS	,MCTL_TRAS);
	mctl_write_w(ch_id + SDR_TRC	,MCTL_TRC);
	mctl_write_w(ch_id + SDR_TRCD	,MCTL_TRCD);
	mctl_write_w(ch_id + SDR_TRRD	,MCTL_TRRD);
	mctl_write_w(ch_id + SDR_TRTP	,MCTL_TRTP);
	mctl_write_w(ch_id + SDR_TWR	,MCTL_TWR);
	mctl_write_w(ch_id + SDR_TWTR	,MCTL_TWTR);
	mctl_write_w(ch_id + SDR_TEXSR	,MCTL_TEXSR);
	mctl_write_w(ch_id + SDR_TXP	,MCTL_TXP);
	mctl_write_w(ch_id + SDR_TXPDLL,MCTL_TXPDLL);
	mctl_write_w(ch_id + SDR_TZQCS	,MCTL_TZQCS);
	mctl_write_w(ch_id + SDR_TZQCSI,MCTL_TZQCSI);
	mctl_write_w(ch_id + SDR_TDQS  ,MCTL_TDQS);
	mctl_write_w(ch_id + SDR_TCKSRE,MCTL_TCKSRE);
	mctl_write_w(ch_id + SDR_TCKSRX,MCTL_TCKSRX);
	mctl_write_w(ch_id + SDR_TCKE 	,MCTL_TCKE);
	mctl_write_w(ch_id + SDR_TMOD 	,MCTL_TMOD);
	mctl_write_w(ch_id + SDR_TRSTL ,MCTL_TRSTL);
	mctl_write_w(ch_id + SDR_TZQCL ,MCTL_TZQCL);
	mctl_write_w(ch_id + SDR_TMRR 	,MCTL_TMRR);
	mctl_write_w(ch_id + SDR_TCKESR,MCTL_TCKESR);
	mctl_write_w(ch_id + SDR_TDPD 	,MCTL_TDPD);
	
	//select 16/32-bits mode for MCTL
	reg_val = 0x0;
	if(MCTL_BUS_WIDTH==16)
		reg_val = 0x1;
	mctl_write_w(ch_id + SDR_PPCFG, reg_val);
	
	//set DFI timing registers
	mctl_write_w(ch_id + SDR_DFITPHYWRL, 1);
	reg_val = MCTL_TCWL - 1;		
	mctl_write_w(ch_id + SDR_DFITPHYWRL, reg_val);
	reg_val = MCTL_TCL - 2;	
	mctl_write_w(ch_id + SDR_DFITRDDEN, reg_val);	
	mctl_write_w(ch_id + SDR_DFITPHYRDL, 15);
		
	reg_val = 0x5;
	mctl_write_w(ch_id + SDR_DFISTCFG0, reg_val);

	//configure memory related attributes of mctl
	if(MCTL_DDR_TYPE==2)			//DDR2
		reg_val = 0x71040;
	else if(MCTL_DDR_TYPE==3)		//DDR3
		reg_val = 0x71061;
	else if(MCTL_DDR_TYPE==5)		//LPDDR
		reg_val = 0x971040;
	else							//LPDDR2
		reg_val = 0xd71040;		
	mctl_write_w(ch_id + SDR_MCFG, reg_val);
	
	//DFI update configuration register
	reg_val = 0x2;		
	mctl_write_w(ch_id + SDR_DFIUPDCFG, reg_val);
	
	reg_val = 0x80000000;		
	mctl_write_w(ch_id + SDR_MCMD, reg_val);

	//move to access state
	reg_val = 0x2;
	mctl_write_w(ch_id + SDR_SCTL, reg_val);	

	while(  (mctl_read_w(ch_id + SDR_SSTAT)&0x7) != 0x3 ) {};	
	
#ifndef SYSTEM_SIMULATION
	aw_delay(0x10);
#endif

	return (1);	
}

uint32 mctl_com_init(uint32 channel_num)
{
	uint32 reg_val;
	
	//set COM memory organization register
	reg_val = 0;
	if(MCTL_CS_NUM == 2)
		reg_val |= 0x1;
	if(MCTL_BANK_SIZE==8)
		reg_val |= 0x1<<2;
	reg_val |= ((MCTL_ROW_WIDTH -1)&0xf)<<4;
	if(MCTL_PAGE_SIZE == 8)
		reg_val |= 0xa<<8;
	else if(MCTL_PAGE_SIZE == 4)
		reg_val |= 0x9<<8;
	else if(MCTL_PAGE_SIZE == 2)
		reg_val |= 0x8<<8;
	else if(MCTL_PAGE_SIZE == 1)
		reg_val |= 0x7<<8;
	else
		reg_val |= 0x6<<8;
	if(MCTL_BUS_WIDTH == 32)
		reg_val |= 0x3<<12;
	else
		reg_val |= 0x1<<12;	
	if(MCTL_ACCESS_MODE == 0)
		reg_val |= 0x1<<15;	
	reg_val |= MCTL_DDR_TYPE<<16;
	if(channel_num == 2)
		reg_val |= 0x1<<19;
	reg_val |= 0x1<<20;
	mctl_write_w(SDR_COM_CR, reg_val);

#ifdef FPGA_PLATFORM
	//set preset readpipe value
	reg_val = mctl_read_w(SDR_COM_CR)&0x3fffff;	
	reg_val |= 0x9<<22;
	reg_val |= 0x9<<27;	
	mctl_write_w(SDR_COM_CR, reg_val);	
#endif
	
	//set COM sclk enable register
//	reg_val = 0x7;
//	mctl_write_w(SDR_COM_CCR, reg_val);

	return (1);	
}

uint32 mctl_port_cfg(void)
{
	uint32 reg_val;	
	//enable DRAM AXI clock for CPU access
	reg_val = mctl_read_w(CCU_AXIGATE);
	reg_val |= 0x1;
	mctl_write_w(CCU_AXIGATE, reg_val);
  	return (1);	
}
//*****************************************************************************
//	uint32 mctl_init()
//  Description:	DRAM Controller Initialize Procession
//
//	Arguments:		None
//  
//	Return Value:	1: Success		0: Fail
//*****************************************************************************
uint32 mctl_init(void)
{	
   //*********************************************** 
   // dram system init
   //*********************************************** 
  	mctl_sys_init();

    //*********************************************** 
    // dram dll init
    //*********************************************** 
	if(MCTL_CHANNEL_NUM == 2)
	{
		mctl_dll_init(0);
		mctl_dll_init(1);		
	}
	else
	{
		mctl_dll_init(0);		
	}  	
	//release mctl reset
	mctl_reset_release();
	
   //*********************************************** 
   // dram mctl & phy init
   //*********************************************** 
	if(MCTL_CHANNEL_NUM == 2)
	{
		mctl_channel_init(0);
		mctl_channel_init(1);		
	}
	else
	{
		mctl_channel_init(0);		
	}
   //*********************************************** 
   // dram com init
   //*********************************************** 
	mctl_com_init(MCTL_CHANNEL_NUM);

   //*********************************************** 
   // dram port configure
   //*********************************************** 
	mctl_port_cfg();
	
	return (1);		
}

//*****************************************************************************
//	void mctl_selfrefresh_entry()
//  Description:	Put DRAM into self refresh state
//
//	Arguments:		
//  
//	Return Value:	1: Success		0: Fail
//*****************************************************************************
void mctl_self_refresh_entry(uint32 channel_num)
{
	uint32 reg_val;
	uint32 ch_id;
	
	if(channel_num == 1)
		ch_id = 0x1000;	
	else
		ch_id = 0x0;
	//set SLEEP command
	reg_val = 0x3;
	mctl_write_w(ch_id + SDR_SCTL, reg_val);		

	//check whether in Low Power State
	while(  (mctl_read_w(ch_id + SDR_SSTAT)&0x7) != 0x5 ) {};		
}

//*****************************************************************************
//	void mctl_self_refresh_exit()
//  Description:	Exit from self refresh state to active state
//
//	Arguments:		
//  
//	Return Value:	1: Success		0: Fail
//*****************************************************************************
void mctl_self_refresh_exit(uint32 channel_num)
{
	uint32 reg_val;
	uint32 ch_id;
	
	if(channel_num == 1)
		ch_id = 0x1000;	
	else
		ch_id = 0x0;
	//set SLEEP command
	reg_val = 0x4;
	mctl_write_w(ch_id + SDR_SCTL, reg_val);		

	//check whether in Low Power State
	while(  (mctl_read_w(ch_id + SDR_SSTAT)&0x7) != 0x3 ) {};		
}
