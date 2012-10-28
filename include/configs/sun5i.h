/*
 * (C) Copyright 2012
 * Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * Configuration settings for the Allwinner A13 (sun5i) CPU
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * Include common sunxi configuration where most the settings are
 */
#include <configs/sunxi-common.h>

/*
 * High Level Configuration Options
 */
#define CONFIG_SUN5I		/* sun5i SoC generation */

#define CONFIG_SYS_PROMPT		"sun5i#"
#define CONFIG_MACH_TYPE		4138

/* Define this to have serial channel 1 (UART0) redirected to SD port */
/* #define CONFIG_UART0_PORT_F */

#ifndef CONFIG_CONS_INDEX
#ifdef CONFIG_UART0_PORT_F
#define CONFIG_CONS_INDEX		1	/* UART0 on PORT_F (sdcard) */
#else
#define CONFIG_CONS_INDEX		2	/* UART1 */
#endif
#endif

/* Leave ICACHE off while debugging with OpenOCD */
#define CONFIG_SYS_ICACHE_OFF

#endif /* __CONFIG_H */
