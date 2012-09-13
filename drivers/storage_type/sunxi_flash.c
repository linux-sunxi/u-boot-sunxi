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

int (* sunxi_flash_read) (unsigned int start_block, unsigned int nblock, void *buffer);
int (* sunxi_flash_write)(unsigned int start_block, unsigned int nblock, void *buffer);
long long (* sunxi_flash_get_size)(void);
int (* sunxi_flash_exit) (void);

static struct mmc *mmc;

static int
sunxi_flash_null_read(unsigned int start_block, unsigned int nblock, void *buffer){
	return 0;
}

static int
sunxi_flash_null_write(unsigned int start_block, unsigned int nblock, void *buffer){
	return 0;
}

static long long
sunxi_flash_null_get_size(unsigned int start_block, unsigned int nblock, void *buffer){
	return 0;
}

static int
sunxi_flash_null_exit(unsigned int start_block, unsigned int nblock, void *buffer){
	return -1;
}


static int
sunxi_flash_nand_read(unsigned int start_block, unsigned int nblock, void *buffer){
	unsigned int nsize;

	nsize = nblock<<9;
	return sunxi_nand_read_opts(&nand_info[0], start_block<<9, &nsize, buffer, 0);
}

static int
sunxi_flash_nand_write(unsigned int start_block, unsigned int nblock, void *buffer){
	unsigned int nsize;

	nsize = nblock<<9;
	return sunxi_nand_write_opts(&nand_info[0], start_block<<9, &nsize, buffer, 0);
}

static long long
sunxi_flash_nand_get_size(void){
	return NAND_GetDiskSize();
}

static int
sunxi_flash_nand_exit(void){

	return NAND_Exit();
}

static int
sunxi_flash_mmc_read(unsigned int start_block, unsigned int nblock, void *buffer){

	int status;
	
	status = mmc->block_dev.block_read(CONFIG_SYS_MMC_ENV_DEV, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);

	return (status>0)?0:-1;
}

static int
sunxi_flash_mmc_write(unsigned int start_block, unsigned int nblock, void *buffer){

	int status;
	
	status = mmc->block_dev.block_write(CONFIG_SYS_MMC_ENV_DEV, start_block + CONFIG_MMC_LOGICAL_OFFSET,
					nblock, buffer);

	return (status>0)?0:-1;
}

static long long
sunxi_flash_mmc_get_size(void){

	return mmc->block_dev.lba;
}

static int
sunxi_flash_mmc_exit(void){
	return 0;
}

int sunxi_flash_handle_init(void){

	sunxi_flash_read  = sunxi_flash_null_read;
	sunxi_flash_write = sunxi_flash_null_write;
	sunxi_flash_get_size  = sunxi_flash_null_get_size;
	sunxi_flash_exit  = sunxi_flash_null_exit;
	
	if(storage_type == 0){
		sunxi_flash_read  = sunxi_flash_nand_read;
		sunxi_flash_write = sunxi_flash_nand_write;
		sunxi_flash_get_size  = sunxi_flash_nand_get_size;
		sunxi_flash_exit  = sunxi_flash_nand_exit;
	}
	else if((storage_type == 1) || (storage_type == 2)){
		mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
		if(!mmc){
			printf("fail to find one useful mmc card\n");
			return -1;
		}

		if (mmc_init(mmc)) {
			puts("MMC init failed\n");
			return  -1;
		}
		
		sunxi_flash_read  = sunxi_flash_mmc_read;
		sunxi_flash_write = sunxi_flash_mmc_write;
		sunxi_flash_get_size  = sunxi_flash_mmc_get_size;
		sunxi_flash_exit  = sunxi_flash_mmc_exit;
	}

	return 0;
}



