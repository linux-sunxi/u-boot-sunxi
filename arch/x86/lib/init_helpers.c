/*
 * (C) Copyright 2011
 * Graeme Russ, <graeme.russ@gmail.com>
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
#include <common.h>
#include <command.h>
#include <fdtdec.h>
#include <stdio_dev.h>
#include <version.h>
#include <malloc.h>
#include <net.h>
#include <ide.h>
#include <serial.h>
#include <spi.h>
#include <status_led.h>
#include <asm/processor.h>
#include <asm/sections.h>
#include <asm/u-boot-x86.h>
#include <linux/compiler.h>

#include <asm/init_helpers.h>

DECLARE_GLOBAL_DATA_PTR;

/************************************************************************
 * Init Utilities							*
 ************************************************************************
 * Some of this code should be moved into the core functions,
 * or dropped completely,
 * but let's get it working (again) first...
 */

int display_banner(void)
{
	printf("\n\n%s\n\n", version_string);

	return 0;
}

int display_dram_config(void)
{
	int i;

	puts("DRAM Configuration:\n");

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		printf("Bank #%d: %08lx ", i, gd->bd->bi_dram[i].start);
		print_size(gd->bd->bi_dram[i].size, "\n");
	}

	return 0;
}

int init_baudrate_f(void)
{
	gd->baudrate = getenv_ulong("baudrate", 10, CONFIG_BAUDRATE);
	return 0;
}

/* Get the top of usable RAM */
__weak ulong board_get_usable_ram_top(ulong total_size)
{
	return gd->ram_size;
}

int calculate_relocation_address(void)
{
	const ulong uboot_size = (uintptr_t)&__bss_end -
			(uintptr_t)&__text_start;
	ulong total_size;
	ulong dest_addr;
	ulong fdt_size = 0;

#if defined(CONFIG_OF_SEPARATE) && defined(CONFIG_OF_CONTROL)
	if (gd->fdt_blob)
		fdt_size = ALIGN(fdt_totalsize(gd->fdt_blob) + 0x1000, 32);
#endif
	total_size = ALIGN(uboot_size, 1 << 12) + CONFIG_SYS_MALLOC_LEN +
		CONFIG_SYS_STACK_SIZE + fdt_size;

	dest_addr = board_get_usable_ram_top(total_size);
	/*
	 * NOTE: All destination address are rounded down to 16-byte
	 *       boundary to satisfy various worst-case alignment
	 *       requirements
	 */
	dest_addr &= ~15;

#if defined(CONFIG_OF_SEPARATE) && defined(CONFIG_OF_CONTROL)
	/*
	 * If the device tree is sitting immediate above our image then we
	 * must relocate it. If it is embedded in the data section, then it
	 * will be relocated with other data.
	 */
	if (gd->fdt_blob) {
		dest_addr -= fdt_size;
		gd->new_fdt = (void *)dest_addr;
		dest_addr &= ~15;
	}
#endif
	/* U-Boot is below the FDT */
	dest_addr -= uboot_size;
	dest_addr &= ~((1 << 12) - 1);
	gd->relocaddr = dest_addr;
	gd->reloc_off = dest_addr - (uintptr_t)&__text_start;

	/* Stack is at the bottom, so it can grow down */
	gd->start_addr_sp = dest_addr - CONFIG_SYS_MALLOC_LEN;

	return 0;
}

int init_cache_f_r(void)
{
	/* Initialise the CPU cache(s) */
	return init_cache();
}

int set_reloc_flag_r(void)
{
	gd->flags = GD_FLG_RELOC;

	return 0;
}

int mem_malloc_init_r(void)
{
	mem_malloc_init(((gd->relocaddr - CONFIG_SYS_MALLOC_LEN)+3)&~3,
			CONFIG_SYS_MALLOC_LEN);

	return 0;
}

bd_t bd_data;

int init_bd_struct_r(void)
{
	gd->bd = &bd_data;
	memset(gd->bd, 0, sizeof(bd_t));

	return 0;
}

#ifndef CONFIG_SYS_NO_FLASH
int flash_init_r(void)
{
	ulong size;

	puts("Flash: ");

	/* configure available FLASH banks */
	size = flash_init();

	print_size(size, "\n");

	return 0;
}
#endif

#ifdef CONFIG_STATUS_LED
int status_led_set_r(void)
{
	status_led_set(STATUS_LED_BOOT, STATUS_LED_BLINKING);

	return 0;
}
#endif

int set_load_addr_r(void)
{
	/* Initialize from environment */
	load_addr = getenv_ulong("loadaddr", 16, load_addr);

	return 0;
}

int init_func_spi(void)
{
	puts("SPI:   ");
	spi_init();
	puts("ready\n");
	return 0;
}

#ifdef CONFIG_OF_CONTROL
int find_fdt(void)
{
#ifdef CONFIG_OF_EMBED
	/* Get a pointer to the FDT */
	gd->fdt_blob = _binary_dt_dtb_start;
#elif defined CONFIG_OF_SEPARATE
	/* FDT is at end of image */
	gd->fdt_blob = (ulong *)&_end;
#endif
	/* Allow the early environment to override the fdt address */
	gd->fdt_blob = (void *)getenv_ulong("fdtcontroladdr", 16,
						(uintptr_t)gd->fdt_blob);

	return 0;
}

int prepare_fdt(void)
{
	/* For now, put this check after the console is ready */
	if (fdtdec_prepare_fdt()) {
		panic("** CONFIG_OF_CONTROL defined but no FDT - please see "
			"doc/README.fdt-control");
	}

	return 0;
}
#endif
