/*
 *  (C) Copyright 2010-2012
 *  NVIDIA Corporation <www.nvidia.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _TEGRA20_COMMON_H_
#define _TEGRA20_COMMON_H_
#include "tegra-common.h"

/*
 * Errata configuration
 */
#define CONFIG_ARM_ERRATA_716044
#define CONFIG_ARM_ERRATA_742230
#define CONFIG_ARM_ERRATA_751472

/*
 * NS16550 Configuration
 */
#define V_NS16550_CLK		216000000	/* 216MHz (pllp_out0) */

/*
 * High Level Configuration Options
 */
#define CONFIG_TEGRA20				/* in a NVidia Tegra20 core */

/* Environment information, boards can override if required */
#define CONFIG_LOADADDR		0x00408000	/* def. location for kernel */

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LOAD_ADDR	0x00A00800	/* default */
#define CONFIG_STACKBASE	0x02800000	/* 40MB */

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_SYS_TEXT_BASE	0x0010E000

/*
 * Memory layout for where various images get loaded by boot scripts:
 *
 * scriptaddr can be pretty much anywhere that doesn't conflict with something
 *   else. Put it above BOOTMAPSZ to eliminate conflicts.
 *
 * kernel_addr_r must be within the first 128M of RAM in order for the
 *   kernel's CONFIG_AUTO_ZRELADDR option to work. Since the kernel will
 *   decompress itself to 0x8000 after the start of RAM, kernel_addr_r
 *   should not overlap that area, or the kernel will have to copy itself
 *   somewhere else before decompression. Similarly, the address of any other
 *   data passed to the kernel shouldn't overlap the start of RAM. Pushing
 *   this up to 16M allows for a sizable kernel to be decompressed below the
 *   compressed load address.
 *
 * fdt_addr_r simply shouldn't overlap anything else. Choosing 32M allows for
 *   the compressed kernel to be up to 16M too.
 *
 * ramdisk_addr_r simply shouldn't overlap anything else. Choosing 33M allows
 *   for the FDT/DTB to be up to 1M, which is hopefully plenty.
 */
#define MEM_LAYOUT_ENV_SETTINGS \
	"scriptaddr=0x10000000\0" \
	"kernel_addr_r=0x01000000\0" \
	"fdt_addr_r=0x02000000\0" \
	"ramdisk_addr_r=0x02100000\0"

/* Defines for SPL */
#define CONFIG_SPL_TEXT_BASE		0x00108000
#define CONFIG_SYS_SPL_MALLOC_START	0x00090000
#define CONFIG_SPL_STACK		0x000ffffc

#define CONFIG_SPL_LDSCRIPT		"$(CPUDIR)/tegra20/u-boot-spl.lds"

/* Align LCD to 1MB boundary */
#define CONFIG_LCD_ALIGNMENT	MMU_SECTION_SIZE

#ifdef CONFIG_TEGRA_LP0
#define TEGRA_LP0_ADDR			0x1C406000
#define TEGRA_LP0_SIZE			0x2000
#define TEGRA_LP0_VEC \
	"lp0_vec=" __stringify(TEGRA_LP0_SIZE)  \
	"@" __stringify(TEGRA_LP0_ADDR) " "
#else
#define TEGRA_LP0_VEC
#endif

/*
 * This parameter affects a TXFILLTUNING field that controls how much data is
 * sent to the latency fifo before it is sent to the wire. Without this
 * parameter, the default (2) causes occasional Data Buffer Errors in OUT
 * packets depending on the buffer address and size.
 */
#define CONFIG_USB_EHCI_TXFIFO_THRESH	10
#define CONFIG_EHCI_IS_TDI

/* Total I2C ports on Tegra20 */
#define TEGRA_I2C_NUM_CONTROLLERS	4

#define CONFIG_SYS_NAND_SELF_INIT
#define CONFIG_SYS_NAND_ONFI_DETECTION

#endif /* _TEGRA20_COMMON_H_ */
