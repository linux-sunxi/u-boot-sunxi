/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
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

#include <common.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

/* TODO add timer init code here */

int timer_init(void)
{
	return 0;
}

/* timer without interrupts */
ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

ulong get_timer_masked(void)
{
	gd->tbl += 0x100;
	return gd->tbl;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	/* TODO add more presice time code here*/
	int i;
	for(i = 10 ; i > 0; i--)
		;
}
