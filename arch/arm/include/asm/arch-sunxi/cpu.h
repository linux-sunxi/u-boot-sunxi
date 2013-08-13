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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SUNXI_CPU_H
#define _SUNXI_CPU_H

#define SUNXI_SRAM_A1_BASE		0x00000000
#define SUNXI_SRAM_A1_SIZE		(16 * 1024)	/* 16 kiB */

#define SUNXI_SRAM_A2_BASE		0x00004000	/* 16 kiB */
#define SUNXI_SRAM_A3_BASE		0x00008000	/* 13 kiB */
#define SUNXI_SRAM_A4_BASE		0x0000b400	/* 3 kiB */
#define SUNXI_SRAM_D_BASE		0x01c00000
#define SUNXI_SRAM_B_BASE		0x01c00000	/* 64 kiB (secure) */

#define SUNXI_SRAMC_BASE		0x01c00000
#define SUNXI_DRAMC_BASE		0x01c01000
#define SUNXI_DMA_BASE			0x01c02000
#define SUNXI_NFC_BASE			0x01c03000
#define SUNXI_TS_BASE			0x01c04000
#define SUNXI_SPI0_BASE			0x01c05000
#define SUNXI_SPI1_BASE			0x01c06000
#define SUNXI_MS_BASE			0x01c07000
#define SUNXI_TVD_BASE			0x01c08000
#define SUNXI_CSI0_BASE			0x01c09000
#define SUNXI_TVE0_BASE			0x01c0a000
#define SUNXI_EMAC_BASE			0x01c0b000
#define SUNXI_LCD0_BASE			0x01c0C000
#define SUNXI_LCD1_BASE			0x01c0d000
#define SUNXI_VE_BASE			0x01c0e000
#define SUNXI_MMC0_BASE			0x01c0f000
#define SUNXI_MMC1_BASE			0x01c10000
#define SUNXI_MMC2_BASE			0x01c11000
#define SUNXI_MMC3_BASE			0x01c12000
#define SUNXI_USB0_BASE			0x01c13000
#define SUNXI_USB1_BASE			0x01c14000
#define SUNXI_SS_BASE			0x01c15000
#define SUNXI_HDMI_BASE			0x01c16000
#define SUNXI_SPI2_BASE			0x01c17000
#define SUNXI_SATA_BASE			0x01c18000
#define SUNXI_PATA_BASE			0x01c19000
#define SUNXI_ACE_BASE			0x01c1a000
#define SUNXI_TVE1_BASE			0x01c1b000
#define SUNXI_USB2_BASE			0x01c1c000
#define SUNXI_CSI1_BASE			0x01c1d000
#define SUNXI_TZASC_BASE		0x01c1e000
#define SUNXI_SPI3_BASE			0x01c1f000

#define SUNXI_CCM_BASE			0x01c20000
#define SUNXI_INTC_BASE			0x01c20400
#define SUNXI_PIO_BASE			0x01c20800
#define SUNXI_TIMER_BASE		0x01c20c00
#define SUNXI_SPDIF_BASE		0x01c21000
#define SUNXI_AC97_BASE			0x01c21400
#define SUNXI_IR0_BASE			0x01c21800
#define SUNXI_IR1_BASE			0x01c21c00

#define SUNXI_IIS_BASE			0x01c22400
#define SUNXI_LRADC_BASE		0x01c22800
#define SUNXI_AD_DA_BASE		0x01c22c00
#define SUNXI_KEYPAD_BASE		0x01c23000
#define SUNXI_TZPC_BASE			0x01c23400
#define SUNXI_SID_BASE			0x01c23800
#define SUNXI_SJTAG_BASE		0x01c23c00

#define SUNXI_TP_BASE			0x01c25000
#define SUNXI_PMU_BASE			0x01c25400
#define SUNXI_CPUCFG_BASE		0x01c25c00	/* sun7i only ? */

#define SUNXI_UART0_BASE		0x01c28000
#define SUNXI_UART1_BASE		0x01c28400
#define SUNXI_UART2_BASE		0x01c28800
#define SUNXI_UART3_BASE		0x01c28c00
#define SUNXI_UART4_BASE		0x01c29000
#define SUNXI_UART5_BASE		0x01c29400
#define SUNXI_UART6_BASE		0x01c29800
#define SUNXI_UART7_BASE		0x01c29c00
#define SUNXI_PS2_0_BASE		0x01c2a000
#define SUNXI_PS2_1_BASE		0x01c2a400

#define SUNXI_TWI0_BASE			0x01c2ac00
#define SUNXI_TWI1_BASE			0x01c2b000
#define SUNXI_TWI2_BASE			0x01c2b400

#define SUNXI_CAN_BASE			0x01c2bc00

#define SUNXI_SCR_BASE			0x01c2c400

#define SUNXI_GPS_BASE			0x01c30000
#define SUNXI_MALI400_BASE		0x01c40000

/* module sram */
#define SUNXI_SRAM_C_BASE		0x01d00000

#define SUNXI_DE_FE0_BASE		0x01e00000
#define SUNXI_DE_FE1_BASE		0x01e20000
#define SUNXI_DE_BE0_BASE		0x01e60000
#define SUNXI_DE_BE1_BASE		0x01e40000
#define SUNXI_MP_BASE			0x01e80000
#define SUNXI_AVG_BASE			0x01ea0000

/* CoreSight Debug Module */
#define SUNXI_CSDM_BASE			0x3f500000

#define SUNXI_DDRII_DDRIII_BASE		0x40000000	/* 2 GiB */

#define SUNXI_BROM_BASE			0xffff0000	/* 32 kiB */

#define SUNXI_CPU_CFG			(SUNXI_TIMER_BASE + 0x13c)

#ifndef __ASSEMBLY__
/* boot type */
enum sunxi_boot_type_t {
	SUNXI_BOOT_TYPE_NULL,
	SUNXI_BOOT_TYPE_MMC0,
	SUNXI_BOOT_TYPE_NAND,
	SUNXI_BOOT_TYPE_MMC2,
	SUNXI_BOOT_TYPE_SPI
};

void sunxi_board_init(void);
extern void sunxi_reset(void);
#endif /* __ASSEMBLY__ */

#endif /* _CPU_H */
