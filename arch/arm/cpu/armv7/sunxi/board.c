/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some init for sunxi platform.
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

#include <common.h>
#include <asm/io.h>
#include <pmu.h>
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>
#include <asm/arch/gpio.h>
#include <asm/arch/key.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/boot_type.h>
#include <asm/arch/sys_partition.h>
/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
int uart_console = 0;

u32 get_base(void) 
{

	u32 val;

	__asm__ __volatile__("mov %0, pc \n":"=r"(val)::"memory");
	val &= 0xF0000000;
	val >>= 28;
	return val;
}

/* do some early init */
void s_init(void)
{
	watchdog_disable();
}

void reset_cpu(ulong addr) 
{
	watchdog_enable();
loop_to_die:
	goto loop_to_die;
}

void v7_outer_cache_enable(void)
{
	return ;
}

void v7_outer_cache_inval_all(void)
{
	return ;
}

void v7_outer_cache_flush_range(u32 start, u32 stop)
{
	return ;
}

void enable_caches(void)
{
    icache_enable();
    dcache_enable();
}

void disable_caches(void)
{
    icache_disable();
	dcache_disable();
}

int display_inner(void)
{
	printf("uboot inner version : %s\n", uboot_spare_head.boot_head.version);

	return 0;
}

int script_init(void)
{
    uint offset, length;
	char *addr;

	offset = uboot_spare_head.boot_head.uboot_length;
	length = uboot_spare_head.boot_head.length - uboot_spare_head.boot_head.uboot_length;
	addr   = (char *)CONFIG_SYS_TEXT_BASE + offset;

    debug("script offset=%x, length = %x\n", offset, length);

	memcpy((void *)SYS_CONFIG_MEMBASE, addr, length);
	script_parser_init((char *)SYS_CONFIG_MEMBASE);

	return 0;
}

int power_init(void)
{
	int clock;
	int set_vol;
	int set_clock;

	set_vol = uboot_spare_head.boot_data.run_core_vol;
	set_clock = uboot_spare_head.boot_data.run_clock;

	if(!axp_probe())
	{
		if(!axp_probe_power_supply_condition())
		{
			if(!axp_set_dcdc2(set_vol))
			{
				debug("axp_set_dcdc2 ok\n");
				clock = sunxi_clock_set_corepll(set_clock, set_vol);
			}
			else
			{
				debug("axp_set_dcdc2 fail\n");
				clock = sunxi_clock_get_corepll();
			}
			printf("set core vol = %d, core clock = %d\n", set_vol, clock);	

			return 0;
		}
		else
		{
			debug("axp_probe_power_supply_condition error\n");
		}
	}
	else
	{
		debug("axp_probe error\n");
	}
	
	puts("set both dcdc2 and clock as default\n");
	
	return 0;
}

int check_update_key(void)
{
	int ret, count;

    sunxi_key_init();
	count = 0;
	__msdelay(10);
	
	ret = sunxi_key_read();			        //读取按键信息
	if(ret < 0) 							//没有按键按下
	{
		printf("no key found\n");
		return 0;
	}

	while(1)
	{
		ret = axp_probe_key();	            //获取power按键信息
		if(ret & PMU_SHORT_KEY_PRESSED) 	//没有POWER按键按下
		{
			count ++;
		}

		__msdelay(40);
		ret = sunxi_key_read();		        //读取按键信息
		if(ret < 0) 						//没有按键按下
		{
			printf("key not pressed anymore\n");
			{
				return 0;
			}
		}

		if(count == 3)
		{
			printf("you can unclench the key to update now\n");

			early_fel();
		}
	}
}



