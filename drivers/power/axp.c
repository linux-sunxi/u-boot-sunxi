/*
 * (C) Copyright 2012
 *     wangflord@allwinnerstech.com
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
#include <asm/arch/power.h>
#include <asm/arch/axp221.h>
#include "axp.h"
#include "axp221_i.h"

DECLARE_GLOBAL_DATA_PTR;

extern int axp_probe_power_supply_condition(void);
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
int axp_probe(void)
{
	u8    pmu_type;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_VERSION, &pmu_type))
	{
		printf("axp read error\n");

		return -1;
	}
	pmu_type &= 0x0f;
	if(pmu_type & 0x06)
	{
		/* pmu type AXP221 */
		tick_printf("PMU: AXP221\n");

		return 0;
	}

	tick_printf("PMU: NULL\n");

	return -1;
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
int axp_get_power_vol_level(void)
{
	return gd->power_step_level;
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
int axp_probe_startup_cause(void)
{
	int ret = -1;
	int buffer_value;
	int status;
	int poweron_reason, next_action = 0;

	buffer_value = axp221_probe_last_poweron_status();
	debug("axp buffer %x\n", buffer_value);
	if(buffer_value < 0)
	{
		return -1;
	}
    if(buffer_value == 0x0e)		//表示前一次是在系统状态，下一次应该也进入系统
    {
    	tick_printf("pre sys mode\n");
    	return -1;
    }
    else if(buffer_value == 0x0f)      //表示前一次是在boot standby状态，下一次也应该进入boot standby
	{
		tick_printf("pre boot mode\n");
		status = axp221_probe_power_status();
    	if(status <= 0)
    	{
    		return 0;
    	}
    	else if(status == 2)	//only vbus exist
    	{
    		return 2;
    	}
    	else if(status == 3)	//dc exist(dont care wether vbus exist)
    	{
    		return 3;
    	}
		return 0;
	}
	//获取 开机原因，是按键触发，或者插入电压触发
	poweron_reason = axp221_probe_poweron_cause();
	if(poweron_reason == AXP_POWER_ON_BY_POWER_KEY)
	{
		tick_printf("key trigger\n");
		next_action = 0x0e;
		ret = 0;
	}
	else if(poweron_reason == AXP_POWER_ON_BY_POWER_TRIGGER)
	{
		tick_printf("power trigger\n");
		next_action = 0x0f;
    	ret = 1;
	}
	//把开机原因写入寄存器
	axp221_set_next_poweron_status(next_action);

    return ret;
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
int axp_set_hardware_poweron_vol(void) //设置开机之后，PMU硬件关机电压为2.9V
{
	int vol_value;

	if(script_parser_fetch("pmu_para", "pmu_pwron_vol", &vol_value, 1))
	{
		printf("boot power:unable to find power off vol set\n");
		vol_value = 2900;
	}

	return axp221_set_poweroff_vol(vol_value);
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
int axp_set_hardware_poweroff_vol(void) //设置关机之后，PMU硬件下次开机电压为3.3V
{
	int vol_value;

	if(script_parser_fetch("pmu_para", "pmu_pwroff_vol", &vol_value, 1))
	{
		printf("boot power:unable to find power off vol set\n");
		vol_value = 3300;
	}

	return axp221_set_poweroff_vol(vol_value);
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
int  axp_set_power_off(void)
{
	return axp221_set_power_off();
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
int axp_set_next_poweron_status(int value)
{
	return axp221_set_next_poweron_status(value);
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
int  axp_power_get_dcin_battery_exist(int *dcin_exist, int *battery_exist)
{
	*dcin_exist = axp221_probe_dcin_exist();
	*battery_exist = axp221_probe_battery_exist();

	return 0;
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
int  axp_probe_battery_vol(void)
{
	return axp221_probe_battery_vol();
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
int  axp_probe_rest_battery_capacity(void)
{
	return axp221_probe_battery_ratio();
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
int  axp_probe_key(void)
{
	return axp221_probe_key();
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
int  axp_set_dc1sw(int on_off)
{
	return axp221_set_dc1sw(on_off);
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
int  axp_set_dcdc1(int set_vol)
{
	return axp221_set_dcdc1(set_vol);
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
int  axp_set_dcdc2(int set_vol)
{
	return axp221_set_dcdc2(set_vol);
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
int  axp_set_dcdc3(int set_vol)
{
	return axp221_set_dcdc3(set_vol);
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
int  axp_set_dcdc4(int set_vol)
{
	return axp221_set_dcdc4(set_vol);
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
int  axp_set_dcdc5(int set_vol)
{
	return axp221_set_dcdc5(set_vol);
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
int  axp_set_eldo3(int on_off)
{
	return axp221_set_eldo3(on_off);
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
int  axp_set_dldo3(int on_off)
{
	return axp221_set_dldo3(on_off);
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
int  axp_probe_charge_current(void)
{
	return axp221_probe_charge_current();
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
int  axp_set_charge_current(int current)
{
	return axp221_set_charge_current(current);
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
int  axp_set_charge_control(void)
{
	return axp221_set_charge_control();
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
int axp_set_vbus_limit_dc(void)
{
	axp221_set_vbus_vol_limit(gd->limit_vol);
	axp221_set_vbus_cur_limit(gd->limit_cur);

	return 0;
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
int axp_set_vbus_limit_pc(void)
{
	axp221_set_vbus_vol_limit(gd->limit_pcvol);
	axp221_set_vbus_cur_limit(gd->limit_pccur);

	return 0;
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
int axp_set_all_limit(void)
{
	int usbvol_limit = 0;
	int usbcur_limit = 0;

	script_parser_fetch("pmu_para", "pmu_usbvol_limit", &usbvol_limit, 1);
	script_parser_fetch("pmu_para", "pmu_usbcur_limit", &usbcur_limit, 1);
	script_parser_fetch("pmu_para", "pmu_usbvol", (int *)&gd->limit_vol, 1);
	script_parser_fetch("pmu_para", "pmu_usbcur", (int *)&gd->limit_cur, 1);
	script_parser_fetch("pmu_para", "pmu_usbvol_pc", (int *)&gd->limit_pcvol, 1);
	script_parser_fetch("pmu_para", "pmu_usbcur_pc", (int *)&gd->limit_pccur, 1);
#ifdef DEBUG
	printf("usbvol_limit = %d, limit_vol = %d\n", usbvol_limit, gd->limit_vol);
	printf("usbcur_limit = %d, limit_cur = %d\n", usbcur_limit, gd->limit_cur);
#endif
	if(!usbvol_limit)
	{
		gd->limit_vol = 0;

	}
	if(!usbcur_limit)
	{
		gd->limit_cur = 0;
	}

	axp_set_vbus_limit_pc();

	return 0;
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
int axp_set_suspend_chgcur(void)
{
	return axp221_set_chgcur(gd->pmu_suspend_chgcur);
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
int axp_set_runtime_chgcur(void)
{
	return axp221_set_chgcur(gd->pmu_runtime_chgcur);
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
int axp_set_charge_vol_limit(void)
{
	int ret1;
	int ret2;

	ret1 = script_parser_fetch("pmu_para", "pmu_runtime_chgcur", (int *)&gd->pmu_runtime_chgcur, 1);
	ret2 = script_parser_fetch("pmu_para", "pmu_suspend_chgcur", (int *)&gd->pmu_suspend_chgcur, 1);

	if(ret1)
	{
		gd->pmu_runtime_chgcur = 600;
	}
	if(ret2)
	{
		gd->pmu_suspend_chgcur = 1500;
	}
#if DEBUG
	printf("pmu_runtime_chgcur=%d\n", gd->pmu_runtime_chgcur);
	printf("pmu_suspend_chgcur=%d\n", gd->pmu_suspend_chgcur);
#endif
	axp_set_suspend_chgcur();

	return 0;
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
int axp_set_power_supply_output(void)
{
	int vol_value;
	int cpus_sel;

	//set dcdc1
	if(!script_parser_fetch("power_sply", "dcdc1_vol", &vol_value, 1))
	{
		if(!axp_set_dcdc1(vol_value))
		{
			printf("dcdc1 %d\n", vol_value);
		}
	}
	//set dcdc2
	if(!script_parser_fetch("power_sply", "dcdc2_vol", &vol_value, 1))
	{
		if(!axp_set_dcdc2(vol_value))
		{
			printf("dcdc2 %d\n", vol_value);
		}
	}
#ifdef DEBUG
	else
	{
		printf("boot power:unable to find dcdc2 set\n");
	}
#endif
	//set dcdc4
	if(!script_parser_fetch("power_sply", "dcdc4_vol", &vol_value, 1))
	{
		if(!axp_set_dcdc4(vol_value))
		{
			printf("dcdc4 %d\n", vol_value);
		}
	}
#ifdef DEBUG
	else
	{
		printf("boot power:unable to find dcdc4 set\n");
	}
#endif
    //set dcdc5
	if(!script_parser_fetch("power_sply", "dcdc5_vol", &vol_value, 1))
	{
		if(!axp_set_dcdc5(vol_value))
		{
			printf("dcdc5 %d\n", vol_value);
		}
	}
#ifdef DEBUG
	else
	{
		printf("boot power:unable to find dcdc5 set\n");
	}
#endif
#if 0
	//set ldo2
	if(!script_parser_fetch("power_sply", "ldo2_vol", &vol_value, 1))
	{
		if(!axp_set_ldo2(vol_value))
		{
			printf("boot power:set ldo2 to %d ok\n", vol_value);
		}
	}
#ifdef DEBUG
	else
	{
		printf("boot power:unable to find ldo2 set\n");
	}
#endif
	//set ldo3
	if(!script_parser_fetch("power_sply", "ldo3_vol", &vol_value, 1))
	{
		if(!axp_set_ldo3(vol_value))
		{
			printf("boot power:set ldo3 to %d ok\n", vol_value);
		}
	}
#ifdef DEBUG
	else
	{
		printf("boot power:unable to find ldo3 set\n");
	}
#endif
#endif
	//set ldo4
//	if(!script_parser_fetch("power_sply", "ldo4_vol", &vol_value, 1))
//	{
//		axp_set_ldo4(vol_value);
//	}
//	else
//	{
//		printf("boot power:unable to find ldo4 set\n");
//	}
	if(!script_parser_fetch("power_sply", "cpus_source", &cpus_sel, 1))
	{
		if(cpus_sel)
		{
			if(!script_parser_fetch("power_sply", "gpio1ldo_vol", &vol_value, 1))
			{
				if(!axp221_set_gpio1ldo(cpus_sel, vol_value))
				{
					printf("boot power:set gpio1ldo to %d ok\n", vol_value);
				}
			}
		}
	}

	return 0;
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
int axp_probe_power_supply_condition(void)
{
	int   dcin_exist, bat_vol;
	//int   buffer_value;
	//char  bat_value, bat_cou;
	int   ratio;

	//检测电压，决定是否开机
    dcin_exist = axp221_probe_dcin_exist();
#ifdef DEBUG
    printf("dcin_exist = %x\n", dcin_exist);
#endif
    //先判断条件，如果上次关机记录的电量百分比<=5%,同时库仑计值小于5mAh，则关机，否则继续判断
    if(!dcin_exist)
    {
    	bat_vol = axp221_probe_battery_vol();
		tick_printf("PMU: bat vol = %d\n", bat_vol);
		if(bat_vol < 3400)
		{
			printf("bat vol is lower than 3400 and dcin is not exist\n");
			printf("we have to close it\n");
		    axp_set_hardware_poweroff_vol();
			axp221_set_power_off();
			for(;;);
		}
    }

	ratio = axp221_probe_battery_ratio();
	tick_printf("PMU: bat ratio = %d\n", ratio);
	if(ratio < 0)
	{
		return -1;
	}
	else if(ratio < 1)
	{
		if(dcin_exist)
		{
			//外部电源存在，电池电量极低
			gd->power_step_level = BATTERY_RATIO_TOO_LOW_WITH_DCIN;
		}
		else
		{
			//外部电源不存在，电池电量极低
			gd->power_step_level = BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN;
		}
	}
	else
	{
		gd->power_step_level = BATTERY_RATIO_ENOUGH;
	}

	return 0;
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
int axp_init(void)
{
	int ret = -1;
	int dcdc3_vol;

	ret = script_parser_fetch("power_sply", "dcdc3_vol", &dcdc3_vol, 1);
	if(ret)
	{
		dcdc3_vol = 1200;
	}
#ifdef DEBUG
	printf("set dcdc3 to %d\n", dcdc3_vol);
#endif
	if(!axp_probe())
	{
		if(!axp_probe_power_supply_condition())
		{
			if(!axp_set_dcdc3(dcdc3_vol))
			{
				tick_printf("PMU: dcdc3 %d\n", dcdc3_vol);
				ret = 0;
			}
			else
			{
				printf("axp_set_dcdc3 fail\n");
			}
		}
		else
		{
			printf("axp_probe_power_supply_condition error\n");
		}
	}
	else
	{
		printf("axp_probe error\n");
	}

	return ret;
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
static __u8 power_int_value[8];

int axp_int_enable(__u8 *value)
{
	axp221_int_enable_read(power_int_value);
	axp221_int_enable_write(value);
	//打开小cpu的中断使能
	*(volatile unsigned int *)(0x01f00c00 + 0x10) |= 1;
	*(volatile unsigned int *)(0x01f00c00 + 0x40) |= 1;

	return 0;
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
int axp_int_disable(void)
{
	*(volatile unsigned int *)(0x01f00c00 + 0x10) |= 1;
	*(volatile unsigned int *)(0x01f00c00 + 0x40) &= ~1;

	return axp221_int_enable_write(power_int_value);
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
int axp_int_query(__u8 *addr)
{
	int ret;

	ret = axp221_int_query(addr);
	*(volatile unsigned int *)(0x01f00c00 + 0x10) |= 1;

	return ret;
}
