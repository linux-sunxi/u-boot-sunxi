/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * panlong <panlong@allwinnertech.com>
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

#ifndef _BOOT_TYPE_H_
#define _BOOT_TYPE_H_

#include "private_uboot.h"

extern struct spare_boot_head_t  uboot_spare_head;

extern int uart_console;

extern int sunxi_flash_read (unsigned int start_block, unsigned int nblock, void *buffer);
extern int sunxi_flash_write(unsigned int start_block, unsigned int nblock, void *buffer);
extern uint sunxi_flash_size (void);
extern int sunxi_flash_exit (void);

#endif
