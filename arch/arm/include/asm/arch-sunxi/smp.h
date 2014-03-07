/*
 * (C) Copyright 2013
 * Carl van Schaik <carl@ok-labs.com>
 *
 * CPU configuration registers for the sun7i (A20).
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_SMP_H_
#define _SUNXI_SMP_H_

#ifndef __ASSEMBLY__

void startup_secondaries(void);

/* Assembly entry point */
extern void secondary_init(void);

#endif /* __ASSEMBLY__ */

#endif /* _SUNXI_SMP_H_ */
