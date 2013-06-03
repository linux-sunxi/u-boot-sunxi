#include <common.h>
#include <asm/arch/nand_bsp.h>
#include <malloc.h>

#define  OOB_BUF_SIZE                   32


extern int NAND_DmaInit(void);
extern int NAND_DmaExit(void);
extern int OSAL_printf(const char * str, ...);

static block_dev_desc_t 	nand_blk_dev;

block_dev_desc_t *nand_get_dev(int dev)
{
	nand_blk_dev.dev = dev;
	nand_blk_dev.lba = sunxi_nand_getpart_size(dev);

	return ((block_dev_desc_t *) & nand_blk_dev);
}

unsigned long  nand_read_uboot(int dev_num, unsigned long start, unsigned long blkcnt, void *dst)
{
	start += sunxi_nand_getpart_offset(dev_num);
	if(!NAND_LogicRead((int)start, (int )blkcnt, dst))
	{
		return blkcnt;
	}
	return 0;
}

unsigned long  nand_write_uboot(int dev_num, unsigned long start, unsigned long blkcnt, void *dst)
{
	start += sunxi_nand_getpart_offset(dev_num);
	if(!NAND_LogicWrite((int)start, (int )blkcnt, dst))
	{
		return blkcnt;
	}
	return 0;
}

int nand_erase_uboot(char *dev_part)
{
	int start;
	int size;
	char *tmp_buf;
	int total_size;
	int pre_ratio, this_ratio;
	int dev_num;
	char partname[12];

	if((*dev_part >= '0') && (*dev_part <= '9'))
	{
		dev_num = simple_strtoul(dev_part, 0, 10);
		start =  sunxi_nand_getpart_offset(dev_num);
		size  =  sunxi_nand_getpart_size(dev_num);
		sunxi_nand_getpart_name(dev_num, &partname[0]);
	}
	else
	{
		start =  sunxi_nand_getpart_offset_byname(dev_part);
		size  =  sunxi_nand_getpart_size_byname(dev_part);
		strncpy(partname, dev_part, 12);
	}

	if(start < 0)
	{
		printf("error: unknown part name\n");
		return -1;
	}
	if(!size)
	{
		printf("error: part size is zero\n");
		return -1;
	}
	printf("part name = %s\n", partname);
	printf("part size = %d\n", size);
	pre_ratio  = 0;
	this_ratio = 0;
	tmp_buf = malloc(32 * 1024);
	memset(tmp_buf, 0xff, 32 * 1024);
	printf("erase this part:    00%%");
	total_size = size;
	while(size >= ((32 * 1024)>>9))
	{
		NAND_LogicWrite(start, (32 * 1024)>>9, tmp_buf);
		start += (32 * 1024)>>9;
		size  -= (32 * 1024)>>9;
		this_ratio = 100 - (size * 100)/total_size;
		if(pre_ratio != this_ratio)
		{
			printf("\b\b\b%2d%%", this_ratio);
		}
		pre_ratio = this_ratio;
	}
	if(size)
	{
		NAND_LogicWrite(start, size, tmp_buf);
	}
	printf("\b\b\b\b%3d%%", 100);
	puts("\n");
	free(tmp_buf);
	LML_FlushPageCache();

	return 0;
}


int nand_init_uboot(int verbose)
{
	nand_blk_dev.if_type = IF_TYPE_NAND;
	nand_blk_dev.part_type = PART_TYPE_DOS;
	nand_blk_dev.dev = 0;
	nand_blk_dev.lun = 0;
	nand_blk_dev.type = 0;

	/* FIXME fill in the correct size (is set to 32MByte) */
	nand_blk_dev.blksz = 512;
	nand_blk_dev.lba = 0;
	nand_blk_dev.removable = 0;
	nand_blk_dev.block_read = nand_read_uboot;
	nand_blk_dev.block_write = nand_write_uboot;

	sunxi_nand_scan_partition();
	//fat_register_device(&nand_blk_dev, 1);
	return 0;
}
/*
************************************************************************************************************************
*                       READ ONE SINGLE PAGE
*
*Description: read one page data from nand based on single plane;
*
*Arguments  : *readop - the structure with physical address in nand and data buffer
*
*Return     :   = SUCESS  read ok;
*               = FAIL    read fail.
************************************************************************************************************************
*/
int NAND_PhyRead (struct boot_physical_param *readop)
{
	return(PHY_SimpleRead (readop));
}

/*
************************************************************************************************************************
*                       READ ONE SINGLE PAGE
*
*Description: read one page data from nand based on single plane;
*
*Arguments  : *readop - the structure with physical address in nand and data buffer
*
*Return     :   = SUCESS  read ok;
*               = FAIL    read fail.
************************************************************************************************************************
*/
int NAND_PhyRead_Seq (struct boot_physical_param *readop)
{
	return(PHY_SimpleRead_Seq (readop));
}

int NAND_PhyRead_1K (struct boot_physical_param *readop)
{
	return(PHY_SimpleRead_1K (readop));
}
/*
************************************************************************************************************************
*                       WRITE ONE SINGLE PAGE
*
*Description: write one page data to nand based on single plane;
*
*Arguments  : *writeop - the structure with physical address in nand and data buffer
*
*Return     :   = SUCESS  	write ok;
*               = FAIL    	write fail.
*				= BADBLOCK	write fail and bad block made by program
************************************************************************************************************************
*/
int NAND_PhyWrite (struct boot_physical_param *writeop)
{
	return (PHY_SimpleWrite(writeop));
}
int NAND_PhyWrite_1K (struct boot_physical_param *writeop)
{
	return (PHY_SimpleWrite_1K(writeop));
}
int NAND_PhyWrite_Seq (struct boot_physical_param *writeop)
{
	return (PHY_SimpleWrite_Seq(writeop));
}
/*
************************************************************************************************************************
*                       ERASE ONE SINGLE BLOCK
*
*Description: erase one block in nand based on single plane;
*
*Arguments  : *eraseop - the structure with physical address in nand and data buffer
*
*Return     :   = SUCESS  	erase ok;
*               = FAIL    	erase fail.
*				= BADBLOCK	erase fail and bad block made by erase
************************************************************************************************************************
*/
int NAND_PhyErase(struct boot_physical_param *eraseop)
{
	return (PHY_SimpleErase (eraseop ));
}
/*
************************************************************************************************************************
*                       INIT NAND FLASH
*
*Description: initial nand flash,request hardware resources;
*
*Arguments  : void.
*
*Return     :   = SUCESS  initial ok;
*               = FAIL    initial fail.
************************************************************************************************************************
*/
int NAND_PhyInit(void)
{
	int ret;

	ret = PHY_Init();
	if (ret)
	{
		OSAL_printf("NB1 : nand phy init fail\n");
		return ret;
	}

	ret = SCN_AnalyzeNandSystem();
	if (ret)
	{
		OSAL_printf("NB1 : nand scan fail\n");
		return ret;
	}
	//modify ValidBlkRatio
    //NAND_SetValidBlkRatio(nand_good_blk_ratio);

	OSAL_printf("NB1 : nand phy init ok\n");
	return(PHY_ChangeMode(1));
}

/*
************************************************************************************************************************
*                       RELEASE NAND FLASH
*
*Description: release  nand flash and free hardware resources;
*
*Arguments  : void.
*
*Return     :   = SUCESS  release ok;
*               = FAIL    release fail.
************************************************************************************************************************
*/
int NAND_PhyExit(void)
{
	PHY_Exit();

	return 0;
}
/*
************************************************************************************************************************
*                       WRITE SOME LOGICAl SECTORS
*
*Description: write some logic area;
*
*Arguments  : nSectnum   - start sector number
*			  nSectorCnt - sector count
*			  *pBuf		 - data buffer
*Return     :   = SUCESS  	write ok;
*               = FAIL    	write fail.

************************************************************************************************************************
*/
int NAND_LogicWrite(uint nSectNum, uint nSectorCnt, void * pBuf)
{
    #ifdef BOOT_CACHE_OPEN
	    return (NAND_CacheWrite(nSectNum, nSectorCnt, pBuf));
	#else
	    return (LML_Write(nSectNum, nSectorCnt, pBuf));
	#endif
}
/*
************************************************************************************************************************
*                       READ SOME LOGICAl SECTORS
*
*Description: read some logic area;
*
*Arguments  : nSectnum   - start sector number
*			  nSectorCnt - sector count
*			  *pBuf		 - data buffer
*Return     :   = SUCESS  	read ok;
*               = FAIL    	read fail.

************************************************************************************************************************
*/
int NAND_LogicRead(uint nSectNum, uint nSectorCnt, void * pBuf)
{
    #ifdef BOOT_CACHE_OPEN
	    return NAND_CacheRead(nSectNum,nSectorCnt,pBuf);
      #else
	    return LML_Read(nSectNum,nSectorCnt,pBuf);
	#endif

}
/*
************************************************************************************************************************
*                       INIT NAND DRIEVER
*
*Description: configure nand flash driver;
*
*Arguments  : void
*Return     :   = SUCESS  	init ok;
*               = other    	init fail.

************************************************************************************************************************
*/
int NAND_Init(void)
{
	int  result;

	OSAL_printf("NB1 : enter NFB_Init\n");

	//while((*(volatile __u32 *)0) != 0);
	result = NAND_PhyInit();
	if (result < 0)
	{
		OSAL_printf("NB1 : phy init fail\n");
		return -1;
	}

    //Format nand flash for logical access
    result = FMT_Init();
    if(result < 0)
    {
    	OSAL_printf("NB1 : format init fail\n");
		return -5;
    }

    result = FMT_FormatNand();
    if(result < 0)
    {
    	OSAL_printf("NB1 : format fail\n");
        return -6;
    }

    result = FMT_Exit();
    if(result < 0)
    {
        return -7;
    }

    //Init nand flash logical module
    result = LML_Init();
    if(result < 0)
    {
         return -8;
    }
#ifdef BOOT_CACHE_OPEN
    result = NAND_CacheOpen();
#endif

    OSAL_printf("NB1 : init ok\n");
    nand_init_uboot(0);

    return result;
}
/*
************************************************************************************************************************
*                       EXIT NAND DRIVER
*
*Description: ;
*
*Arguments  : void
*Return     :   = SUCESS  	exit ok;
*               = FAIL    	exit fail.

************************************************************************************************************************
*/
int NAND_Exit(void)
{
	int   result;

#ifdef BOOT_CACHE_OPEN
    NAND_CacheClose();
#endif

    //exit nand flahs logic module
    result = LML_Exit();
    if(result < 0)
    {
        return -1;
    }

    //exit nand flash physical module
    result = NAND_PhyExit();
    if(result < 0)
    {
        return -2;
    }
    return result;
}

/*
************************************************************************************************************************
*                       SCAN NAND HW
*
*Description: initial nand flash,request hardware resources;
*
*Arguments  : void.
*
*Return     :   = SUCESS  initial ok;
*               = FAIL    initial fail.
************************************************************************************************************************
*/
int NAND_HWScanStart(boot_nand_para_t *nand_param)
{
	int ret;

	OSAL_printf("NHW : start nand scan\n");
	ret = PHY_Init();
	if (ret)
	{
		OSAL_printf("NHW : nand phy init fail\n");
		return ret;
	}

	ret = SCN_AnalyzeNandSystem();
	if (ret)
	{
		OSAL_printf("NHW : nand scan fail\n");
		return ret;
	}

	NAND_GetParam(nand_param);

	nand_param->good_block_ratio = NAND_GetFrequencePar();
	//nand_good_block_ratio_flag = nand_param->good_block_ratio;
	OSAL_printf("NHW : nand hw scan ok\n");

	return(PHY_ChangeMode(1));
}

/*
************************************************************************************************************************
*                       RELEASE NAND FLASH
*
*Description: release  nand flash and free hardware resources;
*
*Arguments  : void.
*
*Return     :   = SUCESS  release ok;
*               = FAIL    release fail.
************************************************************************************************************************
*/
int NAND_HWScanStop(void)
{
	PHY_Exit();

	OSAL_printf("NHW : nand hw exit\n");

	return 0;
}

static uint max_badblk(uint v0, uint v1)
{
	uint val;

	if(v0 > v1)
		val = v0;
	else
		val = v1;

	return val;
}

static int _cal_real_chip(uint global_bank, uint chip_connect_info)
{
	uint chip;
	uint i,cnt;

	cnt = 0;
	chip = global_bank;

	for (i = 0; i < 8; i++ )
	{
		if (chip_connect_info & (1 << i))
            {
    			cnt++;
    			if (cnt == (chip+1))
    			    {
        				chip = i;
        				return chip;
        			}
    		}
	}

	return -1;
}

/*******************************************************************************
*函数名称: mark_bad_block
*函数原型：mark_bad_block( uint chip_num, uint blk_num )
*函数功能: 标记blk_num指定的块为坏块。
*入口参数: chip_num           chip number
           blk_num            块号
*返 回 值: 0              编程操作成功
*          other          编程操作失败
*备    注:
*******************************************************************************/
static int mark_bad_block( uint chip_num, uint blk_num)
{
    boot_flash_info_t info;
	struct boot_physical_param  para;
	unsigned char   oob_buf[OOB_BUF_SIZE];
	unsigned char*  page_buf;
	int  page_index[4];
    uint  page_with_bad_block, page_per_block;
	uint  i;
	int  mark_err_flag = -1;


	if( NAND_GetFlashInfo( &info ))
	{
		OSAL_printf("get flash info failed.\n");
		return -1;
	}

	//cal nand parameters
	//page_buf = (unsigned char*)(MARK_BAD_BLK_BUF_ADR);
    page_buf = (unsigned char*)malloc(32 * 1024);
    if(!page_buf)
    {
        OSAL_printf("malloc memory for page buf fail\n");
        return -1;
    }

	page_with_bad_block = info.pagewithbadflag;
	page_per_block = info.blocksize/info.pagesize;

	//read the first, second, last, last-1 page for check bad blocks
	page_index[0] = 0;
	page_index[1] = 0xEE;
	page_index[2] = 0xEE;
	page_index[3] = 0xEE;

    switch(page_with_bad_block & 0x03)
    {
        case 0x00:
            //the bad block flag is in the first page, same as the logical information, just read 1 page is ok
            break;

        case 0x01:
            //the bad block flag is in the first page or the second page, need read the first page and the second page
            page_index[1] = 1;
            break;

        case 0x02:
            //the bad block flag is in the last page, need read the first page and the last page
            page_index[1] = page_per_block - 1;
            break;

        case 0x03:
            //the bad block flag is in the last 2 page, so, need read the first page, the last page and the last-1 page
            page_index[1] = page_per_block - 1;
            page_index[2] = page_per_block - 2;
            break;
    }

	for(i =0; i<4; i++)
	{
		oob_buf[0] = 0x0;
		oob_buf[1] = 0x1;
		oob_buf[2] = 0x2;
		oob_buf[3] = 0x3;
		oob_buf[4] = 0x89;
		oob_buf[5] = 0xab;
		oob_buf[6] = 0xcd;
		oob_buf[7] = 0xef;

		para.chip = chip_num;
		para.block = blk_num;
		para.page = page_index[i];
		para.mainbuf = page_buf;
		para.oobbuf = oob_buf;

		if(para.page == 0xEE)
		    continue;

        NAND_PhyWrite( &para );
        NAND_PhyRead( &para );

		if(oob_buf[0] !=0xff)
			mark_err_flag = 0;
	}

	free(page_buf);

	return mark_err_flag;
}
/*
************************************************************************************************************************
*                       NAND_VersionGet
*
*Description: Get the nand flash driver is current version
*
*Arguments  : pointer.
*
*Return     :   = 0  version get;
*
************************************************************************************************************************
*/
int NAND_VersionGet(unsigned char *version)
{
    version[0] = 0xff;     //bad block flag
    version[1] = 0x00;     //reserved, set to 0x00
    version[2] = NAND_VERSION_0;     //nand driver version 0, current vresion is 0x02
    version[3] = NAND_VERSION_1;     //nand driver version 1, current vresion is 0x00

    return 0;
}
/*
************************************************************************************************************************
*                       NAND_VersionCheck
*
*Description: Check the nand flash driver is current version
*
*Arguments  : void.
*
*Return     :   = 0  version match;
*                 1  version not match
*                -1  can't get invalid version info
************************************************************************************************************************
*/
int NAND_VersionCheck(void)
{
	struct boot_physical_param boot0_readop_temp;
	struct boot_physical_param *boot0_readop = NULL;
	uint block_index;
	uint cnt1;
	int version_match_flag = -1;
	//uint chip_type;
	int i;
	unsigned char  oob_buf[32];
	uint* main_data;
	unsigned char  nand_version[4];

    /********************************************************************************
    *   nand_version[2] = 0xFF;          //the sequnece mode version <
    *   nand_version[2] = 0x01;          //the first interleave mode version, care ecc
    *                                      2010-06-05
    *   nand_version[2] = 0x02;          //the current version, don't care ecc
    *                                      2010-07-13
    *   NOTE:  need update the nand version in update_boot0 at the same time
    ********************************************************************************/
    nand_version[0] = 0xff;     //bad block flag
    nand_version[1] = 0x00;     //reserved, set to 0x00
    nand_version[2] = NAND_VERSION_0;     //nand driver version 0, current vresion is 0x02
    nand_version[3] = NAND_VERSION_1;     //nand driver version 1, current vresion is 0x00

    OSAL_printf("check nand version start.\n");
	OSAL_printf("Current nand driver version is %x %x %x %x \n", nand_version[0], nand_version[1], nand_version[2], nand_version[3]);

    boot0_readop = &boot0_readop_temp;

	//init boot0_readop
	boot0_readop->block = 0x0;
	boot0_readop->chip = 0;
	boot0_readop->mainbuf = (void*)malloc(32 * 1024);
    if(!boot0_readop->mainbuf)
    {
        OSAL_printf("malloc memory for boot0 read operation fail\n");
        return -1;
    }

	boot0_readop->oobbuf = oob_buf;
	boot0_readop->page = 0;
	boot0_readop->sectorbitmap = 0;

    main_data = (uint*)malloc(32 * 1024);
    if(!main_data)
    {
        OSAL_printf("malloc memory for main data fail\n");
        return -1;
    }

    //scan boot0 area blocks
	for(block_index=0;block_index<1 + 1;block_index++)
	{

		boot0_readop->block = block_index;
		boot0_readop->page = 0;
		cnt1 = 0;

        NAND_PhyRead_1K(boot0_readop);

        //check the current block is a bad block
		if(oob_buf[0] != 0xFF)
	    {
			OSAL_printf("block %u is bad block.\n",block_index);
			continue;
	    }


		//check the current block is a all 0xFF block
		for(i=0; i<256; i++)
		{
			if(*(main_data+i) == 0xffffffff)
				cnt1++;
		}

		if(cnt1 == 256)
	    {
			OSAL_printf("block %u is cleared block.\n",block_index);
			continue;
	    }

	    if((oob_buf[1] == 0x00) || (oob_buf[1] == 0xFF))
	    {
	       OSAL_printf("Media version is valid in block %u, version info is %x %x %x %x \n", block_index, oob_buf[0], oob_buf[1], oob_buf[2], oob_buf[3]);
	       if(oob_buf[2] == nand_version[2])
	       {
	            OSAL_printf("nand driver version match ok in block %u.\n",block_index);
    		    version_match_flag = 0;
    		    break;
	       }
	       else
	       {
	            OSAL_printf("nand driver version match fail in block %u.\n",block_index);
    		    version_match_flag = 1;
    		    break;
	       }

	    }
	    else
	    {
	        OSAL_printf("Media version is invalid in block %uversion info is %x %x %x %x \n", block_index, oob_buf[0], oob_buf[1], oob_buf[2], oob_buf[3]);
	    }

	}

    if(block_index == (1 + 1))
    {
         OSAL_printf("can't find valid version info in boot blocks. \n");
         version_match_flag = -1;
    }

    free(main_data);
	return version_match_flag;
}

/*
************************************************************************************************************************
*                       NAND_EraseBootBlocks
*
*Description: Check the nand flash driver is current version
*
*Arguments  : connecnt info.
*
*Return     :   = 0     OK;
*               other  Fail.
************************************************************************************************************************
*/
int  NAND_EraseBootBlocks( const boot_nand_para_t *nand_param)
{

    struct boot_physical_param  para;
	int  i;
	int  ret;

	OSAL_printf("Ready to erase boot blocks.\n");

	for( i = 0;  i < 7;  i++ )
	{
		para.chip = 0;
		para.block = i;
		ret = NAND_PhyErase( &para ) ;
		if(ret)
		    OSAL_printf("erasing block %u failed.\n", i );
	}

    OSAL_printf("has cleared the boot blocks.\n");

    return 0;

}

/*
************************************************************************************************************************
*                       NAND_EraseChip
*
*Description: Erase chip, only erase the all 0x0 blocks
*
*Arguments  : connecnt info.
*
*Return     :   = 0     OK;
*               other  Fail.
************************************************************************************************************************
*/
int  NAND_EraseChip( const boot_nand_para_t *nand_param)
{
	boot_flash_info_t info;
	struct boot_physical_param  para_read;

	int  i,j,k,m;
	int  ret;
	int  cnt0, cnt1;
	int  mark_err_flag;
	uint  bad_block_flag;
	uint  page_with_bad_block, page_size, page_per_block;
	int  page_index[4];
	uint  chip;
	unsigned char   oob_buf_read[OOB_BUF_SIZE];
	unsigned char*  page_buf_read;
	int  error_flag = 0;

    page_buf_read = (unsigned char*)malloc(32 * 1024);
    if(!page_buf_read)
    {
        OSAL_printf("malloc memory for page read fail\n");
        return -1;
    }
	OSAL_printf("Ready to erase chip.\n");
	// get nand info to cal
	OSAL_printf("nfb phy init ok.\n");
	if( NAND_GetFlashInfo( &info ) != 0 )
	{
		OSAL_printf("get flash info failed.\n");
		free(page_buf_read);
		return -1;
	}
	OSAL_printf("Succeed in getting flash info.\n");

	page_size = 512*info.pagesize;
	page_with_bad_block = info.pagewithbadflag;
	page_per_block = info.blocksize/info.pagesize;
	OSAL_printf("page size:%x, block size: %x, bad block position: %x.\n",page_size, page_per_block,page_with_bad_block);

    page_index[0] = 0;
	page_index[1] = 0xEE;
	page_index[2] = 0xEE;
	page_index[3] = 0xEE;

    switch(page_with_bad_block & 0x03)
    {
        case 0x00:
            //the bad block flag is in the first page, same as the logical information, just read 1 page is ok
            break;

        case 0x01:
            //the bad block flag is in the first page or the second page, need read the first page and the second page
            page_index[1] = 1;
            break;

        case 0x02:
            //the bad block flag is in the last page, need read the first page and the last page
            page_index[1] = page_per_block - 1;
            break;

        case 0x03:
            //the bad block flag is in the last 2 page, so, need read the first page, the last page and the last-1 page
            page_index[1] = page_per_block - 1;
            page_index[2] = page_per_block - 2;
            break;
    }

    OSAL_printf("chip_cnt = %x, chip_connect = %x, rb_cnt = %x,  rb_connect = %x,  good_block_ratio =%x \n",nand_param->ChipCnt,nand_param->ChipConnectInfo,nand_param->RbCnt,nand_param->RbConnectInfo,nand_param->good_block_ratio);

	for( i = 0;  i < nand_param->ChipCnt;  i++ )
	{
	    //select chip
		chip = _cal_real_chip( i, nand_param->ChipConnectInfo );
        OSAL_printf("erase chip %u \n", chip);

        //scan for bad blocks, only erase good block, all 0x00 blocks is defined bad blocks
		for( j = 0;  j < info.blk_cnt_per_chip;  j++ )
		{
			para_read.chip = chip;
			para_read.block = j;
			para_read.mainbuf = page_buf_read;
			para_read.oobbuf = oob_buf_read;

			bad_block_flag = 0;

			for(k = 0; k<4; k++)
			{
				cnt0 =0;
				cnt1 =0;
				para_read.page = page_index[k];
				if( para_read.page== 0xEE)
				    break;

				ret = NAND_PhyRead(& para_read );

				//check the current block is a all 0x00 block

				for(m=0; m<8; m++)   //check user data, 8 byte
			    {
			        if(oob_buf_read[m] == ((unsigned char)0x0) )
			            cnt1++;
			        else
			            break;
			    }
				for(m=0; m<page_size; m++)  //check main data
				{
					if(page_buf_read[m] == ((unsigned char)0x0) )
						cnt0++;
					else
						break;
			    }

				if((cnt0 == page_size)&&(cnt1 == 8))
				{
					bad_block_flag = 1;
					OSAL_printf("find a all 0x00 block %u\n", j);
					break;
				}

			}

			if(bad_block_flag)
				continue;

			ret = NAND_PhyErase( &para_read );
			if( ret != 0 )
	    		{
	    		    OSAL_printf("erasing block %u failed.\n", j );
			    	mark_err_flag = mark_bad_block( i, j );
	    		    if( mark_err_flag!= 0 )
	    		        {
        					error_flag++;
        					OSAL_printf("error in marking bad block flag in chip %u, block %u, mark error flag %u.\n", i, j, mark_err_flag);
        				}
	    		}

    		}
	}

	OSAL_printf("has cleared the chip.\n");
	if(error_flag)
		OSAL_printf("the nand is Bad.\n");
	else
		OSAL_printf("the nand is OK.\n");

    free(page_buf_read);

	return 0;

}

/*******************************************************************************
*函数名称: NAND_BadBlockScan
*函数原型：bad_blcok_scan(const boot_nand_para_t *nand_param)
*函数功能: 标记blk_num指定的块为坏块。
*入口参数: nand_param
*返 回 值: >0              编程操作成功
*          -1              编程操作失败
*备    注:
*******************************************************************************/
int NAND_BadBlockScan(const boot_nand_para_t *nand_param)
{
	boot_flash_info_t info;
	struct boot_physical_param  para;

	int  i, j, k;
	uint  page_with_bad_block, page_per_block;
	int  page_index[4];
	uint  bad_block_cnt[8];
	uint  bad_block_num = 0;
	uint  good_block_num = 0;
	int  good_block_ratio = -1;
	uint  chip;
	unsigned char   oob_buf[OOB_BUF_SIZE];
	unsigned char*  page_buf;

	for(i=0; i<8; i++)
	    bad_block_cnt[i] = 0;

	OSAL_printf("Ready to scan bad blocks.\n");

    // get nand info to cal
	OSAL_printf("nfb phy init ok.\n");
	if( NAND_GetFlashInfo( &info ) )
	    {
    		OSAL_printf("get flash info failed.\n");
    		return -1;
    	}
	OSAL_printf("Succeed in getting flash info.\n");

	//cal nand parameters
	//page_buf = (unsigned char*)(BAD_BLK_SCAN_BUF_ADR);
    page_buf = (unsigned char*)malloc(32 * 1024);
    if(!page_buf)
    {
        OSAL_printf("malloc memory for page buf fail\n");
        return -1;
    }

	page_with_bad_block = info.pagewithbadflag;
	page_per_block = info.blocksize/info.pagesize;

	//read the first, second, last, last-1 page for check bad blocks
	page_index[0] = 0;
	page_index[1] = 0xEE;
	page_index[2] = 0xEE;
	page_index[3] = 0xEE;

    switch(page_with_bad_block & 0x03)
    {
        case 0x00:
            //the bad block flag is in the first page, same as the logical information, just read 1 page is ok
            break;

        case 0x01:
            //the bad block flag is in the first page or the second page, need read the first page and the second page
            page_index[1] = 1;
            break;

        case 0x02:
            //the bad block flag is in the last page, need read the first page and the last page
            page_index[1] = page_per_block - 1;
            break;

        case 0x03:
            //the bad block flag is in the last 2 page, so, need read the first page, the last page and the last-1 page
            page_index[1] = page_per_block - 1;
            page_index[2] = page_per_block - 2;
            break;
    }

    //scan bad blocks
	for( i = 0;  i < info.chip_cnt;  i++ ){

		chip = _cal_real_chip( i, nand_param->ChipConnectInfo );
		OSAL_printf("scan CE %u\n", chip);
		bad_block_cnt[chip] = 0;

		for( j = 0;  j < info.blk_cnt_per_chip;  j++ )
		{
			para.chip = chip;
			para.block = j;
			para.mainbuf = page_buf;
			para.oobbuf = oob_buf;

			for(k = 0; k<4; k++)
			{
				// read pages for check
				para.page = page_index[k];
				if(para.page == 0xEE)
				    continue;
				NAND_PhyRead(&para );

				// find bad blocks
				if(oob_buf[0] != 0xff)
				{
				    OSAL_printf("find defined bad block in chip %u, block %u.\n", i, j);
					bad_block_cnt[chip]++;
                    break;
				}
			}
		}
	}

	// cal bad block num
	if(info.chip_cnt == 0x1)        //for one CE
    	{
    	    if(nand_param->ChipConnectInfo == 0x1)
    	        {
    	            bad_block_num = bad_block_cnt[0]<<1;
    	        }
    		else
    		    {
    		        OSAL_printf("chip connect parameter %x error \n", nand_param->ChipConnectInfo);
    		        free(page_buf);

        			return -1;
    		    }
    	}
	else if(info.chip_cnt == 2)     //for two CE
    	{
    		if(nand_param->ChipConnectInfo == 0x3)
    		    {
    			    bad_block_num = (bad_block_cnt[0] + bad_block_cnt[1])<<1;
    		    }
    		else if(nand_param->ChipConnectInfo == 0x5)
        		{
        			bad_block_num = (bad_block_cnt[0] + bad_block_cnt[2])<<1;
        		}
    		else if(nand_param->ChipConnectInfo == 0x81)
        		{
        			bad_block_num = (bad_block_cnt[0] + bad_block_cnt[7])<<1;
        		}
    		else
        		{
        			OSAL_printf("chip connect parameter %x error \n", nand_param->ChipConnectInfo);
        			free(page_buf);

        			return -1;
        		}
    	}
	else if(info.chip_cnt == 4)     //for four CE
	    {
    		if(nand_param->ChipConnectInfo == 0xf)
    		    {
    			    bad_block_num = max_badblk((bad_block_cnt[0] + bad_block_cnt[2]),(bad_block_cnt[1] + bad_block_cnt[3]))<<1;
    		    }
    		else if(nand_param->ChipConnectInfo == 0x55)
    		    {
    			    bad_block_num = max_badblk((bad_block_cnt[0] + bad_block_cnt[2]),(bad_block_cnt[4] + bad_block_cnt[6]))<<1;
    		    }
    		else
    		    {
    			    OSAL_printf("chip connect parameter %x error \n",nand_param->ChipConnectInfo);
    			    free(page_buf);

    			    return -1;
    		    }

	    }
	else if(info.chip_cnt == 8)     //for eight CE
	    {
    		if(nand_param->ChipConnectInfo == 0xff)
    		    {
        			bad_block_num = max_badblk((bad_block_cnt[0] + bad_block_cnt[2]),(bad_block_cnt[1] + bad_block_cnt[3]));
        			bad_block_num = 2*max_badblk(bad_block_num, max_badblk((bad_block_cnt[4] + bad_block_cnt[6]),(bad_block_cnt[5] + bad_block_cnt[7])));

        		}
    		else
    		    {
        			OSAL_printf("chip connect parameter %x error \n",nand_param->ChipConnectInfo);
        			free(page_buf);

        			return -1;
        		}
	    }
	else
	    {
	        OSAL_printf("chip cnt parameter %x error \n",nand_param->ChipCnt);
	        free(page_buf);

        	return -1;
	    }


	//cal good block num required per 1024 blocks
	good_block_num = (1024*(info.blk_cnt_per_chip - bad_block_num))/info.blk_cnt_per_chip -20;
    for(i=0; i<info.chip_cnt; i++)
    {
		chip = _cal_real_chip( i, nand_param->ChipConnectInfo );
		OSAL_printf(" %d bad blocks in CE %u \n", bad_block_cnt[chip], chip);
	}
	OSAL_printf("cal bad block num is %u \n", bad_block_num);
	OSAL_printf("cal good block num is %u \n", good_block_num);

    //cal good block ratio
	for(i=0; i<5; i++)
	{
		if(good_block_num >= (nand_param->good_block_ratio - 32*i))
		    {
    			good_block_ratio = (nand_param->good_block_ratio - 32*i);
    			OSAL_printf("good block ratio is %u \n",good_block_ratio);
    			break;
    		}
	}
    free(page_buf);

 	return good_block_ratio;
}
