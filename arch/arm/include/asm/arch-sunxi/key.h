/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
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
