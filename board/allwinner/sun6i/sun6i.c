/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some board init for the Allwinner A10-evb board.
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
#include <malloc.h>
#include <asm/io.h>
#include <fastboot.h>

#include <asm/arch/nand_bsp.h>
#include <mmc.h>
#include <asm/arch/android_misc.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sunxi_mbr.h>
#include <asm/arch/boot_type.h>

DECLARE_GLOBAL_DATA_PTR;

static struct bootloader_message misc_message;

void fastboot_partition_init(void)
{
	fastboot_ptentry fb_part;
	int index, part_total;
	char partition_sets[256];
	char part_line[48];

	printf("--------fastboot partitions--------\n");
	part_total = sunxi_partition_get_total_num();
	printf("-total partitions:%d-\n", part_total);
	printf("%-12s  %-12s  %-12s\n", "-name-", "-start-", "-size-");
	memset(partition_sets, 0, 256);

	for(index = 0; index < part_total; index++)
	{
		sunxi_partition_get_name(index, &fb_part.name[0]);
		fb_part.start = sunxi_partition_get_offset(index) * 512;
		fb_part.length = sunxi_partition_get_size(index) * 512;
		fb_part.flags = 0;
		printf("%-12s: %-12x  %-12x\n", fb_part.name, fb_part.start, fb_part.length);
		fastboot_flash_add_ptn(&fb_part);

		memset(part_line, 0, 48);
		if(!uboot_spare_head.boot_data.storage_type)
		{
			sprintf(part_line, "%s@nand%c:", fb_part.name, 'a' + index);
		}
		else
		{
			if(!index)
			{
				sprintf(part_line, "%s@mmcblk0p2:", fb_part.name);
			}
			else
			{
				sprintf(part_line, "%s@mmcblk0p%d:", fb_part.name, 4 + index);
			}
		}
		strcat(partition_sets, part_line);
	}
	printf("-----------------------------------\n");

	setenv("partitions", partition_sets);
}

static struct bootloader_message misc_message;

int android_misc_flash_check(void) {

	u32   misc_offset = 0;
	char  buffer[2048];

	misc_offset = sunxi_partition_get_offset_byname("misc");
	if(!misc_offset)
	{
		puts("no misc partition is found\n");
		return 0;
	}
	memset(buffer, 0, 2048);
#ifdef DEBUG
	printf("misc_offset  : %d\n", (int )misc_offset);
#endif
	sunxi_flash_read(misc_offset, 2048/512, buffer);
	memcpy(&misc_message, buffer, sizeof(misc_message));
#ifdef DEBUG
	printf("misc.command  : %s\n", misc_message.command);
	printf("misc.status   : %s\n", misc_message.status);
	printf("misc.recovery : %s\n", misc_message.recovery);
#endif
	if(uboot_spare_head.boot_data.storage_type)
	{
		if(!strcmp(misc_message.command, "boot-recovery")) {
			/* there is a recovery command */
			puts("find boot recovery\n");
			setenv("bootcmd", "run setargs_mmc boot_recovery");
			puts("Recovery detected, will boot recovery\n");
			/* android recovery will clean the misc */
		}
		else{
			printf("bootcmd set setargs_mmc\n");
			setenv("bootcmd", "run setargs_mmc boot_normal");
		}
		if(!strcmp(misc_message.command, "boot-fastboot")) {
			/* there is a fastboot command */
			setenv("bootcmd", "run setargs_mmc boot_fastboot");
			puts("Fastboot detected, will enter fastboot\n");
			/* clean the misc partition ourself */
			memset(buffer, 0, 2048);
			sunxi_flash_write(misc_offset, 2048/512, buffer);
		}
	}
	else
	{
		if(!strcmp(misc_message.command, "boot-recovery")) {
			/* there is a recovery command */
			puts("find boot recovery\n");
			setenv("bootcmd", "run setargs_nand boot_recovery");
			puts("Recovery detected, will boot recovery\n");
			/* android recovery will clean the misc */
		}
		else{
			printf("bootcmd set setargs_nand\n");
			setenv("bootcmd", "run setargs_nand boot_normal");
		}

		if(!strcmp(misc_message.command, "boot-fastboot")) {
			/* there is a fastboot command */
			setenv("bootcmd", "run setargs_nand boot_fastboot");
			puts("Fastboot detected, will enter fastboot\n");
			/* clean the misc partition ourself */
			memset(buffer, 0, 2048);
			sunxi_flash_write(misc_offset, 2048/512, buffer);
		}
	}

	return 0;
}

int check_android_misc(void)
{
	debug("check_android_misc storage type = %d\n", uboot_spare_head.boot_data.storage_type);

	return android_misc_flash_check();
}

void set_boot_type_arg(void){

	if(uboot_spare_head.boot_data.storage_type){
		setenv("bootcmd", "run setargs_mmc boot_normal");
	}
	else{
		setenv("bootcmd", "run setargs_nand boot_normal");
	}
}

/* add board specific code here */
int board_init(void) {

	gd->bd->bi_arch_number = 3892;
	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100);
	debug("board_init storage_type = %d\n",uboot_spare_head.boot_data.storage_type);

	return 0;
}

/* Partition init must be after NAND init, so we put the fastboot
 * partition init here in the board late init.
 */
int board_late_init(void)
{
	fastboot_partition_init();
	set_boot_type_arg();
	check_android_misc();
	return 0;
}
void dram_init_banksize(void)
{
	/*
	 * We should init the Dram options, and kernel get it by tag.
	 */
	int dram_size;
	int ret;
	//gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	ret = script_parser_fetch("dram_para", "dram_para1", &dram_size, 1);
	if(!ret)
	{
		dram_size &= 0xffff;
		if(dram_size)
		{
			gd->bd->bi_dram[0].size = dram_size * 1024 * 1024;
		}
		else
		{
			gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
		}
	}
	else
	{
		gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	}
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);

	return 0;
}

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	sunxi_mmc_init(bis->bi_card_num);

	return 0;
}

void board_mmc_pre_init(int card_num)
{
    bd_t *bd;

	bd = gd->bd;
	gd->bd->bi_card_num = card_num;
	mmc_initialize(bd);
    //gd->bd->bi_card_num = card_num;
}

int board_mmc_get_num(void)
{
    return gd->boot_card_num;
}


void board_mmc_set_num(int num)
{
    gd->boot_card_num = num;
}


int mmc_get_env_addr(struct mmc *mmc, u32 *env_addr) {

	*env_addr = sunxi_partition_get_offset_byname(CONFIG_SUNXI_ENV_PARTITION);
	return 0;
}
#endif

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	puts("Board: SUN6I\n");
	return 0;
}
#endif
