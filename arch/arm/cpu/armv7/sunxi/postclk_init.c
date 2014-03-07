/*
 * (C) Copyright 2013
 * Carl van Schaik <carl@ok-labs.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#if defined(CONFIG_SYS_SECONDARY_ON)
#include <asm/arch/smp.h>
#endif


int board_postclk_init(void)
{
#if defined(CONFIG_SYS_SECONDARY_ON)
	startup_secondaries();
#endif
	return 0;
}
