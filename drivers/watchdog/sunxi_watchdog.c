/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@hno.se>
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
#include <asm/arch/timer.h>
#include <asm/armv7.h>
#include <asm/io.h>

void watchdog_reset(void)
{
	static struct sunxi_wdog *const wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;

	/* a little magic to reload the watchdog */
	writel(0xA57 << 1 | 1 << 0, &wdog->ctl);
}

static void watchdog_set(int interval)
{
	static struct sunxi_wdog *const wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;

	/* Set timeout, reset & enable */
	writel(interval << 2 | 1 << 1 | 1 << 0, &wdog->mode);
	watchdog_reset();
}

void watchdog_init(void)
{
#ifdef CONFIG_WATCHDOG
	watchdog_set(23);	/* max possible timeout */
#endif
}
