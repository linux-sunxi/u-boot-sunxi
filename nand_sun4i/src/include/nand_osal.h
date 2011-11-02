/*
*********************************************************************************************************
*											        eBIOS
*						            the Easy Portable/Player Develop Kits
*									           dma sub system
*
*						        (c) Copyright 2006-2008, David China
*											All	Rights Reserved
*
* File    : osal_for_nand.c
* By      : Neil
* Version : V1.00
*********************************************************************************************************
*/
#ifndef __NAND_OSAL_H__
#define __NAND_OSAL_H__

#include "nand_oal.h"

extern unsigned int NAND_RequestDMA  	(unsigned int dmatype);
extern int  		NAND_ReleaseDMA  	(unsigned int hDma);
extern int  		NAND_SettingDMA		(unsigned int hDMA, void * pArg);
extern int  		NAND_StartDMA		(unsigned int hDMA, unsigned int saddr, unsigned int daddr, unsigned int bytes);
extern int  		NAND_RestartDMA		(unsigned int hDma);
extern int  		NAND_StopDMA		(unsigned int hDma);
extern unsigned int  NAND_QueryDmaStat	(unsigned int hDma);
extern unsigned int  NAND_QueryDmaChannelNo(unsigned int hDma);
extern void   		NAND_WaitDmaFinish	(void);

extern void   		NAND_GetPin			(void);
extern void   		NAND_ReleasePin		(void);
extern int  		NAND_SetClock		(unsigned int nand_max_clock);
extern void   		NAND_SetSrcClkName	(unsigned int pll_name);

extern void   		NAND_OpenAHBClock	(void);
extern void   		NAND_CloseAHBClock	(void);

extern int 			NAND_SetDrqCbMethod	(unsigned int used);

#endif

