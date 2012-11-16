/*
 * include/mach/memory.h
 * (C) Copyright 2010-2015
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Benn Huang <benn@allwinnertech.com>
 *
 * core header file for Lichee Linux BSP
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#ifndef __SW_MEMORY_H
#define __SW_MEMORY_H

/* physical offset of RAM */
#define PLAT_PHYS_OFFSET		(0x40000000)

#define SW_VE_MEM_BASE                    (PLAT_PHYS_OFFSET + SZ_64M)
#define SW_VE_MEM_SIZE                    (SZ_64M)
#define SW_FB_MEM_BASE                    (SW_VE_MEM_BASE + SW_VE_MEM_SIZE)
#define SW_FB_MEM_SIZE                    (SZ_16M)
#define SW_GPU_MEM_BASE                   (SW_FB_MEM_BASE + SW_FB_MEM_SIZE)
#define SW_GPU_MEM_SIZE                   (SZ_64M)
#define SW_BANK1_SIZE                     (SZ_64M + SW_VE_MEM_SIZE + SW_FB_MEM_SIZE)
#define SW_BANK2_OFFSET                   (SW_BANK1_SIZE + SW_GPU_MEM_SIZE)

/*
#error "fixme"
#define SW_VE_MEM_BASE                    (PLAT_PHYS_OFFSET + SZ_64M)
#define SW_VE_MEM_SIZE                    (SZ_64M + SZ_16M)
#define SW_FB_MEM_BASE                    (SW_VE_MEM + SW_MEM_VE_SIZE)
#define SW_FB_MEM_SIZE                    (SZ_32M)
#define SW_GPU_MEM_BASE                   (SW_FB_MEM_BASE + SW_FB_MEM_SIZE)
#define SW_GPU_MEM_SIZE                   (SZ_64M)
#define SW_G2D_MEM_BASE
#define SW_G2D_MEM_SIZE                   0x1000000
*/


#endif

