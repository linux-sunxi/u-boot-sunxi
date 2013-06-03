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

#ifndef __PLATFORM_H
#define __PLATFORM_H

#define SRAM_A1_BASE		(0x0)
#define SRAM_A1_SIZE		(0x4000)
#define SRAM_A2_BASE		(0x4000)
#define SRAM_A2_SIZE		(0x4000)
#define SRAM_A3_BASE		(0x8000)
#define SRAM_A3_SIZE		(0x3400)
#define SRAM_A4_BASE		(0xb400)
#define SRAM_A4_SIZE		(0x0c00)

#define SRAM_NAND_BASE		(0x0c00)
#define SRAM_D_BASE		(0x010000)
#define SRAM_B_BASE		(0x020000)

/* base address of modules */
#define SRAMC_BASE		(0x01c00000)
#define DRAMCOM_BASE		(0x01c01000)
#define DMA_BASE		(0x01c02000)
#define NFC_BASE		(0x01c03000)
#define TSC_BASE		(0x01c04000)
#define SPI0_BASE		(0x01c05000)
#define SPI1_BASE		(0x01c06000)
#define MS_BASE			(0x01c07000)
#define TVD_BASE		(0x01c08000)
#define CSI0_BASE		(0x01c09000)
#define TVE_BASE		(0x01c0a000)
#define EMAC_BASE		(0x01c0b000)
#define LCD0_BASE		(0x01c0c000)
#define LCD1_BASE		(0x01c0d000)
#define VE_BASE			(0x01c0e000)
#define MMC0_BASE		(0x01c0f000)
#define MMC1_BASE		(0x01c10000)
#define MMC2_BASE		(0x01c11000)
#define MMC3_BASE		(0x01c12000)
#define USB0_BASE		(0x01c13000)
#define USB1_BASE		(0x01c14000)
#define SS_BASE			(0x01c15000)
#define HDMI_BASE		(0x01c16000)
#define SPI2_BASE		(0x01c17000)
#define SATA_BASE		(0x01c18000)
#define PATA_BASE		(0x01c19000)
#define ACE_BASE		(0x01c1A000)
#define TVE1_BASE		(0x01c1B000)
#define USB2_BASE		(0x01c1C000)
#define CSI1_BASE		(0x01c1D000)
#define TZASC_BASE		(0x01c1e000)
#define SPI3_BASE		(0x01c1F000)
#define CCM_BASE		(0x01c20000)

#define PIO_BASE		(0x01c20800)
#define TIMER_BASE		(0x01c20c00)
#define SPDIF_BASE		(0x01c21000)
#define AC97_BASE		(0x01c21400)
#define CIR0_BASE		(0x01f21800)
#define CIR1_BASE		(0x01f21C00)

#define I2S_BASE		(0x01c22400)
#define LRADC_BASE		(0x01c22800)
#define ADDA_BASE		(0x01c22c00)
#define KP_BASE			(0x01c23000)
#define TZPC_BASE		(0x01c23400)
#define SID_BASE		(0x01c23800)
#define SJTAG_BASE		(0x01c23c00)

#define TP_BASE			(0x01c25000)
#define PMU_BASE		(0x01c25400)

#define CPUCFG_BASE		(0x01c25C00)

#define UART0_BASE		(0x01c28000)
#define UART1_BASE		(0x01c28400)
#define UART2_BASE		(0x01c28800)
#define UART3_BASE		(0x01c28c00)
#define UART4_BASE		(0x01c29000)
#define UART5_BASE		(0x01c29400)
#define UART6_BASE		(0x01c29800)
#define UART7_BASE		(0x01c29C00)
#define PS2_0_BASE		(0x01c2A000)
#define PS2_1_BASE		(0x01c2A400)

#define TWI0_BASE		(0x01c2ac00)
#define TWI1_BASE		(0x01c2b000)
#define TWI2_BASE		(0x01c2b400)

#define CAN_BASE		(0x01c2BC00)

#define GPS_BASE		(0x01c30000)
#define GPU_BASE		(0x01c40000)
#define GMAC_BASE		(0x01c50000)

#define HSTMR_BASE		(0x01c60000)

#define ARMV7_SCU_BASE		(0x01c80000)
#define ARMV7_GIC_BASE		(0x01c81000)
#define ARMV7_CPUIF_BASE	(0x01c82000)
#define ARMV7_CPUBIST_BASE	(0x01c88000)

#define SRAM_C_BASE		(0x01D00000)

#define DEFE0_BASE		(0x01e00000)
#define DEFE1_BASE		(0x01e20000)
#define DEBE0_BASE		(0x01e60000)
#define DEBE1_BASE		(0x01e40000)
#define MP_BASE			(0x01e80000)
#define AVG_BASE		(0x01ea0000)

#define CORESIGHT_BASE		(0x3F500000)
#endif
