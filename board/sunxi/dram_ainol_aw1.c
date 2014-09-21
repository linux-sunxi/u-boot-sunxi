/* this file is generated, don't edit it yourself */

#include "common.h"
#include <asm/arch/dram.h>

static struct dram_para dram_para = {
	.clock = 432,
	.type = 3,
	.rank_num = 1,
	.density = 2048,
	.io_width = 16,
	.bus_width = 32,
	.cas = 9,
	.zq = 0x7b,
	.odt_en = 0,
	.size = 512,
	.tpr0 = 0x42d899b7,
	.tpr1 = 0xa090,
	.tpr2 = 0x22a00,
	.tpr3 = 0,
	.tpr4 = 1,
	.tpr5 = 0,
	.emr1 = 0x4,
	.emr2 = 0x10,
	.emr3 = 0,
};

unsigned long sunxi_dram_init(void)
{
	return dramc_init(&dram_para);
}
