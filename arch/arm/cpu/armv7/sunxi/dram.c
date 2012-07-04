/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Berg Xing <bergxing@allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Sunxi platform dram controller init.
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
#include <asm/arch/dram.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>

static inline void dram_ddr_reset(struct sunxi_dram_reg *dram) {


	/* different cpu revision in bit [7:6] */
	if(readl(SUNXI_CPU_CFG)) {
		sr32(&dram->mcr, 12, 1, SDRAM_RST_PIN_HIGH);
		sdelay(0x100);
		sr32(&dram->mcr, 12, 1, SDRAM_RST_PIN_LOW);
	} else {
		sr32(&dram->mcr, 12, 1, SDRAM_RST_PIN_LOW);
		sdelay(0x100);
		sr32(&dram->mcr, 12, 1, SDRAM_RST_PIN_HIGH);
	}

}

static inline void dram_set_high_speed(struct sunxi_dram_reg *dram) {

	sr32(&dram->mcr, 13, 2, MCR_ENABLE_MODE);
	sr32(&dram->mcr, 0, 2, MCR_MODE_NORMAL);
	sr32(&dram->mcr, 2, 2, MCR_DQ_OUT_MODE_HS);
	sr32(&dram->mcr, 4, 2, MCR_ADDR_OUT_MODE_HS);
	sr32(&dram->mcr, 6, 2, MCR_DQ_IN_MODE_HS);
	sr32(&dram->mcr, 8, 3, MCR_DQ_HS_TURNON_DLY);
	sr32(&dram->mcr, 11, 1, MCR_ADDR_IN_MODE_HS);
}

static inline void dram_close_clock(struct sunxi_ccm_reg *ccm) {

	sr32(&ccm->dram_clk_cfg, DCLK_OUT_OFFSET, 1, CLK_GATE_CLOSE);
}

static inline void dram_open_clock(struct sunxi_ccm_reg *ccm) {

	sr32(&ccm->dram_clk_cfg, DCLK_OUT_OFFSET, 1, CLK_GATE_OPEN);
}

static inline void dram_select_controller(struct sunxi_dram_reg *dram) {

	/* write magic number to select dram controller */
	writel(DRAM_CTRL_SELECT_MAGIC, &dram->csel);
}

static inline void dram_disable_itm(struct sunxi_dram_reg *dram) {

	/* disable interface timing module */
	sr32(&dram->ccr, 28, 1, CCR_INTF_TIMING_DISABLE);
}

static inline void dram_enable_itm(struct sunxi_dram_reg *dram) {

	/* enable interface timing module */
	sr32(&dram->ccr, 28, 1, CCR_INTF_TIMING_ENABLE);
}

static inline void dram_enable_dll(struct sunxi_dram_reg *dram, int i) {

	sr32(&dram->dllcr[i], 31, 1, DLL_DISABLE);
	sr32(&dram->dllcr[i], 30, 1, ~DLL_RESET);
	sdelay(0x100);
	sr32(&dram->dllcr[i], 31, 1, DLL_ENABLE);
	sr32(&dram->dllcr[i], 30, 1, ~DLL_RESET);
	sdelay(0x1000);
	sr32(&dram->dllcr[i], 31, 1, DLL_ENABLE);
	sr32(&dram->dllcr[i], 30, 1, DLL_RESET);
	sdelay(0x1000);
}

static inline void dram_config_type(struct sunxi_dram_reg *dram) {

	sr32(&dram->dcr, 0, 1, DCR_TYPE_DDR3);
	sr32(&dram->dcr, 1, 2, DCR_IO_WIDTH_16);
	sr32(&dram->dcr, 3, 3, DCR_CHIP_DENSITY_2Gb);
	sr32(&dram->dcr, 6, 3, DCR_BUS_WIDTH_32);
	sr32(&dram->dcr, 10, 2, DCR_ONE_RANK);
	sr32(&dram->dcr, 12, 1, DCR_CMD_ON_ALL_RANKS);
	sr32(&dram->dcr, 13, 2, DCR_INTERLEAVE_MODE);
}

static inline void dram_set_odt(struct sunxi_dram_reg *dram) {

	/* set odt(On Die Termination) impedance divide ratio */
	sr32(&dram->zqcr0, 20, 8, ZQCR0_IMP_DIV);
}

static inline void dram_init_chip(struct sunxi_dram_reg *dram) {

	sr32(&dram->ccr, 31, 1, CCR_INIT_CHIP);
}
static inline void dram_wait_init(struct sunxi_dram_reg *dram) {

	while(readl(&dram->ccr) & (1 << 31)) {};
}

static inline void dram_set_io_config(struct sunxi_dram_reg *dram) {

	writel(0x00cc0000, &dram->iocr);
}

static inline void dram_set_self_refresh(struct sunxi_dram_reg *dram) {

	writel(0x0861e776, &dram->drr);
}

static inline void dram_set_timing(struct sunxi_dram_reg *dram) {

	writel(0x30926692, &dram->tpr0);
	writel(0x1090, &dram->tpr1);
	writel(0x1a0c8, &dram->tpr2);
}

static inline void dram_set_mode(struct sunxi_dram_reg *dram) {

	sr32(&dram->mr, 0, 3, MR_BURST_LENGTH);
	sr32(&dram->mr, 12, 1, MR_POWER_DOWN);
	sr32(&dram->mr, 4, 3, MR_CAS_LATENCY);
	sr32(&dram->mr, 9, 3, MR_WRITE_RECOVERY);
}

static inline void dram_set_emr(struct sunxi_dram_reg *dram) {

	writel(0, &dram->emr);
	writel(0, &dram->emr2);
	writel(0, &dram->emr3);
}

static inline void dram_set_dqs(struct sunxi_dram_reg *dram) {

	sr32(&dram->ccr, 14, 1, DQS_GATE_ON);
	sr32(&dram->ccr, 17, 1, DQS_DRIFT_COMPENSATION);
}

static inline int dram_read_pipe(struct sunxi_dram_reg *dram) {

	sr32(&dram->ccr, 30, 1, CCR_DAT_TRAIN_TRIG);
	/* wait data trainning end */
	while(readl(&dram->csr) & (1 << 30)) {};

	if(readl(&dram->csr) & (1 << 20)) {
		return -1;
	}

	return 0;
}

static inline void dram_config_hostport(struct sunxi_dram_reg *dram) {

	int i;
	u32 val_tbl[32] = {

		0x00000301,0x00000301,0x00000301,0x00000301,
		0x00000301,0x00000301,0x0,       0x0,
		0x0,       0x0,       0x0,       0x0,
		0x0,       0x0,       0x0,       0x0,
		0x00001031,0x00001031,0x00000735,0x00001035,
		0x00001035,0x00000731,0x00001031,0x00000735,
		0x00001035,0x00001031,0x00000731,0x00001035,
		0x00001031,0x00000301,0x00000301,0x00000731,
	};

	for(i = 0; i < 32; i++)
		writel(val_tbl[i], &dram->hpcr[i]);
}

int sunxi_dram_init(void) {

	struct sunxi_dram_reg *dram =
		(struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	struct sunxi_ccm_reg *ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	dram_ddr_reset(dram);
	dram_set_high_speed(dram);

	dram_close_clock(ccm);

	dram_select_controller(dram);
	dram_disable_itm(dram);
	dram_enable_dll(dram, 0);
	dram_config_type(dram);

	dram_open_clock(ccm);
	sdelay(0x10);
	dram_wait_init(dram);

	dram_enable_dll(dram, 1);
	dram_enable_dll(dram, 2);
	dram_enable_dll(dram, 3);
	dram_enable_dll(dram, 4);

	dram_set_odt(dram);

	dram_set_io_config(dram);
	dram_set_self_refresh(dram);
	dram_set_timing(dram);
	dram_set_mode(dram);
	dram_set_emr(dram);
	dram_set_dqs(dram);

	dram_init_chip(dram);
	dram_wait_init(dram);

	dram_enable_itm(dram);
	dram_read_pipe(dram);
	sdelay(0x4000);
	dram_config_hostport(dram);

	return 0;
}
