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
#include "common.h"
#include "axp-power.h"

extern  __u32 PMU_type;

extern  __u32 trans_dcdc2_user_set;

static  __u32 power_step_level;
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

static __s32 _axp_clr_status(void)
{
	__u8  reg_addr1,value1;

	reg_addr1 = BOOT_POWER20_DATA_BUFFER0;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER1;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER2;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER3;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER4;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER5;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER6;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER7;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER8;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER9;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER10;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	reg_addr1 = BOOT_POWER20_DATA_BUFFER11;
	value1 = 0x00;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1))
	{
		return -1;
	}

	return ;
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
static __s32 _axp_power_off(void)
{
    __u8   reg_addr, value;

	if(PMU_type == PMU_TYPE_AXP209)            //AXP20
    {
    	reg_addr = BOOT_POWER20_OFF_CTL;
    	if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
    	{
        	return -1;
    	}
    	value |= 1 << 7;
    	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
    }

    return 0;
}


int axp_power_init(void)
{
	uchar buffer;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_VERSION, &buffer))
	{
		printf("axp read error\n");
		return -1;
	}
	if(buffer == 1)
	{
		pmu_type = PMU_TYPE_AXP209;
	}
	else
}

__s32 eGon2_power_init(void *power_para)
{
    __u8  reg_addr, value;
    __u8  reg_addr1,value1;
    boot_core_para_t   *core_para = (boot_core_para_t *)power_para;
    __u32 dcin_exist, bat_exist;
    __u32 bat_vol;

    //try to check if the pmu is AXP199
    reg_addr = 0x03;
    if(!core_para->user_set_core_vol)
    {
        core_para->user_set_core_vol = DCDC2_VALUE_FOR_BOOT;
    }
    trans_dcdc2_user_set = core_para->user_set_core_vol;
    if(!BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
    {
    	value &= 0xf;
        if(value == 0x01)
        {
      		PMU_type     = PMU_TYPE_AXP209;
      	}
      	else if((!value) || (value == 0x03))
      	{
        	PMU_type     = PMU_TYPE_AXP199;
       	}
       	else
       	{
       		PMU_type = PMU_TYPE_NONE;

       		return -1;
       	}
    }
    else
    {
        //value is invalid
        PMU_type = PMU_TYPE_NONE;

        return -1;
    }
    eGon2_printf("pmu type = %d\n", PMU_type);
    eGon2_power_data_trans();
    //为兼容新旧版本IC，取消PMU硬件检测RDC功能
    if(PMU_type == PMU_TYPE_AXP209)
    {
    	reg_addr1 = BOOT_POWER20_COULOMB_CAL;
    	BOOT_TWI_Read(AXP20_ADDR, &reg_addr1, &value1);
    	value1 |= 0x80;//open lock
    	BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1);
    	value1 = 0xBA;
    	BOOT_TWI_Read(AXP20_ADDR, &reg_addr1, &value1);
    	value1 &= 0x7F;
    	BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1);
			reg_addr1 = BOOT_POWER20_COULOMB_CAL;
    	BOOT_TWI_Read(AXP20_ADDR, &reg_addr1, &value1);
    	value1 &= 0x7F;//lock
    	BOOT_TWI_Write(AXP20_ADDR, &reg_addr1, &value1);
    }

    //Evan add 20120610
	reg_addr1 = 0xB8;
	if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr1, &value1))
	{
    	return -1;
	}
	if(!(value1 & 0x80))
	{
		_axp_clr_status();
	}

    //使能库仑计
	eGon2_power_enable_coulomb();
    //检测电压，决定是否开机
    eGon2_power_get_dcin_battery_exist(&dcin_exist, &bat_exist);
	//先判断条件，如果上次关机记录的电量百分比<=5%,同时库仑计值小于5mAh，则关机，否则继续判断
	eGon2_power_get_battery_vol(&bat_vol);
	eGon2_printf("bat vol = %d\n", bat_vol);
	if((bat_vol < 3400) && (!dcin_exist))
	{
		eGon2_set_power_off_vol();
	}
    reg_addr1 = BOOT_POWER20_DATA_BUFFER1;										//读之前的比分比记录
    BOOT_TWI_Read(AXP20_ADDR, &reg_addr1, &value1);
	if(!core_para->vol_threshold)
	{
		core_para->vol_threshold = 3600;
	}
	if(value1 & 0x80)																	//检测标志位
	{
		int bat_cou;
		int bat_value;

		bat_value = value1 & 0x7f;
		bat_cou = Get_Bat_Coulomb_Count();
		if((bat_value <= 0) && bat_cou < 30)     //比例小于5%同时库仑计值小于6，则不开机
		{
			eGon2_printf("bat_cou=%x\n", bat_cou);
			if(dcin_exist)
			{
				if(bat_vol > (core_para->vol_threshold+100))
				{
					_axp_clr_status();
					power_step_level = 2;
					eGon2_printf("dcin_exist\n");
				}
				else
				{
					power_step_level = 3;
				}
			}
			else
			{
				if(bat_vol > core_para->vol_threshold)
				{
					power_step_level = 2;
					_axp_clr_status();
					eGon2_printf("bat_vol > core_para->vol_threshold\n");
				}
				else
				{
					power_step_level = 1;
				}
			}
			eGon2_printf("power_step_level =%x, bat_vol = %x\n", power_step_level,bat_vol);
		}
		else															//其中一个条件不满足，就可以开机
		{
			power_step_level = 2;
		}
	}
	else
	{
		if(dcin_exist)
		{
			power_step_level = 2;
		}
		else
		{
			if(bat_vol >= core_para->vol_threshold)
			{
				power_step_level = 2;
			}
			else
			{
				power_step_level = 1;
			}
		}
	}

    //set dcdc2 value
    eGon2_power_set_dcdc2(core_para->user_set_core_vol);
    eGon2_power_set_dcdc3(1250);
    if(PMU_type == PMU_TYPE_AXP209)
    {
		// open bat acin vbus voltage and current adc
		reg_addr = BOOT_POWER20_ADC_EN1;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        value |= 0xfe << 0;
        reg_addr = BOOT_POWER20_ADC_EN1;
        if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        //Enable Charge
        reg_addr = BOOT_POWER20_CHARGE1;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        value |= 0x80 << 0;
        reg_addr = BOOT_POWER20_CHARGE1;
        if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        //set bus current limit off
        reg_addr = BOOT_POWER20_IPS_SET;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        value &= 0xfC;
        value |= 0x03;
        reg_addr = BOOT_POWER20_IPS_SET;
        if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        {
        	__u8 coulomb_val, tmp_val;
	        //hold
	        reg_addr = BOOT_POWER20_COULOMB_CAL;
	        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &coulomb_val))
	        {
	            return -1;
	        }
	        coulomb_val |= 0x80;
	        if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &coulomb_val))
	        {
	            return -1;
	        }
	        //
	        reg_addr = 0xBA;
	        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &tmp_val))
	        {
	            return -1;
	        }
	        tmp_val &= ~0x80;
	        if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &tmp_val))
	        {
	            return -1;
	        }
	        reg_addr = BOOT_POWER20_COULOMB_CAL;
	        coulomb_val &= ~0x80;
	        if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &coulomb_val))
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
__s32 eGon2_power_exit(void)
{
    return 0;
}
__s32 eGon2_power_enable_coulomb(void)
{
    __u8   reg_addr;
    __u8   tmp_value;

	reg_addr = 0xB8;
	tmp_value = 0x80;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &tmp_value))
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
__s32 eGon2_power_get_battery_vol(__u32 *battery_vol)
{
    __u8 reg_addr, value;

    if(PMU_type == PMU_TYPE_AXP209)
    {
        //pmu is AXP209
        reg_addr = BOOT_POWER20_BAT_AVERVOL_H8;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        *battery_vol  = (value << 4);
        reg_addr = BOOT_POWER20_BAT_AVERVOL_L4;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
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
__s32 eGon2_power_set_off(void)
{
    return _axp_power_off();
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
__s32 eGon2_power_set_data_buffer(int value)
{
    __u8   reg_addr;
    __u8   tmp_value;

	reg_addr = 0x0f;
	tmp_value = value & 0xff;
	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &tmp_value))
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
__s32 eGon2_power_check_startup(void)
{
    __u8   dc_in,  startup_trigger;
    __u8   reg_addr, value;

    if(PMU_type == PMU_TYPE_AXP209)
    {
    	__u8 tmp_value;
    	__s32 ret;

        reg_addr = BOOT_POWER20_DATA_BUFFER11;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        if(value == 0x0e)		//表示前一次是在系统状态，下一次应该也进入系统
        {
        	eGon2_printf("pre sys mode\n");
        	return -1;
        }
        else if(value == 0x0f)      //表示前一次是在boot standby状态，下一次也应该进入boot standby
    	{
    		eGon2_printf("pre boot mode\n");
    		return 0;
    	}

        reg_addr = BOOT_POWER20_STATUS;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        //dc_in = ((value >> 4) & 0x01) |  ((value >> 6) & 0x01);
        startup_trigger  = (value >> 0) & 0x01;

		reg_addr = 0xf;
        if(startup_trigger)    //如果ACIN/VBUS存在且由ACIN/VBUS触发开机，则power_off
        {
        	eGon2_printf("power trigger\n");
	        tmp_value = 0x0f;
	        ret = 0;
	    }
	    else
	    {
	    	tmp_value = 0x0e;
	    	ret = 1;
	    }
	    if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &tmp_value))
        {
            return -1;
        }

        return ret;
    }

    return -1;

ext_power_checked:

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
__s32 eGon2_power_set_sw1(__u32 on_off)
{
    __u8 reg_addr, value;

    if(PMU_type == PMU_TYPE_AXP209)
    {
    	;
    }
    else
    {
        return -1;
    }

    return 0;
}

__s32 eGon2_power_set_vol(void)
{
	eGon2_power_set_dcdc3(-1);
	eGon2_power_set_ldo2(-1);
	eGon2_power_set_ldo3(-1);
	eGon2_power_set_ldo4(-1);

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
__s32 eGon2_power_type(void)
{
	__u8  reg_addr;
	__u8  value;
	int   type;

	reg_addr = BOOT_POWER20_STATUS;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
    {
        return -1;
    }
    type = (value >>5) & 0x01;
	type |= ((value >>7) & 0x01) << 1;

	return type;
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
__s32 eGon2_set_power_on_vol(void) //设置开机之后，PMU硬件关机电压为2.9V
{
	__u8  reg_addr;
	__u8  reg_value;
	__s32 vol_value, ret;

	ret = eGon2_script_parser_fetch("pmu_para", "pmu_pwron_vol", &vol_value, 1);
	if(ret)
	{
		eGon2_printf("boot power:unable to find power off vol set\n");
		vol_value = 2900;
	}
	reg_addr = BOOT_POWER20_VOFF_SET;
	if(!BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &reg_value))
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
		if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &reg_value))
    	{
			eGon2_printf("boot power:unable to set voff vol\n");
		}
		else
		{
			return 0;
		}
	}

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
__s32 eGon2_set_power_off_vol(void) //设置关机之后，PMU硬件下次开机电压为3.3V
{
	__u8  reg_addr;
	__u8  reg_value;
	__s32 vol_value, ret;

	eGon2_config_charge_current(1);
	ret = eGon2_script_parser_fetch("pmu_para", "pmu_pwroff_vol", &vol_value, 1);
	if(ret)
	{
		eGon2_printf("boot power:unable to find power on vol set\n");
		vol_value = 3300;
	}
	reg_addr = BOOT_POWER20_VOFF_SET;
	if(!BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &reg_value))
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
		if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &reg_value))
    	{
			eGon2_printf("boot power:unable to set power on vol\n");
		}
		else
		{
			//设置VBUS不限流
//			eGon2_power_vbus_cur_limit();
			//设置完成，同时关机
	    	reg_addr = POWER_OFF_CTL;
	    	if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &reg_value))
	    	{
	        	return -1;
	    	}
	    	reg_value |= 1 << 7;
	    	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &reg_value))
	        {
	            return -1;
	        }

			return 0;
		}
	}

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
__s32 eGon2_get_power_vol_level(void)
{
	return power_step_level;
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
__s32 eGon2_set_charge_current(int current)
{
	__u8  reg_addr, value;
	int   step;

	if(PMU_type == PMU_TYPE_AXP209)
	{
		reg_addr = BOOT_POWER20_CHARGE1;
		if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
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
		if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &value))
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
__s32 eGon2_get_charge_current(void)
{
	__u8  reg_addr, value;
	int	  current;

	if(PMU_type == PMU_TYPE_AXP209)
	{
		reg_addr = BOOT_POWER20_CHARGE1;
		if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
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
__s32 eGon2_config_charge_current(int start_time)
{
	__s32 ret;
	int   value;
	script_gpio_set_t  gpio;
	__u32  gpio_hd;

	//1 代表关机状态
	//0 代表开机状态
	if(start_time == 1)
	{
		ret = eGon2_script_parser_fetch("pmu_para", "pmu_used2", &value, 1);
		if((ret >= 0) && (value == 1))
		{
			ret = eGon2_script_parser_fetch("pmu_para", "pmu_adpdet", (void *)&gpio, sizeof(script_gpio_set_t)/sizeof(int));
			if(ret >= 0)
			{
				gpio_hd = eGon2_GPIO_Request((void *)&gpio, 1);
				value = eGon2_GPIO_Read_One_PIN_Value(gpio_hd, 0);
				if(value > 0)
				{
					ret = eGon2_script_parser_fetch("pmu_para", "pmu_shutdown_chgcur", &value, 1);
				}
				else
				{
					ret = eGon2_script_parser_fetch("pmu_para", "pmu_shutdown_chgcur2", &value, 1);
				}
				if(ret >= 0)
				{
					eGon2_set_charge_current(value);

					return 0;
				}
			}
		}
		else
		{
			ret = eGon2_script_parser_fetch("pmu_para", "pmu_shutdown_chgcur", &value, 1);
			if(ret >= 0)
			{
				eGon2_set_charge_current(value);

				return 0;
			}
		}
	}
	else
	{
		eGon2_set_charge_current(300);
	}

	return -1;
}

