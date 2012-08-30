/*
 * Copyright 2008 Freescale Semiconductor, Inc.
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
#include <config.h>
#include <command.h>
#include <asm/arch/intc.h>
#include <asm/arch/timer.h>

static  void  timer_test_func(void *p)
{
	struct timer_list *timer_t;

	timer_t = (struct timer_list *)p;
	debug("timer number = %d\n", timer_t->timer_num);
	printf("this is timer test\n");

	del_timer(timer_t);
	return;
}

int do_timer_test (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int  base_count;
	struct timer_list timer_t;

	if(argc == 1)
	{
		base_count = 1000;
	}
	else if(argc == 2)
	{
		base_count = simple_strtol(argv[1], NULL, 10);
	}
	timer_t.data = (void *)&timer_t;
	timer_t.expires = base_count;
	timer_t.function = timer_test_func;
	
	init_timer(&timer_t);
	add_timer(&timer_t);
}

U_BOOT_CMD(
	timer_test, 2, 0, do_timer_test,
	"do a timer and int test",
	"[delay time]"
);


///* Implemented in $(CPU)/interrupts.c */
//int do_irqinfo (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
//
//U_BOOT_CMD(
//	irqinfo,    1,    1,     do_irqinfo,
//	"print information about IRQs",
//	""
//);
