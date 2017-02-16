/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
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
#ifndef 	__NAND_OAL__
#define  	__NAND_OAL__


#include <common.h>
#include <asm/arch/dma.h>


//define the memory set interface
#define MEMSET(x,y,z)                   	memset(x,y,z)

//define the memory copy interface
#define MEMCPY(x,y,z)                   	memcpy(x,y,z)

//define the memory alocate interface
#define MALLOC(x)                       	malloc(x)

//define the memory release interface
#define FREE(x,size)                   		free(x)

//define the message print interface
#define PRINT							    printf
//#define PRINT(...)

#endif
