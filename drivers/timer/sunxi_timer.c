/*
 * Copyright 2008 allwill technology, Inc.
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


#define TMRC_REGS_BASE			(0x1c20c00)


#define CFG_SW_TIMER_MAX       2
#define CFG_SW_TIMER_CTL0	   (TMRC_REGS_BASE + 0x0010)	      /* timer0 */
#define CFG_SW_TIMER_CTL1	   (TMRC_REGS_BASE + 0x0020)          /* timer1 */

#define CFG_SW_TIMER_INT_CTRL   (*(volatile unsigned int *)(TMRC_REGS_BASE + 0x0000))         /* timer int status */
#define CFG_SW_TIMER_INT_STATS  (*(volatile unsigned int *)(TMRC_REGS_BASE + 0x0004))         /* timer int status */
#define CFG_SW_WATCHDOG_CTRL    (*(volatile unsigned int *)(TMRC_REGS_BASE + 0x0030))         /* watchdog control */
#define CFG_SW_WATCHDOG_INTERVAL   (*(volatile unsigned int *)(TMRC_REGS_BASE + 0x0034))      /* watchdog interval */

#define CFG_SW_TIMER0_CTRL      (*(volatile unsigned int *)(CFG_SW_TIMER_CTL0 + 0x0000))
#define CFG_SW_TIMER0_COUNT     (*(volatile unsigned int *)(CFG_SW_TIMER_CTL0 + 0x0004))


static  void  timer_test_func(void *p)
{
	CFG_SW_TIMER_INT_STATS |=  1;
	CFG_SW_TIMER_INT_CTRL  &= ~1;
	printf("this is timer test\n");
	printf("delay time = %d\n", (__u32)p);
	return;
}

static int timer_test(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	__u32 reg_val;
	__s32 base_count;

	reg_val =  (0  << 0)  |            // 不启动TIMER
               (0  << 1)  |            // 使用单次模式
               (1  << 2)  |            // 使用高频晶振24M
               (5  << 4);              // 除频系统32，保证当设置时间是1的时候，触发延时1ms
    reg_val |= (0  << 0)  |            // 暂时没有start timer
               (1  << 1)  |              // 自动更新初始值用于计时
			   (1  << 7);
	CFG_SW_TIMER0_CTRL = reg_val;
	base_count = 1000;
	if(argc > 1)
	{
		base_count = simple_strtol(argv[1], NULL, 10);
	}
	CFG_SW_TIMER0_COUNT = base_count * (24000 / 32);
	CFG_SW_TIMER_INT_CTRL |= 1;
	CFG_SW_TIMER0_CTRL    |= 1;

	irq_install_handler(22, timer_test_func, (void *)base_count);
}

int timer_run (void *func, int delay_time, int auto_restart)
{

}

U_BOOT_CMD(
	timer_test, 5, 0, do_timer_test,
	"enable or disable interrupts",
	"[on, off]"
);


