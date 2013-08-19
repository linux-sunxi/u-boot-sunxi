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
#include <asm/io.h>
#include <asm/arch/watchdog.h>

int do_sunxi_watchdog(cmd_tbl_t *cmdtp, int flag, int argc, const char *argv[])
{
	unsigned long interval;

	if (argc < 2) {
		printf("usage: watchdog seconds\n");
		printf("over %d to disable watchdog\n", WDT_MAX_TIMEOUT);
	}
	interval = simple_strtoul(argv[1], NULL, 10);
	watchdog_set((unsigned int)interval);

	return 0;
}

U_BOOT_CMD(
	watchdog, 2, 1, do_sunxi_watchdog,
	"Set watchdog [0 - 16]. [17+} disables",
	""
);
