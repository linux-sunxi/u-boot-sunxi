/*
**********************************************************************************************************************
*											        eGon
*						                     the Embedded System
*									       script parser sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : standby.c
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include <common.h>
#include "standby_i.h"
#include "standby.h"

DECLARE_GLOBAL_DATA_PTR;

static int boot_enter_standby(void);
static int boot_exit_standby(void);
static int boot_standby_detect(void);
static int boot_mod_enter_standby(void);
static int boot_mod_exit_standby(void);
static int boot_early_standby_mode(void);

static int standby_flag = 0;

int boot_standby_mode(void)
{
	int status;

	//axp_set_suspend_chgcur();

	boot_store_sp();
	boot_set_sp();

	status = boot_early_standby_mode();

	boot_restore_sp();

	return status;
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
int boot_early_standby_mode(void)
{
	__s32 key_status, usb_status;
	__s32 status;

	//检查是否有按键按下
	key_status = standby_axp_probe_key();
	if(key_status & 0x01)			//长按键的情况下，不管电源是否移除，直接进入系统
	{
		if(standby_flag)
		{
			boot_mod_exit_standby();
		}
		return 3;
	}
	//检查外部电源是否存在
	if(standby_axp_probe_power_exist() <= 0)
	{
		if(standby_flag)
		{
			boot_mod_exit_standby();
		}
		return 4;
	}
	if(key_status & 0x02)			//短按电压按键的情况下，显示充电动画
	{
		if(standby_flag)
		{
			boot_mod_exit_standby();
		}
		return 2;
	}
	if(!standby_flag)
	{
		boot_mod_enter_standby();      //控制模块进入standby
	}
	//检查是否有USB电源插入
	usb_status = standby_axp_probe_usb();
	if(usb_status > 0)
	{
		return 8;
	}
	status = -1;
	boot_enter_standby();
	//while((*(volatile unsigned int *)(0)) != 0x1234);
	do
	{
		//开始循环检查是否开始唤醒
		boot_halt();
		status = boot_standby_detect();
	}
	while(status <= 0);
	//发现需要唤醒，退出standby
	boot_exit_standby();
	//退出模块的standby
	if(status != 8)
	{
		boot_mod_exit_standby();
		standby_flag = 0;
	}
	else
	{
		standby_flag = 1;
	}

	return status;
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
static int boot_enter_standby(void)
{
	//处理中断
	standby_int_disable();

	//mctl_deep_sleep_test();
	dram_power_save_process();

	//standby_serial_putc('1');

	standby_gic_store();
	//standby_serial_putc('m');
	standby_axp_store_int_status();
	//standby_serial_putc('n');
	standby_clock_to_24M();
	//standby_serial_putc('o');
	standby_clock_plldisable();
	//standby_serial_putc('p');
	standby_axp_output_control(0);

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
static int boot_exit_standby(void)
{
	standby_axp_output_control(1);
	standby_clock_pllenable();
	standby_clock_to_pll1();
	//standby_serial_putc('7');
	dram_power_up_process();
	//standby_serial_putc('8');
	standby_axp_restore_int_status();
	standby_gic_restore();
	standby_int_enable();
	//standby_serial_putc('9');

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
static int boot_standby_detect(void)
{
	__u8  power_int_status[8];

	//检查中断触发
	standby_axp_int_query(power_int_status);
	//standby_serial_putc('1');
	if(power_int_status[2] & 0x02)			//电源按键短按
	{
		return 2;
	}
	if(power_int_status[2] & 0x01)			//电源按键长按
	{
		return 3;
	}
	if(power_int_status[0] & 0x24)			//外部电源移除
	{
		if(standby_axp_probe_power_exist() <= 0)	//没有外部电源存在
		{
			return 4;
		}
		else
		{
			return 8;						//还有外部电源存在
		}
	}
	if(power_int_status[0] & 0x48)			//外部电源插入
	{
		return 8;
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
extern int DRV_DISP_Standby(uint cmd, void *pArg);

typedef int (* boot_mod_standby)(uint cmd, void *pArg);

static int boot_mod_enter_standby(void)
{
	uint addr;
	boot_mod_standby  mod_func;

	addr = (uint)DRV_DISP_Standby + gd->reloc_off;
	mod_func = (boot_mod_standby *)addr;

	mod_func(BOOT_MOD_ENTER_STANDBY, 0);

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
static int boot_mod_exit_standby(void)
{
	uint addr;
	boot_mod_standby  mod_func;

	addr = (uint)DRV_DISP_Standby + gd->reloc_off;
	mod_func = (boot_mod_standby *)addr;

	mod_func(BOOT_MOD_EXIT_STANDBY, 0);

	return 0;
}




