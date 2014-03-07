/*
 * (C) Copyright 2013
 * Carl van Schaik <carl@ok-labs.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/smp.h>
#include <asm/arch/cpucfg.h>

/* Right now we assume only a single secondary as in sun7i */
#if defined(CONFIG_SUN7I)
#define NUM_CORES 2
#else
#error unsupported SoC
#endif

static void secondary_pen(void)
{
	struct sunxi_cpucfg *cpucfg = (struct sunxi_cpucfg *)SUNXI_CPUCFG_BASE;

	while (1) {
		__asm__ __volatile__("wfe" ::: "memory");

		unsigned long boot_addr = readl(&cpucfg->boot_addr);

		__asm__ __volatile__(
			"mov	r14, %0	\n"
			"bx	r14	\n"
			: : "r" (boot_addr)
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
	struct sunxi_cpucfg *cpucfg = (struct sunxi_cpucfg *)SUNXI_CPUCFG_BASE;

	writel((u32)secondary_init, &cpucfg->boot_addr);

	for (i = 1; i < NUM_CORES; i++) {
		/* Assert CPU reset just in case */
		writel(CPU_RESET_SET, &cpucfg->cpu[i].reset_ctrl);
		/* Ensure CPU reset also invalidates L1 caches */
		clrbits_le32(&cpucfg->general_ctrl,
				GENERAL_CTRL_NO_L1_RESET_CPU(i));
		/* Lock CPU */
		clrbits_le32(&cpucfg->debug1_ctrl, 1 << i);

		/* Ramp up power to CPU1 */
		assert(i == 1);
		u32 j = 0xff << 1;
		do {
			j = j >> 1;
			writel(j, &cpucfg->cpu1_power_clamp);
		} while (j != 0);

		udelay(10*1000); /* 10ms */

		clrbits_le32(&cpucfg->cpu1_power_off, 1);
		/* Release CPU reset */
		writel(CPU_RESET_CLEAR, &cpucfg->cpu[i].reset_ctrl);

		/* Unlock CPU */
		setbits_le32(&cpucfg->debug1_ctrl, 1 << i);

		printf("Secondary CPU%d power-on\n", i);
	}
}
