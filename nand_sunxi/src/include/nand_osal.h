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

#define  __hdle  unsigned int

extern __hdle NAND_RequestDMA  (__u32 dmatype);
extern __s32  NAND_ReleaseDMA  (__hdle hDma);
extern __s32  NAND_SettingDMA(__hdle hDMA, void * pArg);
extern __s32  NAND_StartDMA(__hdle hDMA, __u32 saddr, __u32 daddr, __u32 bytes);
extern __s32  NAND_RestartDMA(__hdle hDma);
extern __s32  NAND_StopDMA(__hdle hDma);
extern __u32  NAND_QueryDmaStat(__hdle hDma);
extern __u32  NAND_QueryDmaChannelNo(__hdle hDma);
extern void   		NAND_WaitDmaFinish	(void);

extern void   		NAND_GetPin			(void);
extern void   		NAND_ReleasePin		(void);
extern __s32  NAND_SetClock(__u32 nand_max_clock);
extern __s32  NAND_GetClock(void);
extern void   NAND_SetSrcClkName(__u32 pll_name);

extern void   		NAND_OpenAHBClock	(void);
extern void   		NAND_CloseAHBClock	(void);

extern __s32 NAND_SetDrqCbMethod(__u32 used);

#endif

