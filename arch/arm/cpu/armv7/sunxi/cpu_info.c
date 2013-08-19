/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
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
#include <asm/io.h>
#include <asm/arch/cpu.h>

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
#ifdef CONFIG_SUN4I
	puts("CPU:   Allwinner A10 (SUN4I)\n");
#elif defined CONFIG_SUN5I
	/* TODO: Distinguish A13/A10s */
	puts("CPU:   Allwinner A13/A10s (SUN5I)\n");
#elif defined CONFIG_SUN7I
	puts("CPU:   Allwinner A20 (SUN7I)\n");
#else
#warning Please update cpu_info.c with correct CPU information
	puts("CPU:   SUNXI Family\n");
#endif
	return 0;
}
#endif
