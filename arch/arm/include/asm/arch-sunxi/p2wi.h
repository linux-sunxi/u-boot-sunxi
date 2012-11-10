/*
*********************************************************************************************************
*                                                AR100 SYSTEM
*                                     AR100 Software System Develop Kits
*                                              	  p2wi module
*
*                                    (c) Copyright 2012-2016, Sunny China
*                                             All Rights Reserved
*
* File    : p2wi_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2012-5-22
* Descript: p2wi internal header.
* Update  : date                auther      ver     notes
*           2012-5-22 9:46:18	Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __P2WI_I_H__
#define __P2WI_I_H__

#include <asm/arch/cpu.h>

#define P2WI_SCK			(2000000)
#define P2WI_SDAODLY		(1)

#define PMU_TRANS_BYTE_MAX  (8)
//register define
#define	P2WI_REG_BASE			(SUNXI_P2WI_BASE)
#define P2WI_REG_CTRL			(P2WI_REG_BASE + 0x00)
#define P2WI_REG_CCR			(P2WI_REG_BASE + 0x04)
#define P2WI_REG_INTE			(P2WI_REG_BASE + 0x08)
#define P2WI_REG_STAT			(P2WI_REG_BASE + 0x0c)
#define P2WI_REG_DADDR0			(P2WI_REG_BASE + 0x10)
#define P2WI_REG_DADDR1			(P2WI_REG_BASE + 0x14)
#define P2WI_REG_DLEN			(P2WI_REG_BASE + 0x18)
#define P2WI_REG_DATA0			(P2WI_REG_BASE + 0x1c)
#define P2WI_REG_DATA1			(P2WI_REG_BASE + 0x20)
#define P2WI_REG_LCR			(P2WI_REG_BASE + 0x24)
#define P2WI_REG_PMCR			(P2WI_REG_BASE + 0x28)

//p2wi control bit field
#define P2WI_SOFT_RST		(1U << 0)
#define P2WI_GLB_INTEN		(1U << 1)
#define P2WI_ABT_TRANS		(1U << 6)
#define P2WI_START_TRANS 	(1U << 7)

//p2wi state bit field
#define P2WI_TOVER_INT		(1U << 0)
#define P2WI_TERR_INT		(1U << 1)
#define P2WI_LBSY_INT		(1U << 2)

//p2wi pmu mode bit field
#define P2WI_PMU_INIT		(1U << 31)

//local functions
s32  p2wi_wait_state(void);
void p2wi_set_pmu_mode(u32 slave_addr, u32 reg, u32 data);
void p2wi_set_clk(u32 sck);

/*
*********************************************************************************************************
*                                       INITIALIZE P2WI
*
* Description: 	initialize p2wi module.
*
* Arguments  : 	none.
*
* Returns    : 	OK if initialize p2wi succeeded, others if failed.
*********************************************************************************************************
*/
s32 p2wi_init(void);

/*
*********************************************************************************************************
*                                       EXIT P2WI
*
* Description: 	exit p2wi module.
*
* Arguments  : 	none.
*
* Returns    : 	OK if exit p2wi succeeded, others if failed.
*********************************************************************************************************
*/
s32 p2wi_exit(void);

/*
*********************************************************************************************************
*                                       	READ DATA BY P2WI
*
* Description: 	read data by p2wi.
*
* Arguments  : 	addr	: the point of address, size is specified by len.
*				data	: the point of data, size is specified by len.
*				len		: the length of data.
*
* Returns    : 	OK if read data succeeded, others if failed.
*********************************************************************************************************
*/
s32 p2wi_read(u8 *addr, u8 *data, u32 len);

/*
*********************************************************************************************************
*                                       	WRITE DATA BY P2WI
*
* Description: 	write data by p2wi.
*
* Arguments  : 	addr	: the point of address, size is specified by len.
*				data	: the point of data, size is specified by len.
*				len		: the length of data.
*
* Returns    : 	OK if write data succeeded, others if failed.
*********************************************************************************************************
*/
s32 p2wi_write(u8 *addr, u8 *data, u32 len);


#endif	//__P2WI_I_H__
