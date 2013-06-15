/*
 * Watchdog driver for the Allwinner sunxi platform.
 * Copyright (C) 2013  Oliver Schinagl <oliver@schinagl.nl>
 * http://www.linux-sunxi.org/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#ifndef _SUNXI_WATCHDOG_H_
#define _SUNXI_WATCHDOG_H_

/* Timeout limits */
#define WDT_MAX_TIMEOUT 16
#define WDT_OFF -1

#ifndef __ASSEMBLY__
void watchdog_reset(void);
void watchdog_set(int timeout);
void watchdog_init(void);
#endif /* __ASSEMBLY__ */

#endif
