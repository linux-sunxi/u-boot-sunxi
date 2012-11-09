/*
************************************************************************************************************************
*                                                      eNand
*                                     Nand flash driver logic control module define
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : bsp_nand.h
*
* Author : Kevin.z
*
* Version : v0.1
*
* Date : 2008.03.25
*
* Description : This file define the function interface and some data structure export
*               for the nand bsp.
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
#ifndef __BSP_NAND_H__
#define __BSP_NAND_H__

int nand_uboot_init(int boot_mode);

int nand_uboot_exit(void);

uint nand_uboot_read(uint start, uint sectors, void *buffer);

uint nand_uboot_write(uint start, uint sectors, void *buffer);

int nand_download_boot0(uint length, void *buffer);

int nand_download_uboot(uint length, void *buffer);

int nand_uboot_erase(int user_erase);

uint nand_uboot_get_flash_info(void *buffer, uint length);

uint nand_uboot_set_flash_info(void *buffer, uint length);

uint nand_uboot_get_flash_size(void);


#endif  //ifndef __NAND_LOGIC_H__



