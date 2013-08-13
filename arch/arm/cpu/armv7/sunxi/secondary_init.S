/*
 * A lowlevel_init function that sets up the stack to call a C function to
 * perform further init.
 *
 * (C) Copyright 2013
 * Carl van Schaik <carl@ok-labs.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm-offsets.h>
#include <config.h>
#include <linux/linkage.h>

ENTRY(secondary_init)
	/* Get cpu number : r5 */
	mrc	p15, 0, r5, c0, c0, 5
	and	r5, r5, #0xff

	/*
	 * Setup a secondary stack, each core gets 128 bytes.
	 */
	ldr	sp, =secondary_stack
	mov	r0, #0x80
	add	sp, sp, r0, lsl r5

	/*
	 * Jump to C
	 */
	bl	secondary_start
ENDPROC(secondary_init)

