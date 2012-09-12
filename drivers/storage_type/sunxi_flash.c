/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC driver for allwinner sunxi platform.
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

#include <common.h>
#include <mmc.h>
#include <asm/arch/nand_bsp.h>
#include <asm/arch/boot_type.h>
#include "sunxi_flash.h"

/*
************************************************************************************************************
*
*                                             function
*
*    
*
*    
*
*    
*    
*
************************************************************************************************************
*/

static int
sunxi_null_op(unsigned int start_block, unsigned int nblock, void *buffer){
	return 0;
}

static uint
sunxi_null_size(void){
	return 0;
}

static int
sunxi_null_exit(void){
	return -1;
}
/*
************************************************************************************************************
*
*                                             function
*
*    
*
*    
*
*    
*    
*
************************************************************************************************************
*/


int (* sunxi_flash_read) (uint start_block, uint nblock, void *buffer) = sunxi_null_op;
int (* sunxi_flash_write)(uint start_block, uint nblock, void *buffer) = sunxi_null_op;
uint (* sunxi_flash_size)(void) = sunxi_null_size;
int (* sunxi_flash_exit) (void) = sunxi_null_exit;

int (* sunxi_sprite_read) (uint start_block, uint nblock, void *buffer) = sunxi_null_op;
int (* sunxi_sprite_write)(uint start_block, uint nblock, void *buffer) = sunxi_null_op;
uint (* sunxi_sprite_size)(void) = sunxi_null_size;
int (* sunxi_sprite_exit) (void) = sunxi_null_exit;

static struct mmc *mmc_boot, *mmc_sprite;

/*
************************************************************************************************************
*
*                                             function
*
*    
*
*    
*
*    
*    
*
************************************************************************************************************
*/

static int
sunxi_flash_nand_read(uint start_block, uint nblock, void *buffer)
{
    int ret;

	ret = NAND_LogicRead(start_block, nblock, buffer);
	return (!ret)?nblock:0;
}

static int
sunxi_flash_nand_write(uint start_block, uint nblock, void *buffer)
{
    int ret;
	
	ret = NAND_LogicWrite(start_block, nblock, buffer);
	return (!ret)?nblock:0;
}

static uint
sunxi_flash_nand_size(void)
{
	return NAND_GetDiskSize();
}

static int
sunxi_flash_nand_exit(void)
{
	return NAND_Exit();
}
/*
************************************************************************************************************
*
*                                             function
*
*    
*
*    
*
*    
*    
*
************************************************************************************************************
*/

static int
sunxi_flash_mmc_read(unsigned int start_block, unsigned int nblock, void *buffer){

	int status;
	
	status = mmc_boot->block_dev.block_read(mmc_boot->block_dev.dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);

	return status;
}

static int
sunxi_flash_mmc_write(unsigned int start_block, unsigned int nblock, void *buffer){

	int status;
	
	status = mmc_boot->block_dev.block_write(mmc_boot->block_dev.dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);

	return status;
}

static uint
sunxi_flash_mmc_size(void){

	return mmc_boot->block_dev.lba;
}

static int
sunxi_flash_mmc_exit(void){
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    
*
*    
*
*    
*    
*
************************************************************************************************************
*/
static int
sunxi_sprite_mmc_read(unsigned int start_block, unsigned int nblock, void *buffer){

	int status;
	
	status = mmc_sprite->block_dev.block_read(mmc_sprite->block_dev.dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);

	return status;
}

static int
sunxi_sprite_mmc_write(unsigned int start_block, unsigned int nblock, void *buffer){

	int status;
	
	status = mmc_sprite->block_dev.block_write(mmc_sprite->block_dev.dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);

	return status;
}

static uint
sunxi_sprite_mmc_size(void){

	return mmc_sprite->block_dev.lba;
}

static int
sunxi_sprite_mmc_exit(void){
	return 0;
}
/*
************************************************************************************************************
*
*											  function
*
*	 
*
*	 
*
*	 
*	 
*
************************************************************************************************************
*/

int sunxi_flash_handle_init(void)
{
    int workmode;
	int storage_type;
	int card_no;

    workmode = uboot_spare_head.boot_data.work_mode;
	storage_type = uboot_spare_head.boot_data.storage_type;

#ifdef DEBUG
    printf("workmode = %d\n", workmode);
#endif

	if(workmode == WORK_MODE_BOOT)
	{
	    int nand_used, sdc_used;
        if((storage_type == 1) || (storage_type == 2))
		{
		    if(2 == storage_type)
			{
			    nand_used = 0;
			    sdc_used  = 1;
				script_parser_patch("nand_para", "nand_used", &nand_used, 1);
		        script_parser_patch("mmc2_para", "sdc_used", &sdc_used, 1);
			}
			puts("MMC:   ");
		    card_no = (storage_type == 1)?0:2;
			board_mmc_set_num(card_no);
			board_mmc_pre_init(card_no);
			mmc_boot = find_mmc_device(card_no);
			if(!mmc_boot){
				printf("fail to find one useful mmc card\n");
				return -1;
			}

			if (mmc_init(mmc_boot)) {
				puts("MMC init failed\n");
				return  -1;
			}
			
			sunxi_flash_read  = sunxi_flash_mmc_read;
			sunxi_flash_write = sunxi_flash_mmc_write;
			sunxi_flash_size  = sunxi_flash_mmc_size;
			sunxi_flash_exit  = sunxi_flash_mmc_exit;			
		}
		else
		{
		    nand_used = 1;
			sdc_used  = 0;
            script_parser_patch("nand_para", "nand_used", &nand_used, 1);
		    script_parser_patch("mmc2_para", "sdc_used", &sdc_used, 1);
			
		    puts("NAND:   ");
		    NAND_Init();
			
			sunxi_flash_read  = sunxi_flash_nand_read;
			sunxi_flash_write = sunxi_flash_nand_write;
			sunxi_flash_size  = sunxi_flash_nand_size;
			sunxi_flash_exit  = sunxi_flash_nand_exit;
		}
		sunxi_flash_init_uboot(0);
	}
	else if(workmode & WORK_MODE_PRODUCT)		/* 量产模式 */
	{
	    if(!NAND_Init())                       /* burn nand */
        {
            sunxi_sprite_read  = sunxi_flash_nand_read;
			sunxi_sprite_write = sunxi_flash_nand_write;
			sunxi_sprite_size  = sunxi_flash_nand_size;
			sunxi_sprite_exit  = sunxi_flash_nand_exit;	
        }
        else                                   /* burn sdcard 2 */
		{
		    board_mmc_pre_init(2);
			mmc_sprite = find_mmc_device(2);
			if(!mmc_sprite){
				printf("fail to find one useful mmc card\n");
				return -1;
			}

			if (mmc_init(mmc_sprite)) {
				puts("MMC init failed\n");
				return  -1;
			}
			sunxi_sprite_read  = sunxi_sprite_mmc_read;
			sunxi_sprite_write = sunxi_sprite_mmc_write;
			sunxi_sprite_size  = sunxi_sprite_mmc_size;
			sunxi_sprite_exit  = sunxi_sprite_mmc_exit;	
	    }

		if(workmode & 0x07)     //sdcard burn mode
		{
            board_mmc_pre_init(0);	
			mmc_sprite = find_mmc_device(0);
			if(!mmc_sprite)
			{
				printf("fail to find one useful mmc card\n");
				return -1;
			}

			if (mmc_init(mmc_sprite))
			{
				puts("MMC sprite init failed\n");
				return  -1;
			}
			sunxi_flash_read  = sunxi_flash_mmc_read;
			sunxi_flash_write = sunxi_flash_mmc_write;
			sunxi_flash_size  = sunxi_flash_mmc_size;
			sunxi_flash_exit  = sunxi_flash_mmc_exit;			    
		}
	}
	else if(workmode & WORK_MODE_UPDATE)		/* 升级模式 */
	{
	}
	else   /* undefined mode */
	{
	}
	
	return 0;
}
/*
************************************************************************************************************
*
*											  function
*
*	 
*
*	 
*
*	 
*	 
*
************************************************************************************************************
*/

static block_dev_desc_t 	sunxi_flash_blk_dev;

block_dev_desc_t *sunxi_flash_get_dev(int dev)
{
	sunxi_flash_blk_dev.dev = dev;
	sunxi_flash_blk_dev.lba = sunxi_partition_get_size(dev);

	return ((block_dev_desc_t *) & sunxi_flash_blk_dev);
}

unsigned long  sunxi_flash_part_read(int dev_num, unsigned long start, unsigned long blkcnt, void *dst)
{
	start += sunxi_partition_get_offset(dev_num);
#ifdef DEBUG
    printf("nand try to read from %x, length %x block\n", (int )start, (int )blkcnt);
#endif
#ifdef DEBUG
    int ret;

    ret = sunxi_flash_read((int)start, (int )blkcnt, dst);
    printf("read result = %d\n", ret);

	return ret;
#else
    return sunxi_flash_read((int)start, (int )blkcnt, dst);
#endif
}

unsigned long  sunxi_flash_part_write(int dev_num, unsigned long start, unsigned long blkcnt, void *dst)
{
	start += sunxi_partition_get_offset(dev_num);
#ifdef DEBUG
    printf("nand try to write from %x, length %x block\n", (int )start, (int )blkcnt);
#endif
#ifdef DEBUG
	int ret;

	ret = sunxi_flash_write((int)start, (int )blkcnt, dst);
	printf("read result = %d\n", ret);

	return ret;
#else
	return sunxi_flash_write((int)start, (int )blkcnt, dst);
#endif
}


int sunxi_flash_init_uboot(int verbose)
{
#ifdef DEBUG
    puts("sunxi flash init uboot\n");
#endif
	sunxi_flash_blk_dev.if_type = IF_TYPE_SUNXI_FLASH;
	sunxi_flash_blk_dev.part_type = PART_TYPE_DOS;
	sunxi_flash_blk_dev.dev = 0;
	sunxi_flash_blk_dev.lun = 0;
	sunxi_flash_blk_dev.type = 0;

	/* FIXME fill in the correct size (is set to 32MByte) */
	sunxi_flash_blk_dev.blksz = 512;
	sunxi_flash_blk_dev.lba = 0;
	sunxi_flash_blk_dev.removable = 0;
	sunxi_flash_blk_dev.block_read = sunxi_flash_part_read;
	sunxi_flash_blk_dev.block_write = sunxi_flash_part_write;

	return 0;
}


