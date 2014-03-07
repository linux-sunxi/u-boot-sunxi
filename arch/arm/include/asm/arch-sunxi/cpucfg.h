/*
 * (C) Copyright 2013
 * Carl van Schaik <carl@ok-labs.com>
 *
 * CPU configuration registers for the sun7i (A20).
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_CPUCFG_H_
#define _SUNXI_CPUCFG_H_

#ifndef __ASSEMBLY__

struct sunxi_cpu_ctrl {
	u32 reset_ctrl;
	u32 cpu_ctrl;
	u32 status;
	u32 _res[13];
};

#define CPU_RESET_SET	0
#define CPU_RESET_CLEAR	3

#define CPU_STATUS_SMP	(1 << 0)
#define CPU_STATUS_WFE	(1 << 1)
#define CPU_STATUS_WFI	(1 << 2)

struct sunxi_cpucfg {
	u32 _res1[16];			/* 0x000 */
	struct sunxi_cpu_ctrl cpu[2];	/* 0x040 */
	u32 _res2[48];			/* 0x0c0 */
	u32 _res3;			/* 0x180 */
	u32 general_ctrl;		/* 0x184 */
	u32 _res4[2];			/* 0x188 */
	u32 event_input;		/* 0x190 */
	u32 _res5[4];			/* 0x194 */
	u32 boot_addr;			/* 0x1a4 - also known as PRIVATE_REG */
	u32 _res6[2];			/* 0x1a8 */
	u32 cpu1_power_clamp;		/* 0x1b0 */
	u32 cpu1_power_off;		/* 0x1b4 */
	u32 _res7[10];			/* 0x1b8 */
	u32 debug0_ctrl;		/* 0x1e0 */
	u32 debug1_ctrl;		/* 0x1e4 */
};

#define GENERAL_CTRL_NO_L1_RESET_CPU(x)	(1UL << (x))
#define GENERAL_CTRL_NO_L2_AUTO_RESET	(1UL << 4)
#define GENERAL_CTRL_L2_RESET_SET	(0UL << 5)
#define GENERAL_CTRL_L2_RESET_CLEAR	(1UL << 5)
#define GENERAL_CTRL_CFGSDISABLE	(1UL << 8)

#endif /* __ASSEMBLY__ */

#endif /* _SUNXI_CPUCFG_H_ */
