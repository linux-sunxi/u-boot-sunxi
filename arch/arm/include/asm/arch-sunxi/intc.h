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
#ifndef _INTC_REGS_H_
#define _INTC_REGS_H_

#include <linux/types.h>
/*
*********************************************************************************************************
*   Interrupt controller define
*********************************************************************************************************
*/

struct sunxi_int_reg
{
	u32  vector;				/* 0x00: int vector 				*/
	u32  base_addr;				/* 0x04: int base address 			*/
	u32  reseved0;				/* unused */
	u32  control;				/* 0x0C: int control 				*/

	u32  irq_pending[3];		/* 0x10: int irq pending regisrers 	*/
	u32  irq_pending_reserved;	/* unused */

	u32  fiq_pending[3];		/* 0x20: int fiq pending regisrers 	*/
	u32  fiq_pending_reserved;	/* unused */

	u32  select[3];				/* 0x30: int type select regisrers 	*/
	u32  select_reserved;		/* unused */

	u32  enable[3];				/* 0x40: int enable regisrers 		*/
	u32  enable_reserved;		/* unused */

	u32  mask[3];				/* 0x50: int mask regisrers 		*/
	u32  mask_reserved;			/* unused */

	u32  response[3];			/* 0x60: int response regisrers 	*/
	u32  response_reserved;		/* unused */

	u32  fast_forcing[3];		/* 0x70: int fast forcing regisrers */
	u32  fast_forcing_reserved;	/* unused */

	u32  priority[8];			/* 0x80: int source priority regisrers */
};

/* int source */
#define  INTC_IRQNO_FIQ              0
#define  INTC_IRQNO_UART0            1
#define  INTC_IRQNO_UART1            2
#define  INTC_IRQNO_UART2            3
#define  INTC_IRQNO_UART3            4
#define  INTC_IRQNO_IR0              5
#define  INTC_IRQNO_IR1              6
#define  INTC_IRQNO_TWI0             7
#define  INTC_IRQNO_TWI1             8
#define  INTC_IRQNO_TWI2             9

#define  INTC_IRQNO_SPI0             10
#define  INTC_IRQNO_SPI1             11
#define  INTC_IRQNO_SPI2             12
#define  INTC_IRQNO_SPDIF            13
#define  INTC_IRQNO_AC97             14
#define  INTC_IRQNO_TS               15
#define  INTC_IRQNO_IIS              16

#define  INTC_IRQNO_UART4            17
#define  INTC_IRQNO_UART5            18
#define  INTC_IRQNO_UART6            19
#define  INTC_IRQNO_UART7            20

#define  INTC_IRQNO_KEYPAD           21
#define  INTC_IRQNO_TIMER0           22
#define  INTC_IRQNO_TIMER1           23
#define  INTC_IRQNO_TIMER245         24
#define  INTC_IRQNO_TIMER3           25
#define  INTC_IRQNO_CAN              26
#define  INTC_IRQNO_DMA              27
#define  INTC_IRQNO_PIO              28
#define  INTC_IRQNO_TP               29

#define  INTC_IRQNO_ADDA             30
#define  INTC_IRQNO_LRADC            31
#define  INTC_IRQNO_SDMMC0           32
#define  INTC_IRQNO_SDMMC1           33
#define  INTC_IRQNO_SDMMC2           34
#define  INTC_IRQNO_SDMMC3           35
#define  INTC_IRQNO_MS               36
#define  INTC_IRQNO_NAND             37
#define  INTC_IRQNO_USB0             38
#define  INTC_IRQNO_USB1             39
#define  INTC_IRQNO_USB2             40

#define  INTC_IRQNO_SCR              41

#define  INTC_IRQNO_CSI0             42
#define  INTC_IRQNO_CSI1             43
#define  INTC_IRQNO_LCD0             44
#define  INTC_IRQNO_LCD1             45
#define  INTC_IRQNO_MP               46
#define  INTC_IRQNO_DE_FE0           47
#define  INTC_IRQNO_DE_FE1           48
#define  INTC_IRQNO_PMU              49

#define  INTC_IRQNO_SPI3             50
#define  INTC_IRQNO_TZASC            51
#define  INTC_IRQNO_PATA             52
#define  INTC_IRQNO_VE               53

#define  INTC_IRQNO_SS               54
#define  INTC_IRQNO_EMAC             55
#define  INTC_IRQNO_SATA             56
#define  INTC_IRQNO_GPS              57
#define  INTC_IRQNO_HDMI             58
#define  INTC_IRQNO_TVE01            59

#define  INTC_IRQNO_ACE              60
#define  INTC_IRQNO_TVD              61
#define  INTC_IRQNO_PS0              62
#define  INTC_IRQNO_PS1              63
#define  INTC_IRQNO_USB3             64
#define  INTC_IRQNO_USB4             65
#define  INTC_IRQNO_PLE              66
#define  INTC_IRQNO_TIMER4           67
#define  INTC_IRQNO_TIMER5           68
#define  INTC_IRQNO_GPU_GP           69

#define  INTC_IRQNO_GPU_MP           70
#define  INTC_IRQNO_GPU_PP0          71
#define  INTC_IRQNO_GPU_PPMMU0       72
#define  INTC_IRQNO_GPU_PMU          73
#define  INTC_IRQNO_GPU_RSV0         74
#define  INTC_IRQNO_GPU_RSV1         75
#define  INTC_IRQNO_GPU_RSV2         76
#define  INTC_IRQNO_GPU_RSV3         77
#define  INTC_IRQNO_GPU_RSV4         78
#define  INTC_IRQNO_GPU_RSV5         79
#define  INTC_IRQNO_GPU_RSV6         80


extern  int irq_enable(int irq_no);
extern  int irq_disable(int irq_no);


#endif  /* _INTC_H_ */

