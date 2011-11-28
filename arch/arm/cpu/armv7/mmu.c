/*
 * (C) Copyright 2008 Texas Insturments
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
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

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <asm/system.h>
#include <asm/cache.h>
#include <asm/armv7.h>

void mmu_create(__u32 mmu_base)
{
	__s32   *mmu_tlb_address = (__s32 *)mmu_base;
	__u32    i;
	__u32   c1format;

	//建立16k的段表，表项大小为1M
	for(i = 0; i < 4 * 1024; i++)
	{
		mmu_tlb_address[i] = ((i << 20))   |
							  (0 << 19)    |
							  (0 << 18)    |
							  (0 << 17)    |
							  (0 << 16)    |
							  (0 << 15)    |
							  (0 << 12)    |
							  (3 << 10)    |
							  (0 <<  9)    |
							  (15 << 5)    |
							  (0  << 4)    |
							  (0  << 3)    |
							  (0  << 2)    |
							  (2  << 0);
	}
	//cache sram
	mmu_tlb_address[0] = (0 << 20)    |         //地址
						 (0 << 19)    |         //安全区域
						 (0 << 18)    |         //1M段表
						 (0 << 17)    |         //not global
						 (0 << 16)    |         //not shared
						 (0 << 15)    |         //
						 (0 << 12)    |         //
						 (3 << 10)    |         //访问权限 特权
						 (0 <<  9)    |         //
						 (15 << 5)    |         //域控制
						 (0  << 4)    |         //
						 (1  << 3)    |         //cache
						 (0  << 2)    |         //无buffer
						 (2  << 0);             //段表
	//cache dram
	for(i = 0; i < (4 * 1024); i++)
	{
		mmu_tlb_address[i + (CONFIG_SYS_SDRAM_BASE>>20)] = (CONFIG_SYS_SDRAM_BASE + (i << 20))  |
									     (0 << 19)    |
										 (0 << 18)    |
										 (0 << 17)    |
										 (0 << 16)    |
										 (0 << 15)    |
										 (0 << 12)    |
										 (3 << 10)    |
									     (0 <<  9)    |
										 (15 << 5)    |
										 (0  << 4)    |
										 (1  << 3)    |
										 (0  << 2)    |
										 (2  << 0);
	}
	//set ttb
	mmu_base |= (1 << 0) | (1 << 1) | (2 << 3);
	asm volatile ("mcr p15, 0, %0, c2, c0, 0" : : "r" (mmu_base));
	asm volatile ("mcr p15, 0, %0, c2, c0, 1" : : "r" (mmu_base));
	//clean i/d cache
	flush_dcache_all();
	flush_icache_all();
	//set domain controller
	mmu_set_domain_access();
	//打开ICACHE，DCACHE, MMU，DISABLE ALIGIN CHECK
	asm volatile ("mrc p15, 0, %0, c1, c0, 0" : : "r" (c1format));
	c1format &= ~ 0x1007;
	c1format |=   0x1005;
	asm volatile ("mcr p15, 0, %0, c1, c0, 0" : : "r" (c1format));
}
