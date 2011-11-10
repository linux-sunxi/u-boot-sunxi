/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Configuration settings for the Allwinner A10-evb board.
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

DECLARE_GLOBAL_DATA_PTR;



void fastboot_partition_init(void)
{
	fastboot_ptentry ptn[4] = {
		{
			.name	= "mbr",
			.start	= 0x00,		/* start sector */
			.length	= 0x200,	/* 512B */
			.flags	= 0,
		},
		{
			.name	= "bootfs",
			.start	= 0x100000,	/* start sector */
			.length	= 0x100000,	/* 512B */
			.flags	= 0,
		},
		{
			.name	= "rootfs",
			.start	= 0x400000,	/* start sector */
			.length	= 0x400000,	/* 512B */
			.flags	= 0,
		},
		{
			.name	= "datafs",
			.start	= 0x800000,	/* start sector */
			.length	= 0x800000,	/* 512B */
			.flags	= 0,
		},
		/* Rest fastboot can not see */
	};

	int i;
	for (i=0; i < 4; i++) {
		fastboot_flash_add_ptn(&ptn[i]);;
	}
}

/* TODO add board specific code here */
int board_init(void)
{
	gd->bd->bi_arch_number = 0x1000;
	gd->bd->bi_boot_params = 0x50000000;
	fastboot_partition_init();
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

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("Board:\tA10-EVB\n");
	return 0;
}
#endif
