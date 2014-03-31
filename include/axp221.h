/*
 * (C) Copyright 2013 Oliver Schinagl <oliver@schinagl.nl>
 *
 * X-Powers AXP221 Power Management IC driver
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#define AXP221_CHIP_ADDR 0x68
#define AXP221_CTRL_ADDR 0x3e
#define AXP221_INIT_DATA 0x3e

#define AXP221_CHIP_ID		0x03
#define AXP221_OUTPUT_CTRL1	0x10
#define AXP221_OUTPUT_CTRL2	0x12
#define AXP221_OUTPUT_CTRL3	0x13
#define AXP221_DLDO1_CTRL	0x15
#define AXP221_DCDC1_CTRL	0x21
#define AXP221_DCDC2_CTRL	0x22
#define AXP221_DCDC3_CTRL	0x23
#define AXP221_DCDC4_CTRL	0x24
#define AXP221_DCDC5_CTRL	0x25

int axp221_set_dcdc1(unsigned int mvolt);
int axp221_set_dcdc2(unsigned int mvolt);
int axp221_set_dcdc3(unsigned int mvolt);
int axp221_set_dcdc4(unsigned int mvolt);
int axp221_set_dcdc5(unsigned int mvolt);
int axp221_set_dldo1(unsigned int mvolt);
int axp221_init(void);
