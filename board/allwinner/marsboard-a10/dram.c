/*
 * MarsBoard <marsboard@gmail.com>
 *
 * Some board init for the Allwinner A10-evb board.
 *
 */

#include <common.h>
#include <asm/arch/dram.h>

static struct dram_para dram_para = {
	.clock = 408,
	.type = 3,
	.rank_num = 1,
	.density = 4096,
	.io_width = 16,
	.bus_width = 32,
	.cas = 6,
	.zq = 123,
	.odt_en = 1,
	.size = 1024,
	.tpr0 = 0x30926692,
	.tpr1 = 0x1090,
	.tpr2 = 0x1a0c8,
	.tpr3 = 0,
	.tpr4 = 0,
	.tpr5 = 0,
	.emr1 = 0,
	.emr2 = 0,
	.emr3 = 0,
};

int sunxi_dram_init(void)
{
	return DRAMC_init(&dram_para);
}
