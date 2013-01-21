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

DECLARE_GLOBAL_DATA_PTR;


void  reposition_boot_standby( void )
{
//#if 0
//	memcpy((void *)&_standby_start, (void *)&_standby_start_lma, ((__u32)&_standby_end - (__u32)&_standby_start));
//#else
//	int *dst = &_standby_start;
//	int *src = &_standby_end;
//
//	while (dst < &_standby_end)
//	{
//		*dst++ = *src++;
//	}
//#endif
}

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

	if(length)
	{
		memcpy((void *)SYS_CONFIG_MEMBASE, addr, length);
		script_parser_init((char *)SYS_CONFIG_MEMBASE);
	}
	else
	{
		script_parser_init(NULL);
	}

	return 0;
}

int power_init(void)
{
	return axp_init();
}


#define   KEY_DELAY_MAX          (8)
#define   KEY_MAX_COUNT_GO_ON    ((KEY_DELAY_MAX * 1000)/40)


int check_update_key(void)
{
	int ret;
	int fel_key_max, fel_key_min;
	int fel_key_mode = 0;

	ret = script_parser_fetch("fel_key", "fel_key_max", &fel_key_max, 1);
    if(ret)
    {
    	printf("fel key old mode\n");
		fel_key_mode = 1;
	}

	printf("key\n");
	sunxi_key_read();
	__msdelay(10);

	if(!fel_key_mode)
	{
		int key_value;
		int fel_key_max, fel_key_min;


	    key_value = sunxi_key_read();  		//读取按键信息
	    if(key_value < 0)             				//没有按键按下
	    {
	        printf("no key found\n");
	        return -1;
	    }
		ret = script_parser_fetch("fel_key", "fel_key_max", &fel_key_max, 1);
	    if(ret)
	    {
	    	printf("fel key max not found\n");

	    	return 0;
	    }

		ret = script_parser_fetch("fel_key", "fel_key_min", &fel_key_min, 1);
	    if(ret)
	    {
	    	printf("fel key min not found\n");

	    	return 0;
	    }

		if((key_value <= fel_key_max) && (key_value >= fel_key_min))
		{
			printf("fel key detected\n");

			return 1;
		}

		printf("fel key value %d is not in the range from %d to %d\n", key_value, fel_key_min, fel_key_max);

		return 0;
	}
	else
	{
	    int count, time_tick;
	    int value_old, value_new, value_cnt;
	    int new_key, new_key_flag;

	    time_tick = 0;
	    count = 0;
	    value_cnt = 0;
	    new_key = 0;
	    new_key_flag = 0;
	    ret = sunxi_key_read();  				//读取按键信息
	    if(ret < 0)             				//没有按键按下
	    {
	        printf("no key found\n");
	        return 0;
	    }
	    else
	    {
	    	value_old = ret;
	    }

	    while(1)
	    {
	        time_tick ++;
	        ret = axp_probe_key();  			//获取power按键信息
	        if(ret > 0)              	  		//检测到POWER按键按下
	        {
	            count ++;
	        }

	        __msdelay(40);
	        ret = axp_probe_key();  			//读取按键信息
	        if(ret < 0)             			//没有按键按下
	        {
	            printf("key not pressed anymore\n");
	            if(count == 1)
	            {
	            	if(new_key >= 2)
	            	{
	            		printf("1\n");
	            		printf("force to debug mode\n");

	            		return -2;
	            	}
	            }
	        	if(value_cnt >= 2)
	        	{
	        		gd->key_pressd_value = value_old;
				}

				return 0;
	        }
	        else
	        {
	        	value_new = ret;
	        	if(value_old == value_new)
	        	{
	        		value_cnt ++;
	        		if(new_key_flag == 1)
	        		{
	        			new_key ++;
	        			new_key_flag ++;
	        		}
	        		else if(!new_key_flag)
	        		{
	        			new_key_flag ++;
	        		}
	        	}
	        	else
	        	{
	        		new_key_flag = 0;
	        		value_old = value_new;
	        	}
	        }

	        if(count == 3)
	        {
	        	printf("you can unclench the key to update now\n");
	            return -1;
	        }

	        if((!count) && (time_tick >= KEY_MAX_COUNT_GO_ON))
	        {
	        	gd->key_pressd_value = value_old;
	            printf("timeout, but no power key found\n");
	        }

			return 0;
	    }
	}
}



