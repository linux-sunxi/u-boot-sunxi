/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
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
