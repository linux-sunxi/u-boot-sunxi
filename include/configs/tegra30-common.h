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

#ifndef _TEGRA30_COMMON_H_
#define _TEGRA30_COMMON_H_
#include "tegra-common.h"

/*
 * Errata configuration
 */
#define CONFIG_ARM_ERRATA_743622
#define CONFIG_ARM_ERRATA_751472

/*
 * NS16550 Configuration
 */
#define V_NS16550_CLK		408000000	/* 408MHz (pllp_out0) */

/*
 * High Level Configuration Options
 */
#define CONFIG_TEGRA30			/* in a NVidia Tegra30 core */

/* Environment information, boards can override if required */
#define CONFIG_LOADADDR		0x80408000	/* def. location for kernel */

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LOAD_ADDR	0x80A00800	/* default */
#define CONFIG_STACKBASE	0x82800000	/* 40MB */

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_SYS_TEXT_BASE	0x8010E000

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
	"scriptaddr=0x90000000\0" \
	"kernel_addr_r=0x81000000\0" \
	"fdt_addr_r=0x82000000\0" \
	"ramdisk_addr_r=0x82100000\0"

/* Defines for SPL */
#define CONFIG_SPL_TEXT_BASE		0x80108000
#define CONFIG_SYS_SPL_MALLOC_START	0x80090000
#define CONFIG_SPL_STACK		0x800ffffc

#define CONFIG_SPL_LDSCRIPT		"$(CPUDIR)/tegra30/u-boot-spl.lds"

/* Total I2C ports on Tegra30 */
#define TEGRA_I2C_NUM_CONTROLLERS	5

#endif /* _TEGRA30_COMMON_H_ */
