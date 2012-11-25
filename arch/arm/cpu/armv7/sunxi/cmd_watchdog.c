/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
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

int do_sunxi_watchdog(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct sunxi_wdog *wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;
	long interval;

	if (argc < 2) {
		int i;
		u32 *wd = (void *)wdog;
		printf("usage: watchdog seconds\n");
		printf("0 to disable watchdog\n");
		for (i = 0; i < 4; i++)
			printf("%p: %08x\n", &wd[i],wd[i]);
		return 1;
	}
	interval = simple_strtoul(argv[1], NULL, 10);
	if (interval > (1 << 6) - 1)
		interval = (1 << 6) - 1;
	writel(0, &wdog->mode); /* make sure bit 0 is 0 to reset counter */
	CP15DMB;
	if (interval)
		writel(interval << 2 | 1 << 1 | 1 << 0, &wdog->mode);
	return 0;
}

U_BOOT_CMD(
	watchdog, 2, 1, do_sunxi_watchdog,
	"Set watchdog. 0 disables",
	""
);
