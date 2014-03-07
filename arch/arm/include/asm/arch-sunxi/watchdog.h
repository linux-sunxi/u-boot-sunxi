/*
 * Watchdog driver for the Allwinner sunxi platform.
 * Copyright (C) 2013  Oliver Schinagl <oliver@schinagl.nl>
 * http://www.linux-sunxi.org/
 *
 * SPDX-License-Identifier:	GPL-2.0+
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
