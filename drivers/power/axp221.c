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
#include <asm/arch/axp221.h>
#include "axp.h"
#include "axp221_i.h"
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
int axp221_set_charge_control(void)
{
	u8 reg_value;
	//disable ts adc, enable all other adc
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ADC_EN, &reg_value))
    {
        return -1;
    }
    reg_value |= 0xfe;
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ADC_EN, reg_value))
    {
        return -1;
    }
    //enable charge
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_CHARGE1, &reg_value))
    {
        return -1;
    }
    reg_value |= 0x80;
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_CHARGE1, reg_value))
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
int axp221_clear_data_buffer(void)
{
	int i;

	for(i=BOOT_POWER22_DATA_BUFFER0;i<=BOOT_POWER22_DATA_BUFFER11;i++)
	{
		if(axp_i2c_write(AXP22_ADDR, i, 0x00))
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
int axp221_probe_dcin_exist(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_STATUS, &reg_value))
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
int axp221_probe_battery_ratio(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_COULOMB_CAL, &reg_value))
    {
        return -1;
    }

	return reg_value & 0x7f;
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
int axp221_probe_battery_exist(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_MODE_CHGSTATUS, &reg_value))
    {
        return -1;
    }

	if(reg_value & 0x10)
	{
		return (reg_value & 0x20);
	}
	else
	{
		return -1;
	}
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
int axp221_probe_battery_vol(void)
{
	u8  reg_value_h, reg_value_l;
	int bat_vol, tmp_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_BAT_AVERVOL_H8, &reg_value_h))
    {
        return -1;
    }
    if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_BAT_AVERVOL_L4, &reg_value_l))
    {
        return -1;
    }
    tmp_value = (reg_value_h << 4) | reg_value_l;
    bat_vol = tmp_value * 11;
    bat_vol /= 10;

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
int axp221_probe_buttery_resistance_record(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DATA_BUFFER1, &reg_value))
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
int axp221_probe_key(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS3, &reg_value))
    {
        return -1;
    }
    reg_value &= 0x03;
	if(reg_value)
	{
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_INTSTS3, reg_value))
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
int axp221_probe_last_poweron_status(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DATA_BUFFER11, &reg_value))
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
int axp221_set_next_poweron_status(int data)
{
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DATA_BUFFER11, (u8)data))
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
int axp221_write_data_buffer(int buffer, uchar value)
{
	if(axp_i2c_write(AXP22_ADDR, (u8)buffer, value))
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
int axp221_probe_poweron_cause(void)
{
    uchar   reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_STATUS, &reg_value))
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
int axp221_set_dc1sw(int on_off)
{
    u8   reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
    if(on_off)
    {
		reg_value |= (1 << 7);
	}
	else
	{
		reg_value &= ~(1 << 7);
	}
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to set dc1sw\n");

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
int axp221_set_dcdc1(int set_vol)
{
    u8   reg_value;

	if((set_vol < 1600) || (set_vol > 3400))
	{
		debug("%d\n", __LINE__);
		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC1OUT_VOL, &reg_value))
    {
		debug("%d\n", __LINE__);
        return -1;
    }
	reg_value = ((set_vol - 1600)/100);
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC1OUT_VOL, reg_value))
	{
		printf("sunxi pmu error : unable to set dcdc1\n");

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
int axp221_set_dcdc2(int set_vol)
{
    u8   reg_value;

	if((set_vol < 600) || (set_vol > 1540))
	{
		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC2OUT_VOL, &reg_value))
    {
        return -1;
    }
    reg_value &= ~0x3f;
    reg_value |= (set_vol - 600)/20;
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC2OUT_VOL, reg_value))
    {
    	printf("sunxi pmu error : unable to set dcdc2\n");
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
int axp221_set_dcdc3(int set_vol)
{
    u8   reg_value;

	if((set_vol < 600) || (set_vol > 1860))
	{
		debug("%d\n", __LINE__);

		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC3OUT_VOL, &reg_value))
    {
    	debug("%d\n", __LINE__);

        return -1;
    }
	reg_value = ((set_vol - 600)/20);
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC3OUT_VOL, reg_value))
	{
		printf("sunxi pmu error : unable to set dcdc3\n");

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
int axp221_set_dcdc4(int set_vol)
{
    u8   reg_value;

	if((set_vol < 600) || (set_vol > 1540))
	{
		debug("%d\n", __LINE__);

		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC4OUT_VOL, &reg_value))
    {
    	debug("%d\n", __LINE__);

        return -1;
    }
	reg_value = ((set_vol - 600)/20);
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC4OUT_VOL, reg_value))
	{
		printf("sunxi pmu error : unable to set dcdc4\n");

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
int axp221_set_dcdc5(int set_vol)
{
    u8   reg_value;

	if((set_vol < 1000) || (set_vol > 2550))
	{
		debug("%d\n", __LINE__);

		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, &reg_value))
    {
    	debug("%d\n", __LINE__);

        return -1;
    }
	reg_value = ((set_vol - 1000)/50);
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, reg_value))
	{
		printf("sunxi pmu error : unable to set dcdc5\n");

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
int axp221_set_aldo1(int set_vol)
{
	u8 gate;
	u8 vol_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &gate))
    {
        return -1;
    }
	if(!set_vol)
	{
	    gate &= ~(1<<6);
	}
	else
	{
		gate |=  (1<<6);
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO1OUT_VOL, &vol_value))
	    {
	        return -1;
	    }
	    vol_value &= 0x1f;
		vol_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ALDO1OUT_VOL, vol_value))
	    {
	        return -1;
	    }
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, gate))
    {
    	printf("sunxi pmu error : unable to set aldo1\n");
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
int axp221_set_aldo2(int set_vol)
{
	u8 gate;
	u8 vol_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &gate))
    {
        return -1;
    }
	if(!set_vol)
	{
	    gate &= ~(1<<7);
	}
	else
	{
		gate |=  (1<<7);
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO2OUT_VOL, &vol_value))
	    {
	        return -1;
	    }
	    vol_value &= 0x1f;
		vol_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ALDO2OUT_VOL, vol_value))
	    {
	        return -1;
	    }
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, gate))
    {
    	printf("sunxi pmu error : unable to set aldo2\n");
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
int axp221_set_aldo3(int set_vol)
{
	u8 gate;
	u8 vol_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL3, &gate))
    {
        return -1;
    }
	if(!set_vol)
	{
	    gate &= ~(1<<7);
	}
	else
	{
		gate |=  (1<<7);
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO3OUT_VOL, &vol_value))
	    {
	        return -1;
	    }
	    vol_value &= 0x1f;
		vol_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ALDO3OUT_VOL, vol_value))
	    {
	        return -1;
	    }
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL3, gate))
    {
    	printf("sunxi pmu error : unable to set aldo3\n");
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
int axp221_set_dldo1(int on_off)
{
	u8 gate;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &gate))
    {
        return -1;
    }
	if(!on_off)
	{
	    gate &= ~(1<<3);
	}
	else
	{
		gate |=  (1<<3);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DLDO1_VOL, 0x0b))
		{
			printf("sunxi pmu error : unable to set dldo1\n");

			return -1;
		}
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, gate))
    {
    	printf("sunxi pmu error : unable to set dldo1\n");
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
int axp221_set_dldo2(int on_off)
{
	u8 gate;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &gate))
    {
        return -1;
    }
	if(!on_off)
	{
	    gate &= ~(1<<4);
	}
	else
	{
		gate |=  (1<<4);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DLDO2_VOL, 0x0b))
		{
			printf("sunxi pmu error : unable to set dldo2\n");

			return -1;
		}
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, gate))
    {
    	printf("sunxi pmu error : unable to set dldo2\n");
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
int axp221_set_dldo3(int on_off)
{
	u8 gate;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &gate))
    {
        return -1;
    }
	if(!on_off)
	{
	    gate &= ~(1<<5);
	}
	else
	{
		gate |=  (1<<5);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DLDO3_VOL, 0x0b))
		{
			printf("sunxi pmu error : unable to set dldo3\n");

			return -1;
		}
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, gate))
    {
    	printf("sunxi pmu error : unable to set dldo3\n");
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
int axp221_set_dldo4(int on_off)
{
	u8 gate;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &gate))
    {
        return -1;
    }
	if(!on_off)
	{
	    gate &= ~(1<<6);
	}
	else
	{
		gate |=  (1<<6);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DLDO4_VOL, 0x0b))
		{
			printf("sunxi pmu error : unable to set dldo4\n");

			return -1;
		}
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, gate))
    {
    	printf("sunxi pmu error : unable to set dldo4\n");
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
int axp221_set_eldo1(int on_off)
{
	u8 gate;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &gate))
    {
        return -1;
    }
	if(!on_off)
	{
	    gate &= ~(1<<0);
	}
	else
	{
		gate |=  (1<<0);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ELDO1_VOL, 0x0b))
		{
			printf("sunxi pmu error : unable to set eldo1\n");

			return -1;
		}
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, gate))
    {
    	printf("sunxi pmu error : unable to set eldo2\n");
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
int axp221_set_eldo2(int on_off)
{
	u8 gate;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &gate))
    {
        return -1;
    }
	if(!on_off)
	{
	    gate &= ~(1<<1);
	}
	else
	{
		gate |=  (1<<1);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ELDO1_VOL, 0x0b))
		{
			printf("sunxi pmu error : unable to set eldo1\n");

			return -1;
		}
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, gate))
    {
    	printf("sunxi pmu error : unable to set eldo2\n");
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
int axp221_set_eldo3(int on_off)
{
	u8 gate;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &gate))
    {
        return -1;
    }
	if(!on_off)
	{
	    gate &= ~(1<<2);
	}
	else
	{
		gate |=  (1<<2);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ELDO3_VOL, 0x0b))
		{
			printf("sunxi pmu error : unable to set eldo3\n");

			return -1;
		}
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, gate))
    {
    	printf("sunxi pmu error : unable to set eldo3\n");
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
int axp221_set_gpio1ldo(int onoff, int set_vol)
{
	u8 reg_value;

	if(onoff)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_GPIO1_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xf0;
	    if(set_vol < 700)
	    {
	    	set_vol = 700;
	    }
	    else if(set_vol > 3300)
	    {
	    	set_vol = 3300;
	    }
	    reg_value |= ((set_vol - 700)/100) & 0x0f;
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_GPIO1_VOL, reg_value))
	    {
	        return -1;
	    }

		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_GPIO1_CTL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~7;
	    reg_value |=  3;
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_GPIO1_CTL, reg_value))
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
int axp221_set_power_off(void)
{
    u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OFF_CTL, &reg_value))
    {
        return -1;
    }
    reg_value |= 1 << 7;
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OFF_CTL, reg_value))
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
int axp221_set_poweroff_vol(int set_vol)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_VOFF_SET, &reg_value))
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
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_VOFF_SET, reg_value))
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
int axp221_set_charge_current(int current)
{
	u8   reg_value;
	int  step;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_CHARGE1, &reg_value))
    {
        return -1;
    }
	reg_value &= ~0x0f;
	if(current > 2550)
	{
		current = 2550;
	}
	else if(current < 300)
	{
		current = 300;
	}
	step       = (current/150) - 2;
	reg_value |= (step & 0x0f);

	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_CHARGE1, reg_value))
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
int axp221_probe_charge_current(void)
{
	uchar  reg_value;
	int	  current;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_CHARGE1, &reg_value))
    {
        return -1;
    }
	reg_value &= 0x0f;
	current = (reg_value + 2) * 150;

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
int axp221_probe_charge_status(void)
{
	uchar  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS2, &reg_value))
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
int axp221_set_vbus_cur_limit(int current)
{
	uchar reg_value;

	//set bus current limit off
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_IPS_SET, &reg_value))
    {
        return -1;
    }
    reg_value &= 0xfC;
	if(!current)
	{
	    reg_value |= 0x03;
	}
	else if(current <= 500)		//limit to 500
	{
		reg_value |= 0x01;
	}
	else						//limit to 900
	{
		reg_value |= 0;
	}
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_IPS_SET, reg_value))
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
int axp221_set_vbus_vol_limit(int vol)
{
	uchar reg_value;

	//set bus vol limit off
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_IPS_SET, &reg_value))
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
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_IPS_SET, reg_value))
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
int axp221_set_chgcur(int cur)
{
	uchar reg_value;

	if(cur < 300)
	{
		cur = 300;
	}
	else if(cur > 2550)
	{
		cur = 2550;
	}
	//set charge current
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_CHARGE1, &reg_value))
    {
        return -1;
    }
    reg_value &= 0xf0;
    reg_value |= (((cur - 300)/150) & 0x0f);
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_CHARGE1, reg_value))
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
int axp221_probe_rest_battery_capacity(void)
{
	u8  reg_value;
	int bat_rest;

	if(axp221_probe_charge_status() == 1)
	{
		return 100;
	}
    if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_COULOMB_CAL, &reg_value))
    {
        return -1;
    }
	bat_rest = reg_value & 0x7F;
    return bat_rest;
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
int axp221_int_query(uchar *addr)
{
	int   i;

	for(i=0;i<5;i++)
	{
	    if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS1 + i, addr + i))
	    {
	        return -1;
	    }
	}

	for(i=0;i<5;i++)
	{
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_INTSTS1 + i, 0xff))
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
int axp221_int_enable_read(uchar *addr)
{
	int   i;
	uchar  int_reg = BOOT_POWER22_INTEN1;

	for(i=0;i<5;i++)
	{
		if(axp_i2c_read(AXP22_ADDR, int_reg, addr + i))
	    {
	        return -1;
	    }
	    int_reg ++;
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
int axp221_int_enable_write(uchar *addr)
{
	int   i;
	uchar  int_reg = BOOT_POWER22_INTEN1;

	for(i=0;i<5;i++)
	{
		if(axp_i2c_write(AXP22_ADDR, int_reg, addr[i]))
	    {
	        return -1;
	    }
	    int_reg ++;
	}

	return 0;
}

