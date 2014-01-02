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
#include <asm/arch/timer.h>

DECLARE_GLOBAL_DATA_PTR;

#define TIMER_MODE   (0x0 << 7)	/* continuous mode */
#define TIMER_DIV    (0x0 << 4)	/* pre scale 1 */
#define TIMER_SRC    (0x1 << 2)	/* osc24m */
#define TIMER_RELOAD (0x1 << 1)	/* reload internal value */
#define TIMER_EN     (0x1 << 0)	/* enable timer */

#define TIMER_CLOCK		(24 * 1000 * 1000)
#define COUNT_TO_USEC(x)	((x) / 24)
#define USEC_TO_COUNT(x)	((x) * 24)
#define TICKS_PER_HZ		(TIMER_CLOCK / CONFIG_SYS_HZ)
#define TICKS_TO_HZ(x)		((x) / TICKS_PER_HZ)

#define TIMER_LOAD_VAL		0xffffffff

#define TIMER_NUM		0	/* we use timer 0 */

static struct sunxi_timer *timer_base =
	&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->timer[TIMER_NUM];

/* macro to read the 32 bit timer: since it decrements, we invert read value */
#define READ_TIMER() (~readl(&timer_base->val))

/* init timer register */
int timer_init(void)
{
	writel(TIMER_LOAD_VAL, &timer_base->inter);
	writel(TIMER_MODE | TIMER_DIV | TIMER_SRC | TIMER_RELOAD | TIMER_EN,
	       &timer_base->ctl);

	return 0;
}

/* timer without interrupts */
ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

ulong get_timer_masked(void)
{
	/* current tick value */
	ulong now = TICKS_TO_HZ(READ_TIMER());

	if (now >= gd->arch.lastinc)	/* normal (non rollover) */
		gd->arch.tbl += (now - gd->arch.lastinc);
	else {
		/* rollover */
		gd->arch.tbl += (TICKS_TO_HZ(TIMER_LOAD_VAL)
				- gd->arch.lastinc) + now;
	}
	gd->arch.lastinc = now;

	return gd->arch.tbl;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	long tmo = USEC_TO_COUNT(usec);
	ulong now, last = READ_TIMER();

	while (tmo > 0) {
		now = READ_TIMER();
		if (now > last)	/* normal (non rollover) */
			tmo -= now - last;
		else		/* rollover */
			tmo -= TIMER_LOAD_VAL - last + now;
		last = now;
	}
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	ulong tbclk;
	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}
