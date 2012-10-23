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

#include <nand.h>

//---------------------------------------------------------------
//  nand driver 版本号
//---------------------------------------------------------------
#define  NAND_VERSION_0                 0x02
#define  NAND_VERSION_1                 0x06

//---------------------------------------------------------------
//  结构体 定义
//---------------------------------------------------------------
typedef struct  boot_nand_para_set_t0
{
    unsigned char        ChipCnt;                            //the count of the total nand flash chips are currently connecting on the CE pin
    unsigned short       ChipConnectInfo;                    //chip connect information, bit == 1 means there is a chip connecting on the CE pin
	unsigned char		 RbCnt;
	unsigned char		 RbConnectInfo;						//the connect  information of the all rb  chips are connected
    unsigned char        RbConnectMode;						//the rb connect  mode
	unsigned char        BankCntPerChip;                     //the count of the banks in one nand chip, multiple banks can support Inter-Leave
    unsigned char        DieCntPerChip;                      //the count of the dies in one nand chip, block management is based on Die
    unsigned char        PlaneCntPerDie;                     //the count of planes in one die, multiple planes can support multi-plane operation
    unsigned char        SectorCntPerPage;                   //the count of sectors in one single physic page, one sector is 0.5k
    unsigned short       PageCntPerPhyBlk;                   //the count of physic pages in one physic block
    unsigned short       BlkCntPerDie;                       //the count of the physic blocks in one die, include valid block and invalid block
    unsigned short       OperationOpt;                       //the mask of the operation types which current nand flash can support support
    unsigned char        FrequencePar;                       //the parameter of the hardware access clock, based on 'MHz'
    unsigned char        EccMode;                            //the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
    unsigned char        NandChipId[5];                      //the nand chip id of current connecting nand chip
    unsigned short        ValidBlkRatio;                      //the ratio of the valid physical blocks, based on 1024
	unsigned int 		 good_block_ratio;					//good block ratio get from hwscan
	unsigned int		 ReadRetryType;						//the read retry type
	unsigned int		 Reserved[32];
}boot_nand_para_t0;

typedef struct boot_flash_info0
{
	unsigned int chip_cnt;
	unsigned int blk_cnt_per_chip;
	unsigned int blocksize;
	unsigned int pagesize;
	unsigned int pagewithbadflag; /*bad block flag was written at the first byte of spare area of this page*/
}boot_flash_info_t0;


//for simple
struct boot_physical_param
{
	unsigned char   chip; //chip no
	unsigned short  block; // block no within chip
	unsigned short  page; // apge no within block
	unsigned short  sectorbitmap; //done't care
	void   *mainbuf; //data buf
	void   *oobbuf; //oob buf
};
//---------------------------------------------------------------
//  函数 定义
//---------------------------------------------------------------
/*
*   Description:
*   1. if u wanna set dma callback hanlder(sleep during dma transfer) to free cpu for other tasks,
*      one must call the interface before nand flash initialization.
      this option is also protected by dma poll method,wakup(succeed or timeout) then check
      dma transfer complete or still running.
*   2. if u use dma poll method,no need to call the interface.
*
*   3. default is unuse dma callback hanlder,that is,dma poll method.
*   4. input para  : 0:dma poll method;  1:dma callback isr,free cpu for other tasks.
*   5. return value: 0:set succeed; -1:set failed.
*/
int sunxi_nand_scan_partition(void);
int sunxi_nand_getpart_num(void);
int sunxi_nand_getpart_offset(int part_index);
int sunxi_nand_getpart_size(int part_index);
int sunxi_nand_getpart_name(int index, char *buf);
int sunxi_nand_getpart_offset_byname(const char *part_name);
int sunxi_nand_getpart_size_byname(const char *part_name);
int sunxi_nand_getpart_info_byname(const char *part_name, loff_t *part_offset, loff_t *part_size);

int NAND_Init(void);
int NAND_Exit(void);



#endif  //ifndef __NAND_LOGIC_H__

