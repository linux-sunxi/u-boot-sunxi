/*
*********************************************************************************************************
*											        eBIOS
*						                the Base Input Output Subrutines
*									           dma controller sub system
*
*						        (c) Copyright 2006-2007, RICHARD,China
*											All	Rights Reserved
*
* File    : nfc_i.h
* By      : Richard.x
* Version : V1.00
*********************************************************************************************************
*/
#ifndef	_NFC_I1_H_
#define	_NFC_I1_H_

#include "../include/nand_reg.h"
#include "../include/nand_osal.h"
#include "../include/nand_ndfc.h"



#define NFC_READ_REG(reg)   		(reg)
#define NFC_WRITE_REG(reg,data) 	(reg) = (data)


#define ERR_ECC 	12
#define ECC_LIMIT 	10
#define ERR_TIMEOUT 14
#define READ_RETRY_MAX_TYPE_NUM 5
#define READ_RETRY_MAX_REG_NUM	4
#define READ_RETRY_MAX_CYCLE	10
#define LSB_MODE_MAX_REG_NUM	8

/* define various unit data input or output*/
#define NFC_READ_RAM_B(ram)    		(*((volatile __u8 *)(ram)))
#define NFC_WRITE_RAM_B(ram,data)  	(*((volatile __u8 *)(ram)) = (data))
#define NFC_READ_RAM_HW(ram)   		(*((volatile __u16 *)(ram)))
#define NFC_WRITE_RAM_HW(ram,data) 	(*((volatile __u16 *)(ram)) = (data))
#define NFC_READ_RAM_W(ram)   		(*((volatile __u32 *)(ram)))
#define NFC_WRITE_RAM_W(ram,data) 	(*((volatile __u32 *)(ram)) = (data))



#ifdef USE_PHYSICAL_ADDRESS
#define NFC_IS_SDRAM(addr)			((addr >= DRAM_BASE)?1:0)
#else
#define NFC_IS_SDRAM(addr)			( ((addr >= DRAM_BASE))&&(addr < SRAM_BASE)?1:0)
#endif

extern __hdle NAND_RequestDMA  (__u32 dmatype);
extern __s32  NAND_ReleaseDMA  (__hdle hDma);
extern __s32 NAND_SettingDMA(__hdle hDMA, void * pArg);
extern __s32 NAND_StartDMA(__hdle hDMA, __u32 saddr, __u32 daddr, __u32 bytes);
extern __s32 NAND_RestartDMA(__hdle hDma);
extern __s32 NAND_StopDMA(__hdle hDma);
extern __u32 NAND_QueryDmaStat(__hdle hDma);
extern __u32 NAND_QueryDmaChannelNo(__hdle hDma);
extern void NAND_GetPin(void);
extern void NAND_ReleasePin(void);
extern __u32 NAND_GetCmuClk(void);

extern __s32  NAND_SetClock(__u32 nand_max_clock);


#endif	/* _NFC_I_H_ */

