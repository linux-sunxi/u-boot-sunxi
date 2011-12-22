/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON driver power sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : drv_power.c
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
#include  <common.h>
#include  "axp_i.h"

#define DCDC2_VALUE_FOR_BOOT    (1320)      //define dcdc2 to 1.32v for boot


extern  __u32 power_ext_hd;
extern  __u32 PMU_type;
extern  __u32 power_step_level;
extern  __s32 bat_vol;
extern  __u32  dcdc2_user_set;


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
__s32 pmu_set_off(void)
{
    __u8   reg_addr, value;

	if(PMU_type == PMU_TYPE_AXP209)            //AXP20
    {
    	reg_addr = BOOT_POWER20_OFF_CTL;
    	if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
    	{
        	return -1;
    	}
    	value |= 1 << 7;
    	if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
    }

    return 0;
}


__s32 pmu_probe_type(void)
{
	__u8  reg_addr, value;

    if(!BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
    {
    	value &= 0xf;
        if(value == 0x01)
        {
      		return PMU_TYPE_AXP209;
      	}
    }

    return PMU_TYPE_NONE;
}
/*
****************************************************************************************************
*
*             DRV_POWER_INIT
*
*  Description: 初始化函数，通常把只会执行一次的操作放在这里
*               当驱动第一次安装的时候会调用这个函数
*               此函数没有参数，需要参数的时候需要从配置脚本去获取
*  Parameters:
*
*  Return value:
*       0
*       -1
****************************************************************************************************
*/
__s32 pmu_init(int dcdc2_vol)
{
    __u8  reg_addr, value;
    __u32 dcin_exist, bat_exist;
    __u32 bat_vol;
    __s32 ret;

    //try to check if the pmu is AXP199
    reg_addr = 0x03;
    if(!dcdc2_vol)
    {
        dcdc2_vol = DCDC2_VALUE_FOR_BOOT;
    }
    dcdc2_user_set = dcdc2_vol;
	PMU_type = pmu_probe_type();
    //检测电压，决定是否开机
    ret = pmu_probe_dcin_battery_status(&dcin_exist, &bat_exist);
    if(!ret)
    {
	    if(!dcin_exist)
	    {
			pmu_probe_battery_vol(&bat_vol);
			if(bat_vol >= 3500)
			{
				power_step_level = 2;
			}
			else if(bat_vol >= 3400)
			{
				power_step_level = 1;
			}
			else
			{
	    		pmu_set_power_off_vol();
			}
	    }
	    else
	    {
	    	power_step_level = 2;
	    }
	}
    //set dcdc2 value
    pmu_set_dcdc2(dcdc2_vol);
    pmu_set_dcdc3(1250);
	if(PMU_type == PMU_TYPE_AXP209)
    {
		// open bat acin vbus voltage and current adc
		reg_addr = BOOT_POWER20_ADC_EN1;
        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        value |= 0xfe << 0;
        reg_addr = BOOT_POWER20_ADC_EN1;
        if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        //Enable Charge
        reg_addr = BOOT_POWER20_CHARGE1;
        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        value |= 0x80 << 0;
        reg_addr = BOOT_POWER20_CHARGE1;
        if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        //set bus current limit off
        reg_addr = BOOT_POWER20_IPS_SET;
        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        value &= 0xfC;
        value |= 0x03;
        reg_addr = BOOT_POWER20_IPS_SET;
        if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        {
        	__u8 coulomb_val, tmp_val;
	        //hold
	        reg_addr = BOOT_POWER20_COULOMB_CAL;
	        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &coulomb_val))
	        {
	            return -1;
	        }
	        coulomb_val |= 0x80;
	        if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &coulomb_val))
	        {
	            return -1;
	        }
	        //
	        reg_addr = 0xBA;
	        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &tmp_val))
	        {
	            return -1;
	        }
	        tmp_val &= ~0x80;
	        if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &tmp_val))
	        {
	            return -1;
	        }
	        reg_addr = BOOT_POWER20_COULOMB_CAL;
	        coulomb_val &= ~0x80;
	        if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &coulomb_val))
	        {
	            return -1;
	        }
	    }
    }
    else
    {
    	return -1;
    }

    return 0;
}
/*
****************************************************************************************************
*
*             DRV_POWER_EXIT
*
*  Description: 卸载驱动，通常把只会执行一次的操作放在这里
*               当驱动退出的时候会调用这个这个函数
*               卸载函数没有参数
*  Parameters:
*
*  Return value:
*       0
*       -1
****************************************************************************************************
*/
__s32 pmu_exit(void)
{
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
__s32 pmu_probe_battery_vol(__u32 *battery_vol)
{
    __u8 reg_addr, value;

	if(PMU_type == PMU_TYPE_AXP209)
    {
        //pmu is AXP209
        reg_addr = BOOT_POWER20_BAT_AVERVOL_H8;
        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        *battery_vol  = (value << 4);
        reg_addr = BOOT_POWER20_BAT_AVERVOL_L4;
        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        *battery_vol += value;
        *battery_vol *= 1.1;
    }
    else
    {
        return -1;
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
__s32 pmu_set_data_buffer(int value)
{
    __u8   reg_addr;
    __u8   tmp_value;

	reg_addr = 0x0f;
	tmp_value = value & 0xff;
	if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &tmp_value))
    {
        return -1;
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
__s32 pmu_check_startup(void)
{
    __u8   dc_in,  startup_trigger;
    __u8   reg_addr, value;

	if(PMU_type == PMU_TYPE_AXP209)
    {
    	__u8 tmp_value;
    	__s32 ret;

//        reg_addr = 0xf;
//        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
//        {
//            return -1;
//        }
//        if(value == 0x0e)		//表示前一次是在系统状态，下一次应该也进入系统
//        {
//        	return -1;
//        }
//        else if(value == 0x0f)      //表示前一次是在boot standby状态，下一次也应该进入boot standby
//    	{
//    		return 0;
//    	}

        reg_addr = BOOT_POWER20_STATUS;
        if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        startup_trigger  = (value >> 0) & 0x01;

		reg_addr = 0xf;
        if(startup_trigger)    //如果ACIN/VBUS存在且由ACIN/VBUS触发开机，则power_off
        {
	        tmp_value = 0x0f;
	        ret = 0;
	    }
	    else
	    {
	    	tmp_value = 0x0e;
	    	ret = 1;
	    }
	    if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &tmp_value))
        {
            return -1;
        }

        return ret;
    }

    return -1;

ext_power_checked:

	return 0;
}

__s32 pmu_set_power_on_vol(void) //设置开机之后，PMU硬件关机电压为2.9V
{
	__u8  reg_addr;
	__u8  reg_value;
	__s32 vol_value, ret;

	ret = script_parser_fetch("pmu_para", "pmu_pwron_vol", &vol_value, 1);
	if(ret)
	{
		vol_value = 2900;
	}
	reg_addr = BOOT_POWER20_VOFF_SET;
	if(!BOOT_TWI_Read(AXP20_ADDR, reg_addr, &reg_value))
	{
		reg_value &= 0xf8;
		if(vol_value >= 2600 && vol_value <= 3300)
		{
			reg_value |= (vol_value - 2600)/100;
		}
		else if(vol_value <= 2600)
		{
			reg_value |= 0x00;
		}
		else
		{
			reg_value |= 0x07;
		}
		if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &reg_value))
    	{
			;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}

__s32 pmu_set_power_off_vol(void) //设置关机之后，PMU硬件下次开机电压为3.3V
{
	__u8  reg_addr;
	__u8  reg_value;
	__s32 vol_value, ret;

	pmu_config_charge_current(1);
	ret = script_parser_fetch("pmu_para", "pmu_pwroff_vol", &vol_value, 1);
	if(ret)
	{
		vol_value = 3300;
	}
	reg_addr = BOOT_POWER20_VOFF_SET;
	if(!BOOT_TWI_Read(AXP20_ADDR, reg_addr, &reg_value))
	{
		reg_value &= 0xf8;
		if(vol_value >= 2600 && vol_value <= 3300)
		{
			reg_value |= (vol_value - 2600)/100;
		}
		else if(vol_value <= 2600)
		{
			reg_value |= 0x00;
		}
		else
		{
			reg_value |= 0x07;
		}
		if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &reg_value))
    	{
			;
		}
		else
		{
			//设置VBUS不限流
			pmu_vbus_unlimit();
			//设置完成，同时关机
	    	reg_addr = POWER_OFF_CTL;
	    	if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &reg_value))
	    	{
	        	return -1;
	    	}
	    	reg_value |= 1 << 7;
	    	if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &reg_value))
	        {
	            return -1;
	        }

			return 0;
		}
	}

	return -1;
}

__s32 pmu_set_charge_current(int current)
{
	__u8  reg_addr, value;
	int   step;

	if(PMU_type == PMU_TYPE_AXP209)
	{
		reg_addr = BOOT_POWER20_CHARGE1;
		if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
		{
		    return -1;
		}
		value &= ~0x0f;
		if(current > 1800)
		{
			current = 1800;
		}
		else if(current < 300)
		{
			current = 300;
		}
		step   = (current/100) - 3;
		value |= (step & 0x0f);
		if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
	}
	else
	{
		return -1;
	}

	return 0;
}

__s32 pmu_get_charge_current(void)
{
	__u8  reg_addr, value;
	int	  current;

	if(PMU_type == PMU_TYPE_AXP209)
	{
		reg_addr = BOOT_POWER20_CHARGE1;
		if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value))
		{
		    return -1;
		}
		value &= 0x0f;
		current = (value + 3) * 100;
	}
	else
	{
		return -1;
	}

	return current;
}


__s32 pmu_config_charge_current(int start_time)
{
	__s32 ret;
	int   value;
	user_gpio_set_t  gpio;
	__u32  gpio_hd;

	//1 代表关机状态
	//0 代表开机状态
	if(start_time == 1)
	{
		ret = script_parser_fetch("pmu_para", "pmu_used2", &value, 1);
		if((ret >= 0) && (value == 1))
		{
			ret = script_parser_fetch("pmu_para", "pmu_adpdet", (void *)&gpio, sizeof(user_gpio_set_t)/sizeof(int));
			if(ret >= 0)
			{
				gpio_hd = gpio_request((void *)&gpio, 1);
				value = gpio_read_one_pin_value(gpio_hd, 0);
				if(value > 0)
				{
					ret = script_parser_fetch("pmu_para", "pmu_shutdown_chgcur", &value, 1);
				}
				else
				{
					ret = script_parser_fetch("pmu_para", "pmu_shutdown_chgcur2", &value, 1);
				}
				if(ret >= 0)
				{
					pmu_set_charge_current(value);

					return 0;
				}
			}
		}
		else
		{
			ret = script_parser_fetch("pmu_para", "pmu_shutdown_chgcur", &value, 1);
			if(ret >= 0)
			{
				pmu_set_charge_current(value);

				return 0;
			}
		}
	}
	else
	{
		pmu_set_charge_current(300);
	}

	return -1;
}

