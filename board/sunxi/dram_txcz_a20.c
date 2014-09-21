/* this file is generated, don't edit it yourself */

#include "common.h"
#include <asm/arch/dram.h>

static struct dram_para dram_para = {
	.clock = 384,
	.type = 3,
	.rank_num = 1,
	.density = 2048,
	.io_width = 16,
	.bus_width = 32,
	.cas = 9,
	.zq = 0x7f,
	.odt_en = 0,
	.size = 512,
	.tpr0 = 0x38d48893,
	.tpr1 = 0xa0a0,
	.tpr2 = 0x22a00,
	.tpr3 = 0,
	.tpr4 = 0,
	.tpr5 = 0,
	.emr1 = 0x4,
	.emr2 = 0x10,
	.emr3 = 0,
};

unsigned long sunxi_dram_init(void)
{
	return dramc_init(&dram_para);
}
