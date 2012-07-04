/**
 * platform.h - definition for the base address of each controller
 * date:    2012-2-13 8:43:26
 * author:  Aaron<leafy.myeh@allwinnertech.com>
 * history: V0.1
 * modify: 
 * 2012-3-6 13:25:45, remove CSI0/CSI1 STWI
 */
#ifndef __PLATFORM_H
#define __PLATFORM_H

#define SRAM_A1_CPU12_BASE	(0x0)
#define SRAM_A1_CPU0_BASE	(0x8000)
#define SRAM_A1_SIZE		(0x8000)
#define SRAM_A2_CPU12_BASE	(0x40000)
#define SRAM_A2_CPU0_BASE	(0x0)
#define SRAM_A2_SIZE		(0x8000)
#define SRAM_C_BASE			(0x01D00000)

/* base address of modules */
#define SRAMC_BASE			(0x01c00000)

#define DMA_BASE			(0x01c02000)
#define NFC_BASE			(0x01c03000)
#define TSC_BASE			(0x01c04000)

//#define CSI0_BASE			(0x01c09000)
//#define CSI1_BASE			(0x01c0a000)

#define LCD0_BASE			(0x01c0c000)
#define LCD1_BASE			(0x01c0d000)
#define VE_BASE				(0x01c0e000)
#define MMC0_BASE			(0x01c0f000)
#define MMC1_BASE			(0x01c10000)
#define MMC2_BASE			(0x01c11000)
#define MMC3_BASE			(0x01c12000)

#define SS_BASE				(0x01c15000)
#define HDMI_BASE			(0x01c16000)
#define MSGBOX_BASE			(0x01c17000)
#define SPINLOCK_BASE		(0x01c18000)
#define OTG_BASE			(0x01c19000)
#define EHCI0_BASE			(0x01c1a000)
#define OHCI0_BASE			(0x01c1a000)
#define EHCI1_BASE			(0x01c1b000)
#define OHCI1_BASE			(0x01c1b000)
#define OHCI2_BASE			(0x01c1c000)

#define TZASC_BASE			(0x01c1e000)

#define CCM_BASE			(0x01c20000)

#define PIO_BASE			(0x01c20800)
#define TIMER_BASE			(0x01c20c00)
#define SPDIF_BASE			(0x01c21000)
#define PWM03_BASE			(0x01c21400)

#define I2S0_BASE			(0x01c22000)
#define I2S1_BASE			(0x01c22400)
#define LRADC_BASE			(0x01c22800)
#define ADDA_BASE			(0x01c22c00)
#define KP_BASE				(0x01c23000)
#define TZPC_BASE			(0x01c23400)
#define SID_BASE			(0x01c23800)
#define SJTAG_BASE			(0x01c23c00)

#define GPADC_BASE			(0x01c24c00)
#define TP_BASE				(0x01c25000)

#define UART0_BASE			(0x01c28000)
#define UART1_BASE			(0x01c28400)
#define UART2_BASE			(0x01c28800)
#define UART3_BASE			(0x01c28c00)
#define UART4_BASE			(0x01c29000)

//#define STWI_BASE			(0x01c2a800)
#define TWI0_BASE			(0x01c2ac00)
#define TWI1_BASE			(0x01c2b000)
#define TWI2_BASE			(0x01c2b400)
#define TWI3_BASE			(0x01c2b800)

#define GMAC_BASE			(0x01c30000)
#define GPU_BASE			(0x01c40000)

#define HSTMR_BASE			(0x01c60000)

#define DRAMCOM_BASE		(0x01c62000)
#define DRAMCTL0_BASE		(0x01c63000)
#define DRAMCTL1_BASE		(0x01c64000)
#define DRAMPHY0_BASE		(0x01c65000)
#define DRAMPHY1_BASE		(0x01c66000)

#define SPI0_BASE			(0x01c68000)
#define SPI1_BASE			(0x01c69000)
#define SPI2_BASE			(0x01c6a000)
#define SPI3_BASE			(0x01c6b000)

#define ARMA9_SCU_BASE		(0x01c80000)
#define ARMA9_GIC_BASE		(0x01c81000)
#define ARMA9_CPUIF_BASE	(0x01c82000)

#define MIPI_DSI0_BASE		(0x01ca0000)
#define MIPI_DSI0PHY_BASE	(0x01ca1000)

#define CSI0_BASE			(0x01cb0000)
#define MIPI_CSI0_BASE		(0x01cb1000)
#define MIPI_CSI0PHY_BASE	(0x01cb2000)
#define CSI1_BASE			(0x01cb3000)

#define ISP_BASE			(0x01cb8000)
#define ISPMEM_BASE			(0x01cc0000)

#define DEFE0_BASE			(0x01e00000)
#define DEFE1_BASE			(0x01e20000)
#define DRC1_BASE			(0x01e50000)
#define DEBE0_BASE			(0x01e60000)
#define DRC0_BASE			(0x01e70000)
#define DEBE1_BASE			(0x01e40000)
#define MP_BASE				(0x01e80000)
#define DEU1_BASE			(0x01ea0000)
#define DEU0_BASE			(0x01eb0000)

#define RTC_BASE			(0x01f00000)
#define R_BREATH_BASE		(0x01f00400)
#define R_TMR01_BASE		(0x01f00800)
#define R_INTC_BASE			(0x01f00C00)
#define R_WDOG_BASE			(0x01f01000)
#define R_PRCM_BASE			(0x01f01400)
#define R_CPUCFG_BASE		(0x01f01C00)
#define R_CIR_BASE			(0x01f02000)
#define R_TWI_BASE			(0x01f02400)
#define R_UART_BASE			(0x01f02800)
#define R_PIO_BASE			(0x01f02c00)
#define R_1WIRE_BASE		(0x01f03000)
#define R_P2WI_BASE			(0x01f03400)

#endif
