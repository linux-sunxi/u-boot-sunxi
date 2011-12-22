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


__u32 power_ext_hd;
__u32 PMU_type = 0;
__u32 power_step_level = 0;
__s32 bat_vol = 0;
__s32 user_set_dcdc2;


#define ABS(x) ((x > 0)?(x):(-(x)))


#define FUELGUAGE_LOW_VOL	3400	//<3.4v,2%
#define FUELGUAGE_VOL1		3500    //<3.5v,3%
#define FUELGUAGE_VOL2		3600
#define FUELGUAGE_VOL3		3700
#define FUELGUAGE_VOL4		3800
#define FUELGUAGE_VOL5		3900
#define FUELGUAGE_VOL6		4000
#define FUELGUAGE_VOL7		4100
#define FUELGUAGE_TOP_VOL	4160	//>4.16v,100%

#define FUELGUAGE_LOW_LEVEL	2		//<3.4v,2%
#define FUELGUAGE_LEVEL1	3		//<3.5v,3%
#define FUELGUAGE_LEVEL2	5
#define FUELGUAGE_LEVEL3	16
#define FUELGUAGE_LEVEL4	46
#define FUELGUAGE_LEVEL5	66
#define FUELGUAGE_LEVEL6	83
#define FUELGUAGE_LEVEL7	93
#define FUELGUAGE_TOP_LEVEL	100     //>4.16v,100%
/*
*********************************************************************************************************
*                                   SET DCDC2 VALUE FOR BOOT
*
* Description: set dcdc2 value to default for boot.
*
* Arguments  : none
*
* Returns    : result
*
* Note       :
*********************************************************************************************************
*/
__u32  dcdc2_user_set;
__s32  dcdc3_user_set;
__s32  ldo2_user_set;
__s32  ldo3_user_set;
__s32  ldo4_user_set;

__s32 pmu_set_dcdc2(__u32 set_vol)
{
    __u32 vol, tmp, i;
    __u8  reg_addr, value;

	if(!set_vol)
	{
		set_vol = dcdc2_user_set;
	}
    if(PMU_type == PMU_TYPE_AXP209)
    {
        //PMU is AXP209
        reg_addr = BOOT_POWER20_DC2OUT_VOL;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        tmp     = value & 0x3f;
        vol     = tmp * 25 + 700;
        //如果电压过高，则调低
        while(vol > set_vol)
        {
            tmp -= 1;
            value &= ~0x3f;
            value |= tmp;
            reg_addr = BOOT_POWER20_DC2OUT_VOL;
            if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &value))
            {
                return -1;
            }
            for(i=0;i<2000;i++);
            reg_addr = BOOT_POWER20_DC2OUT_VOL;
            if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
            {
                return -1;
            }
            tmp     = value & 0x3f;
            vol     = tmp * 25 + 700;
        }
        //如果电压过低，则调高，根据先调低再调高的过程，保证电压会大于等于用户设定电压
        while(vol < set_vol)
        {
            tmp += 1;
            value &= ~0x3f;
            value |= tmp;
            reg_addr = BOOT_POWER20_DC2OUT_VOL;
            if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &value))
            {
                return -1;
            }
            for(i=0;i<2000;i++);
            reg_addr = BOOT_POWER20_DC2OUT_VOL;
            if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
            {
                return -1;
            }
            tmp     = value & 0x3f;
            vol     = tmp * 25 + 700;
        }
    }

    return 0;
}


__s32 pmu_set_dcdc3(int set_vol)
{
	__s32 vol_value;
	__s32 ret;
	__u8  reg_addr;
	__u8  reg_value;

	if(!set_vol)
	{
		if(!dcdc3_user_set)
		{
			ret = script_parser_fetch("target", "dcdc3_vol", &vol_value, 1);
			if(ret)
			{
				return -1;
			}
			dcdc3_user_set = vol_value;
		}
		vol_value = dcdc3_user_set;
	}
	else
	{
		vol_value = set_vol;
	}

	reg_addr = BOOT_POWER20_DC3OUT_VOL;
	if(!BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &reg_value))
	{
		if((vol_value >= 700) && (vol_value <= 3500))
		{
			reg_value &= 0x80;
			reg_value = ((vol_value - 700)/25);
			if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &reg_value))
			{
				return -1;
			}
		}
	}

	return 0;
}


__s32 pmu_set_ldo2(int set_vol)
{
	__s32 vol_value;
	__s32 ret;
	__u8  reg_addr;
	__u8  reg_value;

	if(!set_vol)
	{
		if(!ldo2_user_set)
		{
			ret = script_parser_fetch("target", "ldo2_vol", &vol_value, 1);
			if(ret)
			{
				return -1;
			}
			ldo2_user_set = vol_value;
		}
		vol_value = ldo2_user_set;
	}
	else
	{
		vol_value = set_vol;
	}

	reg_addr  = BOOT_POWER20_LDO24OUT_VOL;
	if(!BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &reg_value))
	{
		if((vol_value >= 1800) && (vol_value <= 3300))
		{
			reg_value &= 0x0f;
			reg_value = (((vol_value - 1800)/100) << 4);
			if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &reg_value))
			{
				return -1;
			}
		}
    }

	return 0;
}

__s32 pmu_set_ldo3(int set_vol)
{
	__s32 vol_value;
	__s32 ret;
	__u8  reg_addr;
	__u8  reg_value;

	if(!set_vol)
	{
		if(!ldo3_user_set)
		{
			ret = script_parser_fetch("target", "ldo3_vol", &vol_value, 1);
			if(ret)
			{
				return -1;
			}
			ldo3_user_set = vol_value;
		}
		vol_value = ldo3_user_set;
	}
	else
	{
		vol_value = set_vol;
	}

	reg_addr  = BOOT_POWER20_LDO3OUT_VOL;
	if(!BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &reg_value))
	{
		if((vol_value >= 700) && (vol_value <= 3500))
		{
			reg_value &= 0x80;
			reg_value = ((vol_value - 700)/25);
			if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &reg_value))
			{
				return -1;
			}
		}
    }

	return 0;
}


__s32 pmu_set_ldo4(int set_vol)
{
	__s32 vol_value;
	__s32 ret;
	__u8  reg_addr;
	__u8  reg_value;

	if(!set_vol)
	{
		if(!ldo4_user_set)
		{
			ret = script_parser_fetch("target", "ldo4_vol", &vol_value, 1);
			if(ret)
			{
				return -1;
			}
			ldo4_user_set = vol_value;
		}
		vol_value = ldo4_user_set;
	}
	else
	{
		vol_value = set_vol;
	}

	reg_addr  = BOOT_POWER20_LDO24OUT_VOL;
	if(!BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &reg_value))
    {
        reg_value &= 0xf0;
/*************************************************************************************
0    1300     2000    2500     2700   2800    3000   3300  max


*************************************************************************************/
		if(vol_value < 1300)
        {
            reg_value |= 0x00;
        }
        else if(vol_value <= 2000)
        {
        	reg_value |= (vol_value - 1200)/100;
        }
        else if(vol_value < 2700)
        {
        	reg_value |= 0x09;
        }
        else if(vol_value <= 2800)
        {
        	reg_value |= ((vol_value - 2700)/100) + 0x0a;
        }
        else
        {
        	if(vol_value < 3000)
        	{
        		vol_value = 3000;
        	}
        	else if(vol_value > 3300)
        	{
        		vol_value = 3300;
        	}
        	reg_value |= ((vol_value - 3000)/100) + 0x0c;
        }
    	if(BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &reg_value))
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
__s32 pmu_probe_key(void)
{
    __u8  reg_addr, value;
    __s32 key_exist;

    key_exist = 0;
    if(PMU_type == PMU_TYPE_AXP209)
    {
        reg_addr = BOOT_POWER20_INTSTS3;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        key_exist = (value >> 0) & 0x03;
        if(key_exist)
        {
        	value |= 0x01 << 1;
        	BOOT_TWI_Write(AXP20_ADDR, &reg_addr, &value);
    	}
    }
    else
    {
        return -1;
    }

    return key_exist;
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
__s32 pmu_probe_short_key(void)
{
	__s32 ret;

	ret = pmu_probe_key();
	if(ret > 0)
	{
		return ((ret & 0x02)>>1);
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
int pmu_probe_long_key(void)
{
	__s32 ret;

	ret = pmu_probe_key();
	if(ret > 0)
	{
		return (ret & 0x01);
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
__s32  pmu_probe_dcin_battery_status(__u32 *dcin_exist, __u32 *battery_exist)
{
    __u8  reg_addr, value;

    if(PMU_type == PMU_TYPE_AXP209)
    {
        //pmu is AXP209
        reg_addr = BOOT_POWER20_STATUS;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        *dcin_exist = ((value >> 4) & 0x01) |  ((value >> 6) & 0x01);
        reg_addr = BOOT_POWER20_MODE_CHGSTATUS;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        *battery_exist  = (value >> 5) & 0x01;
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
*    说明    ：获取电池充电是否充满
*
*
************************************************************************************************************
*/
__s32 pmu_battery_charge_status(void)
{
    __u8  reg_addr, value;
    __s32 charge_status;

    if(PMU_type == PMU_TYPE_AXP209)
    {
        //pmu is AXP209
        reg_addr = BOOT_POWER20_INTSTS2;
        if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
        {
            return -1;
        }
        charge_status = (value & 0x04);
        value = charge_status;
        if(charge_status)
        {
        	BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value);
	    }

		return charge_status;
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
__s32 pmu_probe_adc_freq(void)
{
	__u8 reg_addr, value;

	reg_addr = BOOT_POWER20_ADC_SPEED;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &value))
    {
        return -1;
    }
	value &= 0xc0;
	switch(value >> 6)
	{
		case 0:
			value = 25;
			break;
		case 1:
			value = 50;
			break;
		case 2:
			value = 100;
			break;
		case 3:
			value = 200;
			break;
		default:
			value = 25;
			break;
	}
	return 	value;
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
__u8 pmu_probe_vol_rate(__s32 Bat_Ocv_Vol)
{
    if(Bat_Ocv_Vol >= FUELGUAGE_TOP_VOL)         //4160
    {
        return FUELGUAGE_TOP_LEVEL;
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_LOW_VOL)    //<3400
    {
        return FUELGUAGE_LOW_LEVEL;
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_VOL1)       //3500
    {
        return FUELGUAGE_LOW_LEVEL + (FUELGUAGE_LEVEL1 - FUELGUAGE_LOW_LEVEL) * ((int)Bat_Ocv_Vol - FUELGUAGE_LOW_VOL) / (FUELGUAGE_VOL1 - FUELGUAGE_LOW_VOL);
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_VOL2)       //3600
    {
        return FUELGUAGE_LEVEL1 + (FUELGUAGE_LEVEL2 - FUELGUAGE_LEVEL1) * ((int)Bat_Ocv_Vol - FUELGUAGE_VOL1) / (FUELGUAGE_VOL2 - FUELGUAGE_VOL1);
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_VOL3)       //3700
    {
        return FUELGUAGE_LEVEL2 + (FUELGUAGE_LEVEL3 - FUELGUAGE_LEVEL2) * ((int)Bat_Ocv_Vol - FUELGUAGE_VOL2) / (FUELGUAGE_VOL3 - FUELGUAGE_VOL2);
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_VOL4)       //3800
    {
        return FUELGUAGE_LEVEL3 + (FUELGUAGE_LEVEL4 - FUELGUAGE_LEVEL3) * ((int)Bat_Ocv_Vol - FUELGUAGE_VOL3) / (FUELGUAGE_VOL4 - FUELGUAGE_VOL3);
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_VOL5)       //3900
    {
        return FUELGUAGE_LEVEL4 + (FUELGUAGE_LEVEL5 - FUELGUAGE_LEVEL4) * ((int)Bat_Ocv_Vol - FUELGUAGE_VOL4) / (FUELGUAGE_VOL5 - FUELGUAGE_VOL4);
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_VOL6)       //4000
    {
        return FUELGUAGE_LEVEL5 + (FUELGUAGE_LEVEL6 - FUELGUAGE_LEVEL5) * ((int)Bat_Ocv_Vol - FUELGUAGE_VOL5) / (FUELGUAGE_VOL6 - FUELGUAGE_VOL5);
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_VOL7)       //4100
    {
        return FUELGUAGE_LEVEL6 + (FUELGUAGE_LEVEL7 - FUELGUAGE_LEVEL6) * ((int)Bat_Ocv_Vol - FUELGUAGE_VOL6) / (FUELGUAGE_VOL7 - FUELGUAGE_VOL6);
    }
    else if(Bat_Ocv_Vol < FUELGUAGE_TOP_VOL)    //4100
    {
        return FUELGUAGE_LEVEL7 + (FUELGUAGE_TOP_LEVEL - FUELGUAGE_LEVEL7) * ((int)Bat_Ocv_Vol - FUELGUAGE_VOL7) / (FUELGUAGE_TOP_VOL - FUELGUAGE_VOL7);
    }
    else
    {
        return 0;
    }
}

__s32 pmu_probe_Bat_Coulomb_Count(void)
{
	__u8  reg_addr;
	__u8  temp[8];
	__s32  rValue1,rValue2;
	__s32 Cur_CoulombCounter_tmp;

	reg_addr = BOOT_POWER20_BAT_CHGCOULOMB3;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &temp[0]))
    {
        return -1;
    }
    reg_addr = BOOT_POWER20_BAT_CHGCOULOMB2;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &temp[1]))
    {
        return -1;
    }
	reg_addr = BOOT_POWER20_BAT_CHGCOULOMB1;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &temp[2]))
    {
        return -1;
    }
    reg_addr = BOOT_POWER20_BAT_CHGCOULOMB0;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &temp[3]))
    {
        return -1;
    }
    reg_addr = BOOT_POWER20_BAT_DISCHGCOULOMB3;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &temp[4]))
    {
        return -1;
    }
    reg_addr = BOOT_POWER20_BAT_DISCHGCOULOMB2;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &temp[5]))
    {
        return -1;
    }
	reg_addr = BOOT_POWER20_BAT_DISCHGCOULOMB1;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &temp[6]))
    {
        return -1;
    }
    reg_addr = BOOT_POWER20_BAT_DISCHGCOULOMB0;
    if(BOOT_TWI_Read(AXP20_ADDR, &reg_addr, &temp[7]))
    {
        return -1;
    }

	rValue1 = ((temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + temp[3]);
	rValue2 = ((temp[4] << 24) + (temp[5] << 16) + (temp[6] << 8) + temp[7]);
	//printf("pmu_probe_Bat_Coulomb_Count -     CHARGINGOULB:[0]=0x%x,[1]=0x%x,[2]=0x%x,[3]=0x%x\n",temp[0],temp[1],temp[2],temp[3]);
	//printf("pmu_probe_Bat_Coulomb_Count - DISCHARGINGCLOUB:[4]=0x%x,[5]=0x%x,[6]=0x%x,[7]=0x%x\n",temp[4],temp[5],temp[6],temp[7]);

	Cur_CoulombCounter_tmp = (4369 * (rValue1 - rValue2) / pmu_probe_adc_freq() / 240 / 2);

	return Cur_CoulombCounter_tmp;				//unit mAh
}



__s32 pmu_probe_rest_cal(void)
{
	__u8  reg_addr,value1,value2;
    __s32 bat_vol,bat_cur,bat_rdc,bat_ocv,bat_rest,Cur_CoulombCounter;

	if(PMU_type == PMU_TYPE_AXP209)
	{
		reg_addr = BOOT_POWER20_DATA_BUFFER1;
    	if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value1))
    	{
        	return -1;
    	}

  		bat_rest = (int) (value1 & 0x7F);

		reg_addr = 0xB9;
    	if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, &value2))
    	{
        	return -1;
    	}

  		Cur_CoulombCounter = ABS(pmu_probe_Bat_Coulomb_Count());
  		if(ABS(bat_rest-(value2 & 0x7F)) >= 10 || Cur_CoulombCounter > 50){
    		bat_rest = (int) (value2 & 0x7F);
  		}

  		if(bat_rest == 127)
  		{
  			bat_rest = 100;
  		}

  		value1 = bat_rest | 0x80;
  		if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, &value1))
        {
            return -1;
        }
	}
	else
	{
		return -1;
	}

    return bat_rest;
}

static  __u8  power_int_value[5];

__s32 pmu_int_enable(void)
{
    __u8  reg_addr;
    __u8  int_enable[5];
    int	  i;

	for(i=0;i<5;i++)
	{
		reg_addr = BOOT_POWER20_INTEN1 + i;
    	if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, int_enable + i))
    	{
        	return -1;
    	}
    }

	int_enable[0] = 0x24;	//开启：VBUS移除，ACIN移除
	int_enable[1] = 0;		//开启：充电完成
	int_enable[2] = 0x3;	//开启：电源按键短按，长按
	int_enable[3] = 0;
	int_enable[4] = 0;

	for(i=0;i<5;i++)
	{
		reg_addr = BOOT_POWER20_INTEN1 + i;
    	if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, int_enable + i))
    	{
        	return -1;
    	}
    }

	return 0;
}

__s32 pmu_int_disable(void)
{
    __u8  reg_addr;
    int   i;

	for(i=0;i<5;i++)
	{
		reg_addr = BOOT_POWER20_INTEN1 + i;
    	if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, power_int_value + i))
    	{
        	return -1;
    	}
    }

	return 0;
}


__s32 pmu_int_query(__u8 *int_status)
{
    __u8  reg_addr;
    int	  i;

	for(i=0;i<5;i++)
	{
		reg_addr = BOOT_POWER20_INTSTS1 + i;
    	if(BOOT_TWI_Read(AXP20_ADDR, reg_addr, int_status + i))
    	{
        	return -1;
    	}
    	if(BOOT_TWI_Write(AXP20_ADDR, reg_addr, int_status + i))
    	{
        	return -1;
    	}
    }

	return 0;
}

__s32 pmu_vbus_unlimit(void)
{
	__u8 reg_addr, value;

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

    return 0;
}


