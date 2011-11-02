/*
************************************************************************************************************************
*                                                      eNand
*                                     Nand flash driver scan module define
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : nand_scan.h
*
* Author : Kevin.z
*
* Version : v0.1
*
* Date : 2008.03.25
*
* Description : This file define the function interface and some data structure export
*               for the nand flash scan module.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Kevin.z         2008.03.25      0.1          build the file
*
************************************************************************************************************************
*/
#ifndef __NAND_SCAN_H__
#define __NAND_SCAN_H__

//#include "nand_type.h"
//#include "nand_physic.h"

//==============================================================================
//  define nand flash manufacture ID number
//==============================================================================

#define TOSHIBA_NAND            0x98                //Toshiba nand flash manufacture number
#define SAMSUNG_NAND            0xec                //Samsunt nand flash manufacture number
#define HYNIX_NAND              0xad                //Hynix nand flash manufacture number
#define MICRON_NAND             0x2c                //Micron nand flash manufacture number
#define ST_NAND                 0x20                //ST nand flash manufacture number
#define INTEL_NAND              0x89                //Intel nand flash manufacture number
#define SPANSION_NAND           0x01                //spansion nand flash manufacture number

//==============================================================================
//  define the function interface for nand storage scan module
//==============================================================================

/*
************************************************************************************************************************
*                           ANALYZE NAND FLASH STORAGE SYSTEM
*
*Description: Analyze nand flash storage system, generate the nand flash physical
*             architecture parameter and connect information.
*
*Arguments  : none
*
*Return     : analyze result;
*               = 0     analyze successful;
*               < 0     analyze failed, can't recognize or some other error.
************************************************************************************************************************
*/
int  SCN_AnalyzeNandSystem(void);

unsigned int NAND_GetValidBlkRatio(void);
int NAND_SetValidBlkRatio(unsigned int ValidBlkRatio);
unsigned int NAND_GetFrequencePar(void);
int NAND_SetFrequencePar(unsigned int FrequencePar);
unsigned int NAND_GetNandVersion(void);
int NAND_GetParam(boot_nand_para_t * nand_param);
//int NAND_GetFlashInfo(struct boot_flash_info *param);
//int NAND_GetConnectInfo(boot_nand_info_t * info);

#endif  //ifndef __NAND_SCAN_H__
