/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : int_service_entry.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.04.06
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.04.06       1.1.0        build the file
*
************************************************************************************************************************
*/
#include <common.h>
#include "intc_reg.h"

#define  N_IRQS    (84)

struct _irq_handler
{
	void                *m_data;
	void (*m_func)( void *data);
};

static struct _irq_handler IRQ_HANDLER[N_IRQS];

static void default_isr(void *data)
{
	printf("default_isr():  called for IRQ %d\n", (int)data);
}

void timer0_isr( void *data)
{
	printf("call time 0 isr\n");
	return ;
}

void timer1_isr( void *data)
{
	printf("call time 1 isr\n");
	return ;
}

__s32 irq_enableInt(int irq_no)
{
    if(irq_no < 32)
    {
        INTC_REG_ENABLE0    |=  (1 << irq_no);
        INTC_REG_MASK0 	    &= ~(1 << irq_no);
    }
    else if(irq_no < 64)
    {
    	irq_no              -= 32;
        INTC_REG_ENABLE1    |=  (1 << irq_no);
        INTC_REG_MASK1 	    &= ~(1 << irq_no);
    }
    else
    {
        irq_no               -= 64;
        INTC_REG_ENABLE2    |=  (1 << irq_no);
        INTC_REG_MASK2 	    &= ~(1 << irq_no);
    }

    return 0;
}

__s32 irq_disableInt(int irq_no)
{
    if(irq_no < 32)
    {
        INTC_REG_ENABLE0    &= ~(1 << irq_no);
        INTC_REG_MASK0 	    |=  (1 << irq_no);
    }
    else if(irq_no < 64)
    {
    	irq_no -= 32;
        INTC_REG_ENABLE1    &= ~(1 << irq_no);
        INTC_REG_MASK1 	    |=  (1 << irq_no);
    }
    else
    {
        irq_no -= 64;
        INTC_REG_ENABLE2    &= ~(1 << irq_no);
        INTC_REG_MASK2 	    |=  (1 << irq_no);
    }

    return 0;
}

void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data)
{
	disable_interrupts();
	if (irq >= N_IRQS || !handle_irq)
	{
		enable_interrupts();
		return;
	}

	IRQ_HANDLER[irq].m_data = data;
	IRQ_HANDLER[irq].m_func = handle_irq;

	irq_enableInt(irq);

	enable_interrupts();
}

#ifdef CONFIG_USE_IRQ
void do_irq (struct pt_regs *pt_regs)
{
	__u32 base;

//	printf ("interrupt request\n");
//	show_regs (pt_regs);

	disable_interrupts();
	base = INTC_REG_VCTR>>2;
	irq_disableInt(base);
	IRQ_HANDLER[base].m_func(IRQ_HANDLER[base].m_data);
	enable_interrupts();

	return;
}
#endif

int arch_interrupt_init (void)
{
	int i;

	/* install default interrupt handlers */
	for (i = 0; i < N_IRQS; i++)
	{
		IRQ_HANDLER[i].m_data = (void *)i;
		IRQ_HANDLER[i].m_func = default_isr;
	}
	IRQ_HANDLER[INTC_IRQNO_TIMER0].m_data = (void *)0;
	IRQ_HANDLER[INTC_IRQNO_TIMER0].m_func = timer0_isr;

	IRQ_HANDLER[INTC_IRQNO_TIMER1].m_data = (void *)0;
	IRQ_HANDLER[INTC_IRQNO_TIMER1].m_func = timer1_isr;

	//关闭所有中断使能
	INTC_REG_ENABLE0 = 0;
	INTC_REG_ENABLE1 = 0;
	INTC_REG_ENABLE2 = 0;
	//打开所有中断mask
	INTC_REG_MASK0 = 0;
	INTC_REG_MASK1 = 0;
	INTC_REG_MASK2 = 0;
	//清除所有中断pengding
	INTC_REG_FIQ_PENDCLR0 = 0xffffffff;
	INTC_REG_FIQ_PENDCLR1 = 0xffffffff;
	INTC_REG_FIQ_PENDCLR2 = 0xffffffff;

	INTC_REG_IRQ_PENDCLR0 = 0xffffffff;
	INTC_REG_IRQ_PENDCLR1 = 0xffffffff;
	INTC_REG_IRQ_PENDCLR2 = 0xffffffff;

	return (0);
}
