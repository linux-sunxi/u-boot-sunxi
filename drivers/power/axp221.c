/*
 * (C) Copyright 2013 Oliver Schinagl <oliver@schinagl.nl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <asm/arch/p2wi.h>
#include <axp221.h>

int axp221_set_dcdc1(unsigned int mvolt)
{
	return p2wi_write(AXP221_DCDC1_CTRL, (mvolt - 1600) / 100);
}

int axp221_set_dcdc2(unsigned int mvolt)
{
	return p2wi_write(AXP221_DCDC2_CTRL, (mvolt - 600) / 20);
}

int axp221_set_dcdc3(unsigned int mvolt)
{
	return p2wi_write(AXP221_DCDC3_CTRL, (mvolt - 600) / 20);
}

int axp221_set_dcdc4(unsigned int mvolt)
{
	return p2wi_write(AXP221_DCDC4_CTRL, (mvolt - 600) / 20);
}

int axp221_set_dcdc5(unsigned int mvolt)
{
	return p2wi_write(AXP221_DCDC5_CTRL, (mvolt - 1000) / 50);
}

int axp221_set_dldo1(unsigned int mvolt)
{
	int ret;
	u8 val;

	ret = p2wi_write(AXP221_DLDO1_CTRL, (mvolt - 700) / 100);
	if (ret)
		return ret;

	ret = p2wi_read(AXP221_OUTPUT_CTRL2, &val);
	if (ret)
		return ret;

	val |= 1 << 3;
	return p2wi_write(AXP221_OUTPUT_CTRL2, val);
}

int axp221_init(void)
{
	u8 axp_chip_id;
	int ret;

	p2wi_init();
	ret = p2wi_set_pmu_address(AXP221_CHIP_ADDR, AXP221_CTRL_ADDR,
				   AXP221_INIT_DATA);
	if (ret)
		return ret;

	ret = p2wi_read(AXP221_CHIP_ID, &axp_chip_id);
	if (ret)
		return ret;

	if (!(axp_chip_id == 0x6 || axp_chip_id == 0x7 || axp_chip_id == 0x17))
		return -ENODEV;

	return 0;
}
