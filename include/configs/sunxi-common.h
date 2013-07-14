/*
 * (C) Copyright 2012-2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * Based on older sun4i config
 *
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

#ifndef _SUNXI_COMMON_CONFIG_H
#define _SUNXI_COMMON_CONFIG_H

/*
 * High Level Configuration Options
 */
#define CONFIG_ALLWINNER	/* It's a Allwinner chip */
#define CONFIG_SUNXI		/* which is sunxi family */

#include <asm/arch/cpu.h>	/* get chip and board defs */

#define CONFIG_SYS_TEXT_BASE		0x4A000000

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* Serial & console */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
/* ns16550 reg in the low bits of cpu reg */
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		(24000000)
#define CONFIG_SYS_NS16550_COM1		SUNXI_UART0_BASE
#define CONFIG_SYS_NS16550_COM2		SUNXI_UART1_BASE
#define CONFIG_SYS_NS16550_COM3		SUNXI_UART2_BASE
#define CONFIG_SYS_NS16550_COM4		SUNXI_UART3_BASE

/* DRAM Base */
#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_INIT_RAM_ADDR	0x0
#define CONFIG_SYS_INIT_RAM_SIZE	0x8000	/* 32K */

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* A10-EVB has 1 banks of DRAM, we use only one in U-Boot */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			CONFIG_SYS_SDRAM_BASE
#if 0
/* Nand config */
#define CONFIG_NAND
#define CONFIG_NAND_SUNXI
#define CONFIG_CMD_NAND                         /* NAND support */
#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_BASE            0x00
#endif

#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_SETEXPR

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING

/* mmc config */
/* Can't use MMC slot 0 if the UART is directed there */
#if !defined CONFIG_UART0_PORT_F || CONFIG_MMC_SUNXI_SLOT != 0
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_MMC
#define CONFIG_MMC_SUNXI
#ifndef CONFIG_MMC_SUNXI_SLOT
#define CONFIG_MMC_SUNXI_SLOT		0
#endif
#define CONFIG_MMC_SUNXI_USE_DMA
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		CONFIG_MMC_SUNXI_SLOT
#endif

/*
 * Size of malloc() pool
 * 1MB = 0x100000, 0x100000 = 1024 * 1024
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))

/* Flat Device Tree (FDT/DT) support */
#define CONFIG_OF_LIBFDT
#define CONFIG_SYS_BOOTMAPSZ		(16 << 20)

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP	/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER	/* use "hush" command parser    */
#define CONFIG_CMD_ECHO
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + (256 << 20))
#define CONFIG_SYS_LOAD_ADDR		0x50000000 /* default load address */

/* standalone support */
#define CONFIG_STANDALONE_LOAD_ADDR	0x50000000

#define CONFIG_SYS_HZ			1000

/* valid baudrates */
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/* The stack sizes are set up in start.S using the settings below */
#define CONFIG_STACKSIZE		(256 << 10)	/* 256 KB */

/* FLASH and environment organization */

#define CONFIG_SYS_NO_FLASH

#define CONFIG_SYS_MONITOR_LEN		(512 << 10)	/* 512 KB */
#define CONFIG_IDENT_STRING		" Allwinner Technology"

#define CONFIG_ENV_OFFSET		(544 << 10) /* (8 + 24 + 512)KB */
#define CONFIG_ENV_SIZE			(128 << 10)	/* 128KB */

#ifdef CONFIG_SPL_FEL
#define RUN_BOOT_RAM	"run boot_ram;"
#else
#define RUN_BOOT_RAM	""
#endif

#define CONFIG_BOOTCOMMAND \
	RUN_BOOT_RAM \
	"if run loadbootenv; then " \
	  "echo Loaded environment from ${bootenv};" \
	  "env import -t ${scriptaddr} ${filesize};" \
	"fi;" \
	"if test -n \\\"${uenvcmd}\\\"; then " \
	  "echo Running uenvcmd ...;" \
	  "run uenvcmd;" \
	"fi;" \
	"if run loadbootscr; then "\
	  "echo Jumping to ${bootscr};" \
	  "source ${scriptaddr};" \
	"fi;" \
	"run autoboot;" \
	""

#ifdef CONFIG_CMD_WATCHDOG
#define	RESET_WATCHDOG "watchdog 0"
#else
#define RESET_WATCHDOG "true"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=ttyS0,115200\0" \
	"panicarg=panic=10\0" \
	"extraargs=\0" \
	"loglevel=8\0" \
	"scriptaddr=0x44000000\0" \
	"device=mmc\0" \
	"partition=0:1\0" \
	"setargs=" \
	  "if test -z \\\\\"$root\\\\\"; then"\
	    " if test \\\\\"$bootpath\\\\\" = \"boot/\"; then"\
	      " root=\"/dev/mmcblk0p1 rootwait\";"\
	    " else" \
	      " root=\"/dev/mmcblk0p2 rootwait\";"\
	    " fi;"\
	  " fi;"\
	  " setenv bootargs console=${console} root=${root}" \
	  " loglevel=${loglevel} ${panicarg} ${extraargs}" \
	  "\0" \
	"kernel=uImage\0" \
	"bootenv=uEnv.txt\0" \
	"bootscr=boot.scr\0" \
	"loadbootscr=" \
	  "fatload $device $partition $scriptaddr ${bootscr}" \
	  " || " \
	  "ext2load $device $partition $scriptaddr boot/${bootscr}" \
	  " ||" \
	  "ext2load $device $partition $scriptaddr ${bootscr}" \
	  "\0" \
	"loadbootenv=" \
	  "fatload $device $partition $scriptaddr ${bootenv}" \
	  " || " \
	  "ext2load $device $partition $scriptaddr boot/${bootenv}" \
	  " || " \
	  "ext2load $device $partition $scriptaddr ${bootenv}" \
	  "\0" \
	"loadkernel=" \
	  "if "\
	    "bootpath=/boot/" \
	    " && " \
	    "ext2load $device $partition 0x43000000 ${bootpath}script.bin" \
	    " && " \
	    "ext2load $device $partition 0x48000000 ${bootpath}${kernel}" \
	  ";then true; elif " \
	    "bootpath=/" \
	    " && " \
	    "fatload $device $partition 0x43000000 script.bin" \
	    " && " \
	    "fatload $device $partition 0x48000000 ${kernel}" \
	  ";then true; elif " \
	    "bootpath=/" \
	    " && " \
	    "ext2load $device $partition 0x43000000 ${bootpath}script.bin" \
	    " && " \
	    "ext2load $device $partition 0x48000000 ${bootpath}${kernel}" \
	  ";then true; else "\
	    "false" \
	  ";fi" \
	  "\0" \
	"autoboot=" \
	  "run loadkernel" \
	  " && " \
	  "run setargs" \
	  " && " \
	  RESET_WATCHDOG \
	  " && " \
	  "bootm 0x48000000" \
	  "\0" \
	"boot_ram=" \
	  "saved_stdout=$stdout;setenv stdout nc;"\
	  "if iminfo 0x41000000; then" \
	    " " RESET_WATCHDOG ";"\
	    " setenv stdout $saved_stdout;" \
	    " source 0x41000000;" \
	  "else" \
	    " setenv stdout $saved_stdout;" \
	  "fi" \
	  "\0" \
	""

#define CONFIG_BOOTDELAY	3
#define CONFIG_SYS_BOOT_GET_CMDLINE
#define CONFIG_AUTO_COMPLETE

#include <config_cmd_default.h>

/* Accept zimage + raw ramdisk without mkimage headers */
#define CONFIG_CMD_BOOTZ
#define CONFIG_SUPPORT_RAW_INITRD

#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_FAT		/* with this we can access fat bootfs */
#define CONFIG_FAT_WRITE	/* enable write access */
#define CONFIG_CMD_EXT2		/* with this we can access ext2 bootfs */
#define CONFIG_CMD_EXT4		/* with this we can access ext4 bootfs */
#define CONFIG_CMD_ZFS		/* with this we can access ZFS bootfs */

#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_DISPLAY_PRINT

#ifdef CONFIG_SPL_FEL

#define CONFIG_SPL
#define CONFIG_SPL_LDSCRIPT "arch/arm/cpu/armv7/sunxi/u-boot-spl-fel.lds"
#define CONFIG_SPL_START_S_PATH "arch/arm/cpu/armv7/sunxi"
#define CONFIG_SPL_TEXT_BASE		0x2000
#define CONFIG_SPL_MAX_SIZE		0x4000		/* 24 KB */

#else /* CONFIG_SPL */

#define CONFIG_SPL_BSS_START_ADDR	0x50000000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000		/* 512 KB */

#define CONFIG_SPL_TEXT_BASE		0x20		/* sram start+header */
#define CONFIG_SPL_MAX_SIZE		0x6000		/* 24 KB */

#define CONFIG_SPL_LIBDISK_SUPPORT
#define CONFIG_SPL_MMC_SUPPORT

#define CONFIG_SPL_LDSCRIPT "arch/arm/cpu/armv7/sunxi/u-boot-spl.lds"

/* 32KB offset */
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR	64
/* SPL starts at offset 8KiB im MMC and has the size of 24KiB */
#define CONFIG_SPL_PAD_TO		24576		/* decimal for 'dd' */

#endif /* CONFIG_SPL */
/* end of 24KB in sram */
#define LOW_LEVEL_SRAM_STACK		0x00006000
#define CONFIG_SPL_STACK		LOW_LEVEL_SRAM_STACK

#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_NFS

/* I2C */
#define CONFIG_SPL_I2C_SUPPORT
#define CONFIG_SYS_I2C_SPEED		400000
#define CONFIG_HARD_I2C
#define CONFIG_SUNXI_I2C
#define CONFIG_SYS_I2C_SLAVE		0x7f
#define CONFIG_CMD_I2C

/* Watchdog */
#if 0
#define CONFIG_WATCHDOG			/* automatic watchdog support */
#define CONFIG_CMD_WATCHDOG		/* watchdog command setting the watchdog timeout */
#endif

/* GPIO */
#define CONFIG_SUNXI_GPIO
#define CONFIG_CMD_GPIO

/* PMU */
#if !defined CONFIG_AXP152_POWER && !defined CONFIG_NO_AXP
#define CONFIG_AXP209_POWER
#endif
#if defined CONFIG_AXP152_POWER || defined CONFIG_AXP209_POWER
#define CONFIG_SPL_POWER_SUPPORT
#endif

#ifdef CONFIG_STATUSLED
#define STATUS_LED_BIT CONFIG_STATUSLED
#endif
#ifdef CONFIG_STATUSLED1
#define STATUS_LED_BIT1 CONFIG_STATUSLED1
#endif
#ifdef CONFIG_STATUSLED2
#define STATUS_LED_BIT2 CONFIG_STATUSLED1
#endif
#ifdef CONFIG_STATUSLED3
#define STATUS_LED_BIT3 CONFIG_STATUSLED1
#endif

#ifndef CONFIG_SPL_BUILD
#ifdef STATUS_LED_BIT
#define CONFIG_GPIO_LED
#define CONFIG_STATUS_LED
#ifndef STATUS_LED_BOOT
#define STATUS_LED_BOOT 0
#endif
#ifndef STATUS_LED_STATE
#define STATUS_LED_STATE STATUS_LED_ON
#define STATUS_LED_PERIOD 1
#endif
#ifndef STATUS_LED_STATE1
#define STATUS_LED_STATE1 STATUS_LED_OFF
#define STATUS_LED_PERIOD1 1
#endif
#ifndef STATUS_LED_STATE2
#define STATUS_LED_STATE2 STATUS_LED_OFF
#define STATUS_LED_PERIOD2 1
#endif
#ifndef STATUS_LED_STATE3
#define STATUS_LED_STATE3 STATUS_LED_OFF
#define STATUS_LED_PERIOD3 1
#endif
#define CONFIG_BOARD_SPECIFIC_LED
#define CONFIG_CMD_LED
#endif
#endif

/* Define this to have serial channel 1 (UART0) redirected to SD port */
/* #define CONFIG_UART0_PORT_F */

#ifndef CONFIG_CONS_INDEX
#define CONFIG_CONS_INDEX              1       /* UART0 */
#endif

/* Ethernet support */
#ifdef CONFIG_SUNXI_EMAC
#define CONFIG_MII			/* MII PHY management		*/
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#endif

#ifdef CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_NFS
#define CONFIG_CMD_SNTP
#define CONFIG_TIMESTAMP		/* Needed by SNTP */
#define CONFIG_CMD_DNS
#define CONFIG_NETCONSOLE
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_NISDOMAIN
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_DNS
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#define CONFIG_BOOTP_NTPSERVER
#define CONFIG_BOOTP_TIMEOFFSET
#define CONFIG_BOOTP_MAY_FAIL
#define CONFIG_BOOTP_SERVERIP
#define CONFIG_BOOTP_DHCP_REQUEST_DELAY		50000
#endif

#if !defined CONFIG_ENV_IS_IN_MMC && \
    !defined CONFIG_ENV_IS_IN_NAND && \
    !defined CONFIG_ENV_IS_IN_FAT && \
    !defined CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_IS_NOWHERE
#endif

#endif /* _SUNXI_COMMON_CONFIG_H */
