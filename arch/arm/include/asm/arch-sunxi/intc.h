/**
 * gic.h - definition for registers of generic interrupt controller
 * date:    2012/2/12 22:34:41
 * author:  Aaron<leafy.myeh@allwinnertech.com>
 * history: V0.1
 *          2012-3-21 9:13:43 remapping fpga irq: gpu, rtc domain etc.
 */
#ifndef __SUNXI_INTC_H__
#define __SUNXI_INTC_H__

#define AW_IRQ_GIC_START    (32)

#ifndef CONFIG_SUN6I_FPGA	//chip irq mapping

#define AW_IRQ_UART0		(AW_IRQ_GIC_START + 0)		/*	UART0		*/
#define AW_IRQ_UART1		(AW_IRQ_GIC_START + 1)		/*	UART1		*/
#define AW_IRQ_UART2		(AW_IRQ_GIC_START + 2)		/*	UART2		*/
#define AW_IRQ_UART3		(AW_IRQ_GIC_START + 3)		/*	UART3		*/
#define AW_IRQ_UART4		(AW_IRQ_GIC_START + 4)		/*	UART3		*/
#define AW_IRQ_UART5		(AW_IRQ_GIC_START + 5)		/*	UART3		*/
#define AW_IRQ_TWI0			(AW_IRQ_GIC_START + 6)		/*	TWI0		*/
#define AW_IRQ_TWI1			(AW_IRQ_GIC_START + 7)		/*	TWI1		*/
#define AW_IRQ_TWI2			(AW_IRQ_GIC_START + 8)		/*	TWI0		*/
#define AW_IRQ_TWI3			(AW_IRQ_GIC_START + 9)		/*	TWI1		*/

#define AW_IRQ_EINTA		(AW_IRQ_GIC_START + 11)		/*	EINTA		*/
#define AW_IRQ_SPDIF		(AW_IRQ_GIC_START + 12)		/*	SPDIF		*/
#define AW_IRQ_DAUDIO0		(AW_IRQ_GIC_START + 13)		/*	DAUDIO0		*/
#define AW_IRQ_DAUDIO1      (AW_IRQ_GIC_START + 14)     /*  DAUDIO1     */
#define AW_IRQ_EINTB        (AW_IRQ_GIC_START + 15)     /*  EINTB       */
#define AW_IRQ_EINTE        (AW_IRQ_GIC_START + 16)     /*  EINTE       */
#define AW_IRQ_EINTG        (AW_IRQ_GIC_START + 17)     /*  EINTG       */
#define AW_IRQ_TIMER0		(AW_IRQ_GIC_START + 18)		/*	Timer0		*/
#define AW_IRQ_TIMER1		(AW_IRQ_GIC_START + 19)		/*	Timer1		*/
#define AW_IRQ_TIMER2		(AW_IRQ_GIC_START + 20)		/*	Timer2		*/
#define AW_IRQ_TIMER3       (AW_IRQ_GIC_START + 21)     /*  Timer3      */
#define AW_IRQ_TIMER4       (AW_IRQ_GIC_START + 22)     /*  Timer4      */
#define AW_IRQ_TIMER5       (AW_IRQ_GIC_START + 23)     /*  Timer5      */
#define	AW_IRQ_WATCHDOG4	(AW_IRQ_GIC_START + 24)		/*	WATCHDOG4	*/
#define	AW_IRQ_WATCHDOG1	(AW_IRQ_GIC_START + 25)		/*	WATCHDOG1	*/
#define	AW_IRQ_WATCHDOG2	(AW_IRQ_GIC_START + 26)		/*	WATCHDOG2	*/
#define	AW_IRQ_WATCHDOG3	(AW_IRQ_GIC_START + 27)		/*	WATCHDOG3	*/
#define	AW_IRQ_TOUCHPANEL	(AW_IRQ_GIC_START + 28)		/*	TOUCH PANEL	*/
#define AW_IRQ_CODEC		(AW_IRQ_GIC_START +	29)		/*	AUDIO CEDEC	*/
#define AW_IRQ_LRADC		(AW_IRQ_GIC_START + 30)		/*	LRADC		*/
#define AW_IRQ_MTCACC		(AW_IRQ_GIC_START + 31)		/*	MTCACC		*/
#define AW_IRQ_NMI			(AW_IRQ_GIC_START + 32)		/*	NMI			*/
#define AW_IRQ_RTIMER0		(AW_IRQ_GIC_START + 33)		/*	R_TIMER 0	*/
#define AW_IRQ_RTIMER1		(AW_IRQ_GIC_START + 34)		/*	R_TIMER 1	*/

#define AW_IRQ_RWATCHDOG	(AW_IRQ_GIC_START + 36)     /*	R_WATCHDO	*/
#define AW_IRQ_RCIR			(AW_IRQ_GIC_START + 37)     /*  R_CIR		*/
#define	AW_IRQ_RUART		(AW_IRQ_GIC_START + 38)		/*	R_UART		*/
#define AW_IRQ_RP2TWI		(AW_IRQ_GIC_START + 39)		/*	R_P2TWI		*/
#define	AW_IRQ_RALARM0		(AW_IRQ_GIC_START + 40)		/*	R_RLARM 0	*/
#define	AW_IRQ_RALARM1		(AW_IRQ_GIC_START + 41)		/*	R_RLARM 1	*/

#define AW_IRQ_R_1WIRE		(AW_IRQ_GIC_START + 43)		/*	R_ONE_WIRE	*/
#define	AW_IRQ_RTWI			(AW_IRQ_GIC_START + 44)		/*	R_TWI		*/
#define AW_IRQ_EINTL		(AW_IRQ_GIC_START + 45)		/*	R_EINTL		*/
#define AW_IRQ_EINTM		(AW_IRQ_GIC_START + 46)		/*	R_EINTM		*/

#define	AW_IRQ_SPINLOCK		(AW_IRQ_GIC_START + 48)		/*	SPINLOCK	*/
#define	AW_IRQ_MBOX			(AW_IRQ_GIC_START + 49)		/*	M-BOX		*/
#define	AW_IRQ_DMA			(AW_IRQ_GIC_START + 50)		/*	DMA			*/
#define AW_IRQ_HSTIMER0		(AW_IRQ_GIC_START + 51)		/*	HSTIMER0	*/
#define AW_IRQ_HSTIMER1		(AW_IRQ_GIC_START + 52)		/*	HSTIMER1	*/
#define AW_IRQ_HSTIMER2		(AW_IRQ_GIC_START + 53)		/*	HSTIMER2	*/
#define AW_IRQ_HSTIMER3		(AW_IRQ_GIC_START + 54)		/*	HSTIMER3	*/

#define	AW_IRQ_TZASC		(AW_IRQ_GIC_START + 56)		/*	TZASC		*/

#define	AW_IRQ_VE			(AW_IRQ_GIC_START + 58)		/*	VE			*/
#define	AW_IRQ_DIGMIC		(AW_IRQ_GIC_START + 59)		/*	DIG_MIC		*/
#define AW_IRQ_MMC0			(AW_IRQ_GIC_START + 60)		/*	MMC0		*/
#define AW_IRQ_MMC1			(AW_IRQ_GIC_START + 61)		/*	MMC1		*/
#define AW_IRQ_MMC2			(AW_IRQ_GIC_START + 62)		/*	MMC2		*/
#define AW_IRQ_MMC3			(AW_IRQ_GIC_START + 63)		/*	MMC3		*/

#define AW_IRQ_SPI0			(AW_IRQ_GIC_START + 65)		/*	SPI0		*/
#define AW_IRQ_SPI1			(AW_IRQ_GIC_START + 66)		/*	SPI1		*/
#define AW_IRQ_SPI2			(AW_IRQ_GIC_START + 67)		/*	SPI2		*/
#define AW_IRQ_SPI3			(AW_IRQ_GIC_START + 68)		/*	SPI3		*/
#define AW_IRQ_NAND1		(AW_IRQ_GIC_START + 69)		/*	NAND1		*/
#define AW_IRQ_NAND0		(AW_IRQ_GIC_START + 70)		/*	NAND0		*/

#define AW_IRQ_USB_OTG		(AW_IRQ_GIC_START + 71)		/*	USB_OTG		*/
#define AW_IRQ_USB_EHCI0	(AW_IRQ_GIC_START + 72)		/*	USB_EHCI0	*/
#define AW_IRQ_USB_OHCI0	(AW_IRQ_GIC_START + 73)		/*	USB_OHCI0	*/
#define AW_IRQ_USB_EHCI1	(AW_IRQ_GIC_START + 74)		/*	USB_EHCI1	*/
#define AW_IRQ_USB_OHCI1	(AW_IRQ_GIC_START + 75)		/*	USB_OHCI1	*/

#define AW_IRQ_USB_OHCI2	(AW_IRQ_GIC_START + 77)		/*	USB_OHCI2	*/


#define AW_IRQ_SS			(AW_IRQ_GIC_START + 80)		/*	SS			*/
#define AW_IRQ_TS			(AW_IRQ_GIC_START + 81)		/*	TS			*/
#define AW_IRQ_GMAC			(AW_IRQ_GIC_START + 82)		/*	GMAC		*/
#define AW_IRQ_MP			(AW_IRQ_GIC_START + 83)		/*	MP			*/
#define AW_IRQ_CSI0			(AW_IRQ_GIC_START + 84)		/*	CSI0		*/
#define AW_IRQ_CSI1			(AW_IRQ_GIC_START + 85)		/*	CSI1		*/
#define AW_IRQ_LCD0			(AW_IRQ_GIC_START + 86)		/*	LCD0		*/
#define AW_IRQ_LCD1			(AW_IRQ_GIC_START + 87)		/*	LCD1		*/
#define AW_IRQ_HDMI			(AW_IRQ_GIC_START + 88)		/*	HDMI		*/
#define AW_IRQ_MIPIDSI		(AW_IRQ_GIC_START + 89)		/*	MIPI DSI	*/
#define AW_IRQ_MIPICSI		(AW_IRQ_GIC_START + 90)		/*	MIPI CSI	*/
#define AW_IRQ_DRC01		(AW_IRQ_GIC_START + 91)		/*	DRC 0/1		*/
#define AW_IRQ_DEU01		(AW_IRQ_GIC_START + 92)		/*	DEU	0/1		*/
#define AW_IRQ_DEFE0		(AW_IRQ_GIC_START + 93)		/*	DE_FE0		*/
#define AW_IRQ_DEFE1		(AW_IRQ_GIC_START + 94)		/*	DE_FE1		*/
#define AW_IRQ_DEBE0		(AW_IRQ_GIC_START + 95)		/*	DE_BE0		*/
#define AW_IRQ_DEBE1		(AW_IRQ_GIC_START + 96)		/*	DE_BE1		*/
#define	AW_IRQ_GPU			(AW_IRQ_GIC_START + 97)		/*	GPU			*/

#define	AW_IRQ_CTI0			(AW_IRQ_GIC_START + 108)	/*	CTI0		*/
#define	AW_IRQ_CTI1			(AW_IRQ_GIC_START + 109)	/*	CTI1		*/
#define	AW_IRQ_CTI2			(AW_IRQ_GIC_START + 110)	/*	CTI2		*/
#define	AW_IRQ_CTI3			(AW_IRQ_GIC_START + 111)	/*	CTI3		*/
#define AW_IRQ_COMMTX0		(AW_IRQ_GIC_START + 112)	/*	COMMTX0		*/
#define AW_IRQ_COMMTX1		(AW_IRQ_GIC_START + 113)	/*	COMMTX1		*/
#define AW_IRQ_COMMTX2		(AW_IRQ_GIC_START + 114)	/*	COMMTX2		*/
#define AW_IRQ_COMMTX3		(AW_IRQ_GIC_START + 115)	/*	COMMTX3		*/
#define AW_IRQ_COMMRX0		(AW_IRQ_GIC_START + 116)	/*	COMMRX0		*/
#define AW_IRQ_COMMRX1		(AW_IRQ_GIC_START + 117)	/*	COMMRX1		*/
#define AW_IRQ_COMMRX2		(AW_IRQ_GIC_START + 118)	/*	COMMRX2		*/
#define AW_IRQ_COMMRX3		(AW_IRQ_GIC_START + 119)	/*	COMMRX3		*/
#define	AW_IRQ_PMU0			(AW_IRQ_GIC_START + 120)	/*	PMU0		*/
#define	AW_IRQ_PMU1			(AW_IRQ_GIC_START + 121)	/*	PMU1		*/
#define	AW_IRQ_PMU2			(AW_IRQ_GIC_START + 122)	/*	PMU2		*/
#define	AW_IRQ_PMU3			(AW_IRQ_GIC_START + 123)	/*	PMU3		*/
#define	AW_IRQ_AXI_ERROR	(AW_IRQ_GIC_START + 124)	/*	AXI_ERROR	*/

#define GIC_IRQ_NUM			(AW_IRQ_AXI_ERROR + 1)

#elif   defined (AW_FPGA_V4_PLATFORM) 			   /* S4 820 */
#define	AW_IRQ_NMI			(AW_IRQ_GIC_START + 0) /*	NMI			*/
#define AW_IRQ_UART0		(AW_IRQ_GIC_START + 1) /*	UART0		*/
#define AW_IRQ_UART1    	(AW_IRQ_GIC_START + 1) /*   UART1   	*/
#define AW_IRQ_TWI0			(AW_IRQ_GIC_START + 2) /*	TWI0		*/
#define AW_IRQ_TWI1			(AW_IRQ_GIC_START + 2) /*	TWI1		*/
#define AW_IRQ_EINTA		(AW_IRQ_GIC_START + 2) /*	EINTA		*/
#define AW_IRQ_SPDIF		(AW_IRQ_GIC_START + 3) /*	SPDIF		*/
#define AW_IRQ_IIS			(AW_IRQ_GIC_START + 3) /*	IIS			*/
#define AW_IRQ_MP			(AW_IRQ_GIC_START + 3) /*	MP			*/
#define AW_IRQ_CSI0			(AW_IRQ_GIC_START + 3) /*	CSI0		*/
#define AW_IRQ_CSI1			(AW_IRQ_GIC_START + 3) /*	CSI1		*/
#define	AW_IRQ_MIPIDSI		(AW_IRQ_GIC_START + 3) /*	MIPIDSI		*/
#define	AW_IRQ_MIPICSI		(AW_IRQ_GIC_START + 3) /*	MIPICSI		*/
#define	AW_IRQ_TIMER0		(AW_IRQ_GIC_START + 4) /*	TIMER0		*/
#define AW_IRQ_TIMER1		(AW_IRQ_GIC_START + 5) /*	TIMER1		*/
#define AW_IRQ_TIMER2		(AW_IRQ_GIC_START + 5) /*	TIMER2		*/
#define AW_IRQ_TIMER3		(AW_IRQ_GIC_START + 5) /*	TIMER3		*/
#define AW_IRQ_TIMER4		(AW_IRQ_GIC_START + 5) /*	TIMER4		*/
#define AW_IRQ_TIMER5		(AW_IRQ_GIC_START + 5) /*	TIMER5		*/
#define AW_IRQ_DMA			(AW_IRQ_GIC_START + 6) /*	DMA			*/
#define	AW_IRQ_WATCHDOG1	(AW_IRQ_GIC_START + 7) /*	WATCHDOG1	*/
#define	AW_IRQ_WATCHDOG2	(AW_IRQ_GIC_START + 8) /*	WATCHDOG2	*/
#define	AW_IRQ_RTIMER0		(AW_IRQ_GIC_START + 9) /*	RTIMER0		*/
#define	AW_IRQ_RTIMER1		(AW_IRQ_GIC_START + 9) /*	RTIMER1		*/
#define	AW_IRQ_RWATCHDOG	(AW_IRQ_GIC_START + 9) /*	RWATCHDOG	*/
#define	AW_IRQ_SPINLOCK		(AW_IRQ_GIC_START + 10)/*	SPINLOCK	*/
#define AW_IRQ_MBOX			(AW_IRQ_GIC_START + 11)/*	MBOX		*/
#define	AW_IRQ_HSTIMER0		(AW_IRQ_GIC_START + 12)/*	HSTIMER		*/
#define	AW_IRQ_MMC0			(AW_IRQ_GIC_START + 13)/*	MMC0		*/
#define AW_IRQ_MMC1			(AW_IRQ_GIC_START + 13)/*	MMC1		*/
#define AW_IRQ_SPI0			(AW_IRQ_GIC_START + 13)/*	SPI0		*/
#define AW_IRQ_NAND0		(AW_IRQ_GIC_START + 14)/*	NAND0		*/
#define AW_IRQ_NAND1		(AW_IRQ_GIC_START + 15)/*	NAND1		*/
#define AW_IRQ_MMC2			(AW_IRQ_GIC_START + 15)/*	MMC2		*/
#define AW_IRQ_MMC3			(AW_IRQ_GIC_START + 15)/*	MMC3		*/
#define AW_IRQ_RCIR			(AW_IRQ_GIC_START + 16)/*	RCIR		*/
#define AW_IRQ_RUART		(AW_IRQ_GIC_START + 16)/*	RUART		*/
#define AW_IRQ_EINTL		(AW_IRQ_GIC_START + 17)/*	EINTL		*/
#define AW_IRQ_EINTM		(AW_IRQ_GIC_START + 17)/*	EINTM		*/
#define AW_IRQ_RALARM0		(AW_IRQ_GIC_START + 18)/*	RALARM0		*/
#define AW_IRQ_RALARM1		(AW_IRQ_GIC_START + 18)/*	RALARM1		*/
#define AW_IRQ_RP2TWI		(AW_IRQ_GIC_START + 19)/*	RP2WI		*/
#define AW_IRQ_RTWI			(AW_IRQ_GIC_START + 19)/*	RTWI		*/
#define AW_IRQ_VE			(AW_IRQ_GIC_START + 20)/*	VE			*/
#define AW_IRQ_USB_OTG		(AW_IRQ_GIC_START + 21)/*	USB_OTG		*/
#define AW_IRQ_USB_HCI		(AW_IRQ_GIC_START + 21)/*	USB_HCI		*/
#define AW_IRQ_USB_EHCI0	(AW_IRQ_GIC_START + 22)/*	USB_EHCI0	*/
#define AW_IRQ_USB_OHCI0	(AW_IRQ_GIC_START + 23)/*	USB_OHCI0	*/
#define AW_IRQ_SS			(AW_IRQ_GIC_START + 24)/*	SS			*/
#define AW_IRQ_TS			(AW_IRQ_GIC_START + 24)/*	TS			*/
#define AW_IRQ_GMAC			(AW_IRQ_GIC_START + 24)/*	GMAC		*/
#define AW_IRQ_CODEC		(AW_IRQ_GIC_START + 24)/*	CODEC		*/
#define AW_IRQ_TP			(AW_IRQ_GIC_START + 24)/*	TP			*/
#define AW_IRQ_LRADC		(AW_IRQ_GIC_START + 24)/*	LRADC		*/
#define AW_IRQ_MTCACC		(AW_IRQ_GIC_START + 24)/*	MTCACC		*/
#define AW_IRQ_LCD0			(AW_IRQ_GIC_START + 25)/*	LCD0		*/
#define AW_IRQ_TZASC		(AW_IRQ_GIC_START + 26)/*	TZASC		*/
#define AW_IRQ_DEFE0		(AW_IRQ_GIC_START + 27)/*	DEFE0		*/
#define AW_IRQ_DEBE0		(AW_IRQ_GIC_START + 28)/*	DEFB0		*/
#define AW_IRQ_DRC01		(AW_IRQ_GIC_START + 29)/*	DRC01		*/
#define AW_IRQ_DEU01		(AW_IRQ_GIC_START + 30)/*	DEU01		*/
#define AW_IRQ_1WIRE		(AW_IRQ_GIC_START + 31)/*	R_1WIRE		*/
#define GIC_IRQ_NUM			(AW_IRQ_1WIRE + 1)

#elif defined (AW_FPGA_V7_PLATFORM)

#define AW_IRQ_NMI			(AW_IRQ_GIC_START + 0) /*	NMI			*/
#define AW_IRQ_UART0		(AW_IRQ_GIC_START + 1) /*	UART0		*/
#define AW_IRQ_TWI0			(AW_IRQ_GIC_START + 2) /*	TWI0		*/
#define AW_IRQ_TWI1			(AW_IRQ_GIC_START + 2) /*	TWI1		*/
#define AW_IRQ_SPDIF		(AW_IRQ_GIC_START + 3) /*	SPDIF		*/
#define AW_IRQ_IIS			(AW_IRQ_GIC_START + 3) /*	IIS			*/
#define AW_IRQ_CSI0			(AW_IRQ_GIC_START + 3) /*	CSI0		*/
#define AW_IRQ_CSI1			(AW_IRQ_GIC_START + 3) /*	CSI1		*/
#define	AW_IRQ_MIPIDSI		(AW_IRQ_GIC_START + 3) /*	MIPIDSI		*/
#define	AW_IRQ_MIPICSI		(AW_IRQ_GIC_START + 3) /*	MIPICSI		*/
#define	AW_IRQ_TIMER0		(AW_IRQ_GIC_START + 4) /*	TIMER0		*/
#define AW_IRQ_TIMER1		(AW_IRQ_GIC_START + 5) /*	TIMER1		*/
#define AW_IRQ_DMA			(AW_IRQ_GIC_START + 6) /*	DMA			*/
#define	AW_IRQ_WATCHDOG1	(AW_IRQ_GIC_START + 7) /*	WATCHDOG1	*/
#define	AW_IRQ_WATCHDOG2	(AW_IRQ_GIC_START + 8) /*	WATCHDOG2	*/
#define	AW_IRQ_RTIMER0		(AW_IRQ_GIC_START + 9) /*	RTIMER0		*/
#define	AW_IRQ_RTIMER1		(AW_IRQ_GIC_START + 9) /*	RTIMER1		*/
#define	AW_IRQ_RWATCHDOG	(AW_IRQ_GIC_START + 9) /*	RWATCHDOG	*/
#define	AW_IRQ_SPINLOCK		(AW_IRQ_GIC_START + 10)/*	SPINLOCK	*/
#define AW_IRQ_MBOX			(AW_IRQ_GIC_START + 11)/*	MBOX		*/
#define	AW_IRQ_HSTIMER0		(AW_IRQ_GIC_START + 12)/*	HSTIMER		*/
#define	AW_IRQ_MMC0			(AW_IRQ_GIC_START + 13)/*	MMC0		*/
#define	AW_IRQ_MMC1			(AW_IRQ_GIC_START + 13)/*	MMC1		*/
#define	AW_IRQ_MMC2			(AW_IRQ_GIC_START + 13)/*	MMC2		*/
#define	AW_IRQ_MMC3			(AW_IRQ_GIC_START + 13)/*	MMC3		*/
#define AW_IRQ_SPI0			(AW_IRQ_GIC_START + 13)/*	SPI0		*/
#define AW_IRQ_NAND0		(AW_IRQ_GIC_START + 14)/*	NAND0		*/
#define AW_IRQ_NAND1		(AW_IRQ_GIC_START + 15)/*	NAND1		*/
#define AW_IRQ_RCIR			(AW_IRQ_GIC_START + 16)/*	RCIR		*/
#define AW_IRQ_RUART		(AW_IRQ_GIC_START + 16)/*	RUART		*/
#define AW_IRQ_RGPIOL		(AW_IRQ_GIC_START + 17)/*	RGPIOL		*/
#define AW_IRQ_RALARM0		(AW_IRQ_GIC_START + 18)/*	RALARM0		*/
#define AW_IRQ_RALARM1		(AW_IRQ_GIC_START + 18)/*	RALARM1		*/
#define AW_IRQ_RP2TWI		(AW_IRQ_GIC_START + 19)/*	RP2WI		*/
#define AW_IRQ_RTWI			(AW_IRQ_GIC_START + 19)/*	RTWI		*/
#define AW_IRQ_VE			(AW_IRQ_GIC_START + 20)/*	VE			*/
#define AW_IRQ_USB_OTG		(AW_IRQ_GIC_START + 21)/*	USB_OTG		*/
#define AW_IRQ_USB_HCI		(AW_IRQ_GIC_START + 21)/*	USB_HCI		*/
#define AW_IRQ_USB_EHCI0	(AW_IRQ_GIC_START + 22)/*	USB_EHCI0	*/
#define AW_IRQ_USB_OHCI0	(AW_IRQ_GIC_START + 23)/*	USB_OHCI0	*/
#define AW_IRQ_SS			(AW_IRQ_GIC_START + 24)/*	SS			*/
#define AW_IRQ_TS			(AW_IRQ_GIC_START + 24)/*	TS			*/
#define AW_IRQ_GMAC			(AW_IRQ_GIC_START + 24)/*	GMAC		*/
#define AW_IRQ_CODEC		(AW_IRQ_GIC_START + 24)/*	CODEC		*/
#define AW_IRQ_TP			(AW_IRQ_GIC_START + 24)/*	TP			*/
#define AW_IRQ_LCD0			(AW_IRQ_GIC_START + 25)/*	LCD0		*/
#define AW_IRQ_TZASC		(AW_IRQ_GIC_START + 26)/*	TZASC		*/
#define AW_IRQ_GPU			(AW_IRQ_GIC_START + 26)/*	GPU			*/
#define AW_IRQ_DEFE0		(AW_IRQ_GIC_START + 27)/*	DEFE0		*/
#define AW_IRQ_DEBE0		(AW_IRQ_GIC_START + 28)/*	DEFB0		*/
#define AW_IRQ_DRC01		(AW_IRQ_GIC_START + 29)/*	DRC01		*/
#define AW_IRQ_DEU01		(AW_IRQ_GIC_START + 30)/*	DEU01		*/
#define AW_IRQ_1WIRE		(AW_IRQ_GIC_START + 31)/*	R_1WIRE		*/

#define GIC_IRQ_NUM			(AW_IRQ_1WIRE + 1)

#endif	//fpga irq mapping

/* processer target */
#define GIC_CPU_TARGET(_n)	(1 << (_n))
#define GIC_CPU_TARGET0		GIC_CPU_TARGET(0)
#define GIC_CPU_TARGET1		GIC_CPU_TARGET(1)
#define GIC_CPU_TARGET2		GIC_CPU_TARGET(2)
#define GIC_CPU_TARGET3		GIC_CPU_TARGET(3)
#define GIC_CPU_TARGET4		GIC_CPU_TARGET(4)
#define GIC_CPU_TARGET5		GIC_CPU_TARGET(5)
#define GIC_CPU_TARGET6		GIC_CPU_TARGET(6)
#define GIC_CPU_TARGET7		GIC_CPU_TARGET(7)
/* trigger mode */
#define GIC_SPI_LEVEL_TRIGGER	(0)	//2b'00
#define GIC_SPI_EDGE_TRIGGER	(2)	//2b'10

extern void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
extern void irq_free_handler(int irq);
extern int irq_enable(int irq_no);
extern int irq_disable(int irq_no);

int arch_interrupt_init (void);

int arch_interrupt_exit (void);


#endif
