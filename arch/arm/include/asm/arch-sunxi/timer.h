/*
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


#ifndef _SUNXI_TIMER_H_
#define _SUNXI_TIMER_H_

#ifndef __ASSEMBLY__

/* General purpose timer */
struct sunxi_timer {
	u32 ctl;
	u32 inter;
	u32 val;
	u8  res[4];	
};

/* Audio video sync*/
struct sunxi_avs {
	u32 ctl;		/* 0x80 */
	u32 cnt0;		/* 0x84 */
	u32 cnt1;		/* 0x88 */
	u32 div;		/* 0x8c */
};

/* 64 bit counter */
struct sunxi_64cnt {
	u32 ctl;		/* 0xa0 */
	u32 lo;			/* 0xa4 */
	u32 hi;			/* 0xa8 */
};

/* Watchdog */
struct sunxi_wdog {
	u32 ctl;		/* 0x90 */
	u32 mode;		/* 0x94 */
};

/* Rtc */
struct sunxi_rtc {
	u32 ctl;		/* 0x100 */
	u32 yymmdd;		/* 0x104 */
	u32 hhmmss;		/* 0x108 */
};

/* Alarm */
struct sunxi_alarm {
	u32 ddhhmmss;	/* 0x10c */
	u32 hhmmss;		/* 0x110 */
	u32 en;			/* 0x114 */
	u32 irqen;		/* 0x118 */
	u32 irqsta;		/* 0x11c */
};

/* Timer general purpose register */
struct sunxi_tgp {
	u32 tgpd;
};

struct sunxi_timer_reg {
	u32 tirqen;		/* 0x00 */
	u32 tirqsta;	/* 0x04 */
	u8  res1[8];
	struct sunxi_timer timer[6];	/* We have 6 timers */
	u8  res2[16];
	struct sunxi_avs avs;
	struct sunxi_wdog wdog;
	u8  res3[8];
	struct sunxi_64cnt cnt64;
	u8  res4[0x5c];
	struct sunxi_rtc rtc;
	struct sunxi_alarm alarm;
	struct sunxi_tgp tgp[4];
};


#endif /* __ASSEMBLY__ */

#endif
