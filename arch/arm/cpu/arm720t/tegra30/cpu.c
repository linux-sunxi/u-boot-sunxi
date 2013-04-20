/*
 * Copyright (c) 2010-2012, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/flow.h>
#include <asm/arch/tegra.h>
#include <asm/arch-tegra/clk_rst.h>
#include <asm/arch-tegra/pmc.h>
#include <asm/arch-tegra/tegra_i2c.h>
#include "../tegra-common/cpu.h"

/* Tegra30-specific CPU init code */
void tegra_i2c_ll_write_addr(uint addr, uint config)
{
	struct i2c_ctlr *reg = (struct i2c_ctlr *)TEGRA_DVC_BASE;

	writel(addr, &reg->cmd_addr0);
	writel(config, &reg->cnfg);
}

void tegra_i2c_ll_write_data(uint data, uint config)
{
	struct i2c_ctlr *reg = (struct i2c_ctlr *)TEGRA_DVC_BASE;

	writel(data, &reg->cmd_data1);
	writel(config, &reg->cnfg);
}

#define TPS65911_I2C_ADDR		0x5A
#define TPS65911_VDDCTRL_OP_REG		0x28
#define TPS65911_VDDCTRL_SR_REG		0x27
#define TPS65911_VDDCTRL_OP_DATA	(0x2300 | TPS65911_VDDCTRL_OP_REG)
#define TPS65911_VDDCTRL_SR_DATA	(0x0100 | TPS65911_VDDCTRL_SR_REG)
#define I2C_SEND_2_BYTES		0x0A02

static void enable_cpu_power_rail(void)
{
	struct pmc_ctlr *pmc = (struct pmc_ctlr *)NV_PA_PMC_BASE;
	u32 reg;

	debug("enable_cpu_power_rail entry\n");
	reg = readl(&pmc->pmc_cntrl);
	reg |= CPUPWRREQ_OE;
	writel(reg, &pmc->pmc_cntrl);

	/*
	 * Bring up CPU VDD via the TPS65911x PMIC on the DVC I2C bus.
	 * First set VDD to 1.4V, then enable the VDD regulator.
	 */
	tegra_i2c_ll_write_addr(TPS65911_I2C_ADDR, 2);
	tegra_i2c_ll_write_data(TPS65911_VDDCTRL_OP_DATA, I2C_SEND_2_BYTES);
	udelay(1000);
	tegra_i2c_ll_write_data(TPS65911_VDDCTRL_SR_DATA, I2C_SEND_2_BYTES);
	udelay(10 * 1000);
}

/**
 * The T30 requires some special clock initialization, including setting up
 * the dvc i2c, turning on mselect and selecting the G CPU cluster
 */
void t30_init_clocks(void)
{
	struct clk_rst_ctlr *clkrst =
			(struct clk_rst_ctlr *)NV_PA_CLK_RST_BASE;
	struct flow_ctlr *flow = (struct flow_ctlr *)NV_PA_FLOW_BASE;
	u32 val;

	debug("t30_init_clocks entry\n");
	/* Set active CPU cluster to G */
	clrbits_le32(flow->cluster_control, 1 << 0);

	/*
	 * Switch system clock to PLLP_OUT4 (108 MHz), AVP will now run
	 * at 108 MHz. This is glitch free as only the source is changed, no
	 * special precaution needed.
	 */
	val = (SCLK_SOURCE_PLLP_OUT4 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT) |
		(SCLK_SOURCE_PLLP_OUT4 << SCLK_SWAKEUP_IRQ_SOURCE_SHIFT) |
		(SCLK_SOURCE_PLLP_OUT4 << SCLK_SWAKEUP_RUN_SOURCE_SHIFT) |
		(SCLK_SOURCE_PLLP_OUT4 << SCLK_SWAKEUP_IDLE_SOURCE_SHIFT) |
		(SCLK_SYS_STATE_RUN << SCLK_SYS_STATE_SHIFT);
	writel(val, &clkrst->crc_sclk_brst_pol);

	writel(SUPER_SCLK_ENB_MASK, &clkrst->crc_super_sclk_div);

	val = (0 << CLK_SYS_RATE_HCLK_DISABLE_SHIFT) |
		(1 << CLK_SYS_RATE_AHB_RATE_SHIFT) |
		(0 << CLK_SYS_RATE_PCLK_DISABLE_SHIFT) |
		(0 << CLK_SYS_RATE_APB_RATE_SHIFT);
	writel(val, &clkrst->crc_clk_sys_rate);

	/* Put i2c, mselect in reset and enable clocks */
	reset_set_enable(PERIPH_ID_DVC_I2C, 1);
	clock_set_enable(PERIPH_ID_DVC_I2C, 1);
	reset_set_enable(PERIPH_ID_MSELECT, 1);
	clock_set_enable(PERIPH_ID_MSELECT, 1);

	/* Switch MSELECT clock to PLLP (00) and use a divisor of 2 */
	clock_ll_set_source_divisor(PERIPH_ID_MSELECT, 0, 2);

	/*
	 * Our high-level clock routines are not available prior to
	 * relocation. We use the low-level functions which require a
	 * hard-coded divisor. Use CLK_M with divide by (n + 1 = 17)
	 */
	clock_ll_set_source_divisor(PERIPH_ID_DVC_I2C, 3, 16);

	/*
	 * Give clocks time to stabilize, then take i2c and mselect out of
	 * reset
	 */
	udelay(1000);
	reset_set_enable(PERIPH_ID_DVC_I2C, 0);
	reset_set_enable(PERIPH_ID_MSELECT, 0);
}

static void set_cpu_running(int run)
{
	struct flow_ctlr *flow = (struct flow_ctlr *)NV_PA_FLOW_BASE;

	debug("set_cpu_running entry, run = %d\n", run);
	writel(run ? FLOW_MODE_NONE : FLOW_MODE_STOP, &flow->halt_cpu_events);
}

void start_cpu(u32 reset_vector)
{
	debug("start_cpu entry, reset_vector = %x\n", reset_vector);
	t30_init_clocks();

	/* Enable VDD_CPU */
	enable_cpu_power_rail();

	set_cpu_running(0);

	/* Hold the CPUs in reset */
	reset_A9_cpu(1);

	/* Disable the CPU clock */
	enable_cpu_clock(0);

	/* Enable CoreSight */
	clock_enable_coresight(1);

	/*
	 * Set the entry point for CPU execution from reset,
	 *  if it's a non-zero value.
	 */
	if (reset_vector)
		writel(reset_vector, EXCEP_VECTOR_CPU_RESET_VECTOR);

	/* Enable the CPU clock */
	enable_cpu_clock(1);

	/* If the CPU doesn't already have power, power it up */
	powerup_cpu();

	/* Take the CPU out of reset */
	reset_A9_cpu(0);

	set_cpu_running(1);
}
