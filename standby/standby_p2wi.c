/*
*********************************************************************************************************
*                                                AR100 SYSTEM
*                                     AR100 Software System Develop Kits
*                                              	  p2wi module
*
*                                    (c) Copyright 2012-2016, Sunny China
*                                             All Rights Reserved
*
* File    : p2wi.c
* By      : Sunny
* Version : v1.0
* Date    : 2012-5-22
* Descript: p2wi module.
* Update  : date                auther      ver     notes
*           2012-5-22 9:49:38	Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "standby_i.h"
#include <asm/arch/p2wi.h>


static __s32 standby_p2wi_wait_state(void)
{
	__s32  ret = -1;
	__u32  stat;

	while (1)
	{
		stat = readl(P2WI_REG_STAT);
		if (stat & P2WI_TERR_INT)
		{
			//transfer error
			ret = -1;
			break;
		}
		if (stat & P2WI_TOVER_INT)
		{
			//transfer complete
			ret = 0;
			break;
		}
	}
	//clear state flag
	writel(stat, P2WI_REG_STAT);

	return ret;
}


//p2wi can be config used or not
__s32 standby_p2wi_read(u8 *addr, u8 *data, u32 len)
{
	__u32 i;
	__u32 addr0 = 0, addr1 = 0;
	__u32 data0 = 0, data1 = 0;

	if((len >= PMU_TRANS_BYTE_MAX) || (len <= 0))
	{
		return -1;
	}

	for (i = 0; i < len; i++)
	{
		if (i < 4)
		{
			//pack 8bit addr0~addr3 into 32bit addr0
			addr0 |= addr[i] << (i * 8);
		}
		else
		{
			//pack 8bit addr4~addr7 into 32bit addr1
			addr1 |= addr[i] << ((i - 4) * 8);
		}
	}

	//write address to register
	writel(addr0, P2WI_REG_DADDR0);
	writel(addr1, P2WI_REG_DADDR1);
	writel((len - 1) | (1 << 4), P2WI_REG_DLEN);

	//start transmit
	writel(readl(P2WI_REG_CTRL) | P2WI_START_TRANS, P2WI_REG_CTRL);
	//wait transfer complete
	if (standby_p2wi_wait_state() != 0)
	{
		return -1;
	}

	//read out data
	data0 = readl(P2WI_REG_DATA0);
	data1 = readl(P2WI_REG_DATA1);
	for (i = 0; i < len; i++)
	{
		if (i < 4)
		{
			data[i] = (data0 >> (i * 8)) & 0xff;
		}
		else
		{
			data[i] = (data1 >> ((i - 4) * 8)) & 0xff;
		}
	}

	return 0;
}


__s32 standby_p2wi_write(u8 *addr, u8 *data, u32 len)
{
	__u32 i;
	__u32 addr0 = 0, addr1 = 0;
	__u32 data0 = 0, data1 = 0;

	if((len >= PMU_TRANS_BYTE_MAX) || (len <= 0))
	{
		return -1;
	}

	for (i = 0; i < len; i++)
	{
		if (i < 4)
		{
			//pack 8bit data0~data3 into 32bit data0
			//pack 8bit addr0~addr3 into 32bit addr0
			addr0 |= addr[i] << (i * 8);
			data0 |= data[i] << (i * 8);
		}
		else
		{
			//pack 8bit data4~data7 into 32bit data1
			//pack 8bit addr4~addr7 into 32bit addr4
			addr1 |= addr[i] << ((i - 4) * 8);
			data1 |= data[i] << ((i - 4) * 8);
		}
	}

	//write register
	writel(addr0, P2WI_REG_DADDR0);
	writel(addr1, P2WI_REG_DADDR1);
	writel(data0, P2WI_REG_DATA0);
	writel(data1, P2WI_REG_DATA1);
	writel(len - 1, P2WI_REG_DLEN);

	//start transfer
	writel(readl(P2WI_REG_CTRL) | P2WI_START_TRANS, P2WI_REG_CTRL);
	//wait transfer complete
	if (standby_p2wi_wait_state() != 0)
	{
		return -1;
	}

	return 0;
}


int standby_axp_i2c_read(unsigned char chip, unsigned char addr, unsigned char *buffer)
{
	return standby_p2wi_read(&addr, buffer, 1);
}

int standby_axp_i2c_write(unsigned char chip, unsigned char addr, unsigned char data)
{
	return standby_p2wi_write(&addr, &data, 1);
}


