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
#include <asm/io.h>
#include <fastboot.h>

#include <asm/arch/nand_bsp.h>
#include <mmc.h>
#include <asm/arch/android_misc.h>
#include <asm/arch/gpio.h>
#include <asm/arch/partition.h>
#include <asm/arch/boot_type.h>
DECLARE_GLOBAL_DATA_PTR;

static struct bootloader_message misc_message;
static sunxi_boot_type_t boot_type;
extern sunxi_boot_type_t boot_from(void);

int save_boot_type(void) {

	boot_type = boot_from();
	printf("boot type: %d\n", boot_type);

	return 0;
}

sunxi_boot_type_t get_boot_type(void) {

	return boot_type;
}

void fastboot_flash_partition_init(void)
{
	fastboot_ptentry fb_part;
	int index, part_total;
	char partition_sets[128];
	char part_name[16] = "nanda";
	char *pa_index;
	int boot_part_flag;

	printf("--------fastboot partitions--------\n");
	part_total = sunxi_partition_get_total_num();
	printf("-total partitions:%d-\n", part_total);
	printf("%-12s  %-12s  %-12s\n", "-name-", "-start-", "-size-");
	memset(partition_sets, ' ', 128);
	boot_part_flag = 0;
	
	pa_index = partition_sets;
	for(index = 0; index < part_total && index < MBR_MAX_PART_COUNT; index++) {
		sunxi_partition_get_name(index, &fb_part.name[0]);
		fb_part.start = sunxi_partition_get_offset(index) * 512;
		fb_part.length = sunxi_partition_get_size(index) * 512;
		fb_part.flags = 0;
		printf("%-12s: %-12x  %-12x\n", fb_part.name, fb_part.start, fb_part.length);
		fastboot_flash_add_ptn(&fb_part);
		if(!strcmp(fb_part.name, "boot"))
		{
			boot_part_flag = 1;
		}
		if(boot_part_flag)
		{
			strcpy(pa_index, part_name);
			pa_index += strlen(part_name);
			*pa_index = '@';
			pa_index ++;			
			strcpy(pa_index, fb_part.name);
			pa_index += strlen(fb_part.name);
			*pa_index = ':' ;
			pa_index ++;
			part_name[4] ++;
		}
	}
	pa_index--;
	*pa_index = '\0';
	printf("-----------------------------------\n");

	setenv("partitions", partition_sets);
	
}

void fastboot_partition_init(void) {
#ifdef DEBUG
	printf("fastboot_partition_init storage type = %d\n", storage_type);
#endif
	fastboot_flash_partition_init();
}
static struct bootloader_message misc_message;

int android_misc_flash_check(void) {

	loff_t misc_offset = 0, misc_size = 0;
	size_t count = sizeof(misc_message);

	sunxi_partition_get_info_byname("misc", &misc_offset, &misc_size);
	if(!misc_offset || !misc_size) {
		sunxi_partition_get_info_byname("MISC", &misc_offset, &misc_size);
		if(!misc_offset || !misc_size) {
			puts("no misc partition is found\n");
			return 0;
		}
	}

	uint blk_start, blk_cnt, n;
	//struct mmc *mmc = find_mmc_device(mmc_card_no);

	//blk_start = ALIGN(misc_offset, mmc->read_bl_len) / mmc->read_bl_len;
	//blk_cnt   = ALIGN(count, mmc->read_bl_len) / mmc->read_bl_len;

	//n = mmc->block_dev.block_read(mmc_card_no, blk_start,
	//	blk_cnt, (uchar *)&misc_message);
	sunxi_flash_read(misc_offset, count, (void *)&misc_message);

#ifdef DEBUG
	printf("misc.command  : %s\n", misc_message.command);
	printf("misc.status   : %s\n", misc_message.status);
	printf("misc.recovery : %s\n", misc_message.recovery);
#endif
	if(!strcmp(misc_message.command, "boot-recovery")) {
		/* there is a recovery command */
		printf("find boot recovery\n");
		setenv("bootcmd", "run setargs boot_recovery");
		puts("Recovery detected, will boot recovery\n");
		/* android recovery will clean the misc */
	}

	if(!strcmp(misc_message.command, "boot-fastboot")) {
		/* there is a fastboot command */
		setenv("bootcmd", "run setargs boot_fastboot");
		puts("Fastboot detected, will enter fastboot\n");
		/* clean the misc partition ourself */
		memset(&misc_message, 0, sizeof(misc_message));
		sunxi_flash_write(misc_offset, count, (void *)&misc_message);
		//n = mmc->block_dev.block_write(mmc_card_no, blk_start,
		//	blk_cnt, (uchar *)&misc_message);

	}

	return 0;
}

int check_android_misc(void){
	int ret;
#ifdef DEBUG
	printf("check_android_misc storage type = %d\n", storage_type);
#endif
	return android_misc_flash_check();
}


/* add board specific code here */
int board_init(void) {

	gd->bd->bi_arch_number = 3495;
	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100);
#ifdef DEBUG
	printf("board_init storage_type = %d\n",storage_type);
#endif

	return 0;
}

/* Partition init must be after NAND init, so we put the fastboot
 * partition init here in the board late init.
 */
int board_late_init(void)
{
	fastboot_partition_init();
	check_android_misc();
	return 0;
}
void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	return 0;
}

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	sunxi_mmc_init(mmc_card_no);
	
	return 0;
}

int mmc_get_env_addr(struct mmc *mmc, u32 *env_addr) {

	*env_addr = sunxi_partition_get_offset_byname(CONFIG_SUNXI_ENV_PARTITION);
	return 0;
}
#endif

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	puts("Board: A10-EVB\n");
	return 0;
}
#endif
