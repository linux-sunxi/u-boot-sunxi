/*
 * (C) Copyright 2012
 *     wangflord@allwinnertech.com
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 */

#include <config.h>
#include <common.h>
#include <asm/arch/private_boot0.h>
#include <asm/arch/private_uboot.h>
#include "sprite_verify.h"

static uint flash_start;
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_sprite_download_raw_init(uint flash_part_start)
{
	flash_start = flash_part_start;

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_sprite_download_raw(void *buffer, void *next_buffer, uint len)
{
	uint sectors = len/512;

	if(sunxi_sprite_write(flash_start, sectors, buffer))
	{
		flash_start += sectors;

		return 0;
	}
	else
	{
		return -1;
	}
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_sprite_download_uboot(void *buffer, int production_media, char *storage_info)
{
    struct spare_boot_head_t    *uboot  = (struct spare_boot_head_t *)buffer;

	//校验特征字符是否正确
	if(memcmp(uboot->boot_head.magic, UBOOT_MAGIC, MAGIC_SIZE))
	{
		printf("sunxi sprite: uboot magic is error\n");

		return -1;
	}
	//校验数据是否正确
	if(sunxi_sprite_verify_checksum(buffer, uboot->boot_head.length, uboot->boot_head.check_sum))
	{
		printf("sunxi sprite: uboot checksum is error\n");

		return -1;
	}
	//读出dram参数
	//填充FLASH信息
	if(!production_media)
	{
		memcpy((void *)uboot->boot_data.nand_spare_data, storage_info, sizeof(boot_nand_para_t0));
	}
	/* regenerate check sum */
	sunxi_sprite_generate_checksum(buffer, uboot->boot_head.length, uboot->boot_head.check_sum);
	//校验数据是否正确
	if(sunxi_sprite_verify_checksum(buffer, uboot->boot_head.length, uboot->boot_head.check_sum))
	{
		printf("sunxi sprite: uboot checksum is error\n");

		return -1;
	}
#if 0
	if(!production_media)
	{
		return Nand_Burn_Boot0((__u32)buffer, length);
	}
	else
	{
		return SDMMC_PhyWrite(BOOT0_SDMMC_START_ADDR, length/512, (void *)buffer, 2);
	}
#else
	return 0;
#endif
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_sprite_download_boot0(void *buffer, int production_media, char *storage_info)
{
    boot0_file_head_t    *boot0  = (boot0_file_head_t *)buffer;

	//校验特征字符是否正确
	if(memcmp(boot0->boot_head.magic, UBOOT_MAGIC, MAGIC_SIZE))
	{
		printf("sunxi sprite: boot0 magic is error\n");

		return -1;
	}
	//校验数据是否正确
	if(sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum))
	{
		printf("sunxi sprite: boot0 checksum is error\n");

		return -1;
	}
	//读出dram参数
	//填充FLASH信息
	if(!production_media)
	{
		memcpy((void *)boot0->prvt_head.storage_data, storage_info, sizeof(boot_nand_para_t0));
	}
	memcpy((void *)&boot0->prvt_head.dram_para, (void *)&uboot_spare_head.boot_data.dram_para, sizeof(boot_dram_para_t));
	/* regenerate check sum */
	sunxi_sprite_generate_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum);
	//校验数据是否正确
	if(sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum))
	{
		printf("sunxi sprite: boot0 checksum is error\n");

		return -1;
	}
#if 0
	if(!production_media)
	{
		return Nand_Burn_Boot0((__u32)buffer, length);
	}
	else
	{
		return SDMMC_PhyWrite(BOOT0_SDMMC_START_ADDR, length/512, (void *)buffer, 2);
	}
#else
	return 0;
#endif
}

