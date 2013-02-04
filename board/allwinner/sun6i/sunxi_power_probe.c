/*
 * (C) Copyright 2012
 *     wangflord@allwinnertech.com
 *
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
 * along with this program;
 *
 */

#include <common.h>
#include <malloc.h>
#include <asm/arch/intc.h>
#include <pmu.h>
#include "power_probe.h"


DECLARE_GLOBAL_DATA_PTR;

extern int boot_standby_action;
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static void power_int_irq(void *p_arg)
{
#ifdef DEBUG
	int i;
#endif
	unsigned char power_int_status[8];
	int  dc_exist, bat_exist;

	axp_int_query(power_int_status);
#ifdef DEBUG
	for(i=0;i<5;i++)
	{
		printf("int status %d %x\n", i, power_int_status[i]);
	}
#endif
	if(power_int_status[0] & 0x48)   //外部电源插入
	{
		printf("power insert\n");
		boot_standby_action &= ~0x10;
		boot_standby_action |= 0x08;
		usb_detect_enter();
	}
	if(power_int_status[0] & 0x24)	//dc remove
	{
		axp_power_get_dcin_battery_exist(&dc_exist, &bat_exist);
		if(!dc_exist)
		{
			printf("power remove\n");
			boot_standby_action |= 0x10;
			boot_standby_action &= ~0x08;
			usb_detect_exit();
		}
		else	//拔掉其中一个外部电源，还有其它外部电源存在
		{
			boot_standby_action |= 0x08;
			boot_standby_action &= ~0x10;
			usb_detect_enter();
		}
	}
	if(power_int_status[2] & 0x2)	//短按键
	{
		printf("short key\n");
		boot_standby_action |= 2;

	}
	if(power_int_status[2] & 0x1)	//长按键
	{
		printf("long key\n");
		boot_standby_action |= 1;
	}

	return;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void power_limit_detect_enter(void)
{
	unsigned char power_int_enable[8];

	power_int_enable[0] = 0x6C;  //dc in/out, usb in/out
	power_int_enable[1] = 0;
	power_int_enable[2] = 3;
	power_int_enable[4] = 0;
	power_int_enable[5] = 0;

	printf("power_limit_detect_enter\n");

	axp_int_enable(power_int_enable);
	irq_install_handler(AW_IRQ_NMI, power_int_irq, 0);
	irq_enable(AW_IRQ_NMI);
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void power_limit_detect_exit(void)
{
	usb_detect_exit();
	axp_int_disable();
	irq_disable(AW_IRQ_NMI);
}