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
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>
#include <asm/arch/intc.h>

#define TIMER_MODE   (0 << 7)   /* continuous mode */
#define TIMER_DIV    (0 << 4)   /* pre scale 1 */
#define TIMER_SRC    (1 << 2)   /* osc24m */
#define TIMER_RELOAD (1 << 1)   /* reload internal value */
#define TIMER_EN     (1 << 0)   /* enable timer */

static  int  timer_used_status;

/* init timer register */
int timer_init(void)
{
	struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	struct sunxi_ccm_reg *ccm_reg = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	timer_reg->tirqen  = 0;
	timer_reg->tirqsta |= 0x043f;
	/* start avs as counter */
	ccm_reg->avs_clk_cfg |= (1 << 31);
	/* avs0 counted by ms */
	timer_reg->avs.ctl  = 3;		
	timer_reg->avs.div  = 0xc2ee0;
	timer_reg->avs.cnt0 = 0;
	
	return 0;
}

void timer_exit(void)
{
	struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	struct sunxi_ccm_reg *ccm_reg = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	timer_reg->tirqen  = 0;
	timer_reg->tirqsta |= 0x043f;	
	timer_reg->avs.ctl = 0;
	timer_reg->avs.div = 0x05DB05DB;
	timer_reg->timer[0].ctl = 0;
    timer_reg->timer[1].ctl = 0;
	ccm_reg->avs_clk_cfg &= ~(1U << 31);

	return ;
}

void watchdog_disable(void)
{
	struct sunxi_wdog *wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;
	/* disable watchdog */
	writel(0, &(wdog->mode));

	return ;	
}

void watchdog_enable(void)
{
	struct sunxi_wdog *wdog =
		&((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;
	/* enable watchdog */
	debug("write to %x value 1\n", (uint)&(wdog->mode));
	writel(3, &(wdog->mode));

	return ;	
	
}
/* timer without interrupts */
/* count the delay by seconds */
ulong get_timer(ulong base)
{
	return get_timer_masked()/1000 - base;
}

int runtime_tick(void)
{
	struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	return timer_reg->avs.cnt0;
}

ulong get_timer_masked(void)
{
	/* current tick value */
	ulong now = runtime_tick();
	/* notice:  this function doesnt consider if the timer count overrage */
#if 0
	if (now >= gd->lastinc)	/* normal (non rollover) */
		gd->tbl += (now - gd->lastinc);
	else			/* rollover */
		gd->tbl += (TICKS_TO_HZ(TIMER_LOAD_VAL) - gd->lastinc) + now;
	gd->lastinc = now;
	return gd->tbl;
#endif
	return now;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	u32 t1, t2;
	struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	timer_reg->avs.cnt1 = 0;

	t1 = timer_reg->avs.cnt1;
	t2 = t1 + usec;
	do
	{
		t1 = timer_reg->avs.cnt1;
	}
	while(t2 >= t1);

	return ;
}

/* delay x mseconds */
void __msdelay(unsigned long msec)
{
	u32 t1, t2;
	struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	t1 = timer_reg->avs.cnt0;
	t2 = t1 + msec;
	do
	{
		t1 = timer_reg->avs.cnt0;
	}
	while(t2 >= t1);

	return ;
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	ulong tbclk;
	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}

static void timerX_callback_default(void *data);

struct __timer_callback
{
	void (*func_back)( void *data);
	unsigned int  data;
};

struct __timer_callback timer_callback[2] = 
{
    	{timerX_callback_default, 0},
    	{timerX_callback_default, 1}
};

static void timerX_callback_default(void *data)
{
    printf("this is only for test, timer number=%d\n", (u32)data);

	return ;
}

void timer0_func(void *data)
{
	struct sunxi_timer_reg *timer_control = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	if(!(timer_control->tirqsta & 0x01))
    {
        return ;
    }
	timer_control->tirqen  &= ~0x01;
    timer_control->tirqsta  =  0x01;
    irq_disable(AW_IRQ_TIMER0);
	timer_used_status &= ~1;

	debug("timer 0 occur\n");

    timer_callback[0].func_back((void *)timer_callback[0].data); 

	return ;
}

void timer1_func(void *data)
{
	struct sunxi_timer_reg *timer_control = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	if(!(timer_control->tirqsta & 0x02))
    {
        return ;
    }
	timer_control->tirqen  &= ~0x02;
    timer_control->tirqsta  =  0x02;
    irq_disable(AW_IRQ_TIMER1);
	timer_used_status &= ~(1<<1);

	debug("timer 1 occur\n");

    timer_callback[1].func_back((void *)timer_callback[1].data); 

	return ;
}


void init_timer(struct timer_list *timer)
{
    return ;
}

void add_timer(struct timer_list *timer)
{
	u32 reg_val;	
	int timer_num;
	struct sunxi_timer     *timer_tcontrol;
	struct sunxi_timer_reg *timer_reg;
	
	if(timer->expires <= 0)
	{
		timer->expires = 1000;
	}
	if(!timer->expires)
	{
		return ;
	}
	if(!(timer_used_status & 0x01))
	{
		timer_used_status |= 0x01;
		timer_num = 0;
	}
	else if(!(timer_used_status & 0x02))
	{
		timer_used_status |= 0x02;
		timer_num = 1;
	}
	else
	{
		printf("timer err: there is no timer cound be used\n");

		return ;
	}
	debug("timer status = %x, number = %d\n", timer_used_status, timer_num);
	timer->timer_num = timer_num;
	timer_reg      =   (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	timer_tcontrol = &((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->timer[timer_num];	
	reg_val =   (0 << 0)  |            // 不启动TIMER              
				(1 << 1)  |            // 使用单次模式              
				(1 << 2)  |            // 使用高频晶振24M              
				(5 << 4)  |            // 除频系统32，保证当设置时间是1的时候，触发延时1ms    			  
				(1 << 7);	
	
	timer_tcontrol->ctl = reg_val;
	timer_tcontrol->inter = timer->expires * (24000 / 32);	
	timer_callback[timer_num].func_back = timer->function;
	timer_callback[timer_num].data      = timer->data;
	if(!timer_num)
	{
		irq_install_handler(AW_IRQ_TIMER0 + timer_num, timer0_func, (void *)&timer_callback[timer_num].data);
	}
	else
	{
		irq_install_handler(AW_IRQ_TIMER0 + timer_num, timer1_func, (void *)&timer_callback[timer_num].data);
	}
	debug("timer number = %d\n", timer_num);
	irq_enable(AW_IRQ_TIMER0 + timer_num);
	timer_tcontrol->ctl |= 1;
	timer_reg->tirqsta  = (1 << timer_num);
	timer_reg->tirqen  |= (1 << timer_num);
	debug("timer number = %d\n", timer_num);

	return ;	    
}


void del_timer(struct timer_list *timer)
{
	struct sunxi_timer *timer_tcontrol;
	struct sunxi_timer_reg *timer_reg;
	int    num = timer->timer_num;

	debug("timer status at delling begin = %x, number = %d\n", timer_used_status, num);
	
	timer_reg      =   (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	timer_tcontrol = &((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->timer[num];	

	irq_disable(AW_IRQ_TIMER0 + num);
	timer_tcontrol->ctl &= ~1;
	timer_reg->tirqsta = (1<<num);
	timer_reg->tirqen  &= ~(1<<num);

	timer_callback[num].data = num;
	timer_callback[num].func_back = timerX_callback_default;
	timer_used_status &= ~(1 << num);
	debug("timer status at delling end = %x\n", timer_used_status);

	return ;
}


