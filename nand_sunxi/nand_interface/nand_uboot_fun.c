#include <common.h>
#include <malloc.h>
#include "nand_bsp.h"

#define  OOB_BUF_SIZE                   32
#define NAND_BOOT0_BLK_START    0
#define NAND_BOOT0_BLK_CNT		2
#define NAND_UBOOT_BLK_START    (NAND_BOOT0_BLK_START+NAND_BOOT0_BLK_CNT)
#define NAND_UBOOT_BLK_CNT		5
#define NAND_BOOT0_PAGE_CNT_PER_COPY     64

static char nand_para_store[256];

int msg(const char * str, ...)
{
    NAND_Print(str);
}


int NAND_PhyInit(void)
{
	int ret;

	debug("PHY_Init\n");
	ret = PHY_Init();
	if (ret)
	{
		printf("NB1 : nand phy init fail\n");
		return ret;
	}

	debug("SCN_AnalyzeNandSystem\n");
	ret = SCN_AnalyzeNandSystem();
	if (ret)
	{
		printf("NB1 : nand scan fail\n");
		return ret;
	}
	//modify ValidBlkRatio
    //NAND_SetValidBlkRatio(nand_good_blk_ratio);

	debug("NB1 : nand phy init ok\n");
	return(PHY_ChangeMode(1));
}


int NAND_PhyExit(void)
{
	debug("PHY_Exit\n");

	PHY_Exit();

	return 0;
}


int NAND_LogicWrite(uint nSectNum, uint nSectorCnt, void * pBuf)
{
    #ifdef NAND_CACHE_OPEN
	    return (NAND_CacheWrite(nSectNum, nSectorCnt, pBuf));
	#else
	    return (LML_Write(nSectNum, nSectorCnt, pBuf));
	#endif
}


int NAND_LogicRead(uint nSectNum, uint nSectorCnt, void * pBuf)
{
    #ifdef NAND_CACHE_OPEN
	    return NAND_CacheRead(nSectNum,nSectorCnt,pBuf);
      #else
	    return LML_Read(nSectNum,nSectorCnt,pBuf);
	#endif

}


int NAND_LogicInit(void)
{
	int  result;

	debug("NB1 : enter NFB_Init\n");

    //Format nand flash for logical access
    result = FMT_Init();
    if(result < 0)
    {
    	printf("NB1 : format init fail\n");
		return -5;
    }

    result = FMT_FormatNand();
    if(result < 0)
    {
    	printf("NB1 : format fail\n");
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
#ifdef NAND_CACHE_OPEN
    result = NAND_CacheOpen();
#endif

    return result;
}


int NAND_LogicExit(void)
{
	int   result;

#ifdef NAND_CACHE_OPEN
    NAND_CacheClose();
#endif

    //exit nand flahs logic module
    result = LML_Exit();
    if(result < 0)
    {
        return -1;
    }

    //exit nand flash physical module
    //result = NAND_PhyExit();
    //if(result < 0)
    //{
    //    return -2;
    //}
    return result;
}


uint max_badblk(uint v0, uint v1)
{
	uint val;

	if(v0 > v1)
		val = v0;
	else
		val = v1;

	return val;
}

int cal_real_chip(uint global_bank, uint chip_connect_info)
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


int mark_bad_block( uint chip_num, uint blk_num)
{
	unsigned char   oob_buf[OOB_BUF_SIZE];
	unsigned char*  page_buf;
	int  page_index[4];
    uint  page_with_bad_block, page_per_block;
	uint  i;
	int  mark_err_flag = -1;
	struct boot_physical_param para;


	//cal nand parameters
	//page_buf = (unsigned char*)(MARK_BAD_BLK_BUF_ADR);
    page_buf = (unsigned char*)malloc(32 * 1024);
    if(!page_buf)
    {
        printf("malloc memory for page buf fail\n");
        return -1;
    }

	page_with_bad_block = NAND_GetBadBlockFlagPos();
	page_per_block = NAND_GetPageCntPerBlk();
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

        PHY_SimpleWrite( &para );
        PHY_SimpleRead( &para );

		if(oob_buf[0] !=0xff)
			mark_err_flag = 0;
	}

	free(page_buf);

	return mark_err_flag;
}



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
	uint nand_version_value;

    /********************************************************************************
    *   nand_version[2] = 0xFF;          //the sequnece mode version <
    *   nand_version[2] = 0x01;          //the first interleave mode version, care ecc
    *                                      2010-06-05
    *   nand_version[2] = 0x02;          //the current version, don't care ecc
    *                                      2010-07-13
    *   NOTE:  need update the nand version in update_boot0 at the same time
    ********************************************************************************/
    NAND_GetVersion(nand_version);


    printf("check nand version start.\n");
	printf("Current nand driver version is 0x%x \n", *((__u32 *)(nand_version)));

    boot0_readop = &boot0_readop_temp;
    //printf("boot0_readop addr: 0x%x\n", (__u32)boot0_readop);

	//init boot0_readop
	boot0_readop->block = 0x0;
	boot0_readop->chip = 0;
	boot0_readop->mainbuf = (void*)malloc(32 * 1024);
    //printf("%s %d boot0_readop addr: 0x%x, mainbuf: 0x%x\n", __FILE__, __LINE__, (__u32)boot0_readop, (__u32)boot0_readop->mainbuf);
    if(!boot0_readop->mainbuf)
    {
        printf("malloc memory for boot0 read operation fail\n");
        return -1;
    }

	boot0_readop->oobbuf = oob_buf;
	boot0_readop->page = 0;
	boot0_readop->sectorbitmap = 0;

    main_data = boot0_readop->mainbuf;

    //scan boot0 area blocks
	for(block_index=0;block_index<1 + 1;block_index++)
	{

		boot0_readop->block = block_index;
		boot0_readop->page = 0;
		cnt1 = 0;

        //printf("%s %d mainbuf: 0x%x\n", __FILE__, __LINE__, (__u32)boot0_readop->mainbuf);
        PHY_SimpleRead_1K(boot0_readop);

        //check the current block is a bad block
		if(oob_buf[0] != 0xFF)
	    {
			printf("block %u is bad block.\n",block_index);
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
			printf("block %u is cleared block.\n",block_index);
			continue;
	    }

	    if((oob_buf[1] == 0x00) || (oob_buf[1] == 0xFF))
	    {
	       printf("Media version is valid in block %u, version info is 0x%x \n", block_index, *((__u32 *)(oob_buf)));
	       if(oob_buf[2] == nand_version[2])
	       {
	            printf("nand driver version match ok in block %u.\n",block_index);
    		    version_match_flag = 0;
    		    break;
	       }
	       else
	       {
	            printf("nand driver version match fail in block %u.\n",block_index);
    		    version_match_flag = 1;
    		    break;
	       }

	    }
	    else
	    {
	        printf("Media version is invalid in block %u version info is %x \n", block_index, *((__u32 *)(oob_buf)));
	    }

	}

    if(block_index == (1 + 1))
    {
         printf("can't find valid version info in boot blocks. \n");
         version_match_flag = -1;
    }

    free(main_data);
	return version_match_flag;
}


int  NAND_EraseBootBlocks(void)
{

    struct boot_physical_param  para;
	int  i;
	int  ret;

	printf("Ready to erase boot blocks.\n");

	for( i = 0;  i < 7;  i++ )
	{
		para.chip = 0;
		para.block = i;
		ret = PHY_SimpleErase( &para ) ;
		if(ret)
		    printf("erasing block %u failed.\n", i );
	}

    printf("has cleared the boot blocks.\n");

    return 0;

}


int  NAND_EraseChip(void)
{
	struct boot_physical_param  para_read;
	int  i,j,k,m;
	int  ret;
	int  cnt0, cnt1;
	int  mark_err_flag;
	uint  bad_block_flag;
	uint  chip_cnt, chip_connect, page_size, page_per_block, blk_cnt_per_chip;
	int  page_index[4];
	uint  chip;
	unsigned char   oob_buf_read[OOB_BUF_SIZE];
	unsigned char*  page_buf_read;
	int  error_flag = 0;

    page_buf_read = (unsigned char*)malloc(32 * 1024);
    if(!page_buf_read)
    {
        printf("malloc memory for page read fail\n");
        return -1;
    }
	printf("Ready to erase chip.\n");
	// get nand info to cal
	printf("nfb phy init ok.\n");


	page_size = NAND_GetPageSize();
	page_per_block = NAND_GetPageCntPerBlk();
	blk_cnt_per_chip = NAND_GetBlkCntPerChip();
	chip_cnt = NAND_GetChipCnt();
	debug("chip_cnt=%d\n", chip_cnt);
	chip_connect = NAND_GetChipConnect();
    debug("chip_cnt = %x, chip_connect = %x\n",chip_cnt,chip_connect);

    page_index[0] = 0;
    page_index[1] = 1;
    page_index[2] = page_per_block - 2;
    page_index[0] = page_per_block - 1;


	for( i = 0;  i < chip_cnt;  i++ )
	{
	    //select chip
		chip = cal_real_chip( i, chip_connect );
        printf("erase chip %u \n", chip);

        //scan for bad blocks, only erase good block, all 0x00 blocks is defined bad blocks
		for( j = 0;  j < blk_cnt_per_chip;  j++ )
		{

			if(j%0x100==0)
				printf("erase block %u\n", j);
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

				ret = PHY_SimpleRead_2CH(& para_read );

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
					printf("find a all 0x00 block %u\n", j);
					break;
				}

			}

			if(bad_block_flag)
				continue;

			ret = PHY_SimpleErase_2CH( &para_read );
			if( ret != 0 )
	    		{
	    		    printf("erasing block %u failed.\n", j );
			    	mark_err_flag = mark_bad_block( i, j );
	    		    if( mark_err_flag!= 0 )
	    		        {
        					error_flag++;
        					printf("error in marking bad block flag in chip %u, block %u, mark error flag %u.\n", i, j, mark_err_flag);
        				}
	    		}

    		}
	}

	printf("has cleared the chip.\n");
	if(error_flag)
		printf("the nand is Bad.\n");
	else
		printf("the nand is OK.\n");

    free(page_buf_read);

	return 0;

}


int NAND_BadBlockScan(void)
{
	int  i, j, k;
	uint  page_with_bad_block, page_per_block, block_cnt_per_chip, chip_cnt, chip_connect_mode;
	int  page_index[4];
	uint  bad_block_cnt[8];
	uint  bad_block_num = 0;
	uint  good_block_num = 0;
	int  good_block_ratio = -1, default_good_block_ratio =-1;
	uint  chip;
	unsigned char   oob_buf[OOB_BUF_SIZE];
	unsigned char*  page_buf;
	struct boot_physical_param  para;

	for(i=0; i<8; i++)
	    bad_block_cnt[i] = 0;

	debug("Ready to scan bad blocks.\n");

	//cal nand parameters
	//page_buf = (unsigned char*)(BAD_BLK_SCAN_BUF_ADR);
    page_buf = (unsigned char*)malloc(32 * 1024);
    if(!page_buf)
    {
        printf("malloc memory for page buf fail\n");
        return -1;
    }

	page_with_bad_block = NAND_GetBadBlockFlagPos();
	page_per_block = NAND_GetPageCntPerBlk();
	block_cnt_per_chip = NAND_GetBlkCntPerChip();
	chip_cnt = NAND_GetChipCnt();
	chip_connect_mode = NAND_GetChipConnect();
	default_good_block_ratio = NAND_GetValidBlkRatio();


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
	for( i = 0;  i < chip_cnt;  i++ ){

		chip = cal_real_chip( i, chip_connect_mode );
		printf("scan CE %u\n", chip);
		bad_block_cnt[chip] = 0;

		for( j = 0;  j < block_cnt_per_chip;  j++ )
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
				PHY_SimpleRead_2CH(&para );

				// find bad blocks
				if(oob_buf[0] != 0xff)
				{
				    printf("find defined bad block in chip %u, block %u.\n", i, j);
					bad_block_cnt[chip]++;
                    break;
				}
			}
		}
	}

	// cal bad block num
	if(chip_cnt == 0x1)        //for one CE
    	{
    	    if(chip_connect_mode == 0x1)
    	        {
    	            bad_block_num = bad_block_cnt[0]<<1;
    	        }
    		else
    		    {
    		        printf("chip connect parameter %x error \n", chip_connect_mode);
    		        free(page_buf);

        			return -1;
    		    }
    	}
	else if(chip_cnt == 2)     //for two CE
    	{
    		if(chip_connect_mode == 0x3)
    		    {
    			    bad_block_num = (bad_block_cnt[0] + bad_block_cnt[1])<<1;
    		    }
    		else if(chip_connect_mode == 0x5)
        		{
        			bad_block_num = (bad_block_cnt[0] + bad_block_cnt[2])<<1;
        		}
    		else if(chip_connect_mode == 0x81)
        		{
        			bad_block_num = (bad_block_cnt[0] + bad_block_cnt[7])<<1;
        		}
    		else
        		{
        			printf("chip connect parameter %x error \n", chip_connect_mode);
        			free(page_buf);

        			return -1;
        		}
    	}
	else if(chip_cnt == 4)     //for four CE
	    {
    		if(chip_connect_mode == 0xf)
    		    {
    			    bad_block_num = max_badblk((bad_block_cnt[0] + bad_block_cnt[2]),(bad_block_cnt[1] + bad_block_cnt[3]))<<1;
    		    }
    		else if(chip_connect_mode == 0x55)
    		    {
    			    bad_block_num = max_badblk((bad_block_cnt[0] + bad_block_cnt[2]),(bad_block_cnt[4] + bad_block_cnt[6]))<<1;
    		    }
    		else
    		    {
    			    printf("chip connect parameter %x error \n",chip_connect_mode);
    			    free(page_buf);

    			    return -1;
    		    }

	    }
	else if(chip_cnt == 8)     //for eight CE
	    {
    		if(chip_connect_mode == 0xff)
    		    {
        			bad_block_num = max_badblk((bad_block_cnt[0] + bad_block_cnt[2]),(bad_block_cnt[1] + bad_block_cnt[3]));
        			bad_block_num = 2*max_badblk(bad_block_num, max_badblk((bad_block_cnt[4] + bad_block_cnt[6]),(bad_block_cnt[5] + bad_block_cnt[7])));

        		}
    		else
    		    {
        			printf("chip connect parameter %x error \n",chip_connect_mode);
        			free(page_buf);

        			return -1;
        		}
	    }
	else
	    {
	        printf("chip cnt parameter %x error \n",chip_connect_mode);
	        free(page_buf);

        	return -1;
	    }


	//cal good block num required per 1024 blocks
	good_block_num = (1024*(block_cnt_per_chip - bad_block_num))/block_cnt_per_chip -50;
    for(i=0; i<chip_cnt; i++)
    {
		chip = cal_real_chip( i, chip_connect_mode );
		printf(" %d bad blocks in CE %u \n", bad_block_cnt[chip], chip);
	}
	printf("cal bad block num is %u \n", bad_block_num);
	printf("cal good block num is %u \n", good_block_num);

    //cal good block ratio
	for(i=0; i<5; i++)
	{
		if(good_block_num >= (default_good_block_ratio - 32*i))
	    {
			good_block_ratio = (default_good_block_ratio - 32*i);
			printf("good block ratio is %u \n",good_block_ratio);
			break;
		}
	}
    free(page_buf);

 	return good_block_ratio;
}


int NAND_UbootInit(int boot_mode)
{
	int ret = 0;
	int enable_bad_block_scan_flag = 0;
	uint good_block_ratio=0;

	debug("NAND_UbootInit start\n");

	ret = NAND_PhyInit();

	debug("NAND_PhyInit end: 0x%x\n", ret);

	if(!boot_mode)
	{
	    /* scan bad blocks */
	    if( enable_bad_block_scan_flag)
	    {
	    	good_block_ratio = NAND_BadBlockScan();
		    if(good_block_ratio <= 0)
		    {
				return -1;
		    }
	    }
		NAND_GetParam(nand_para_store, 256);
	}

    /* logic init */
	ret |= NAND_LogicInit();

	debug("NAND_UbootInit end: 0x%x\n", ret);

	return ret;

}


int NAND_UbootExit(void)
{
	int ret = 0;

	debug("NAND_UbootExit \n");

	ret = NAND_LogicExit();
	ret |= NAND_PhyExit();

	return ret;
}



uint NAND_UbootRead(uint start, uint sectors, void *buffer)
{
	int ret = 0;

	if(NAND_LogicRead(start, sectors, buffer))
		ret = sectors;

	return ret;

}

uint NAND_UbootWrite(uint start, uint sectors, void *buffer)
{
	int ret = 0;

	if(NAND_LogicWrite(start, sectors, buffer))
		ret = sectors;

	return ret;
}

int NAND_Uboot_Erase(int erase_flag)
{
	int version_match_flag;

	debug("erase_flag = %d\n", erase_flag);
	NAND_PhyInit();
	NAND_EraseBootBlocks();
	if(erase_flag)
	{
		debug("erase by flag %d\n", erase_flag);
		NAND_EraseChip();
	}
	else
	{
		version_match_flag = NAND_VersionCheck();
		debug("nand version = %x\n", version_match_flag);
		if (version_match_flag > 0)
		{
			NAND_EraseChip();
		}
	}
	NAND_PhyExit();

	return 0;
}

__s32  burn_boot0_1k_mode( __u32 read_retry_type, __u32 Boot0_buf )
{
    __u32 i, j, k;
    __u32 length;
	__u32 pages_per_block;
	__u32 copies_per_block;
    __u8  oob_buf[32];
    struct boot_physical_param  para;

    debug("burn boot0 normal mode!\n");

    for(i=0;i<32;i++)
        oob_buf[i] = 0xff;

    NAND_GetVersion(oob_buf);
	if((oob_buf[0]!=0xff)||(oob_buf[1]!= 0x00))
	{
		debug("get flash driver version error!");
		goto error;
	}

	/* 检查 page count */
	pages_per_block = NAND_GetPageCntPerBlk();
	if(pages_per_block%64)
	{
		debug("get page cnt per block error %x!", pages_per_block);
		goto error;
	}

	/* cal copy cnt per bock */
	copies_per_block = pages_per_block / NAND_BOOT0_PAGE_CNT_PER_COPY;

	/* burn boot0 copys */
    for( i = NAND_BOOT0_BLK_START;  i < (NAND_BOOT0_BLK_START + NAND_BOOT0_BLK_CNT);  i++ )
    {
        debug("boot0 %x \n", i);

		/* 擦除块 */
		para.chip  = 0;
		para.block = i;
		if( PHY_SimpleErase( &para ) <0 )
		{
		    debug("Fail in erasing block %d.\n", i );
    		continue;
    	}

        /* 在块中烧写boot0备份 */
        for( j = 0;  j < copies_per_block;  j++ )
       	{

			for( k = 0;  k < NAND_BOOT0_PAGE_CNT_PER_COPY;  k++ )
			{
				para.chip  = 0;
				para.block = i;
				para.page = j * NAND_BOOT0_PAGE_CNT_PER_COPY + k;
				para.mainbuf = (void *) (Boot0_buf + k * 1024);
				para.oobbuf = oob_buf;
				if( PHY_SimpleWrite_1K( &para ) <0)
				{
					debug("Warning. Fail in writing page %d in block %d.\n", j * NAND_BOOT0_PAGE_CNT_PER_COPY + k, i );
       			}
       		}
       	}
    }
	return 0;

error:
    return -1;
}

__s32  burn_boot0_lsb_mode(__u32 read_retry_type, __u32 Boot0_buf )
{
    __u32 i, j, k;
    __u8  oob_buf[32];
    __u32 page_size;
    struct boot_physical_param  para;

    debug("burn boot0 lsb mode!\n");

    for(i=0;i<32;i++)
        oob_buf[i] = 0xff;

	/* get nand driver version */
    NAND_GetVersion(oob_buf);
	if((oob_buf[0]!=0xff)||(oob_buf[1]!= 0x00))
	{
		debug("get flash driver version error!");
		goto error;
	}

	/* lsb enable */
	debug("lsb enalbe \n");
	debug("read retry mode: 0x%x\n", read_retry_type);
	if( NFC_LSBInit(read_retry_type) )
	{
	    debug("lsb init failed.\n");
		goto error;
	}
	NFC_LSBEnable(0, read_retry_type);



	/* 检查 page count */
	page_size = NAND_GetPageSize();
	{
		if(page_size %1024)
		{
			debug("get flash page size error!");
			goto error;
		}
	}


	/* burn boot0 */
    for( i = NAND_BOOT0_BLK_START;  i < (NAND_BOOT0_BLK_START + NAND_BOOT0_BLK_CNT);  i++ )
    {
        debug("down boot0 %x \n", i);

		/* 擦除块 */
		para.chip  = 0;
		para.block = i;
		if( PHY_SimpleErase( &para ) <0 )
		{
		    debug("Fail in erasing block %d.\n", i );
    		continue;
    	}

        /* 在块中烧写boot0备份, lsb mode下，每个块只能写前4个page */
		for( k = 0;  k < 4;  k++ )
		{
			para.chip  = 0;
			para.block = i;
			para.page  = k;
			para.mainbuf = (void *) (Boot0_buf + k * page_size);
			para.oobbuf = oob_buf;
			if( PHY_SimpleWrite_Seq( &para ) <0 )
			{
				debug("Warning. Fail in writing page %d in block %d.\n", k, i );
   			}
   		}

    }

    //check boot0
    for( i = NAND_BOOT0_BLK_START;  i < (NAND_BOOT0_BLK_START + NAND_BOOT0_BLK_CNT);  i++ )
    {
		struct boot_physical_param  para;
		__u32  k;

        debug("verify boot0 %x \n", i);

        /* 在块中烧写boot0备份, lsb mode下，每个块只能写前4个page */
		for( k = 0;  k < 4;  k++ )
		{
			para.chip  = 0;
			para.block = i;
			para.page  = k;
			para.mainbuf = (void *) (Boot0_buf + k * page_size);
			para.oobbuf = oob_buf;
			if( PHY_SimpleRead_Seq( &para ) <0 )
			{
				debug("Warning. Fail in reading page %d in block %d.\n",  k, i );
   			}
   		}

    }

    /* lsb disable */
    NFC_LSBDisable(0, read_retry_type);
    NFC_LSBExit(read_retry_type);
	debug("lsb disalbe \n");

	return 0;

error:
    return -1;
}

int NAND_BurnBoot0(uint length, void *buffer)
{
	__u32 read_retry_type = 0, read_retry_mode;
	int blk_index, page_index;
	int page_cnt_per_block;

	read_retry_type = NAND_GetReadRetryType();
	read_retry_mode = (read_retry_type>>16)&0xff;
	if( (read_retry_type>0)&&(read_retry_mode < 0x10))
	{
	    if( burn_boot0_lsb_mode(read_retry_type, (__u32)buffer) )
	        goto error;
	}
	else
	{
	    if( burn_boot0_1k_mode(read_retry_type, (__u32)buffer) )
	        goto error;
	}

	return 0;

error:
    return -1;

}


__s32 burn_uboot_in_one_blk(__u32 UBOOT_buf, __u32 length)
{
    __u32 i, j, k;
    __u8  oob_buf[32];
    __u32 page_size, pages_per_block, pages_per_copy, page_index;
    struct boot_physical_param  para;

     debug("burn uboot normal mode!\n");
     //debug("uboot_buf: 0x%x \n", UBOOT_buf);

    for(i=0;i<32;i++)
        oob_buf[i] = 0xff;

	/* get nand driver version */
    NAND_GetVersion(oob_buf);
    if((oob_buf[0]!=0xff)||(oob_buf[1]!= 0x00))
	{
		debug("get flash driver version error!");
		goto error;
	}


	/* 检查 page count */
	page_size = NAND_GetPageSize();
	{
		if(page_size %1024)
		{
			debug("get flash page size error!");
			goto error;
		}
	}

	/* 检查 page count */
	pages_per_block = NAND_GetPageCntPerBlk();
	if(pages_per_block%64)
	{
		debug("get page cnt per block error %x!", pages_per_block);
		goto error;
	}

	debug("pages_per_block: 0x%x\n", pages_per_block);

	/* 计算每个备份所需page */
	if(length%page_size)
	{
		debug("uboot length check error!\n");
		goto error;
	}
	pages_per_copy = length/page_size;
	if(pages_per_copy>pages_per_block)
	{
		debug("pages_per_copy check error!\n");
		goto error;
	}

	debug("pages_per_copy: 0x%x\n", pages_per_copy);

	//while((*(volatile unsigned int *)0) != 0x1234);
	/* burn uboot */
    for( i = NAND_UBOOT_BLK_START;  i < (NAND_UBOOT_BLK_START + NAND_UBOOT_BLK_CNT);  i++ )
    {
        debug("uboot %x \n", i);

		/* 擦除块 */
		para.chip  = 0;
		para.block = i;
		if( PHY_SimpleErase( &para ) <0 )
		{
		    debug("Fail in erasing block %d.\n", i );
    		continue;
    	}

        /* 在块中烧写boot0备份, lsb mode下，每个块只能写前4个page */
		for( k = 0;  k < pages_per_copy;  k++ )
		{
			para.chip  = 0;
			para.block = i;
			para.page  = k;
			para.mainbuf = (void *) (UBOOT_buf + k * page_size);
			para.oobbuf = oob_buf;
			//debug("burn uboot: block: 0x%x, page: 0x%x, mainbuf: 0x%x, maindata: 0x%x \n", para.block, para.page, (__u32)para.mainbuf, *((__u32 *)para.mainbuf));
			if( PHY_SimpleWrite( &para ) <0 )
			{
				debug("Warning. Fail in writing page %d in block %d.\n", k, i );
   			}
   		}

    }

	memset(oob_buf, 0, 32);
    //check uboot
    for( i = NAND_UBOOT_BLK_START;  i < (NAND_UBOOT_BLK_START + NAND_UBOOT_BLK_CNT);  i++ )
    {
	    debug("verify uboot blk %x \n", i);

        /* 擦除块 */

      	for( k = 0;  k < pages_per_copy;  k++ )
		{
			para.chip  = 0;
			para.block = i;
			para.page  = k;
			para.mainbuf = (void *) (UBOOT_buf + k * page_size);
			para.oobbuf = oob_buf;
			//debug("burn uboot: block: 0x%x, page: 0x%x, mainbuf: 0x%x, maindata: 0x%x \n", para.block, para.page, (__u32)para.mainbuf, *((__u32 *)para.mainbuf));

			if( PHY_SimpleRead( &para ) <0 )
			{
				debug("Warning. Fail in read page %d in block %d.\n", k, i );
   			}
   		}
    }

	return 0;

error:
    return -1;
}

__s32 burn_uboot_in_many_blks(__u32 UBOOT_buf, __u32 length)
{
     __u32 i, j, k;
    __u8  oob_buf[32];
    __u32 page_size, pages_per_block, pages_per_copy, page_index;
    struct boot_physical_param  para;

     debug("burn uboot normal mode!\n");

    for(i=0;i<32;i++)
        oob_buf[i] = 0xff;

	/* get nand driver version */
    NAND_GetVersion(oob_buf);
	if((oob_buf[0]!=0xff)||(oob_buf[1]!= 0x00))
	{
		debug("get flash driver version error!");
		goto error;
	}


	/* 检查 page count */
	page_size = NAND_GetPageSize();
	{
		if(page_size %1024)
		{
			debug("get flash page size error!");
			goto error;
		}
	}

	/* 检查 page count */
	pages_per_block = NAND_GetPageCntPerBlk();
	if(pages_per_block%64)
	{
		printf("get page cnt per block error %x!", pages_per_block);
		goto error;
	}

	/* 计算每个备份所需page */
	if(length%page_size)
	{
		printf("uboot length check error!\n");
		goto error;
	}
	pages_per_copy = length/page_size;
	if(pages_per_copy<=pages_per_block)
	{
		printf("pages_per_copy check error!\n");
		goto error;
	}


	/* burn uboot */
	page_index = 0;
    for( i = NAND_UBOOT_BLK_START;  i < (NAND_UBOOT_BLK_START + NAND_UBOOT_BLK_CNT);  i++ )
    {
        debug("uboot %x \n", i);

		/* 擦除块 */
		para.chip  = 0;
		para.block = i;
		if( PHY_SimpleErase( &para ) <0 )
		{
		    printf("Fail in erasing block %d.\n", i );
    		continue;
    	}

        /* 在块中烧写boot0备份, lsb mode下，每个块只能写前4个page */
		for( k = 0;  k < pages_per_block;  k++ )
		{
			para.chip  = 0;
			para.block = i;
			para.page  = k;
			para.mainbuf = (void *) (UBOOT_buf + page_index* page_size);
			para.oobbuf = oob_buf;
			if( PHY_SimpleWrite( &para ) <0 )
			{
				printf("Warning. Fail in writing page %d in block %d.\n", k, i );
   			}
   			page_index++;

   			if(page_index >= pages_per_copy)
   				break;
   		}

   		if(page_index >= pages_per_copy)
   			break;

    }

    if(page_index >= pages_per_copy)
		return 0;
	else
		goto error;

error:
    return -1;
}


int NAND_BurnUboot(uint length, void *buffer)
{
	int ret = 0;
	int blk_index, page_index;
	__u32 page_size, pages_per_block, block_size;

	/* 检查 page count */
	page_size = NAND_GetPageSize();
	{
		if(page_size %1024)
		{
			printf("get flash page size error!\n");
			goto error;
		}
	}

	/* 检查 page count */
	pages_per_block = NAND_GetPageCntPerBlk();
	if(pages_per_block%64)
	{
		printf("get page cnt per block error %x!\n", pages_per_block);
		goto error;
	}

	block_size = page_size*pages_per_block;
	if(length%page_size)
	{
		printf(" uboot length check error!\n");
		goto error;
	}

	if(length<=block_size)
	{
		ret = burn_uboot_in_one_blk((__u32)buffer, length);
	}
	else
	{
		ret = burn_uboot_in_many_blks((__u32)buffer, length);
	}

	return ret;

error:
	return -1;

}


int NAND_GetParam_store(void *buffer, uint length)
{
	memcpy(buffer, nand_para_store, length);

	return 0;
}



