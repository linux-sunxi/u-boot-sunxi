/*
 * (C) Copyright 2012-2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Configuration settings for the Allwinner sunxi series of boards.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_COMMON_CONFIG_H
#define _SUNXI_COMMON_CONFIG_H

/*
 * High Level Configuration Options
 */
#define CONFIG_SUNXI		/* sunxi family */
#ifdef CONFIG_SPL_BUILD
#ifndef CONFIG_SPL_FEL
#define CONFIG_SYS_THUMB_BUILD	/* Thumbs mode to save space in SPL */
#define CONFIG_SPL_NAND_BOOT
#endif
#endif

#include <asm/arch/cpu.h>	/* get chip and board defs */

#define CONFIG_SYS_TEXT_BASE		0x4a000000

/*asdfljsadf*/
#define CONFIG_SPL_NAND_BOOT
/*#define CONFIG_SYS_NAND_U_BOOT_OFFS     0x200000
#define CONFIG_SYS_NAND_U_BOOT_DST	0x44000000
#define CONFIG_SYS_NAND_U_BOOT_SIZE	0x100000
#define CONFIG_SYS_NAND_U_BOOT_START	0x00
*/
#define MTDIDS_DEFAULT "nand0=mtd-nand-sunxi.0"
#define MTDPARTS_DEFAULT "mtdparts=mtd-nand-sunxi.0:"       \
        "16m(mbr),"                                \
        "16m(bootloader),"                                \
        "16m(env),"                                \
        "16m(boot),"                                \
        "640m(system),"                                \
        "512m(data),"                                \
        "16m(misc),"                                \
        "32m(recovery),"                                \
        "640m(cache),"                                \
        "256m(databk),"                                \
        "16m(private),"                                \
        "-(UDISK)"
 
/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* Serial & console */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
/* ns16550 reg in the low bits of cpu reg */
#define CONFIG_SYS_NS16550_REG_SIZE	-4
#define CONFIG_SYS_NS16550_CLK		24000000
#define CONFIG_SYS_NS16550_COM1		SUNXI_UART0_BASE
#define CONFIG_SYS_NS16550_COM2		SUNXI_UART1_BASE
#define CONFIG_SYS_NS16550_COM3		SUNXI_UART2_BASE
#define CONFIG_SYS_NS16550_COM4		SUNXI_UART3_BASE
#define CONFIG_SYS_NS16550_COM5		SUNXI_R_UART_BASE

/* DRAM Base */
#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_INIT_RAM_ADDR	0x0
#define CONFIG_SYS_INIT_RAM_SIZE	0x8000	/* 32 KiB */

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* A10 has 1 banks of DRAM, we use only bank 1 in U-Boot */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_0			CONFIG_SYS_SDRAM_BASE
#if defined(CONFIG_SUN6I) || defined(CONFIG_SUN7I)
#define PHYS_SDRAM_0_SIZE		0x80000000 /* 2 GiB */
#else
#define PHYS_SDRAM_0_SIZE		0x40000000 /* 1 GiB */
#endif

#define CONFIG_CMD_EXT4_WRITE
#define CONFIG_BZIP2


#ifdef CONFIG_NAND
/* Nand config */
#define CONFIG_LIB_RAND
#define CONFIG_NAND_SUNXI 1
#define CONFIG_CMD_NAND                         /* NAND support */
#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_BASE            0x00
#define CONFIG_SUNXI_DMA

#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_PACKIMG
#define CONFIG_CMD_NAND_PACKIMG
#define CONFIG_CMD_NAND_1K

#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_ECC
#define CONFIG_SPL_NAND_BASE
#define CONFIG_SPL_NAND_DRIVERS
#define CONFIG_SPL_DMA_SUPPORT
#define CONFIG_SYS_NAND_U_BOOT_OFFS     0x100000

#define CONFIG_SPL_OS_BOOT
/*#define CONFIG_SYS_SPL_ARGS_ADDR        0x44000000*/
#define CONFIG_SYS_NAND_SPL_KERNEL_OFFS 0x1100000
#define CONFIG_SUNXI_PACKIMG_START      0x800000
#define CONFIG_SUNXI_PACKIMG_END        0x1100000

#define CONFIG_SYS_NAND_PAGE_SIZE sunxi_nand_spl_page_size
#define CONFIG_SYS_NAND_BLOCK_SIZE sunxi_nand_spl_block_size

#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET	0x2000000 /*0x400000*/	/* at 4MB */
#define CONFIG_ENV_SIZE		(128<<10)
/*#define CONFIG_ENV_RANGE        0x300000*/
#else
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_OFFSET		(544 << 10) /* (8 + 24 + 512) KiB */
#define CONFIG_ENV_SIZE			(128 << 10)	/* 128 KiB */
#define CONFIG_SPL_LIBDISK_SUPPORT
#define CONFIG_SPL_MMC_SUPPORT

#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR        80      /* 40KiB */
#define CONFIG_SPL_PAD_TO              32768           /* decimal for 'dd' */

#endif

#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_SETEXPR

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG

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
#define CONFIG_SYS_MMC_ENV_DEV		0	/* first detected MMC controller */
#endif

/* 4MB of malloc() pool */
/*#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (4 << 20)) */
/* 16MB of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (16 << 20))

/*
 * Miscellaneous configurable options
 */
#define CONFIG_CMD_ECHO
#define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */
#define CONFIG_SYS_GENERIC_BOARD

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_SYS_LOAD_ADDR		0x48000000 /* default load address */

/* standalone support */
#define CONFIG_STANDALONE_LOAD_ADDR	0x48000000

#define CONFIG_SYS_HZ			1000

/* baudrate */
#define CONFIG_BAUDRATE			115200

/* The stack sizes are set up in start.S using the settings below */
#define CONFIG_STACKSIZE		(256 << 10)	/* 256 KiB */

/* FLASH and environment organization */

#define CONFIG_SYS_NO_FLASH

#define CONFIG_SYS_MONITOR_LEN		(512 << 10)	/* 512 KiB */
#define CONFIG_IDENT_STRING		" Allwinner Technology"

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
	"bootm_size=0x10000000\0" \
	"console=ttyS0,115200\0" \
	"panicarg=panic=10\0" \
	"extraargs=\0" \
	"loglevel=8\0" \
	"scriptaddr=0x44000000\0" \
	"device=mmc\0" \
	"partition=0:1\0" \
	"setargs=" \
	  "if test -z \\\\\"$root\\\\\"; then"\
	    " if test \\\\\"$bootpath\\\\\" = \"/boot/\"; then"\
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
	"script=script.bin\0" \
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
	    "ext2load $device $partition 0x43000000 ${bootpath}${script}" \
	    " && " \
	    "ext2load $device $partition 0x48000000 ${bootpath}${kernel}" \
	  ";then true; elif " \
	    "bootpath=/" \
	    " && " \
	    "fatload $device $partition 0x43000000 ${script}" \
	    " && " \
	    "fatload $device $partition 0x48000000 ${kernel}" \
	  ";then true; elif " \
	    "bootpath=/" \
	    " && " \
	    "ext2load $device $partition 0x43000000 ${bootpath}${script}" \
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
	"mtdparts=" \
	  MTDPARTS_DEFAULT \
	  "\0" \
	""

#define CONFIG_SYS_BOOT_GET_CMDLINE

#include <config_cmd_default.h>

#define CONFIG_FAT_WRITE	/* enable write access */

#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_DISPLAY_PRINT

/* Falcon boot mode support */
/* Disabled by default on sun4i/sun7i. Many GCC versions produces a too
 * large SPL for A10/A20 with this on. sun5i however accepts a much larger
 * SPL
 */
#if defined( CONFIG_SUN5I ) || defined ( CONFIG_SYS_THUMB_BUILD )
#define CONFIG_SPL_OS_BOOT
#endif

#ifdef CONFIG_SPL_FEL

#define CONFIG_SPL
#define CONFIG_SPL_LDSCRIPT "arch/arm/cpu/armv7/sunxi/u-boot-spl-fel.lds"
#define CONFIG_SPL_START_S_PATH "arch/arm/cpu/armv7/sunxi"
#define CONFIG_SPL_TEXT_BASE		0x2000
#define CONFIG_SPL_MAX_SIZE		0x4000		/* 16 KiB */

#else /* CONFIG_SPL */

#define CONFIG_SPL_BSS_START_ADDR	0x4ff80000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000		/* 512 KiB */

#define CONFIG_SPL_TEXT_BASE		0x20		/* sram start+header */
#ifdef CONFIG_SUN5I
#define CONFIG_SPL_MAX_SIZE		0x75e0		/* 7748+ is used */
#else
#define CONFIG_SPL_MAX_SIZE		0x5fe0		/* 24KB on sun4i/sun7i */
#endif

/* #define CONFIG_SPL_LIBDISK_SUPPORT */
/* #define CONFIG_SPL_MMC_SUPPORT */

#define CONFIG_SPL_LDSCRIPT "arch/arm/cpu/armv7/sunxi/u-boot-spl.lds"

/* #define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR	80	*//* 40KiB */
/* #define CONFIG_SPL_PAD_TO		32768		*//* decimal for 'dd' */

#endif /* CONFIG_SPL */
/* end of 32 KiB in sram */
#define LOW_LEVEL_SRAM_STACK		0x00008000 /* End of sram */
#define CONFIG_SPL_STACK		LOW_LEVEL_SRAM_STACK
#define CONFIG_SYS_SPL_MALLOC_START	0x4ff00000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x00080000	/* 512 KiB */

#ifdef CONFIG_SPL_OS_BOOT
#define CONFIG_CMD_SPL
#define CONFIG_SYS_SPL_ARGS_ADDR		(PHYS_SDRAM_0 + 0x100)
#ifdef CONFIG_SPL_MMC_SUPPORT
#define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR	1344
#define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS  256
#define CONFIG_SYS_MMCSD_RAW_MODE_KERNEL_SECTOR 1600
#endif
#endif

#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_NFS

/* I2C */
#if !defined CONFIG_SUN6I && !defined CONFIG_SUN8I
#define CONFIG_SPL_I2C_SUPPORT
#endif
/* No CONFIG_SYS_I2C as we use the non converted mvtwsi driver */
#define CONFIG_HARD_I2C
#define CONFIG_SYS_I2C_SUNXI
#define CONFIG_SYS_I2C_SPEED		400000
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
#if !defined CONFIG_AXP152_POWER && !defined CONFIG_AXP221_POWER && !defined CONFIG_NO_AXP
#define CONFIG_AXP209_POWER
#endif
#if defined CONFIG_AXP152_POWER || defined CONFIG_AXP209_POWER || defined CONFIG_AXP221_POWER
#define CONFIG_SPL_POWER_SUPPORT
#endif

#ifdef CONFIG_STATUSLED
#define STATUS_LED_BIT CONFIG_STATUSLED
#endif
#ifdef CONFIG_STATUSLED1
#define STATUS_LED_BIT1 CONFIG_STATUSLED1
#endif
#ifdef CONFIG_STATUSLED2
#define STATUS_LED_BIT2 CONFIG_STATUSLED2
#endif
#ifdef CONFIG_STATUSLED3
#define STATUS_LED_BIT3 CONFIG_STATUSLED3
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
#endif

#ifdef CONFIG_SUNXI_GMAC
#define CONFIG_DESIGNWARE_ETH		/* GMAC can use designware driver */
#define CONFIG_DW_AUTONEG
#define CONFIG_PHY_GIGE			/* GMAC can use gigabit PHY	*/
#define CONFIG_PHY_ADDR		1
#define CONFIG_MII			/* MII PHY management		*/
#define CONFIG_PHYLIB
#endif

#ifdef CONFIG_CMD_NET
#define CONFIG_CMD_NFS
#define CONFIG_CMD_DNS
#define CONFIG_NETCONSOLE
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#endif

#if !defined CONFIG_ENV_IS_IN_MMC && \
    !defined CONFIG_ENV_IS_IN_NAND && \
    !defined CONFIG_ENV_IS_IN_FAT && \
    !defined CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_MISC_INIT_R

#ifndef CONFIG_SPL_BUILD
#include <config_distro_defaults.h>
#endif

#endif /* _SUNXI_COMMON_CONFIG_H */
