/*
 * Copyright (C) 2013 Boundary Devices Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef __ASM_ARCH_MX6_DDR_H__
#define __ASM_ARCH_MX6_DDR_H__

#ifdef CONFIG_MX6Q
#include "mx6q-ddr.h"
#else
#if defined(CONFIG_MX6DL) || defined(CONFIG_MX6S)
#include "mx6dl-ddr.h"
#else
#error "Please select cpu"
#endif	/* CONFIG_MX6DL or CONFIG_MX6S */
#endif	/* CONFIG_MX6Q */

#define MX6_MMDC_P0_MDCTL	0x021b0000
#define MX6_MMDC_P0_MDPDC	0x021b0004
#define MX6_MMDC_P0_MDOTC	0x021b0008
#define MX6_MMDC_P0_MDCFG0	0x021b000c
#define MX6_MMDC_P0_MDCFG1	0x021b0010
#define MX6_MMDC_P0_MDCFG2	0x021b0014
#define MX6_MMDC_P0_MDMISC	0x021b0018
#define MX6_MMDC_P0_MDSCR	0x021b001c
#define MX6_MMDC_P0_MDREF	0x021b0020
#define MX6_MMDC_P0_MDRWD	0x021b002c
#define MX6_MMDC_P0_MDOR	0x021b0030
#define MX6_MMDC_P0_MDASP	0x021b0040
#define MX6_MMDC_P0_MAPSR	0x021b0404
#define MX6_MMDC_P0_MPZQHWCTRL	0x021b0800
#define MX6_MMDC_P0_MPWLDECTRL0	0x021b080c
#define MX6_MMDC_P0_MPWLDECTRL1	0x021b0810
#define MX6_MMDC_P0_MPODTCTRL	0x021b0818
#define MX6_MMDC_P0_MPRDDQBY0DL	0x021b081c
#define MX6_MMDC_P0_MPRDDQBY1DL	0x021b0820
#define MX6_MMDC_P0_MPRDDQBY2DL	0x021b0824
#define MX6_MMDC_P0_MPRDDQBY3DL	0x021b0828
#define MX6_MMDC_P0_MPDGCTRL0	0x021b083c
#define MX6_MMDC_P0_MPDGCTRL1	0x021b0840
#define MX6_MMDC_P0_MPRDDLCTL	0x021b0848
#define MX6_MMDC_P0_MPWRDLCTL	0x021b0850
#define MX6_MMDC_P0_MPMUR0	0x021b08b8

#define MX6_MMDC_P1_MDCTL	0x021b4000
#define MX6_MMDC_P1_MDPDC	0x021b4004
#define MX6_MMDC_P1_MDOTC	0x021b4008
#define MX6_MMDC_P1_MDCFG0	0x021b400c
#define MX6_MMDC_P1_MDCFG1	0x021b4010
#define MX6_MMDC_P1_MDCFG2	0x021b4014
#define MX6_MMDC_P1_MDMISC	0x021b4018
#define MX6_MMDC_P1_MDSCR	0x021b401c
#define MX6_MMDC_P1_MDREF	0x021b4020
#define MX6_MMDC_P1_MDRWD	0x021b402c
#define MX6_MMDC_P1_MDOR	0x021b4030
#define MX6_MMDC_P1_MDASP	0x021b4040
#define MX6_MMDC_P1_MAPSR	0x021b4404
#define MX6_MMDC_P1_MPZQHWCTRL	0x021b4800
#define MX6_MMDC_P1_MPWLDECTRL0	0x021b480c
#define MX6_MMDC_P1_MPWLDECTRL1	0x021b4810
#define MX6_MMDC_P1_MPODTCTRL	0x021b4818
#define MX6_MMDC_P1_MPRDDQBY0DL	0x021b481c
#define MX6_MMDC_P1_MPRDDQBY1DL	0x021b4820
#define MX6_MMDC_P1_MPRDDQBY2DL	0x021b4824
#define MX6_MMDC_P1_MPRDDQBY3DL	0x021b4828
#define MX6_MMDC_P1_MPDGCTRL0	0x021b483c
#define MX6_MMDC_P1_MPDGCTRL1	0x021b4840
#define MX6_MMDC_P1_MPRDDLCTL	0x021b4848
#define MX6_MMDC_P1_MPWRDLCTL	0x021b4850
#define MX6_MMDC_P1_MPMUR0	0x021b48b8

#endif	/*__ASM_ARCH_MX6_DDR_H__ */
