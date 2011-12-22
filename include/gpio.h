/*
 *==========================================================================
 *
 *      crc.h
 *
 *      Interface for the CRC algorithms.
 *
 *==========================================================================
 *####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating System.
 * Copyright (C) 2002 Andrew Lunn
 *
 * eCos is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 *
 * eCos is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 *
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 *
 * Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
 * at http: *sources.redhat.com/ecos/ecos-license/
 * -------------------------------------------
 *####ECOSGPLCOPYRIGHTEND####
 *==========================================================================
 *#####DESCRIPTIONBEGIN####
 *
 * Author(s):    Andrew Lunn
 * Contributors: Andrew Lunn
 * Date:         2002-08-06
 * Purpose:
 * Description:
 *
 * This code is part of eCos (tm).
 *
 *####DESCRIPTIONEND####
 *
 *==========================================================================
 */

#ifndef _SERVICES_GPIO_H_
#define _SERVICES_GPIO_H_

#include <linux/types.h>


typedef struct
{
    char  gpio_name[32];
    int port;
    int port_num;
    int mul_sel;
    int pull;
    int drv_level;
    int data;
}
user_gpio_set_t;
/*
************************************************************************************************************
*
*                                             gpio_init
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ： GPIO管理初始化，传递一个GPIO基地址
*
*
************************************************************************************************************
*/
extern  int      gpio_script_init(void);
/*
************************************************************************************************************
*
*                                             gpio_exit
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：  GPIO管理退出，无操作，直接退出
*
*
************************************************************************************************************
*/
extern  int      gpio_script_exit(void);
/*
************************************************************************************************************
*
*                                             gpio_Request
*
*    函数名称：
*
*    参数列表： gpio_list 用户数据地址，用于传递用户的GPIO数据信息
*               group_count_max  用户数据的个数。这个数值应该大于或者等于用户实际的GPIO个数
*
*    返回值  ： 申请成功，返回一个句柄。否则返回0值
*
*    说明    ： GPIO请求。用户数据按照结构体传递，一个结构体保存一个GPIO信息。
*
*
************************************************************************************************************
*/
extern  unsigned gpio_request                 (user_gpio_set_t *gpio_list,                                                              unsigned group_count_max               );
extern  unsigned gpio_request_ex(char *main_name, const char *sub_name);  //设备申请GPIO函数扩展接口
/*
************************************************************************************************************
*
*                                             gpio_Release
*
*    函数名称：
*
*    参数列表：  p_handler  申请到的句柄
*                if_release_to_default_status   释放后的状态。可以释放后状态不变，可以变成全输入状态，可以变成申请前的状态
*
*    返回值  ：
*
*    说明    ： 用户不再使用GPIO，释放掉。
*
*
************************************************************************************************************
*/
extern  int      gpio_release                 (unsigned p_handler,                                                                      int if_release_to_default_status       );
/*
************************************************************************************************************
*
*                                             gpio_get_all_pin_status
*
*    函数名称：
*
*    参数列表： p_handler  申请到的句柄
*               gpio_status  保存用户数据的地址
*               gpio_count_max 保存用户数据的结构体的个数，这个数值应该大于或者等于实际的GPIO个数
*               if_get_from_hardware  希望获取到的GPIO信息来源于实际的寄存器状态，或者是
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
extern  int      gpio_get_all_pin_status(unsigned p_handler, user_gpio_set_t *gpio_status, unsigned gpio_count_max, unsigned if_get_from_hardware);
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
extern  int      gpio_get_one_pin_status      (unsigned p_handler, user_gpio_set_t *gpio_status,             const char *gpio_name,     unsigned if_get_from_hardware          );
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
extern  int      gpio_set_one_pin_status      (unsigned p_handler, user_gpio_set_t *gpio_status,             const char *gpio_name,     unsigned if_set_to_current_input_status);
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
extern  int      gpio_set_one_pin_io_status   (unsigned p_handler, unsigned         if_set_to_output_status, const char *gpio_name                                             );
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
extern  int      gpio_set_one_pin_pull        (unsigned p_handler, unsigned         set_pull_status,         const char *gpio_name                                             );
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
extern  int      gpio_set_one_pin_driver_level(unsigned p_handler, unsigned         set_driver_level,        const char *gpio_name                                             );
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
extern  int      gpio_read_one_pin_value      (unsigned p_handler,                                           const char *gpio_name                                             );
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
extern  int      gpio_write_one_pin_value     (unsigned p_handler, unsigned         value_to_gpio,           const char *gpio_name                                             );





#endif /* _SERVICES_CRC_CRC_H_ */
