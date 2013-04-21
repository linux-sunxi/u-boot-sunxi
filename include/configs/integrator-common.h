/*
 * (C) Copyright 2012
 * Linaro
 * Linus Walleij <linus.walleij@linaro.org>
 * Common ARM Integrator configuration settings
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

#define CONFIG_INTEGRATOR

#define CONFIG_SYS_TEXT_BASE		0x01000000
#define CONFIG_SYS_MEMTEST_START	0x100000
#define CONFIG_SYS_MEMTEST_END		0x10000000
#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_TIMERBASE		0x13000100	/* Timer1 */
#define CONFIG_SYS_LOAD_ADDR		0x7fc0	/* default load address */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size*/
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE /* Boot Argument Buffer Size*/
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128*1024) /* Size of malloc() pool */

#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_OF_LIBFDT		/* enable passing a Device Tree */
#define CONFIG_MISC_INIT_R		/* call misc_init_r during start up */

/*
 * There are various dependencies on the core module (CM) fitted
 * Users should refer to their CM user guide
 */
#include "armcoremodule.h"

/*
 * Initialize and remap the core module, use SPD to detect memory size
 * If CONFIG_SKIP_LOWLEVEL_INIT is not defined &
 * the core module has a CM_INIT register
 * then the U-Boot initialisation code will
 * e.g. ARM Boot Monitor or pre-loader is repeated once
 * (to re-initialise any existing CM_INIT settings to safe values).
 *
 * This is usually not the desired behaviour since the platform
 * will either reboot into the ARM monitor (or pre-loader)
 * or continuously cycle thru it without U-Boot running,
 * depending upon the setting of Integrator/CP switch S2-4.
 *
 * However it may be needed if Integrator/CP switch S2-1
 * is set OFF to boot direct into U-Boot.
 * In that case comment out the line below.
 */
#define CONFIG_CM_INIT
#define CONFIG_CM_REMAP
#define CONFIG_CM_SPD_DETECT

/*
 * The ARM boot monitor initializes the board.
 * However, the default U-Boot code also performs the initialization.
 * If desired, this can be prevented by defining SKIP_LOWLEVEL_INIT
 * - see documentation supplied with board for details of how to choose the
 * image to run at reset/power up
 * e.g. whether the ARM Boot Monitor runs before U-Boot
 */
/* #define CONFIG_SKIP_LOWLEVEL_INIT */

/*
 * The ARM boot monitor does not relocate U-Boot.
 * However, the default U-Boot code performs the relocation check,
 * and may relocate the code if the memory map is changed.
 * If necessary this can be prevented by defining SKIP_RELOCATE_UBOOT
 */
/* #define SKIP_CONFIG_RELOCATE_UBOOT */


/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1		/* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		0x00000000	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x08000000	/* 128 MB */
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
#define CONFIG_SYS_INIT_RAM_SIZE PHYS_SDRAM_1_SIZE
#define CONFIG_SYS_GBL_DATA_OFFSET (CONFIG_SYS_SDRAM_BASE + \
				    CONFIG_SYS_INIT_RAM_SIZE - \
				    GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR CONFIG_SYS_GBL_DATA_OFFSET
