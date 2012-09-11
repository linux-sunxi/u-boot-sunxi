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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 */
#define CONFIG_ALLWINNER			/* It's a Allwinner chip */
#define	CONFIG_SUNXI				/* which is sunxi family */
#define CONFIG_SUN4I				/* which is sun4i */
#define CONFIG_A10_EVB				/* working with A10-EVB board */

#include <asm/arch/cpu.h>			/* get chip and board defs */

#define CONFIG_SYS_TEXT_BASE		0x4A000000

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* Clock Defines */


/* Serial & console */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)		/* ns16550 reg in the low bits of cpu reg */
#define CONFIG_SYS_NS16550_CLK		(24000000)
#define CONFIG_SYS_NS16550_COM1		SUNXI_UART0_BASE
#define CONFIG_SYS_NS16550_COM2		SUNXI_UART1_BASE
#define CONFIG_SYS_NS16550_COM3		SUNXI_UART2_BASE
#define CONFIG_SYS_NS16550_COM4		SUNXI_UART3_BASE

#define CONFIG_CONS_INDEX			1			/* which serial channel for console */

/* DRAM Base */
#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_INIT_RAM_ADDR	0x0
#define CONFIG_SYS_INIT_RAM_SIZE	0x8000      /* 32K */

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* A10-EVB has 1 banks of DRAM, we use only one in U-Boot */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1				CONFIG_SYS_SDRAM_BASE	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE			(512 << 20)				/* 0x20000000, 512 MB Bank #1 */

#if 0
/* Nand config */
#define CONFIG_NAND
#define CONFIG_NAND_SUNXI
#define CONFIG_CMD_NAND                         /* NAND support */
#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_BASE            0x00
#endif

#define CONFIG_CMD_MEMORY

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING

/* mmc config */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_MMC
#define CONFIG_MMC_SUNXI
#define CONFIG_MMC_SUNXI_SLOT		0		/* which mmc slot to use, could be 0,1,2,3 */
#define CONFIG_MMC_SUNXI_USE_DMA
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		CONFIG_MMC_SUNXI_SLOT		/* env in which mmc */

#define CONFIG_DOS_PARTITION

/*
 * Size of malloc() pool
 * 1MB = 0x100000, 0x100000 = 1024 * 1024
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP				/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER			/* use "hush" command parser	*/
#define CONFIG_CMD_ECHO
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_PROMPT		"sun4i#"
#define CONFIG_SYS_CBSIZE	256			/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384			/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16			/* max number of command args */

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE			CONFIG_SYS_CBSIZE

/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 256<<20)	/* 256M */
#define CONFIG_SYS_LOAD_ADDR		0x50000000					/* default load address */

#define CONFIG_SYS_HZ				1000

/* valid baudrates */
#define CONFIG_BAUDRATE				115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE			(256 << 10)				/* 256 KB */


/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* 256 KB */
#define CONFIG_IDENT_STRING			" Allwinner Technology "
#define CONFIG_MACH_TYPE			4104

#define CONFIG_ENV_OFFSET			(544 << 10) /* (8 + 24 + 512)KB */
#define CONFIG_ENV_SIZE				(128 << 10)	/* 128KB */
#define CONFIG_CMD_SAVEENV

#define CONFIG_BOOTCOMMAND \
	"if run loadbootenv; then " \
		"echo Loaded environment from ${bootenv};" \
		"env import -t ${scriptaddr} ${filesize};" \
	"fi;" \
	"if test -n ${uenvcmd}; then " \
		"echo Running uenvcmd ...;" \
		"run uenvcmd;" \
	"fi;" \
	"if run loadbootscr; then "\
		"echo Jumping to ${bootscr};" \
		"source ${scriptaddr};" \
	"fi;" \
	"run setargs boot_mmc;" \

#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=ttyS0,115200\0" \
	"root=/dev/mmcblk0p2\0" \
	"panicarg=panic=10\0" \
	"extraargs=\0" \
	"loglevel=8\0" \
	"scriptaddr=0x44000000\0" \
	"setargs=setenv bootargs console=${console} root=${root}" \
	" loglevel=${loglevel} ${panicarg} ${extraargs}\0" \
	"kernel=uImage\0" \
	"bootenv=uEnv.txt\0" \
	"bootscr=boot.scr\0" \
	"loadbootscr=fatload mmc 0 $scriptaddr ${bootscr} || ext2load mmc 0 $scriptaddr ${bootscr} || ext2load mmc 0 $scriptaddr boot/${bootscr}\0" \
	"loadbootenv=fatload mmc 0 $scriptaddr ${bootenv} || ext2load mmc 0 $scriptaddr ${bootenv} || ext2load mmc 0 $scriptaddr boot/${bootenv}\0" \
	"boot_mmc=fatload mmc 0 0x43000000 script.bin; fatload mmc 0 0x48000000 ${kernel}; bootm 0x48000000\0"

#define CONFIG_BOOTDELAY	3
#define CONFIG_SYS_BOOT_GET_CMDLINE
#define CONFIG_AUTO_COMPLETE

#define CONFIG_CMD_FAT			/* with this we can access fat bootfs */
#define CONFIG_FAT_WRITE		/* enable write access */ 
#define CONFIG_CMD_EXT2			/* with this we can access ext2 bootfs */
#define CONFIG_CMD_BOOTA		/* boot android image */
#define CONFIG_CMD_RUN			/* run a command */
#define CONFIG_CMD_BOOTD		/* boot the default command */
#define CONFIG_CMD_SOURCE		/* boot.scr suppot */
#define CONFIG_CMD_BDI			/* info about boot descriptor */
#define CONFIG_CMD_LOADB		/* load binaries over serial */
#define CONFIG_CMD_SAVES		/* save data over serial */
#define CONFIG_CMD_DUMPASCII		/* save ascii data over serial */

#define CONFIG_SPL

#define CONFIG_SPL_BSS_START_ADDR	0x50000000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000		/* 512 KB */

#define CONFIG_SPL_TEXT_BASE       0x0          /* sram start */
#define CONFIG_SPL_MAX_SIZE        0x8000       /* 32 KB */

#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBDISK_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_MMC_SUPPORT

#define LOW_LEVEL_SRAM_STACK		0x00006000				/* end of 24KB in sram */
#define CONFIG_SPL_STACK         LOW_LEVEL_SRAM_STACK
#define CONFIG_SPL_LDSCRIPT "arch/arm/cpu/armv7/sunxi/u-boot-spl.lds"

#define CONFIG_MMC_U_BOOT_SECTOR_START    (64)    /* 32KB offset */
#define CONFIG_MMC_U_BOOT_SECTOR_COUNT    (1000)  /* 512KB, enough for a full u-boot.bin */

#endif /* __CONFIG_H */
