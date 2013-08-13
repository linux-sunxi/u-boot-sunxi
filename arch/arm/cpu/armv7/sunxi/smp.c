/*
 * (C) Copyright 2013
 * Carl van Schaik <carl@ok-labs.com>
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
#include <asm/arch/cpucfg.h>

/* Right now we assume only a single secondary as in sun7i */
#if defined(CONFIG_SUN7I)
#define NUM_CORES 2
#else
#error unsupported SoC
#endif

/* Assembly entry point */
extern void secondary_init(void);

static void secondary_pen(void)
{
	volatile struct sunxi_cpucfg *cpucfg =
			(struct sunxi_cpucfg *)SUNXI_CPUCFG_BASE;

	while (1) {
		__asm__ __volatile__("wfe");

		__asm__ __volatile__(
			"mov	r14, %0	    \n"
			"bx	r14	    \n"
			: : "r" (cpucfg->boot_addr)
		);
	};
}

u32 secondary_stack[32*(NUM_CORES-1)];

void secondary_start(void)
{
	secondary_pen();
}

/* Power on secondaries */
void startup_secondaries(void)
{
	int i;
	volatile struct sunxi_cpucfg *cpucfg =
			(struct sunxi_cpucfg *)SUNXI_CPUCFG_BASE;

	cpucfg->boot_addr = (u32)secondary_init;

	for (i = 1; i < NUM_CORES; i++) {
		/* Assert CPU reset just in case */
		cpucfg->cpu[i].reset_ctrl = CPU_RESET_SET;
		/* Ensure CPU reset also invalidates L1 caches */
		cpucfg->general_ctrl &= ~GENERAL_CTRL_NO_L1_RESET_CPU(i);
		/* Lock CPU */
		cpucfg->debug1_ctrl &= ~(1 << i);

		/* Ramp up power to CPU1 */
		assert(i == 1);
		u32 j = 0xff;
		do {
			cpucfg->cpu1_power_clamp = j;
			j = j >> 1;
		} while (j != 0);

		udelay(10*1000); /* 10ms */

		cpucfg->cpu1_power_off &= ~1;
		/* Release CPU reset */
		cpucfg->cpu[i].reset_ctrl = CPU_RESET_CLEAR;

		/* Unlock CPU */
		cpucfg->debug1_ctrl |= (1 << i);

		printf("Secondary CPU%d power-on\n", i);
	}
}
