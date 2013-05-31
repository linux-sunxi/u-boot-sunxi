/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
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

#include <asm/arch/gic.h>
#include <asm/arch/gpio.h>
#include <asm/arch/io.h>


pIRQ_Handler irq_vector[GIC_IRQ_NUM];

static void null_irq_hdle(void)
{
	//printk("No irq registered handler for this calling !!\n");
}

s32 irq_request(u32 irq_no, pIRQ_Handler hdle)
{
	if (irq_no < GIC_IRQ_NUM) {
		irq_vector[irq_no] = hdle;
		return irq_no;
	}
	//printk("Wrong irq NO.(%d) to request !!\n", irq_no);
	return -1;
}

s32 irq_free(u32 irq_no)
{
	if (irq_no < GIC_IRQ_NUM) {
		irq_vector[irq_no] = null_irq_hdle;
		return irq_no;
	}
	//printk("Wrong irq NO.(%d) to free !!\n", irq_no);
	return -1;
}

s32 irq_enable(u32 irq_no)
{
	u32 base;
	u32 base_os;
	u32 bit_os;

	if (irq_no >= GIC_IRQ_NUM) {
		//printk("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", irq_no, GIC_IRQ_NUM);
		return -1;
	}

	base_os = irq_no >> 5; // ³ý32
	base = GIC_SET_EN(base_os);
	bit_os = irq_no & 0x1f; // %32
	sr32_aw(base, bit_os, 1, 1);

	return 0;
}

s32 irq_disable(u32 irq_no)
{
	u32 base;
	u32 base_os;
	u32 bit_os;

	if (irq_no >= GIC_IRQ_NUM) {
		//printk("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", irq_no, GIC_IRQ_NUM);
		return -1;
	}

	base_os = irq_no >> 5; // ³ý32
	base = GIC_CLR_EN(base_os);
	bit_os = irq_no & 0x1f; // %32
	sr32_aw(base, bit_os, 1, 1);

	return 0;
}

void gic_sgi_handler(u32 id)
{
	//printk("SGI irq %d coming... \n", id);
}

void gic_ppi_handler(u32 id)
{
	//printk("SGI irq %d coming... \n", id);
}

void gic_spi_handler(u32 id)
{
	if (irq_vector[id] != null_irq_hdle) {
		irq_vector[id]();
	}
}

void gic_clear_pending(u32 id)
{
	u32 base;
	u32 base_os;
	u32 bit_os;

	base_os = id >> 5; // ³ý32
	base = GIC_PEND_CLR(base_os);
	bit_os = id & 0x1f; // %32
	writel(1<<bit_os, base);
}

void gic_irq_handler(void)
{
	u32 idnum;

	idnum = readl(GIC_INT_ACK_REG);
	if (idnum == 1023) {
		//printk("spurious irq !!\n");
		return;
	}
	if (idnum >= GIC_IRQ_NUM) {
		//printk("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", idnum, GIC_IRQ_NUM-32);
		return;
	}
	if (idnum < 16)
		gic_sgi_handler(idnum);
	else if (idnum < 32)
		gic_ppi_handler(idnum);
	else
		gic_spi_handler(idnum);

	writel(idnum, GIC_END_INT_REG);
	writel(idnum, GIC_DEACT_INT_REG);
	gic_clear_pending(idnum);
}

static void gic_distributor_init(void)
{
	u32 cpumask = 0x01010101;
	u32 gic_irqs;
	u32 i;

	writel(0, GIC_DIST_CON);

	/* check GIC hardware configutation */
	gic_irqs = ((readl(GIC_CON_TYPE) & 0x1f) + 1) * 32;
	if (gic_irqs > 1020)
		gic_irqs = 1020;
	else if (gic_irqs < GIC_IRQ_NUM) {
		/*printk("GIC parameter config error, only support %d"
				" irqs < %d(spec define)!!\n", gic_irqs, GIC_IRQ_NUM);*/
		return ;
	}

	/* set trigger type to be level-triggered, active low */
	for (i=GIC_SRC_SPI(0); i<GIC_IRQ_NUM; i+=16)
		writel(0, GIC_IRQ_MOD_CFG(i>>4));				//³ý16
	/* set priority */
	for (i=GIC_SRC_SPI(0); i<GIC_IRQ_NUM; i+=4)
		writel(0xa0a0a0a0, GIC_SPI_PRIO((i-32)>>2));	//³ý4
	/* set processor target */
	for (i=GIC_SRC_SPI(0); i<GIC_IRQ_NUM; i+=4)
		writel(cpumask, GIC_SPI_PROC_TARG((i-32)>>2));	//³ý4

	/* disable all interrupts */
	for (i=GIC_SRC_SPI(0); i<GIC_IRQ_NUM; i+=32)
		writel(0xffffffff, GIC_CLR_EN(i>>5));	//³ý32

	/* clear all interrupt active state */
	for (i=GIC_SRC_SPI(0); i<GIC_IRQ_NUM; i+=32)
		writel(0xffffffff, GIC_ACT_CLR(i>>5));	//³ý32


	writel(1, GIC_DIST_CON);
}

static void gic_cpuif_init(void)
{
	u32 i;

	writel(0, GIC_CPU_IF_CTRL);
	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 */
	writel(0xffff0000, GIC_CLR_EN0);
	writel(0x0000ffff, GIC_SET_EN0);

	/* Set priority on PPI and SGI interrupts */
	for (i=0; i<16; i+=4)
		writel(0xa0a0a0a0, GIC_SGI_PRIO(i>>2));
	for (i=16; i<32; i+=4)
		writel(0xa0a0a0a0, GIC_PPI_PRIO((i-16)>>2));

	writel(0xf0, GIC_INT_PRIO_MASK);
	writel(1, GIC_CPU_IF_CTRL);
}

void init_gic(void)
{
	u32 i;

	for (i=0; i<GIC_IRQ_NUM; i++)
		irq_vector[i] = null_irq_hdle;

	gic_distributor_init();
	gic_cpuif_init();
}
