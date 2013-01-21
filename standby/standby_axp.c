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
#include "standby_i.h"
#include <asm/arch/axp221.h>
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
static __u8 output_control[4];

int standby_axp_output_control(int onoff)
{
//	int  i;
//	__u8 int_reg;

	if(!onoff)
	{
		//store
		if(standby_axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, output_control + 0))
	    {
	        return -1;
	    }
		if(standby_axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, output_control + 1))
	    {
	        return -1;
	    }
	    //eGon2_printf("read %x\n", *(__u32 *)output_control);
	    //close
	    if(standby_axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, 0x3B))
	    {
	        return -1;
	    }
		if(standby_axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, 0))
	    {
	        return -1;
	    }
	}
	else
	{
		//restore
		//eGon2_printf("write %x\n", *(__u32 *)output_control);
		if(standby_axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, output_control[0]))
	    {
	        return -1;
	    }
	    if(standby_axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, output_control[1]))
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
static  __u8  power_int_value[8];

int standby_axp_store_int_status(void)
{
	int   i;
	__u8  int_reg = BOOT_POWER22_INTEN1;
	__u8  standby_int_enable[8];

	for(i=0;i<5;i++)
	{
		if(standby_axp_i2c_read(AXP22_ADDR, int_reg, power_int_value + i))
	    {
	        return -1;
	    }
	    int_reg ++;
	}

	standby_int_enable[0] = 0x2C;
	standby_int_enable[1] = 0x0;
	standby_int_enable[2] = 0x3;
	standby_int_enable[3] = 0x0;
	standby_int_enable[4] = 0x0;

	int_reg = BOOT_POWER22_INTEN1;
	for(i=0;i<5;i++)
	{
		if(standby_axp_i2c_write(AXP22_ADDR, int_reg, standby_int_enable[i]))
	    {
	        return -1;
	    }
	    int_reg ++;
	}
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
__s32 standby_axp_restore_int_status(void)
{
	int   i;
	__u8  int_reg = BOOT_POWER22_INTEN1;

	*(volatile unsigned int *)(0x01f00c00 + 0x10) |= 1;
	*(volatile unsigned int *)(0x01f00c00 + 0x40) &= ~1;
	for(i=0;i<5;i++)
	{
		if(standby_axp_i2c_write(AXP22_ADDR, int_reg, power_int_value[i]))
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
__s32 standby_axp_int_query(__u8 *int_status)
{
	int   i;
	int   ret;
	__u8  int_reg = BOOT_POWER22_INTSTS1;

	ret = -1;
	*(volatile unsigned int *)(0x01f00c00 + 0x10) |= 1;
	for(i=0;i<5;i++)
	{
		if(standby_axp_i2c_read(AXP22_ADDR, int_reg + i, &int_status[i]))
	    {
	        goto __standby_axp_int_query_err;
	    }
	    if(standby_axp_i2c_write(AXP22_ADDR, int_reg + i, 0xff))
	    {
	        goto __standby_axp_int_query_err;
	    }
	}
	ret = 0;

__standby_axp_int_query_err:
	standby_gic_clear_pengding();

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
int standby_axp_probe_dcin_exist(void)
{
	u8 reg_value;

	if(standby_axp_i2c_read(AXP22_ADDR, BOOT_POWER22_STATUS, &reg_value))
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
int standby_axp_probe_key(void)
{
	u8  reg_value;

	if(standby_axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS3, &reg_value))
    {
        return -1;
    }
    reg_value &= 0x03;
	if(reg_value)
	{
		if(standby_axp_i2c_write(AXP22_ADDR, BOOT_POWER22_INTSTS3, reg_value))
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
int standby_axp_probe_usb(void)
{
	u8  reg_value;

	if(standby_axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS3, &reg_value))
    {
        return -1;
    }
    reg_value &= 0x03;
	if(reg_value)
	{
		if(standby_axp_i2c_write(AXP22_ADDR, BOOT_POWER22_INTSTS3, reg_value))
	    {
	        return -1;
	    }
	}

	return reg_value;
}


