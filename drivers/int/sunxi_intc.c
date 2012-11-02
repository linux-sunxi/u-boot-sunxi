/*
 * (C) Copyright 2012
 *     wangflord@allwinnerstech.com
 *
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
 * along with this program;
 *
 */
#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/intc.h>

struct sunxi_int_reg  *sunxi_int_regs;

struct _irq_handler
{
	void                *m_data;
	void (*m_func)( void * data);
};

struct _irq_handler sunxi_int_handlers[INT_MAX_SOURCE];

static void default_isr(void *data)
{
	printf("default_isr():  called from IRQ %d\n", (u32)data);
}

void timer0_isr(void * data)
{
	printf("call time 0 isr\n");
	return ;
}

void timer1_isr(void * data)
{
	printf("call time 1 isr\n");
	return ;
}

int irq_enable(int irq_no)
{
    if(irq_no < 32)
    {
        sunxi_int_regs->enable[0]  |=  (1 << irq_no);
        sunxi_int_regs->mask[0]    &= ~(1 << irq_no);
    }
    else if(irq_no < 64)
    {
    	irq_no                    -= 32;
        sunxi_int_regs->enable[1] |=  (1 << irq_no);
        sunxi_int_regs->mask[1]   &= ~(1 << irq_no);
    }
    else
    {
        irq_no                    -= 64;
        sunxi_int_regs->enable[2] |=  (1 << irq_no);
        sunxi_int_regs->mask[2]   &= ~(1 << irq_no);
    }

    return 0;
}

int irq_disable(int irq_no)
{
    if(irq_no < 32)
    {
        sunxi_int_regs->enable[0]  &= ~(1 << irq_no);
        sunxi_int_regs->mask[0]    |= (1 << irq_no);
    }
    else if(irq_no < 64)
    {
    	irq_no                    -= 32;
        sunxi_int_regs->enable[1] &=  ~(1 << irq_no);
        sunxi_int_regs->mask[1]   |=  (1 << irq_no);
    }
    else
    {
        irq_no                    -= 64;
        sunxi_int_regs->enable[2] &=  ~(1 << irq_no);
        sunxi_int_regs->mask[2]   |=  (1 << irq_no);
    }

    return 0;
}


void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data)
{
	disable_interrupts();
	if (irq >= INT_MAX_SOURCE || !handle_irq)
	{
		enable_interrupts();
		return;
	}

	sunxi_int_handlers[irq].m_data = data;
	sunxi_int_handlers[irq].m_func = handle_irq;

	enable_interrupts();
}

void irq_free_handler(int irq)
{
	disable_interrupts();
	if (irq >= INT_MAX_SOURCE)
	{
		enable_interrupts();
		return;
	}

	sunxi_int_handlers[irq].m_data = (void *)0;
	sunxi_int_handlers[irq].m_func = default_isr;

	enable_interrupts();
}


#ifdef CONFIG_USE_IRQ
void do_irq (struct pt_regs *pt_regs)
{
	u32 base;

	disable_interrupts();
	base = sunxi_int_regs->vector>>2;
	irq_disable(base);
	sunxi_int_handlers[base].m_func(sunxi_int_handlers[base].m_data);
	enable_interrupts();

	return;
}
#endif

int arch_interrupt_init (void)
{
	sunxi_int_regs = (struct sunxi_int_reg *)SUNXI_INTC_BASE;
	//关闭所有中断使能
	sunxi_int_regs->enable[0] = 0;
	sunxi_int_regs->enable[1] = 0;
	sunxi_int_regs->enable[2] = 0;

	sunxi_int_regs->mask[0] = 0;
	sunxi_int_regs->mask[1] = 0;
	sunxi_int_regs->mask[2] = 0;

	sunxi_int_regs->irq_pending[0] = 0xffffffff;
	sunxi_int_regs->irq_pending[1] = 0xffffffff;
	sunxi_int_regs->irq_pending[2] = 0xffffffff;

	sunxi_int_regs->fiq_pending[0] = 0xffffffff;
	sunxi_int_regs->fiq_pending[1] = 0xffffffff;
	sunxi_int_regs->fiq_pending[2] = 0xffffffff;

	return (0);
}

int arch_interrupt_exit(void)
{
	sunxi_int_regs = (struct sunxi_int_reg *)SUNXI_INTC_BASE;
	//关闭所有中断使能
	sunxi_int_regs->enable[0] = 0;
	sunxi_int_regs->enable[1] = 0;
	sunxi_int_regs->enable[2] = 0;

	sunxi_int_regs->mask[0] = 0;
	sunxi_int_regs->mask[1] = 0;
	sunxi_int_regs->mask[2] = 0;

	sunxi_int_regs->irq_pending[0] = 0xffffffff;
	sunxi_int_regs->irq_pending[1] = 0xffffffff;
	sunxi_int_regs->irq_pending[2] = 0xffffffff;

	sunxi_int_regs->fiq_pending[0] = 0xffffffff;
	sunxi_int_regs->fiq_pending[1] = 0xffffffff;
	sunxi_int_regs->fiq_pending[2] = 0xffffffff;    
}
