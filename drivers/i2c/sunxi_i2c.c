/*
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* This code should work for both the S3C2400 and the S3C2410
 * as they seem to have the same I2C controller inside.
 * The different address mapping is handled by the s3c24xx.h files below.
 */

#include <common.h>
#include <i2c.h>

#define	I2C_WRITE		0
#define I2C_READ		1

#define I2C_OK			0
#define I2C_NOK			1
#define I2C_NACK		2
#define I2C_NOK_LA		3	/* Lost arbitration */
#define I2C_NOK_TOUT	4	/* time out */

#define I2C_START_TRANSMIT   	0x08
#define I2C_RESTART_TRANSMIT 	0x10
#define I2C_ADDRWRITE_ACK		0x18
#define I2C_ADDRWRITE_NACK		0x20
#define I2C_DATAWRITE_ACK		0x28
#define I2C_DATAWRITE_NACK		0x30
#define I2C_ADDR_DATA_LOST		0x38
#define I2C_ADDRREAD_ACK		0x40
#define I2C_ADDRREAD_NACK		0x48
#define I2C_DATAREAD_ACK		0x50
#define I2C_DATAREAD_NACK		0x58
#define I2C_READY				0xf8


struct sw_iic
{
    volatile unsigned int addr;        /* slave address     */
    volatile unsigned int xaddr;       /* extend address    */
    volatile unsigned int data;        /* data              */
    volatile unsigned int ctl;         /* control           */
    volatile unsigned int status;      /* status            */
    volatile unsigned int clk;         /* clock             */
    volatile unsigned int srst;        /* soft reset        */
    volatile unsigned int eft;         /* enhanced future   */
};

typedef volatile struct sw_iic *sw_iic_t;

static sw_iic_t twi_ports[3] = {
	(sw_iic_t)SUNXI_TWI0_BASE,
	(sw_iic_t)SUNXI_TWI1_BASE,
	(sw_iic_t)SUNXI_TWI2_BASE
};

#define   i2c_ctl    twi_ports[0]

int  twi_index = 0;
/*
**********************************************************************************************************************
*                                               sw_iic_exit
*
* Description:  通过IIC控制器读取IIC设备一个字节，暂时只支持标准的设备
*
* Arguments  :
*
* Returns    :   读取成功返回0，否则返回 -1
*
* Notes      :    none
*
**********************************************************************************************************************
*/
static __s32 i2c_sendstart(void)
{
    __s32  time = 0xfffff;
    __u32  tmp_val;

    i2c_ctl->srst = 1;

	i2c_ctl->ctl |= 0x20;

    while((time--)&&(!(i2c_ctl->ctl & 0x08)));
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}

	tmp_val = i2c_ctl->status;
    if(tmp_val != I2C_START_TRANSMIT)
    {
		return -I2C_START_TRANSMIT;
    }

    return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               TWIC_SendReStart
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 i2c_sendRestart(void)
{
    __s32  time = 0xffff;
    __u32  tmp_val;

	tmp_val = i2c_ctl->ctl & 0xC0;
	tmp_val |= 0x20;
	i2c_ctl->ctl = tmp_val;

    while( (time--) && (!(i2c_ctl->ctl & 0x08)) );
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}

	tmp_val = i2c_ctl->status;
    if(tmp_val != I2C_RESTART_TRANSMIT)
    {
		return -I2C_RESTART_TRANSMIT;
    }

    return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               TWIC_SendSlaveAddr
*
* Description:
*
* Arguments  :
*
* Returns    :    EPDK_OK = successed;   EPDK_FAIL = failed
*
* Notes      :     none
*
**********************************************************************************************************************
*/
static __s32 i2c_sendslaveaddr(__u32 saddr,  __u32 rw)
{
    __s32  time = 0xffff;
    __u32  tmp_val;

	rw &= 1;
	i2c_ctl->data = ((saddr & 0xff) << 1) | rw;
	i2c_ctl->ctl &=  0xF7;

	while(( time-- ) && (!( i2c_ctl->ctl & 0x08 )));
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}

	tmp_val = i2c_ctl->status;
	if(rw == I2C_WRITE)//+write
	{
		if(tmp_val != I2C_ADDRWRITE_ACK)
		{
			return -I2C_ADDRWRITE_ACK;
		}
	}
	else//+read
	{
		if(tmp_val != I2C_ADDRREAD_ACK)
		{
			return -I2C_ADDRREAD_ACK;
		}
	}

	return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               i2c_SendByteAddr
*
* Description:
*
* Arguments  :
*
* Returns    :    EPDK_OK = successed;   EPDK_FAIL = failed
*
* Notes      :     none
*
**********************************************************************************************************************
*/
static __s32 i2c_sendbyteaddr(__u32 byteaddr)
{
    __s32  time = 0xffff;
    __u32  tmp_val;

	i2c_ctl->data = byteaddr & 0xff;
	i2c_ctl->ctl &= 0xF7;

	while( (time--) && (!(i2c_ctl->ctl & 0x08)) );
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}

	tmp_val = i2c_ctl->status;
	if(tmp_val != I2C_DATAWRITE_ACK)
	{
		return -I2C_DATAWRITE_ACK;
	}

	return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               TWIC_GetData
*
* Description:
*
* Arguments  :
*
* Returns    :    EPDK_OK = successed;   EPDK_FAIL = failed
*
* Notes      :     none
*
**********************************************************************************************************************
*/
static __s32 i2c_getdata(__u8 *data_addr, __u32 data_count)
{
    __s32  time = 0xffff;
    __u32  tmp_val, i;

	if(data_count == 1)
	{
		i2c_ctl->ctl &= 0xF7;

		while( (time--) && (!(i2c_ctl->ctl & 0x08)) );
		if(time <= 0)
		{
			return -I2C_NOK_TOUT;
		}
		for(time=0;time<100;time++);
		*data_addr = i2c_ctl->data;

		tmp_val = i2c_ctl->status;
		if(tmp_val != I2C_DATAREAD_NACK)
		{
			return -I2C_DATAREAD_NACK;
		}
	}
	else
	{
		for(i=0; i< data_count - 1; i++)
		{
			time = 0xffff;
			tmp_val = i2c_ctl->ctl & 0xF7;
			tmp_val |= 0x04;
		    i2c_ctl->ctl = tmp_val;

			while( (time--) && (!(i2c_ctl->ctl & 0x08)) );
			if(time <= 0)
			{
				return -I2C_NOK_TOUT;
			}
			for(time=0;time<100;time++);
			time = 0xffff;
			data_addr[i] = i2c_ctl->data;
		    while( (time--) && (i2c_ctl->status != I2C_DATAREAD_ACK) );
			if(time <= 0)
			{
				return -I2C_NOK_TOUT;
			}
		}

        time = 0xffff;
		i2c_ctl->ctl &= 0xF3;
		while( (time--) && (!(i2c_ctl->ctl & 0x08)) );
		if(time <= 0)
		{
			return -I2C_NOK_TOUT;
		}
		for(time=0;time<100;time++);
		data_addr[data_count - 1] = i2c_ctl->data;
	    while( (time--) && (i2c_ctl->status != I2C_DATAREAD_NACK) );
		if(time <= 0)
		{
			return -I2C_NOK_TOUT;
		}
	}

	return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               i2c_SendData
*
* Description:
*
* Arguments  :
*
* Returns    :    EPDK_OK = successed;   EPDK_FAIL = failed
*
* Notes      :     none
*
**********************************************************************************************************************
*/
static __s32 i2c_senddata(__u8  *data_addr, __u32 data_count)
{
    __s32  time = 0xffff;
    __u32  i;

	for(i=0; i< data_count; i++)
	{
		time = 0xffff;
		i2c_ctl->data = data_addr[i];
		i2c_ctl->ctl &= 0xF7;

		while( (time--) && (!(i2c_ctl->ctl & 0x08)) );
		if(time <= 0)
		{
			return -I2C_NOK_TOUT;
		}
		time = 0xffff;
		while( (time--) && (i2c_ctl->status != I2C_DATAWRITE_ACK) );
		if(time <= 0)
		{
			return -I2C_NOK_TOUT;
		}
	}

	return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               i2c_SendSlaveAddr10
*
* Description:
*
* Arguments  :
*
* Returns    :    EPDK_OK = successed;   EPDK_FAIL = failed
*
* Notes      :     none
*
**********************************************************************************************************************
*/
__s32 i2c_sendslaveaddr10(__u32  slave_addr,  __u32  rw)
{
	__s32 ret;

	ret = i2c_sendslaveaddr(0x78 | ((slave_addr >> 8) & 0x03), rw);
	if(ret)
	{
		return ret;
	}
	ret = i2c_senddata((__u8 *)&slave_addr, I2C_READ);
	if(ret)
	{
		return ret;
	}

	return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               i2c_Stop
*
* Description:
*
* Arguments  :
*
* Returns    :    EPDK_OK = successed;   EPDK_FAIL = failed
*
* Notes      :     none
*
**********************************************************************************************************************
*/
__s32 i2c_stop(void)
{
    __s32  time = 0xffff;
    __u32  tmp_val;

	tmp_val = (i2c_ctl->ctl & 0xC0) | 0x10;
	i2c_ctl->ctl = tmp_val;
	while( (time--) && (i2c_ctl->ctl & 0x10) );
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}
	time = 0xffff;
	while( (time--) && (i2c_ctl->status != I2C_READY) );
	tmp_val = i2c_ctl->status;
	if(tmp_val != I2C_READY)
	{
		return -I2C_NOK_TOUT;
	}

	return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               i2c_setfreq
*
* Description:    SET I2C controller Frequency
*
* Arguments  :    swi2c_t i2c_ctl
*
* Returns    :    0 = successed;   -1 = failed
*
* Notes      :    none
*
**********************************************************************************************************************
*/
__s32  i2c_setfreq(void)
{
	__u32 system_clock, twi_run_clock;
    __u32 clk_m        = 0;
    __u32 clk_n        = 0;
    __u32 src_clk      = system_clock / 10;
    __u32 _2_pow_clk_n = 1;
    __u32 sclk_real    = 0;
    __u32 divider      = src_clk / twi_run_clock;

	system_clock = 24000000;
	twi_run_clock = 400000;
	if (system_clock <= 32000)
	{
		clk_m = 1;
        clk_n = 0;
	}
    else if (system_clock <= 24000000)        //不到24M的情况
    {
        clk_m = 2;
        clk_n = 1;
    }
    else
    {
        while (clk_n < 8)
        {
            clk_m = (divider/_2_pow_clk_n) - 1;
            while (clk_m < 16)
            {
                sclk_real = src_clk/(clk_m+1)/_2_pow_clk_n;
                if (sclk_real <= twi_run_clock)
                    goto set_clk;
                else
                    clk_m++;
            }

            clk_n++;
            _2_pow_clk_n <<= 1;
        }
    }

set_clk:
    i2c_ctl->clk = (clk_m<<3) | clk_n;
    i2c_ctl->ctl = 0x40;
    i2c_ctl->eft = 0;

    return I2C_OK;
}
/*
**********************************************************************************************************************
*                                               swi2c_init
*
* Description:    Init I2C controller
*
* Arguments  :    swi2c_t i2c_ctl
*
* Returns    :    0 = successed;   -1 = failed
*
* Notes      :    none
*
**********************************************************************************************************************
*/
__s32 i2c_init(void)
{
    __s32   reset_delay;
	__u32   reg_val;

	reg_val = __REG(0x1c20000 + 0x6C);
    reg_val &= ~(1 << (0 + twi_index));
    __REG(0x1c20000 + 0x6C) = reg_val;
    msdelay(1);
	reg_val = __REG(0x1c20000 + 0x6C);
    reg_val |=  (1 << (0 + twi_index));
    __REG(0x1c20000 + 0x6C) = reg_val;

    reset_delay = 0xffff;
    i2c_ctl->srst = 1;
    while((i2c_ctl->srst) && (reset_delay--));
    if(reset_delay <= 0)
    {
        return -1;
    }

    return i2c_setfreq();
}
/*
**********************************************************************************************************************
*                                               swi2c_exit
*
* Description:  exit I2C controller
*
* Arguments  :   swi2c_t i2c_ctl
*
* Returns    :   0 = successed;   -1 = failed
*
* Notes      :    none
*
**********************************************************************************************************************
*/
__s32 i2c_exit(void)
{
	__u32 reg_val;

	reg_val = __REG(0x1c20000 + 0x6C);
	reg_val &= ~(1 << (0 + twi_index));
	__REG(0x1c20000 + 0x6C) = reg_val;

    return I2C_OK;
}
/*
****************************************************************************************************
*
*                                       i2c_read
*
*  Description:
*
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    __u32 tmp, rw_flag;
    __s32 i;
    __s32 ret = -1;
    i2c_parameters  arg;

	arg.if_restart 		= 1;
	arg.slave_addr 		= chip;
	arg.slave_addr_flag = 0;
	arg.byte_addr  		= (__u8 *)&addr;
	arg.byte_addr_width = alen;
	arg.byte_count 		= len;
	arg.data       		= buffer;
    if(arg.if_restart)
    {
        tmp = 0;
        rw_flag  = I2C_WRITE;
    }
    else if(arg.byte_addr_width <= 3)
    {
        tmp = arg.byte_addr_width;
        rw_flag  = I2C_READ;
    }
    else
    {
        return -1;
    }

    i2c_ctl->eft = tmp;
    ret = i2c_sendstart();
    if(ret)
	{
		goto i2c_read_stop;
	}
	if(arg.slave_addr_flag)//10 bit saddr
	{
    	ret = i2c_sendslaveaddr10(arg.slave_addr, rw_flag);
    	if(ret)
    	{
    		goto i2c_read_stop;
    	}
	}
	else
	{
	    ret = i2c_sendslaveaddr(arg.slave_addr, rw_flag);
	    if(ret)
	    {
		    goto i2c_read_stop;
    	}
    }
    //send byte address
    for (i = arg.byte_addr_width-1; i>=0; i--)
    {
    	ret = i2c_sendbyteaddr(arg.byte_addr[i] & 0xff);
    	if(ret)
    	{
    		goto i2c_read_stop;
    	}
    }
    //restart
    if(arg.if_restart)
    {
        ret = i2c_sendRestart();
        if(ret)
    	{
    		goto i2c_read_stop;
    	}
        //send slave address + read flag
    	if(arg.slave_addr_flag)//10bit saddr
    	{
        	ret = i2c_sendslaveaddr10(arg.slave_addr, I2C_READ);
        	if(ret)
        	{
        		goto i2c_read_stop;
        	}
    	}
    	else
    	{
    	    ret = i2c_sendslaveaddr(arg.slave_addr, I2C_READ);
    	    if(ret)
    	    {
		        goto i2c_read_stop;
        	}
        }
    }
    //get data
	ret = i2c_getdata(arg.data, arg.byte_count);
	if(ret)
	{
		goto i2c_read_stop;
	}
    ret = 0;

i2c_read_stop:
	i2c_stop();

	//restore the byte address counter to default
	return ret;
}

/*
****************************************************************************************************
*
*             TWIC_Write
*
*  Description:
*       DRV_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    __s16 i;
    __s32 ret = -1;
    i2c_parameters  arg;

	arg.if_restart 		= 1;
	arg.slave_addr 		= chip;
	arg.slave_addr_flag 	= 0;
	arg.byte_addr  		= (__u8 *)&addr;
	arg.byte_addr_width 	= alen;
	arg.byte_count 		= len;
	arg.data       		= buffer;

    i2c_ctl->eft = 0;
	ret = i2c_sendstart();
	if(ret)
	{
		goto i2c_write_stop;
	}
	if(arg.slave_addr_flag)
	{
    	ret = i2c_sendslaveaddr10(arg.slave_addr, I2C_WRITE);
    	if(ret)
    	{
    		goto i2c_write_stop;
    	}
	}
	else
	{
	    ret = i2c_sendslaveaddr(arg.slave_addr, I2C_WRITE);
	    if(ret)
	    {
    		goto i2c_write_stop;
    	}
    }

    for (i = arg.byte_addr_width-1; i >= 0; i--)
    {
    	ret = i2c_sendbyteaddr(arg.byte_addr[i] & 0xff);
    	if(ret)
    	{
    		goto i2c_write_stop;
    	}
    }

	ret = i2c_senddata(arg.data, arg.byte_count);
	if(ret)
	{
		goto i2c_write_stop;
	}
    ret = 0;

i2c_write_stop:
	i2c_stop();

	return ret;
}

