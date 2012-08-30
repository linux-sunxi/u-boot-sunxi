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
#include "axp209.h"
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
int axp209_set_charge_control(void)
{
	u8 reg_value;
	//disable ts adc, enable all other adc
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_ADC_EN1, &reg_value))
    {
        return -1;
    }
    reg_value |= 0xfe;
    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_ADC_EN1, reg_value))
    {
        return -1;
    }
    //enable charge
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_CHARGE1, &reg_value))
    {
        return -1;
    }
    reg_value |= 0x80;
    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_CHARGE1, reg_value))
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
int axp209_clear_data_buffer(void)
{
	int i;

	for(i=BOOT_POWER20_DATA_BUFFER0;i<=BOOT_POWER20_DATA_BUFFER11;i++)
	{
		if(axp_i2c_write(AXP20_ADDR, i, 0x00))
	    {
	        return -1;
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
int axp209_reboot_coulomb(void)
{
    u8   reg_value;
    u8   sp_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_COULOMB_CTL, &reg_value))
    {
        return -1;
    }
    if(!(reg_value & 0x80))
	{
		axp209_clear_data_buffer();
	}
	//enable coulomb
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_COULOMB_CTL, 0x80))
    {
        return -1;
    }
    //hold
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_COULOMB_CAL, &reg_value))
    {
        return -1;
    }
    reg_value |= 0x80;
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_COULOMB_CAL, reg_value))
    {
        return -1;
    }
    //
	if(axp_i2c_read(AXP20_ADDR, 0xBA, &sp_value))
    {
        return -1;
    }
    sp_value &= ~0x80;
	if(axp_i2c_write(AXP20_ADDR, 0xBA, sp_value))
    {
        return -1;
    }
    //hold
    reg_value &= ~0x80;
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_COULOMB_CAL, reg_value))
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
int axp209_probe_dcin_exist(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_STATUS, &reg_value))
    {
        return -1;
    }

    return (reg_value & 0x50);
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
int axp209_probe_battery_exist(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_MODE_CHGSTATUS, &reg_value))
    {
        return -1;
    }

    return (reg_value & 0x20);
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
int axp209_probe_battery_vol(void)
{
	u8  reg_value_h, reg_value_l;
	int bat_vol;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_BAT_AVERVOL_H8, &reg_value_h))
    {
        return -1;
    }
    if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_BAT_AVERVOL_L4, &reg_value_l))
    {
        return -1;
    }
    bat_vol = ((reg_value_h << 4) | reg_value_l) * 1.1;

	return bat_vol;
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
int axp209_probe_shortkey(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_INTSTS3, &reg_value))
    {
        return -1;
    }
    reg_value &= 0x02;
	if(reg_value)
	{
		if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_INTSTS3, reg_value))
	    {
	        return -1;
	    }
	}

	return reg_value>>1;
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
int axp209_probe_buttery_resistance_record(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_DATA_BUFFER1, &reg_value))
    {
        return -1;
    }

	return reg_value;
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
int axp209_probe_longkey(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_INTSTS3, &reg_value))
    {
        return -1;
    }
    reg_value &= 0x01;
	if(reg_value)
	{
		if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_INTSTS3, reg_value))
	    {
	        return -1;
	    }
	}

	return reg_value;
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
int axp209_probe_last_poweron_status(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_DATA_BUFFER11, &reg_value))
    {
        return -1;
    }

	return reg_value;
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
int axp209_set_next_poweron_status(int data)
{
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_DATA_BUFFER11, (u8)data))
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
int axp209_write_data_buffer(int buffer, uchar value)
{
	if(axp_i2c_write(AXP20_ADDR, (u8)buffer, value))
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
static int axp209_fetch_adc_freq(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_ADC_SPEED, &reg_value))
    {
        return -1;
    }

    reg_value = (reg_value & 0xC0) >> 6;

    return 25 * (1 << reg_value);
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
int axp209_probe_bat_coulomb_count(void)
{
	int   i;
	int   rvalue1, rvalue2;
	u8    *tmp;
	int   coulomb_count;

	tmp = (u8 *)&rvalue1;
	for(i=BOOT_POWER20_BAT_CHGCOULOMB3;i<=BOOT_POWER20_BAT_CHGCOULOMB0;i++)
	{
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_BAT_AVERVOL_H8, tmp))
	    {
	        return -1;
	    }
	}
	tmp = (u8 *)&rvalue2;
	for(i=BOOT_POWER20_BAT_DISCHGCOULOMB3;i<=BOOT_POWER20_BAT_DISCHGCOULOMB0;i++)
	{
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_BAT_AVERVOL_H8, tmp))
	    {
	        return -1;
	    }
	}
	coulomb_count = (4369 * (rvalue1 - rvalue2) / axp209_fetch_adc_freq() / 240 / 2);

	return coulomb_count;				//unit mAh

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
int axp209_probe_poweron_cause(void)
{
    __u8   reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_STATUS, &reg_value))
    {
        return -1;
    }

    return reg_value & 0x01;
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
int axp209_set_dcdc2(int set_vol)
{
    uint tmp, i;
    int  vol;
    u8   reg_value;

	if((set_vol < 700) || (set_vol > 2275))
	{
		return -1;
	}
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_DC2OUT_VOL, &reg_value))
    {
        return -1;
    }
    tmp = reg_value & 0x3f;
    vol = tmp * 25 + 700;
    //如果电压过高，则调低
    while(vol > set_vol)
    {
        tmp --;
        reg_value &= ~0x3f;
        reg_value |= tmp;
        if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_DC2OUT_VOL, reg_value))
	    {
	        return -1;
	    }
	    for(i=0;i<2000;i++);
        if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_DC2OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
        tmp     = reg_value & 0x3f;
        vol     = tmp * 25 + 700;
    }
    //如果电压过低，则调高，根据先调低再调高的过程，保证电压会大于等于用户设定电压
    while(vol < set_vol)
    {
        tmp ++;
        reg_value &= ~0x3f;
        reg_value |= tmp;
        if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_DC2OUT_VOL, reg_value))
	    {
	        return -1;
	    }
	    for(i=0;i<2000;i++);
        if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_DC2OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
        tmp     = reg_value & 0x3f;
        vol     = tmp * 25 + 700;
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
int axp209_set_dcdc3(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_OUTPUT_CTL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<1);
	    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_DC2OUT_VOL, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3500))
		{
			return -1;
		}
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_DC3OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0x80;
		reg_value |= ((set_vol - 700)/25);
	    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_DC3OUT_VOL, reg_value))
	    {
	        return -1;
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
int axp209_set_ldo2(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_OUTPUT_CTL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<2);
	    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_OUTPUT_CTL, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 1800) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_LDO24OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0x0f;
		reg_value |= (((reg_value - 1800)/100) << 4);
	    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_LDO24OUT_VOL, reg_value))
	    {
	        return -1;
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
int axp209_set_ldo3(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_OUTPUT_CTL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<6);
	    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_OUTPUT_CTL, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 2275))
		{
			return -1;
		}
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_LDO3OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0x80;
		reg_value |= ((reg_value - 700)/25);
	    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_LDO3OUT_VOL, reg_value))
	    {
	        return -1;
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
int axp209_set_ldo4(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_OUTPUT_CTL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<3);
	    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_OUTPUT_CTL, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 1250) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_LDO24OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
        reg_value &= 0xf0;
/*************************************************************************************
0    1300     2000    2500     2700   2800    3000   3300  max


*************************************************************************************/
		if(set_vol < 1300)
        {
            reg_value |= 0x00;
        }
        else if(set_vol <= 2000)
        {
        	reg_value |= (set_vol - 1200)/100;
        }
        else if(set_vol < 2700)
        {
        	reg_value |= 0x09;
        }
        else if(set_vol <= 2800)
        {
        	reg_value |= ((set_vol - 2700)/100) + 0x0a;
        }
        else
        {
        	if(set_vol < 3000)
        	{
        		set_vol = 3000;
        	}
        	else if(set_vol > 3300)
        	{
        		set_vol = 3300;
        	}
        	reg_value |= ((set_vol - 3000)/100) + 0x0c;
        }
	    if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_LDO24OUT_VOL, reg_value))
	    {
	        return -1;
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
int axp209_set_power_off(void)
{
    u8 reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_OFF_CTL, &reg_value))
    {
        return -1;
    }
    reg_value |= 1 << 7;
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_OFF_CTL, reg_value))
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
int axp209_set_poweroff_vol(int set_vol)
{
	u8 reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_VOFF_SET, &reg_value))
    {
        return -1;
    }
	reg_value &= 0xf8;
	if(set_vol >= 2600 && set_vol <= 3300)
	{
		reg_value |= (set_vol - 2600)/100;
	}
	else if(set_vol <= 2600)
	{
		reg_value |= 0x00;
	}
	else
	{
		reg_value |= 0x07;
	}
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_VOFF_SET, reg_value))
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
int axp209_set_charge_current(int current)
{
	u8   reg_value;
	int  step;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_CHARGE1, &reg_value))
    {
        return -1;
    }
	reg_value &= ~0x0f;
	if(current > 1800)
	{
		current = 1800;
	}
	else if(current < 300)
	{
		current = 300;
	}
	step       = (current/100) - 3;
	reg_value |= (step & 0x0f);

	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_CHARGE1, reg_value))
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
int axp209_probe_charge_current(void)
{
	__u8  reg_value;
	int	  current;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_CHARGE1, &reg_value))
    {
        return -1;
    }
	reg_value &= 0x0f;
	current = (reg_value + 3) * 100;

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
int axp209_probe_charge_status(void)
{
	__u8  reg_value;

	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_INTSTS2, &reg_value))
    {
        return -1;
    }
	return (reg_value >> 2) & 0x01;
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
int axp209_set_vbus_cur_limit(int current)
{
	__u8 reg_value;

	//set bus current limit off
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_IPS_SET, &reg_value))
    {
        return -1;
    }
    reg_value &= 0xfC;
	if(!current)
	{
	    reg_value |= 0x03;
	}
	else if(current < 500)		//limit to 100
	{
		reg_value |= 0x02;
	}
	else if(current < 900)		//limit to 500
	{
		reg_value |= 0x01;
	}
	else						//limit to 900
	{
		reg_value |= 0;
	}
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_IPS_SET, reg_value))
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
int axp209_set_vbus_vol_limit(int vol)
{
	__u8 reg_value;

	//set bus vol limit off
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_IPS_SET, &reg_value))
    {
        return -1;
    }
    reg_value &= ~(7 << 3);
	if(!vol)
	{
	    reg_value &= ~(1 << 6);
	}
	else
	{
		if(vol < 4000)
		{
			vol = 4000;
		}
		else if(vol > 4700)
		{
			vol = 4700;
		}
		reg_value |= ((vol-4000)/100) << 3;
	}
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_IPS_SET, reg_value))
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
int axp209_probe_rest_battery_capacity(void)
{
	u8  reg_value1, reg_value2;
	int bat_rest;
	int cur_coulombcounter;

	if(axp209_probe_charge_status() == 1)
	{
		return 100;
	}
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_DATA_BUFFER1, &reg_value1))
    {
        return -1;
    }
    bat_rest = reg_value1 & 0x7F;
    if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_COULOMB_CAL, &reg_value2))
    {
        return -1;
    }
	cur_coulombcounter = abs(axp209_probe_bat_coulomb_count());
	if(abs(bat_rest-(reg_value2 & 0x7F)) >= 10 || cur_coulombcounter > 50)
	{
    	bat_rest = reg_value2 & 0x7F;
  	}
	if(bat_rest == 127)
	{
		bat_rest = 100;
	}
	reg_value1 = bat_rest | 0x80;
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_COULOMB_CAL, reg_value1))
    {
        return -1;
    }

    return bat_rest;
}

