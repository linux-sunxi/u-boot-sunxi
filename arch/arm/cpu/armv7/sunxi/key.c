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
#include <asm/arch/cpu.h>
#include <asm/arch/key.h>
#include <asm/arch/sys_proto.h>

#undef SUNXI_KEY_DEBUG

static struct sunxi_lradc *sunxi_key_base = (struct sunxi_lradc *)SUNXI_LRADC_BASE;

int sunxi_key_init(void) {

	sr32(&sunxi_key_base->ctrl, 0, 1, LRADC_EN);
	sr32(&sunxi_key_base->ctrl, 2, 2, LRADC_SAMPLE_RATE);
	sr32(&sunxi_key_base->ctrl, 4, 2, LEVELB_VOL);
	sr32(&sunxi_key_base->ctrl, 6, 1, LRADC_HOLD_EN);
	sr32(&sunxi_key_base->ctrl, 12, 2, KEY_MODE_SELECT);

	/* disable all key irq */
	writel(0x0, &sunxi_key_base->intc);
	/* clear all key pending */
	writel(0xFFFFFFFF, &sunxi_key_base->ints);

	return 0;
}

u32 sunxi_read_key(void) {

	u32 ints;
	u32 key = 0;

	ints = readl(&sunxi_key_base->ints);

	/* if there is already data pending,
	 read it */
	if( ints & ADC0_DATA_PENDING) {
		key = readl(&sunxi_key_base->data0);
#ifdef DEBUG
		printf("key pressed, value=0x%x\n", key);
#endif
	}
	/* clear the pending data */
	writel(ints, &sunxi_key_base->ints);
	return key;
}

/* check if one key is recovery key*/
int sunxi_check_recovery(u32 key) {
	int key_max = 0 , key_min = 0 ;

#ifdef SUNXI_KEY_DEBUG
	printf("recovery key max: %s\n", getenv("recovery_key_value_max"));
	printf("recovery key min: %s\n", getenv("recovery_key_value_min"));
#endif
	key_max = simple_strtoul(getenv("recovery_key_value_max"), NULL, 16);
	key_min = simple_strtoul(getenv("recovery_key_value_min"), NULL, 16);

#ifdef SUNXI_KEY_DEBUG
	printf("key: 0x%x ", key);
	printf("key_max: 0x%x ", key_max);
	printf("key_min: 0x%x \n", key_min);
#endif

	if(key && key_max && key_min) {
		if(key <= key_max && key >= key_min)
			return 1;
	}

	return 0;
}

/* check if one key is fastboot key */
int sunxi_check_fastboot(u32 key) {
	int key_max = 0 , key_min = 0 ;

#ifdef SUNXI_KEY_DEBUG
	printf("fastboot key max: %s\n", getenv("fastboot_key_value_max"));
	printf("fastboot key min: %s\n", getenv("fastboot_key_value_min"));
#endif
	key_max = simple_strtoul(getenv("fastboot_key_value_max"), NULL, 16);
	key_min = simple_strtoul(getenv("fastboot_key_value_min"), NULL, 16);

#ifdef SUNXI_KEY_DEBUG
	printf("key: 0x%x ", key);
	printf("key_max: 0x%x ", key_max);
	printf("key_min: 0x%x \n", key_min);
#endif

	if(key && key_max && key_min) {
		if(key <= key_max && key >= key_min)
			return 1;
	}

	return 0;
}

int sunxi_key_hexdump(void) {

	puts("--lradc registers dump--\n");
	printf("ctrl  : 0x%x\n", readl(&sunxi_key_base->ctrl));
	printf("intc  : 0x%x\n", readl(&sunxi_key_base->intc));
	printf("ints  : 0x%x\n", readl(&sunxi_key_base->ints));
	printf("data0 : 0x%x\n", readl(&sunxi_key_base->data0));
	printf("data1 : 0x%x\n", readl(&sunxi_key_base->data1));
	puts("------------------------\n");

	return 0;
}

int do_key_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {

	puts("press a key:\n");
	writel(0xFFFFFFFF, &sunxi_key_base->ints);

	while(1) {
		if(sunxi_read_key()) {
			sunxi_key_hexdump();
			break;
		}
	}

	return 0;

}

U_BOOT_CMD(
	key_test, 1, 0,	do_key_test,
	"Test the key value and dump key registers",
	""
);
