/*
 * (C) Copyright 2013
 * Carl van Schaik <carl@ok-labs.com>
 *
 * CPU configuration registers for the sun7i (A20).
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SUNXI_SMP_H_
#define _SUNXI_SMP_H_

#ifndef __ASSEMBLY__

void startup_secondaries(void);

/* Assembly entry point */
extern void secondary_init(void);

#endif /* __ASSEMBLY__ */

#endif /* _SUNXI_SMP_H_ */
