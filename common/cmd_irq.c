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

struct timer_list timer0_t;
struct timer_list timer1_t;
static int timer_test_flag[2];

static  void  timer0_test_func(void *p)
{
	struct timer_list *timer_t;

	timer_t = (struct timer_list *)p;
	debug("timer number = %d\n", timer_t->timer_num);
	printf("this is timer test\n");

	del_timer(timer_t);
	timer_test_flag[0] = 0;

	return;
}

int do_timer_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int  base_count;

	if(timer_test_flag[0])
	{
		printf("can not test timer 0 now\n");

		return -1;
	}
	if(argc == 1)
	{
		base_count = 1000;
	}
	else if(argc == 2)
	{
		base_count = simple_strtol(argv[1], NULL, 10);
	}
	timer0_t.data = (void *)&timer0_t;
	timer0_t.expires = base_count;
	timer0_t.function = timer0_test_func;

	init_timer(&timer0_t);
	add_timer(&timer0_t);
	timer_test_flag[0] = 1;

	return 0;
}


U_BOOT_CMD(
	timer_test, 2, 0, do_timer_test,
	"do a timer and int test",
	"[delay time]"
);

static  void  timer1_test_func(void *p)
{
	struct timer_list *timer_t;

	timer_t = (struct timer_list *)p;
	debug("timer number = %d\n", timer_t->timer_num);
	printf("this is timer test\n");

	del_timer(timer_t);
	timer_test_flag[1] = 0;

	return;
}


int do_timer_test1(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int  base_count;

	if(timer_test_flag[1])
	{
		printf("can not test timer 1 now\n");

		return -1;
	}

	if(argc == 1)
	{
		base_count = 1000;
	}
	else if(argc == 2)
	{
		base_count = simple_strtol(argv[1], NULL, 10);
	}
	timer1_t.data = (void *)&timer1_t;
	timer1_t.expires = base_count;
	timer1_t.function = timer1_test_func;

	init_timer(&timer1_t);
	add_timer(&timer1_t);

	timer_test_flag[1] = 1;
}


U_BOOT_CMD(
	timer_test1, 2, 0, do_timer_test1,
	"do a timer and int test",
	"[delay time]"
);



