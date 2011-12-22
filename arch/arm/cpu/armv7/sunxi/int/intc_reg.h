/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eBIOS Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : ebios
* File    : intc.h
*
* By      : Jerry
* Version : v2.0
* Date    : 2009-9-11 14:40:43
**********************************************************************************************************************
*/
#ifndef _INTC_REGS_H_
#define _INTC_REGS_H_

#include <common.h>
/*
*********************************************************************************************************
*   Interrupt controller define
*********************************************************************************************************
*/
#define  INTC_REG_BASE_ADDR			 (0x1c20400)
/* Offset */
;/* Offset */
#define  INTC_REG_o_VCTR             0x00
#define  INTC_REG_o_VTBLBADDR        0x04

#define  INTC_REG_o_NMI_CTRL         0x0C

#define  INTC_REG_o_IRQ_PENDCLR0     0x10
#define  INTC_REG_o_IRQ_PENDCLR1     0x14
#define  INTC_REG_o_IRQ_PENDCLR2     0x18

#define  INTC_REG_o_FIQ_PENDCLR0     0x20
#define  INTC_REG_o_FIQ_PENDCLR1     0x24
#define  INTC_REG_o_FIQ_PENDCLR2     0x28

#define  INTC_REG_o_IRQ_SEL0         0x30
#define  INTC_REG_o_IRQ_SEL1         0x34
#define  INTC_REG_o_IRQ_SEL2         0x38

#define  INTC_REG_o_ENABLE0          0x40
#define  INTC_REG_o_ENABLE1          0x44
#define  INTC_REG_o_ENABLE2          0x48

#define  INTC_REG_o_MASK0            0x50
#define  INTC_REG_o_MASK1            0x54
#define  INTC_REG_o_MASK2            0x58

#define  INTC_REG_o_RESP0            0x60
#define  INTC_REG_o_RSEP1            0x64
#define  INTC_REG_o_RESP2            0x68

#define  INTC_REG_o_FF0              0x70
#define  INTC_REG_o_FF1              0x74
#define  INTC_REG_o_FF2              0x78

#define  INTC_REG_o_PRIO0            0x80
#define  INTC_REG_o_PRIO1            0x84
#define  INTC_REG_o_PRIO2            0x88
#define  INTC_REG_o_PRIO3            0x8C
#define  INTC_REG_o_PRIO4            0x90

/* registers */
#define  INTC_REG_VCTR				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_VCTR           )
#define  INTC_REG_VTBLBADDR			 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_VTBLBADDR      )

#define  INTC_REG_NMI_CTRL			 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_NMI_CTRL       )

#define  INTC_REG_IRQ_PENDCLR0		 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_IRQ_PENDCLR0   )
#define  INTC_REG_IRQ_PENDCLR1		 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_IRQ_PENDCLR1   )
#define  INTC_REG_IRQ_PENDCLR2		 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_IRQ_PENDCLR2   )

#define  INTC_REG_FIQ_PENDCLR0		 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_FIQ_PENDCLR0   )
#define  INTC_REG_FIQ_PENDCLR1		 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_FIQ_PENDCLR1   )
#define  INTC_REG_FIQ_PENDCLR2		 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_FIQ_PENDCLR2   )

#define  INTC_REG_IRQ_SEL0			 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_IRQ_SEL0       )
#define  INTC_REG_IRQ_SEL1			 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_IRQ_SEL1       )
#define  INTC_REG_IRQ_SEL2			 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_IRQ_SEL2       )

#define  INTC_REG_ENABLE0			 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_ENABLE0        )
#define  INTC_REG_ENABLE1			 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_ENABLE1        )
#define  INTC_REG_ENABLE2			 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_ENABLE2        )

#define  INTC_REG_MASK0				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_MASK0          )
#define  INTC_REG_MASK1				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_MASK1          )
#define  INTC_REG_MASK2				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_MASK2          )

#define  INTC_REG_RESP0				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_RESP0          )
#define  INTC_REG_RSEP1				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_RSEP1          )
#define  INTC_REG_RESP2				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_RESP2          )

#define  INTC_REG_FF0				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_FF0            )
#define  INTC_REG_FF1				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_FF1            )
#define  INTC_REG_FF2				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_FF2            )

#define  INTC_REG_PRIO0				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_PRIO0          )
#define  INTC_REG_PRIO1				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_PRIO1          )
#define  INTC_REG_PRIO2				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_PRIO2          )
#define  INTC_REG_PRIO3				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_PRIO3          )
#define  INTC_REG_PRIO4				 __REG(  INTC_REG_BASE_ADDR + INTC_REG_o_PRIO4          )


/* mask */
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


#endif  /* _INTC_H_ */

