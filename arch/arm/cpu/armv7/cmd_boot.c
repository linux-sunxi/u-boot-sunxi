/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Misc boot support
 */
#include <common.h>
#include <command.h>

#ifdef CONFIG_CMD_GO
unsigned long do_go_exec(ulong (*entry)(int, char * const []), int argc,
				 char * const argv[])
{
	invalidate_icache_all();
	return entry(argc, argv);
}
#endif
