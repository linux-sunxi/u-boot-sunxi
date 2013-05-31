/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
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
typedef struct  boot_nand_para_set_t
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
}boot_nand_para_t;

typedef struct boot_flash_info
{
	unsigned int chip_cnt;
	unsigned int blk_cnt_per_chip;
	unsigned int blocksize;
	unsigned int pagesize;
	unsigned int pagewithbadflag; /*bad block flag was written at the first byte of spare area of this page*/
}boot_flash_info_t;


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

//for logic
extern int LML_Init(void);
extern int LML_Exit(void);
extern int LML_Read(unsigned int nLba, unsigned int nLength, void* pBuf);
extern int LML_Write(unsigned int nLba, unsigned int nLength, void* pBuf);
extern int LML_FlushPageCache(void);
extern int NAND_CacheFlush(void);
extern int NAND_CacheRead(unsigned int blk, unsigned int nblk, void *buf);
extern int NAND_CacheWrite(unsigned int blk, unsigned int nblk, void *buf);
extern int NAND_CacheOpen(void);
extern int NAND_CacheClose(void);
extern int NAND_LogicRead(uint nSectNum, uint nSectorCnt, void * pBuf);
extern int NAND_LogicWrite(uint nSectNum, uint nSectorCnt, void * pBuf);
extern uint NAND_GetDiskSize(void);
//for format
extern int FMT_Init(void);
extern int FMT_Exit(void);
extern int FMT_FormatNand(void);
extern void  ClearNandStruct( void );

//for scan
int  SCN_AnalyzeNandSystem(void);

//for physical
extern int PHY_Init(void);
extern int PHY_Exit(void);
extern int PHY_ChangeMode(unsigned char serial_mode);

//for simplie(boot0)
extern int PHY_SimpleErase(struct boot_physical_param * eraseop);
extern int PHY_SimpleRead(struct boot_physical_param * readop);
extern int PHY_SimpleWrite(struct boot_physical_param * writeop);
extern int PHY_SimpleWrite_1K(struct boot_physical_param * writeop);
extern int PHY_SimpleWrite_Seq(struct boot_physical_param * writeop);
extern int PHY_SimpleRead_Seq(struct boot_physical_param * readop);
extern int PHY_SimpleRead_1K(struct boot_physical_param * readop);
extern int BOOT_AnalyzeNandSystem(void);

//for param get&set
extern unsigned int NAND_GetValidBlkRatio(void);
extern int NAND_SetValidBlkRatio(unsigned int ValidBlkRatio);
extern unsigned int NAND_GetFrequencePar(void);
extern int NAND_SetFrequencePar(unsigned int FrequencePar);
extern unsigned int NAND_GetNandVersion(void);
extern int NAND_GetParam(boot_nand_para_t * nand_param);
extern int NAND_GetFlashInfo(boot_flash_info_t *info);
extern unsigned int NAND_GetDiskSize(void);
extern void  NAND_SetSrcClkName(unsigned int pll_name);

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
extern int NAND_SetDrqCbMethod(unsigned int used);


int sunxi_nand_scan_partition(void);
int sunxi_nand_getpart_num(void);
int sunxi_nand_getpart_offset(int part_index);
int sunxi_nand_getpart_size(int part_index);
int sunxi_nand_getpart_name(int index, char *buf);
int sunxi_nand_getpart_offset_byname(const char *part_name);
int sunxi_nand_getpart_size_byname(const char *part_name);
int sunxi_nand_getpart_info_byname(const char *part_name, loff_t *part_offset, loff_t *part_size);

int NAND_Init(void);
int sunxi_nand_read_opts(nand_info_t *nand, loff_t offset, size_t *length, u_char *buffer, int flags);
int sunxi_nand_write_opts(nand_info_t *nand, loff_t offset, size_t *length, u_char *buffer, int flags);
int sunxi_nand_erase_opts(nand_info_t *meminfo, const nand_erase_options_t *opts);
int sunxi_nand_flush_opts(nand_info_t *meminfo);

#endif  //ifndef __NAND_LOGIC_H__

