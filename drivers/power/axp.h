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
#ifndef   __AXP_H__
#define   __AXP_H__

#include <i2c.h>

#define  AXP_POWER_ON_BY_POWER_KEY       0
#define  AXP_POWER_ON_BY_POWER_TRIGGER   1


#define  AXP_POWER_NULL					 0
#define  AXP_POWER_PMU209				 1



//static inline int axp_i2c_read(unsigned char chip, unsigned int addr, unsigned char *buffer)
//{
//	return i2c_read(chip, addr, 1, buffer, 1);
//}
//
//static inline int axp_i2c_write(unsigned char chip, unsigned int addr, unsigned char data)
//{
//	return i2c_write(chip, addr, 1, &data, 1);
//}

static inline int axp_i2c_read(unsigned char chip, unsigned int addr, unsigned char *buffer)
{
	return p2wi_read(&addr, buffer, 1);
}

static inline int axp_i2c_write(unsigned char chip, unsigned int addr, unsigned char data)
{
	return p2wi_write(&addr, &data, 1);
}

static inline int abs(int x)
{
	return x>0?x:(-x);
}



#endif /* __AXP_H__ */
