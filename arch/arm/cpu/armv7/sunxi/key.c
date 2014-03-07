/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/key.h>
#include <asm/arch/sys_proto.h>

int sunxi_key_init(void)
{
	struct sunxi_lradc *sunxi_key_base =
		(struct sunxi_lradc *)SUNXI_LRADC_BASE;

	sr32(&sunxi_key_base->ctrl, 0, 1, LRADC_EN);
	sr32(&sunxi_key_base->ctrl, 2, 2, LRADC_SAMPLE_RATE);
	sr32(&sunxi_key_base->ctrl, 4, 2, LEVELB_VOL);
	sr32(&sunxi_key_base->ctrl, 6, 1, LRADC_HOLD_EN);
	sr32(&sunxi_key_base->ctrl, 12, 2, KEY_MODE_SELECT);

	/* disable all key irq */
	writel(0x0, &sunxi_key_base->intc);
	/* clear all key pending */
	writel(0xffffffff, &sunxi_key_base->ints);

	return 0;
}

u32 sunxi_read_key(void)
{
	u32 ints;
	u32 key = 0;
	struct sunxi_lradc *sunxi_key_base =
		(struct sunxi_lradc *)SUNXI_LRADC_BASE;

	ints = readl(&sunxi_key_base->ints);

	/* if there is already data pending,
	 read it */
	if (ints & ADC0_DATA_PENDING) {
		key = readl(&sunxi_key_base->data0);
#ifdef DEBUG
		printf("key pressed, value=0x%x\n", key);
#endif
	}
	/* clear the pending data */
	writel(ints, &sunxi_key_base->ints);
	return key;
}
