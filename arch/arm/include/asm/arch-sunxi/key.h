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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SUNXI_KEY_H
#define _SUNXI_KEY_H

#include <linux/types.h>

struct sunxi_lradc {
	u32 ctrl;         /* lradc control */
	u32 intc;         /* interrupt control */
	u32 ints;         /* interrupt status */
	u32 data0;        /* lradc 0 data */
	u32 data1;        /* lradc 1 data */
};

#define LRADC_EN                  0x1    /* LRADC enable */
#define LRADC_SAMPLE_RATE         0x2    /* 32.25 Hz */
#define LEVELB_VOL                0x2    /* 0x33(~1.6v) */
#define LRADC_HOLD_EN             0x1    /* sample hold enable */
#define KEY_MODE_SELECT           0x0    /* normal mode */

#define ADC0_DATA_PENDING         (0x1 << 0)    /* adc0 has data */
#define ADC0_KEYDOWN_PENDING      (0x1 << 1)    /* key down */
#define ADC0_HOLDKEY_PENDING      (0x1 << 2)    /* key hold */
#define ADC0_ALRDY_HOLD_PENDING   (0x1 << 3)    /* key already hold */
#define ADC0_KEYUP_PENDING        (0x1 << 4)    /* key up */

int sunxi_key_init(void);
u32 sunxi_read_key(void);

#endif
