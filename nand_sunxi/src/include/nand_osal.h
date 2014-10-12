/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
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

