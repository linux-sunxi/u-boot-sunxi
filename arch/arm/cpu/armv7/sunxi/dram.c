/*
 * sunxi DRAM controller initialization
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * Based on sun4i Linux kernel sources mach-sunxi/pm/standby/dram*.c
 * and earlier U-Boot Allwiner A10 SPL work
 *
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Berg Xing <bergxing@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
#include <asm/arch/timer.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>

#define CPU_CFG_CHIP_VER(n) ((n) << 6)
#define CPU_CFG_CHIP_VER_MASK CPU_CFG_CHIP_VER(0x3)
#define CPU_CFG_CHIP_REV_A 0x0
#define CPU_CFG_CHIP_REV_C1 0x1
#define CPU_CFG_CHIP_REV_C2 0x2
#define CPU_CFG_CHIP_REV_B 0x3

static void mctl_ddr3_reset(void)
{
	struct sunxi_dram_reg *dram =
			(struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

#ifdef CONFIG_SUN4I
	struct sunxi_timer_reg *timer =
			(struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
	u32 reg_val;

	writel(0, &timer->cpu_cfg);
	reg_val = readl(&timer->cpu_cfg);

	if ((reg_val & CPU_CFG_CHIP_VER_MASK) !=
	    CPU_CFG_CHIP_VER(CPU_CFG_CHIP_REV_A)) {
		setbits_le32(&dram->mcr, DRAM_MCR_RESET);
		sdelay(0x100);
		clrbits_le32(&dram->mcr, DRAM_MCR_RESET);
	} else
#endif
	{
		clrbits_le32(&dram->mcr, DRAM_MCR_RESET);
		sdelay(0x100);
		setbits_le32(&dram->mcr, DRAM_MCR_RESET);
	}
}

static void mctl_set_drive(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrsetbits_le32(&dram->mcr, DRAM_MCR_MODE_NORM(0x3),
			DRAM_MCR_MODE_EN(0x3) |
			0xffc);
}

static void mctl_itm_disable(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	setbits_le32(&dram->ccr, DRAM_CCR_ITM_OFF);
}

static void mctl_itm_enable(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrbits_le32(&dram->ccr, DRAM_CCR_ITM_OFF);
}

static void mctl_enable_dll0(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrsetbits_le32(&dram->dllcr[0], DRAM_DLLCR_NRESET, DRAM_DLLCR_DISABLE);
	sdelay(0x100);

	clrbits_le32(&dram->dllcr[0], DRAM_DLLCR_NRESET | DRAM_DLLCR_DISABLE);
	sdelay(0x1000);

	clrsetbits_le32(&dram->dllcr[0], DRAM_DLLCR_DISABLE, DRAM_DLLCR_NRESET);
	sdelay(0x1000);
}

/*
 * Note: This differs from pm/standby in that it checks the bus width
 */
static void mctl_enable_dllx(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 i, n, bus_width;

	bus_width = readl(&dram->dcr);

	if ((bus_width & DRAM_DCR_BUS_WIDTH_MASK) ==
	    DRAM_DCR_BUS_WIDTH(DRAM_DCR_BUS_WIDTH_32BIT))
		n = DRAM_DCR_NR_DLLCR_32BIT;
	else
		n = DRAM_DCR_NR_DLLCR_16BIT;

	for (i = 1; i < n; i++)
		clrsetbits_le32(&dram->dllcr[i], DRAM_DLLCR_NRESET,
				DRAM_DLLCR_DISABLE);
	sdelay(0x100);

	for (i = 1; i < n; i++)
		clrbits_le32(&dram->dllcr[i], DRAM_DLLCR_NRESET |
			     DRAM_DLLCR_DISABLE);
	sdelay(0x1000);

	for (i = 1; i < n; i++)
		clrsetbits_le32(&dram->dllcr[i], DRAM_DLLCR_DISABLE,
				DRAM_DLLCR_NRESET);
	sdelay(0x1000);
}

static u32 hpcr_value[32] = {
#ifdef CONFIG_SUN5I
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0x1031, 0x1031, 0x0735, 0x1035,
	0x1035, 0x0731, 0x1031, 0,
	0x0301, 0x0301, 0x0301, 0x0301,
	0x0301, 0x0301, 0x0301, 0
#endif
#ifdef CONFIG_SUN4I
	0x0301, 0x0301, 0x0301, 0x0301,
	0x0301, 0x0301, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0x1031, 0x1031, 0x0735, 0x1035,
	0x1035, 0x0731, 0x1031, 0x0735,
	0x1035, 0x1031, 0x0731, 0x1035,
	0x1031, 0x0301, 0x0301, 0x0731
#endif
};

static void mctl_configure_hostport(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 i;

	for (i = 0; i < 32; i++)
		writel(hpcr_value[i], &dram->hpcr[i]);
}

static void mctl_setup_dram_clock(u32 clk)
{
	u32 reg_val;
	struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* setup DRAM PLL */
	reg_val = readl(&ccm->pll5_cfg);
	reg_val &= ~CCM_PLL5_CTRL_M_MASK;		/* set M to 0 (x1) */
	reg_val |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
	reg_val &= ~CCM_PLL5_CTRL_K_MASK;		/* set K to 0 (x1) */
	reg_val |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(2));
	reg_val &= ~CCM_PLL5_CTRL_N_MASK;		/* set N to 0 (x0) */
	reg_val |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(clk / 24));
	reg_val &= ~CCM_PLL5_CTRL_P_MASK;		/* set P to 0 (x1) */
	reg_val |= CCM_PLL5_CTRL_P(CCM_PLL5_CTRL_P_X(2));
	reg_val &= ~CCM_PLL5_CTRL_VCO_GAIN;		/* PLL VCO Gain off */
	reg_val |= CCM_PLL5_CTRL_EN;			/* PLL On */
	writel(reg_val, &ccm->pll5_cfg);
	sdelay(0x100000);

	setbits_le32(&ccm->pll5_cfg, CCM_PLL5_CTRL_DDR_CLK);

#ifdef CONFIG_SUN4I
	/* reset GPS */
	clrbits_le32(&ccm->gps_clk_cfg, CCM_GPS_CTRL_RESET | CCM_GPS_CTRL_GATE);
	setbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_GPS);
	sdelay(0x20);
	clrbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_GPS);
#endif

	/* setup MBUS clock */
	reg_val = CCM_MBUS_CTRL_GATE |
		  CCM_MBUS_CTRL_CLK_SRC(CCM_MBUS_CTRL_CLK_SRC_PLL5) |
		  CCM_MBUS_CTRL_M(CCM_MBUS_CTRL_M_X(2));
	writel(reg_val, &ccm->mbus_clk_cfg);

	/*
	 * open DRAMC AHB & DLL register clock
	 * close it first
	 */
#ifdef CONFIG_SUN5I
	clrbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_SDRAM | CCM_AHB_GATE_DLL);
#else
	clrbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_SDRAM);
#endif
	sdelay(0x1000);

	/* then open it */
#ifdef CONFIG_SUN5I
	setbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_SDRAM | CCM_AHB_GATE_DLL);
#else
	setbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_SDRAM);
#endif
	sdelay(0x1000);
}

static int dramc_scan_readpipe(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 reg_val;

	/* data training trigger */
	setbits_le32(&dram->ccr, DRAM_CCR_DATA_TRAINING);

	/* check whether data training process has completed */
	while (readl(&dram->ccr) & DRAM_CCR_DATA_TRAINING);

	/* check data training result */
	reg_val = readl(&dram->csr);
	if (reg_val & DRAM_CSR_FAILED)
		return -1;

	return 0;
}

static void dramc_clock_output_en(u32 on)
{
#ifdef CONFIG_SUN5I
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	if (on)
		setbits_le32(&dram->mcr, DRAM_MCR_DCLK_OUT);
	else
		clrbits_le32(&dram->mcr, DRAM_MCR_DCLK_OUT);
#endif
#ifdef CONFIG_SUN4I
	struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	if (on)
		setbits_le32(&ccm->dram_clk_cfg, CCM_DRAM_CTRL_DCLK_OUT);
	else
		clrbits_le32(&ccm->dram_clk_cfg, CCM_DRAM_CTRL_DCLK_OUT);
#endif
}

#ifdef CONFIG_SUN4I
static void dramc_set_autorefresh_cycle(u32 clk)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 reg_val;
	u32 tmp_val;
	u32 reg_dcr;

	if (clk < 600) {
		reg_dcr = readl(&dram->dcr);
		if ((reg_dcr & DRAM_DCR_CHIP_DENSITY_MASK) <=
		    DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_1024M))
			reg_val = (131 * clk) >> 10;
		else
			reg_val = (336 * clk) >> 10;

		tmp_val = (7987 * clk) >> 10;
		tmp_val = tmp_val * 9 - 200;
		reg_val |= tmp_val << 8;
		reg_val |= 0x8 << 24;
		writel(reg_val, &dram->drr);
	} else {
		writel(0x0, &dram->drr);
	}
}
#endif /* SUN4I */

#ifdef CONFIG_SUN5I
static void dramc_set_autorefresh_cycle(u32 clk)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 reg_val;
	u32 tmp_val;
	reg_val = 0x83;

	tmp_val = (7987 * clk) >> 10;
	tmp_val = tmp_val * 9 - 200;
	reg_val |= tmp_val << 8;
	reg_val |= 0x8 << 24;
	writel(reg_val, &dram->drr);
}
#endif /* SUN5I */

int dramc_init(struct dram_para *para)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	u32 reg_val;
	int ret_val;

	/* check input dram parameter structure */
	if (!para)
		return -1;

	/* setup DRAM relative clock */
	mctl_setup_dram_clock(para->clock);

#ifdef CONFIG_SUN5I
	/* Disable any pad power save control */
	writel(0, &dram->ppwrsctl);
#endif

	/* reset external DRAM */
	mctl_ddr3_reset();
	mctl_set_drive();

	/* dram clock off */
	dramc_clock_output_en(0);

#ifdef CONFIG_SUN4I
	/* select dram controller 1 */
	writel(DRAM_CSEL_MAGIC, &dram->csel);
#endif

	mctl_itm_disable();
	mctl_enable_dll0();

	/* configure external DRAM */
	reg_val = 0x0;
	if (para->type == DRAM_MEMORY_TYPE_DDR3)
		reg_val |= DRAM_DCR_TYPE_DDR3;
	reg_val |= DRAM_DCR_IO_WIDTH(para->io_width >> 3);

	if (para->density == 256)
		reg_val |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_256M);
	else if (para->density == 512)
		reg_val |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_512M);
	else if (para->density == 1024)
		reg_val |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_1024M);
	else if (para->density == 2048)
		reg_val |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_2048M);
	else if (para->density == 4096)
		reg_val |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_4096M);
	else if (para->density == 8192)
		reg_val |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_8192M);
	else
		reg_val |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_256M);

	reg_val |= DRAM_DCR_BUS_WIDTH((para->bus_width >> 3) - 1);
	reg_val |= DRAM_DCR_RANK_SEL(para->rank_num - 1);
	reg_val |= DRAM_DCR_CMD_RANK_ALL;
	reg_val |= DRAM_DCR_MODE(DRAM_DCR_MODE_INTERLEAVE);
	writel(reg_val, &dram->dcr);

#ifdef CONFIG_SUN5I
	/* set odt impendance divide ratio */
	reg_val = ((para->zq) >> 8) & 0xfffff;
	reg_val |= ((para->zq) & 0xff) << 20;
	reg_val |= (para->zq) & 0xf0000000;
	writel(reg_val, &dram->zqcr0);
#endif

	/* dram clock on */
	dramc_clock_output_en(1);

	sdelay(0x10);

	while (readl(&dram->ccr) & DRAM_CCR_INIT);

	mctl_enable_dllx();

#ifdef CONFIG_SUN4I
	/* set odt impendance divide ratio */
	reg_val = ((para->zq) >> 8) & 0xfffff;
	reg_val |= ((para->zq) & 0xff) << 20;
	reg_val |= (para->zq) & 0xf0000000;
	writel(reg_val, &dram->zqcr0);
#endif

#ifdef CONFIG_SUN4I
	/* set I/O configure register */
	reg_val = 0x00cc0000;
	reg_val |= (para->odt_en) & 0x3;
	reg_val |= ((para->odt_en) & 0x3) << 30;
	writel(reg_val, &dram->iocr);
#endif

	/* set refresh period */
	dramc_set_autorefresh_cycle(para->clock);

	/* set timing parameters */
	writel(para->tpr0, &dram->tpr0);
	writel(para->tpr1, &dram->tpr1);
	writel(para->tpr2, &dram->tpr2);

	if (para->type == DRAM_MEMORY_TYPE_DDR3) {
		reg_val = DRAM_MR_BURST_LENGTH(0x0);
#ifdef CONFIG_SUN5I
		reg_val |= DRAM_MR_POWER_DOWN;
#endif
		reg_val |= DRAM_MR_CAS_LAT(para->cas - 4);
		reg_val |= DRAM_MR_WRITE_RECOVERY(0x5);
	} else if (para->type == DRAM_MEMORY_TYPE_DDR2) {
		reg_val = DRAM_MR_BURST_LENGTH(0x2);
		reg_val |= DRAM_MR_CAS_LAT(para->cas);
		reg_val |= DRAM_MR_WRITE_RECOVERY(0x5);
	}
	writel(reg_val, &dram->mr);

	writel(para->emr1, &dram->emr);
	writel(para->emr2, &dram->emr2);
	writel(para->emr3, &dram->emr3);

	/* set DQS window mode */
	clrsetbits_le32(&dram->ccr, DRAM_CCR_DQS_DRIFT_COMP, DRAM_CCR_DQS_GATE);

	/* reset external DRAM */
	setbits_le32(&dram->ccr, DRAM_CCR_INIT);
	while (readl(&dram->ccr) & DRAM_CCR_INIT);


	/* scan read pipe value */
	mctl_itm_enable();
	ret_val = dramc_scan_readpipe();

	if (ret_val < 0)
		return 0;

	/* configure all host port */
	mctl_configure_hostport();

	return get_ram_size((long *)PHYS_SDRAM_1, 1 << 30);
}
