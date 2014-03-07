/*
 * A lowlevel_init function that sets up the stack to call a C function to
 * perform further init.
 *
 * (C) Copyright 2013
 * Carl van Schaik <carl@ok-labs.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
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

