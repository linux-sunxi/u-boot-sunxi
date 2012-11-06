/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>

/*
************************************************************************************************************
*
*                                             sunxi_set_gpio_all
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_set_gpio_all(void  *user_gpio_list, u32 group_count_max)
{
	normal_gpio_cfg    *user_gpio_data;
	struct sunxi_gpio  *gpio_group;
	int  				port_first, port_last;
	int                 port_num_first, port_num_last;
	int			        actual_port;
	uint                port_func_cfg, port_pull_cfg, port_drv_cfg;
	int                 port_num_other,port_num_func, last_port_num_other;
	int                 other_cfg_ready, this_max_port_num;
	int                 pull_changed, drv_changed;
	int                 tmp_cfg_offset, tmp_other_offset;
	int                 i, j;

	//judge the count of valid gpio
	user_gpio_data = (normal_gpio_cfg *)user_gpio_list;
	for(i=0;i<group_count_max;i++)
	{
		if(!user_gpio_data->port)
		{
			break;
		}
		user_gpio_data ++;
		actual_port ++;
	}
	actual_port = i;
	if(actual_port <= 0)
	{
		return -1;
	}

	user_gpio_data = (normal_gpio_cfg *)user_gpio_list;
	port_first     = user_gpio_data->port;
	port_num_first = user_gpio_data->port_num;

	user_gpio_data += actual_port - 1;
	port_last      = user_gpio_data->port;
	port_num_last  = user_gpio_data->port_num;

	if(port_first != port_last)
	{
		return -1;
	}
	gpio_group = (struct sunxi_gpio *)(SUNXI_PIO_BASE + sizeof(struct sunxi_gpio) * (port_first - 1));

	other_cfg_ready = -1;
	last_port_num_other = -1;
	pull_changed = 0;
	drv_changed = 0;
	user_gpio_data = (normal_gpio_cfg *)user_gpio_list;

	for(i = port_num_first; i < port_num_last;)
    {
        port_num_func  = (i >> 3);
        port_num_other = (i >> 4);
        if(last_port_num_other != port_num_other)
        {
            last_port_num_other = port_num_other;
            if(other_cfg_ready != -1)
            {
                if(pull_changed)
                {
                    gpio_group->pull[port_num_other] = port_pull_cfg;
                    pull_changed = 0;
                }
                if(drv_changed)
                {
                    gpio_group->drv[port_num_other] = port_drv_cfg;
                    drv_changed = 0;
                }
            }
            other_cfg_ready = 0;
        }

		port_func_cfg  = gpio_group->cfg[port_num_func];
		if(!other_cfg_ready)
		{
			port_pull_cfg   = gpio_group->pull[port_num_other];
			port_drv_cfg    = gpio_group->drv[port_num_other];
			other_cfg_ready = 1;
		}

		this_max_port_num = (i & (~0x07)) + 7;
		if(this_max_port_num  > port_num_last)
		{
			this_max_port_num = port_num_last;
		}

		for(j= i; j <= this_max_port_num; j ++)
		{
		    if(user_gpio_data->port_num != j)
		    {
		        continue;        /* break this gpio */
		    }
			tmp_cfg_offset   = j - (port_num_func<<3);
			tmp_other_offset = j - (port_num_other<<4);
		    port_func_cfg   &= ~(0x07 << (tmp_cfg_offset * 4));
			port_func_cfg   |= user_gpio_data->mul_sel << (tmp_cfg_offset * 4);

			if((user_gpio_data->pull >= 0) && (user_gpio_data->pull <= 2))
			{
				port_pull_cfg &= ~(0x03 << (tmp_other_offset * 2));
				port_pull_cfg |= user_gpio_data->pull << (tmp_other_offset * 2);
				pull_changed = 1;
			}
			if((user_gpio_data->drv_level>= 0) && (user_gpio_data->drv_level<= 2))
			{
				port_drv_cfg &= ~(0x03 << (tmp_other_offset * 2));
				port_drv_cfg |= user_gpio_data->drv_level << (tmp_other_offset * 2);
				drv_changed = 1;
			}
			user_gpio_data ++;
		}

		gpio_group->cfg[port_num_func] = port_func_cfg;
		i = this_max_port_num;
	}

	if(other_cfg_ready != -1)
	{
		if(pull_changed)
		{
			gpio_group->pull[port_num_other] = port_pull_cfg;
			pull_changed = 0;
		}
		if(drv_changed)
		{
			gpio_group->drv[port_num_other] = port_drv_cfg;
			drv_changed = 0;
		}
	}

    return 0;
}






