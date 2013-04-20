/*
 * hardware.h
 *
 * hardware specific header
 *
 * Copyright (C) 2013, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __AM33XX_HARDWARE_H
#define __AM33XX_HARDWARE_H

#include <config.h>
#include <asm/arch/omap.h>
#ifdef CONFIG_AM33XX
#include <asm/arch/hardware_am33xx.h>
#elif defined(CONFIG_TI814X)
#include <asm/arch/hardware_ti814x.h>
#endif

/*
 * Common hardware definitions
 */

/* DM Timer base addresses */
#define DM_TIMER0_BASE			0x4802C000
#define DM_TIMER1_BASE			0x4802E000
#define DM_TIMER2_BASE			0x48040000
#define DM_TIMER3_BASE			0x48042000
#define DM_TIMER4_BASE			0x48044000
#define DM_TIMER5_BASE			0x48046000
#define DM_TIMER6_BASE			0x48048000
#define DM_TIMER7_BASE			0x4804A000

/* GPIO Base address */
#define GPIO0_BASE			0x48032000
#define GPIO1_BASE			0x4804C000

/* BCH Error Location Module */
#define ELM_BASE			0x48080000

/* EMIF Base address */
#define EMIF4_0_CFG_BASE		0x4C000000
#define EMIF4_1_CFG_BASE		0x4D000000

/* PLL related registers */
#define CM_PER				0x44E00000
#define CM_WKUP				0x44E00400
#define CM_DPLL				0x44E00500
#define CM_DEVICE			0x44E00700
#define CM_RTC				0x44E00800
#define CM_CEFUSE			0x44E00A00
#define PRM_DEVICE			0x44E00F00

/* VTP Base address */
#define VTP1_CTRL_ADDR			0x48140E10

/* DDR Base address */
#define DDR_CTRL_ADDR			0x44E10E04
#define DDR_CONTROL_BASE_ADDR		0x44E11404
#define DDR_PHY_CMD_ADDR2		0x47C0C800
#define DDR_PHY_DATA_ADDR2		0x47C0C8C8

/* UART */
#define DEFAULT_UART_BASE		UART0_BASE

#define DDRPHY_0_CONFIG_BASE		(CTRL_BASE + 0x1400)
#define DDRPHY_CONFIG_BASE		DDRPHY_0_CONFIG_BASE

/* GPMC Base address */
#define GPMC_BASE			0x50000000

/* CPSW Config space */
#define CPSW_BASE			0x4A100000

/* OTG */
#define USB0_OTG_BASE			0x47401000
#define USB1_OTG_BASE			0x47401800

#endif /* __AM33XX_HARDWARE_H */
