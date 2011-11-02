/*
************************************************************************************************************************
*                                                      eNand
*                                   Nand flash driver data struct type define
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : nand_type.h
*
* Author : Kevin.z
*
* Version : v0.1
*
* Date : 2008.03.19
*
* Description : This file defines the data struct type and return value type for nand flash driver.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Kevin.z         2008.03.19      0.1          build the file
*
************************************************************************************************************************
*/
#ifndef __NAND_TYPE_H
#define __NAND_TYPE_H

#include "nand_drv_cfg.h"

//==============================================================================
//  define the data structure for physic layer module
//==============================================================================

//define the optional physical operation parameter
struct __OptionalPhyOpPar_t
{
    unsigned char        MultiPlaneReadCmd[2];               //the command for multi-plane read, the sequence is [0] -ADDR- [0] -ADDR- [1] - DATA
    unsigned char        MultiPlaneWriteCmd[2];              //the command for multi-plane program, the sequence is 80 -ADDR- DATA - [0] - [1] -ADDR- DATA - 10/15
    unsigned char        MultiPlaneCopyReadCmd[3];           //the command for multi-plane page copy-back read, the sequence is [0] -ADDR- [1] -ADDR- [2]
    unsigned char        MultiPlaneCopyWriteCmd[3];          //the command for multi-plane page copy-back program, the sequence is [0] -ADDR- [1] - [2] -ADDR- 10
    unsigned char        MultiPlaneStatusCmd;                //the command for multi-plane operation status read, the command may be 0x70/0x71/0x78/...
    unsigned char        InterBnk0StatusCmd;                 //the command for inter-leave bank0 operation status read, the command may be 0xf1/0x78/...
    unsigned char        InterBnk1StatusCmd;                 //the command for inter-leave bank1 operation status read, the command may be 0xf2/0x78/...
    unsigned char        BadBlockFlagPosition;               //the flag that marks the position of the bad block flag,0x00-1stpage/ 0x01-1st&2nd page/ 0x02-last page/ 0x03-last 2 page
    unsigned short       MultiPlaneBlockOffset;              //the value of the block number offset between the left-plane block and the right pane block
};

typedef struct
{
    unsigned char        ChipCnt;                            //the count of the total nand flash chips are currently connecting on the CE pin
    unsigned short       ChipConnectInfo;                    //chip connect information, bit == 1 means there is a chip connecting on the CE pin
	unsigned char		RbCnt;
	unsigned char		RbConnectInfo;						//the connect  information of the all rb  chips are connected
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
    unsigned short       ValidBlkRatio;                      //the ratio of the valid physical blocks, based on 1024
	unsigned int 		good_block_ratio;					//good block ratio get from hwscan
	unsigned int		ReadRetryType;						//the read retry type
	unsigned int		Reserved[32];
}boot_nand_para_t;

typedef struct boot_flash_info{
	unsigned int chip_cnt;
	unsigned int blk_cnt_per_chip;
	unsigned int blocksize;
	unsigned int pagesize;
	unsigned int pagewithbadflag; /*bad block flag was written at the first byte of spare area of this page*/
}boot_flash_info_t;

//define the nand flash storage system information
struct __NandStorageInfo_t
{
    unsigned char        ChipCnt;                            //the count of the total nand flash chips are currently connecting on the CE pin
    unsigned short        ChipConnectInfo;                    //chip connect information, bit == 1 means there is a chip connecting on the CE pin
	unsigned char		RbCnt;
	unsigned char		RbConnectInfo;						//the connect  information of the all rb  chips are connected
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
    unsigned short       ValidBlkRatio;                         //the ratio of the valid physical blocks, based on 1024
    unsigned int		ReadRetryType;						//the read retry type
    struct __OptionalPhyOpPar_t OptPhyOpPar;        //the parameters for some optional operation
};


//define the page buffer pool for nand flash driver
struct __NandPageCachePool_t
{
    unsigned char        *PageCache0;                        //the pointer to the first page size ram buffer
    unsigned char        *PageCache1;                        //the pointer to the second page size ram buffer
    unsigned char        *PageCache2;                        //the pointer to the third page size ram buffer
    unsigned char		*SpareCache;

	unsigned char		*TmpPageCache;
};


//define the User Data structure for nand flash driver
struct __NandUserData_t
{
    unsigned char        BadBlkFlag;                         //the flag that marks if a physic block is a valid block or a invalid block
    unsigned short       LogicInfo;                          //the logical information of the physical block
    unsigned char        Reserved0;                          //reserved for 32bit align
    unsigned short       LogicPageNum;                       //the value of the logic page number, which the physic page is mapping to
    unsigned char        PageStatus;                         //the logical information of the physical page
    unsigned char        Reserved1;                          //reserved for 32bit align
} __attribute__ ((packed));


//define the paramter structure for physic operation function
struct __PhysicOpPara_t
{
    unsigned char        BankNum;                            //the number of the bank current accessed, bank NO. is different of chip NO.
    unsigned char        PageNum;                            //the number of the page current accessed, the page is based on single-plane or multi-plane
    unsigned short       BlkNum;                             //the number of the physic block, the block is based on single-plane or multi-plane
    unsigned int       SectBitmap;                         //the bitmap of the sector in the page which need access data
    void        *MDataPtr;                          //the pointer to main data buffer, it is the start address of a page size based buffer
    void        *SDataPtr;                          //the pointer to spare data buffer, it will be set to NULL if needn't access spare data
};


////define the parameter structure for nand BSP function
//struct __NandBspOpPara_t
//{
//    //to be desided
//};



//==============================================================================
//  define the data structure for logic management module
//==============================================================================

//define the logical architecture parameter structure
struct __LogicArchitecture_t
{
    unsigned short       LogicBlkCntPerZone;                 //the counter that marks how many logic blocks in one zone
    unsigned short       PageCntPerLogicBlk;                 //the counter that marks how many pages in one logical block
    unsigned char        SectCntPerLogicPage;                //the counter that marks how many  sectors in one logical page
    unsigned char        ZoneCntPerDie;                      //the counter that marks how many zones in one die
    unsigned short       Reserved;                           //reserved for 32bit align
};


/*
//define the logical information for the physical block
struct __PhyBlkLogicInfo_t
{
    unsigned short       LogicBlkNum:10;                     //the number mark that which logic block is this physic block mapping to
    unsigned short       ZoneNum:4;                          //the number of the zone which this physic block is belonged to
    unsigned short       PhyBlkType:1;                       //the type of the physic block, 0 - means normal data block, 1 - means special block
    unsigned short       Reserved:1;                         //reserved for extending in the future
};
*/

//define the super block type
struct __SuperPhyBlkType_t
{
    unsigned short       PhyBlkNum;                          //the super physic block offset number in a die,the first block of the die is 0
    unsigned short       BlkEraseCnt;                        //the erase count of the super physic block,record how many times it has been erased
};


//define the log block table item type
struct __LogBlkType_t
{
    unsigned short       LogicBlkNum;                        //the logic block number which the log block is belonged to
    unsigned short       LastUsedPage;                       //the number of the page which is the last used in the super physic block
    struct __SuperPhyBlkType_t PhyBlk;              //the super physic block number which the log block is mapping to
};


//define the zone table position information type
struct __ZoneTblPstInfo_t
{
    unsigned short       PhyBlkNum;                          //the physic block number in the chip which stored the block mapping table
    unsigned short       TablePst;                           //the page number in the physic block which stored the valid block mapping table
};


//define the block mapping table cache access type
struct __BlkMapTblCache_t
{
    unsigned char        ZoneNum;                            //the number of the zone which is the block mapping table belonged to
    unsigned char        DirtyFlag;                          //the flag that marks the status of the table in the nand, notes if the table need write back
    unsigned short       AccessCnt;                          //the counter that record how many times the block mapping table has been accessed
    struct __SuperPhyBlkType_t *DataBlkTbl;         //the pointer to the data block table of the block mapping table
    struct __LogBlkType_t *LogBlkTbl;               //the pointer to the log block table of the block mapping table
    struct __SuperPhyBlkType_t *FreeBlkTbl;         //the pointer to the free block table of the block mapping table
    unsigned short       LastFreeBlkPst;                     //the pointer to the free block position which is got last time
    unsigned short       Reserved;                           //reserved for 32bit align
};

//define the block mapping table cache management parameter type
struct __BlkMapTblCachePool_t
{
    struct __BlkMapTblCache_t *ActBlkMapTbl;                                //the pointer to the active block mapping table
    struct __BlkMapTblCache_t BlkMapTblCachePool[BLOCK_MAP_TBL_CACHE_CNT];  //the pool of the block mapping table cache
    unsigned short       LogBlkAccessAge[MAX_LOG_BLK_CNT];   //the time of accessing log block for the log block
    unsigned short       LogBlkAccessTimer;                  //the timer of the access time for recording the log block accessing time
    unsigned short       SuperBlkEraseCnt;                   //the counter of the super block erase, for do wear-levelling
};


//define the page mapping table item type
struct __PageMapTblItem_t
{
    unsigned short       PhyPageNum;                         //the physic page number which the logic page mapping to
};

//define the page mapping table access type
struct __PageMapTblCache_t
{
    unsigned char        ZoneNum;                            //the zone number which the page mapping table is belonged to
    unsigned char        LogBlkPst;                          //the position of the log block in the log block table
    unsigned short       AccessCnt;                          //the counter that the page mapping table has been accessed
	struct __PageMapTblItem_t *PageMapTbl;          //the pointer to the page mapping table
    unsigned char        DirtyFlag;                          //the flag that marks if the page mapping table need be writen back to nand flash
    unsigned char        Reserved[3];                        //reserved for 32bit align
};

//define the page mapping table cache management parameter type
struct __PageMapTblCachePool_t
{
    struct __PageMapTblCache_t *ActPageMapTbl;                              //the poninter to the active page mapping table
    struct __PageMapTblCache_t PageMapTblCachePool[PAGE_MAP_TBL_CACHE_CNT]; //the pool of the page mapping table cache
};


//define the global logical page parameter type
struct __GlobalLogicPageType_t
{
    unsigned int       LogicPageNum;                       //the global page number of the logic page, it is based on super page size
    unsigned int       SectorBitmap;                       //the bitmap of the sector in the logic page which data need access
};


//define the global logcial page based on zone and block parameter type
struct __LogicPageType_t
{
    unsigned int       SectBitmap;                         //the bitmap marks which sectors' data in the logical page need access
    unsigned short       BlockNum;                           //the value of the number of the logical block which the page is belonged to
    unsigned short       PageNum;                            //the value of the number of the page in the logical block
    unsigned char        ZoneNum;                            //the value of the number of the zone, which the page is belonged to
    unsigned char        Reserved[3];                        //reserved for 32bit align
};


//define the logical control layer management parameter type
struct __LogicCtlPar_t
{
    unsigned char        OpMode;                             //record nand flash driver last operation, may be read, write, or none.
    unsigned char        ZoneNum;                            //the number of the zone which is accessed last time
    unsigned short       LogicBlkNum;                        //the number of the logic block which is accessed last time
    unsigned short       LogicPageNum;                       //the number of the logic page which is accessed last time
    unsigned short       LogPageNum;                         //the number of the log page, which is accessed last time
    struct __SuperPhyBlkType_t  DataBlkNum;         //the number of the data block, which is accessed last time
    struct __SuperPhyBlkType_t  LogBlkNum;          //the number of the log block, which is accessed last time
    unsigned int       DiskCap;                            //the capacity of the logical disk
};


//define the nand flash physical information parameter type
struct __NandPhyInfoPar_t
{
    unsigned char        NandID[4];                          //the ID number of the nand flash chip
    unsigned char        DieCntPerChip;                      //the count of the Die in one nand flash chip
    unsigned char        SectCntPerPage;                     //the count of the sectors in one single physical page
    unsigned short       PageCntPerBlk;                      //the count of the pages in one single physical block
    unsigned short       BlkCntPerDie;                       //the count fo the physical blocks in one nand flash Die
    unsigned short       OperationOpt;                       //the bitmap that marks which optional operation that the nand flash can support
    unsigned short       ValidBlkRatio;                      //the valid block ratio, based on 1024 blocks
    unsigned short       AccessFreq;                         //the highest access frequence of the nand flash chip, based on MHz
    unsigned short       EccMode;                            //the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
    unsigned int 		ReadRetryType;
    struct __OptionalPhyOpPar_t *OptionOp;          //the pointer point to the optional operation parameter
};


//define the global paramter for nand flash driver to access all parameter
struct __NandDriverGlobal_t
{
    struct __NandStorageInfo_t  *NandStorageInfo;               //the pointer to the nand flash hardware information parameter
    struct __ZoneTblPstInfo_t   *ZoneTblPstInfo;                //the pointer to the block mapping table information parameter
    struct __BlkMapTblCachePool_t   *BlkMapTblCachePool;        //the pointer to the block mapping thable cache pool management parameter
    struct __PageMapTblCachePool_t  *PageMapTblCachePool;       //the pointer to the page mapping table cache pool management parameter
    struct __LogicArchitecture_t    *LogicalArchitecture;       //the pointer to the logical archtecture parameter
    struct __NandPageCachePool_t    *PageCachePool;             //the pointer to the page cache pool parameter
};


//==============================================================================
//  define some constant variable for the nand flash driver used
//==============================================================================

//define the mask for the nand flash optional operation
#define NAND_CACHE_READ         (1<<0)              //nand flash support cache read operation
#define NAND_CACHE_PROGRAM      (1<<1)              //nand flash support page cache program operation
#define NAND_MULTI_READ         (1<<2)              //nand flash support multi-plane page read operation
#define NAND_MULTI_PROGRAM      (1<<3)              //nand flash support multi-plane page program operation
#define NAND_PAGE_COPYBACK      (1<<4)              //nand flash support page copy-back command mode operation
#define NAND_INT_INTERLEAVE     (1<<5)              //nand flash support internal inter-leave operation, it based multi-bank
#define NAND_EXT_INTERLEAVE     (1<<6)              //nand flash support external inter-leave operation, it based multi-chip
#define NAND_RANDOM		        (1<<7)			    // nand flash support RANDOMIZER
#define NAND_READ_RETRY	        (1<<8)			    //nand falsh support READ RETRY
#define NAND_READ_UNIQUE_ID	    (1<<9)			    //nand falsh support READ UNIQUE_ID


//define the mask for the nand flash operation status
#define NAND_OPERATE_FAIL       (1<<0)              //nand flash program/erase failed mask
#define NAND_CACHE_READY        (1<<5)              //nand flash cache program true ready mask
#define NAND_STATUS_READY       (1<<6)              //nand flash ready/busy status mask
#define NAND_WRITE_PROTECT      (1<<7)              //nand flash write protected mask


//define the mark for physical page status
#define FREE_PAGE_MARK          0xff                //the page is storing no data, is not used
#define DATA_PAGE_MARK          0x55                //the physical page is used for storing the update data
#define TABLE_PAGE_MARK         0xaa                //the physical page is used for storing page mapping table

#define TABLE_BLK_MARK          0xaa                //the mark for the block mapping table block which is a special type block
#define BOOT_BLK_MARK           0xbb                //the mark for the boot block which is a special type block

//define the count of the physical blocks managed by one zone
#define BLOCK_CNT_OF_ZONE       1024                //one zone is organized based on 1024 blocks

//define the size of the sector
#define SECTOR_SIZE             512                 //the size of a sector, based on byte


//==============================================================================
//  define the function return value for different modules
//==============================================================================

#define NAND_OP_TRUE            (0)                     //define the successful return value
#define NAND_OP_FALSE           (-1)                    //define the failed return value


//define the return value
#define ECC_LIMIT               10                  //reach the limit of the ability of ECC
#define ERR_MALLOC              11                  //request buffer failed
#define ERR_ECC                 12                  //too much ecc error
#define ERR_NANDFAIL            13                  //nand flash program or erase fail
#define ERR_TIMEOUT             14                  //hardware timeout
#define ERR_PHYSIC              15                  //physical operation module error
#define ERR_SCAN                16                  //scan module error
#define ERR_FORMAT              17                  //format module error
#define ERR_MAPPING             18                  //mapping module error
#define ERR_LOGICCTL            19                  //logic control module error
#define ERR_ADDRBEYOND          20                  //the logical sectors need be accessed is beyond the logical disk
#define ERR_INVALIDPHYADDR		  21

#endif //ifndef __NAND_TYPE_H

