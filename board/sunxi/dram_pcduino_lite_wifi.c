// place this file in board/sunxi/ in u-boot
/* this file is generated, don't edit it yourself */

#include "common.h"
#include <asm/arch/dram.h>

static struct dram_para dram_para = {
	.clock = 408,
	.type = 3,
	.rank_num = 1,
	.density = 2048,
	.io_width = 16,
	.bus_width = 16,
	.cas = 6,
	.zq = 0x7b,
	.odt_en = 0,
	.size = 256,
	.tpr0 = 0x30926692,
	.tpr1 = 0x1090,
	.tpr2 = 0x1a0c8,
	.tpr3 = 0x00,
	.tpr4 = 0x00,
	.tpr5 = 0x00,
	.emr1 = 0x00,
	.emr2 = 0x00,
	.emr3 = 0x00,
};

unsigned long sunxi_dram_init(void)
{
	return dramc_init(&dram_para);
}
