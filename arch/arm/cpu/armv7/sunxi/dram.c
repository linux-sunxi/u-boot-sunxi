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

static void mctl_ddr3_reset(void)
{
	__u32 reg_val;

#ifdef CONFIG_SUN4I
	writel(0, TIMER_CPU_CFG_REG);
	reg_val = readl(TIMER_CPU_CFG_REG);
	reg_val >>= 6;
	reg_val &= 0x3;

	if (reg_val != 0) {
		reg_val = readl(SDR_CR);
		reg_val |= (0x1 << 12);
		writel(reg_val, SDR_CR);
		sdelay(0x100);
		reg_val = readl(SDR_CR);
		reg_val &= ~(0x1 << 12);
		writel(reg_val, SDR_CR);
	} else
#endif
	{
		reg_val = readl(SDR_CR);
		reg_val &= ~(0x1 << 12);
		writel(reg_val, SDR_CR);
		sdelay(0x100);
		reg_val = readl(SDR_CR);
		reg_val |= (0x1 << 12);
		writel(reg_val, SDR_CR);
	}
}

static void mctl_set_drive(void)
{
	__u32 reg_val;

	reg_val = readl(SDR_CR);
	reg_val |= (0x6 << 12);
	reg_val |= 0xFFC;
	reg_val &= ~0x3;
	writel(reg_val, SDR_CR);
}

static void mctl_itm_disable(void)
{
	__u32 reg_val = 0x0;

	reg_val = readl(SDR_CCR);
	reg_val |= 0x1 << 28;
	writel(reg_val, SDR_CCR);
}

static void mctl_itm_enable(void)
{
	__u32 reg_val = 0x0;

	reg_val = readl(SDR_CCR);
	reg_val &= ~(0x1 << 28);
	writel(reg_val, SDR_CCR);
}

static void mctl_enable_dll0(void)
{
	writel((readl(SDR_DLLCR0) & ~0x40000000) | 0x80000000, SDR_DLLCR0);
	sdelay(0x100);

	writel(readl(SDR_DLLCR0) & ~0xC0000000, SDR_DLLCR0);
	sdelay(0x1000);

	writel((readl(SDR_DLLCR0) & ~0x80000000) | 0x40000000, SDR_DLLCR0);
	sdelay(0x1000);
}

/*
 * Note: This differs from pm/standby in that it checks the bus width
 */
static void mctl_enable_dllx(void)
{
	__u32 i = 0;
	__u32 n;
	__u32 bus_width;

	bus_width = readl(SDR_DCR);
	bus_width >>= 6;
	bus_width &= 7;

	if (bus_width == 3) {
		n = 5;
	} else {
		n = 3;
		i = 1;
	}

	for (i = 1; i < n; i++) {
		writel((readl(SDR_DLLCR0 + (i << 2)) & ~0x40000000)
		       | 0x80000000, SDR_DLLCR0 + (i << 2));
	}
	sdelay(0x100);

	for (i = 1; i < n; i++) {
		writel(readl(SDR_DLLCR0 + (i << 2)) & ~0xC0000000,
		       SDR_DLLCR0 + (i << 2));
	}
	sdelay(0x1000);

	for (i = 1; i < n; i++) {
		writel((readl(SDR_DLLCR0 + (i << 2)) & ~0x80000000)
		       | 0x40000000, SDR_DLLCR0 + (i << 2));
	}
	sdelay(0x1000);
}

// test-only: not used at all, really needed???
#if 0
static void mctl_disable_dll(void)
{
	__u32 reg_val;

	reg_val = readl(SDR_DLLCR0);
	reg_val &= ~(0x1 << 30);
	reg_val |= 0x1U << 31;
	writel(reg_val, SDR_DLLCR0);

	reg_val = readl(SDR_DLLCR1);
	reg_val &= ~(0x1 << 30);
	reg_val |= 0x1U << 31;
	writel(reg_val, SDR_DLLCR1);

	reg_val = readl(SDR_DLLCR2);
	reg_val &= ~(0x1 << 30);
	reg_val |= 0x1U << 31;
	writel(reg_val, SDR_DLLCR2);

	reg_val = readl(SDR_DLLCR3);
	reg_val &= ~(0x1 << 30);
	reg_val |= 0x1U << 31;
	writel(reg_val, SDR_DLLCR3);

	reg_val = readl(SDR_DLLCR4);
	reg_val &= ~(0x1 << 30);
	reg_val |= 0x1U << 31;
	writel(reg_val, SDR_DLLCR4);
}
#endif

static __u32 hpcr_value[32] = {
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
	__u32 i;

	for (i = 0; i < 32; i++)
		writel(hpcr_value[i], SDR_HPCR + (i << 2));
}

static void mctl_setup_dram_clock(__u32 clk)
{
	__u32 reg_val;

	/* setup DRAM PLL */
	reg_val = readl(DRAM_CCM_SDRAM_PLL_REG);
	reg_val &= ~0x3;
	reg_val |= 0x1;			/* m factor */
	reg_val &= ~(0x3 << 4);
	reg_val |= 0x1 << 4;		/* k factor */
	reg_val &= ~(0x1f << 8);
	reg_val |= ((clk / 24) & 0x1f) << 8;	/* n factor */
	reg_val &= ~(0x3 << 16);
	reg_val |= 0x1 << 16;		/* p factor */
	reg_val &= ~(0x1 << 29);	/* PLL on */
	reg_val |= (__u32) 0x1 << 31;	/* PLL En */
	writel(reg_val, DRAM_CCM_SDRAM_PLL_REG);
	sdelay(0x100000);

	reg_val = readl(DRAM_CCM_SDRAM_PLL_REG);
	reg_val |= 0x1 << 29;
	writel(reg_val, DRAM_CCM_SDRAM_PLL_REG);

#ifdef CONFIG_SUN4I
	/* reset GPS */
	reg_val = readl(DRAM_CCM_GPS_CLK_REG);
	reg_val &= ~0x3;
	writel(reg_val, DRAM_CCM_GPS_CLK_REG);
	reg_val = readl(DRAM_CCM_AHB_GATE_REG);
	reg_val |= (0x1 << 26);
	writel(reg_val, DRAM_CCM_AHB_GATE_REG);
	sdelay(0x20);
	reg_val = readl(DRAM_CCM_AHB_GATE_REG);
	reg_val &= ~(0x1 << 26);
	writel(reg_val, DRAM_CCM_AHB_GATE_REG);
#endif

	/* setup MBUS clock */
	reg_val = (0x1 << 31) | (0x2 << 24) | (0x1);
	writel(reg_val, DRAM_CCM_MUS_CLK_REG);

	/*
	 * open DRAMC AHB & DLL register clock
	 * close it first
	 */
	reg_val = readl(DRAM_CCM_AHB_GATE_REG);
#ifdef CONFIG_SUN5I
	reg_val &= ~(0x3 << 14);
#else
	reg_val &= ~(0x1 << 14);
#endif
	writel(reg_val, DRAM_CCM_AHB_GATE_REG);
	sdelay(0x1000);

	/* then open it */
#ifdef CONFIG_SUN5I
	reg_val |= 0x3 << 14;
#else
	reg_val |= 0x1 << 14;
#endif
	writel(reg_val, DRAM_CCM_AHB_GATE_REG);
	sdelay(0x1000);
}

static int dramc_scan_readpipe(void)
{
	__u32 reg_val;

	/* data training trigger */
	reg_val = readl(SDR_CCR);
	reg_val |= 0x1 << 30;
	writel(reg_val, SDR_CCR);

	/* check whether data training process is end */
	while (readl(SDR_CCR) & (0x1 << 30))
		;

	/* check data training result */
	reg_val = readl(SDR_CSR);
	if (reg_val & (0x1 << 20))
		return -1;

	return 0;
}

// test-only: cant this be done via DCLK_OUT_OFFSET (dram.h)??? (no #ifdef here)
static void dramc_clock_output_en(__u32 on)
{
	__u32 reg_val;

#ifdef CONFIG_SUN5I
	reg_val = readl(SDR_CR);

	if (on)
		reg_val |= 0x1 << 16;
	else
		reg_val &= ~(0x1 << 16);

	writel(reg_val, SDR_CR);
#endif
#ifdef CONFIG_SUN4I
	reg_val = readl(DRAM_CCM_SDRAM_CLK_REG);

	if (on)
		reg_val |= 0x1 << 15;
	else
		reg_val &= ~(0x1 << 15);

	writel(reg_val, DRAM_CCM_SDRAM_CLK_REG);
#endif
}

// test-only: arghhh! clean-up this #ifdef mess!!!!
static void dramc_set_autorefresh_cycle(__u32 clk)
{
	__u32 reg_val;
	__u32 tmp_val;
#ifdef CONFIG_SUN4I
	__u32 dram_size;

	dram_size = readl(SDR_DCR);
	dram_size >>= 3;
	dram_size &= 0x7;

	if (clk < 600) {
		if (dram_size <= 0x2)
			tmp_val = (131 * clk) >> 10;
		else
			tmp_val = (336 * clk) >> 10;
		reg_val = tmp_val;
#else
	reg_val = 131;
#endif

	tmp_val = (7987 * clk) >> 10;
	tmp_val = tmp_val * 9 - 200;
	reg_val |= tmp_val << 8;
	reg_val |= 0x8 << 24;
	writel(reg_val, SDR_DRR);
#ifdef CONFIG_SUN4I
}

else
{
	writel(0x0, SDR_DRR);
}
#endif
}

/*
 * Get DRAM Size in MB unit;
 */
unsigned dramc_get_dram_size(void)
{
	__u32 reg_val;
	__u32 dram_size;
	__u32 chip_den;

	reg_val = readl(SDR_DCR);
	chip_den = (reg_val >> 3) & 0x7;

	// test-only: use an algorythm, like dram_size = (32 << chip_den)
	if (chip_den == 0)
		dram_size = 32;
	else if (chip_den == 1)
		dram_size = 64;
	else if (chip_den == 2)
		dram_size = 128;
	else if (chip_den == 3)
		dram_size = 256;
	else if (chip_den == 4)
		dram_size = 512;
	else
		dram_size = 1024;

	if (((reg_val >> 1) & 0x3) == 0x1)
		dram_size <<= 1;
	if (((reg_val >> 6) & 0x7) == 0x3)
		dram_size <<= 1;
	if (((reg_val >> 10) & 0x3) == 0x1)
		dram_size <<= 1;

	return dram_size;
}

int dramc_init(struct dram_para *para)
{
	__u32 reg_val;
	__s32 ret_val;

	/* check input dram parameter structure */
	if (!para)
		return -1;

	/* setup DRAM relative clock */
	mctl_setup_dram_clock(para->clock);

#ifdef CONFIG_SUN5I
	// test-only: new code? does it work? change or remove comment
	/* This is new unknown code! */
	writel(0, SDR_0x23c);
#endif

	/* reset external DRAM */
	mctl_ddr3_reset();
	mctl_set_drive();

	/* dram clock off */
	dramc_clock_output_en(0);

#ifdef CONFIG_SUN4I
	/* select dram controller 1 */
	writel(0x16237495, SDR_SCSR);
#endif

	mctl_itm_disable();
	mctl_enable_dll0();

	/* configure external DRAM */
	reg_val = 0;
	if (para->type == 3)
		reg_val |= 0x1;
	reg_val |= (para->io_width >> 3) << 1;

	if (para->density == 256)
		reg_val |= 0x0 << 3;
	else if (para->density == 512)
		reg_val |= 0x1 << 3;
	else if (para->density == 1024)
		reg_val |= 0x2 << 3;
	else if (para->density == 2048)
		reg_val |= 0x3 << 3;
	else if (para->density == 4096)
		reg_val |= 0x4 << 3;
	else if (para->density == 8192)
		reg_val |= 0x5 << 3;
	else
		reg_val |= 0x0 << 3;

	reg_val |= ((para->bus_width >> 3) - 1) << 6;

	reg_val |= (para->rank_num - 1) << 10;

	reg_val |= 0x1 << 12;
	reg_val |= ((0x1) & 0x3) << 13;

	writel(reg_val, SDR_DCR);

#ifdef CONFIG_SUN5I
	/* set odt impendance divide ratio */
	reg_val = ((para->zq) >> 8) & 0xfffff;
	reg_val |= ((para->zq) & 0xff) << 20;
	reg_val |= (para->zq) & 0xf0000000;
	writel(reg_val, SDR_ZQCR0);
#endif

	/* dram clock on */
	dramc_clock_output_en(1);

	sdelay(0x10);

	while (readl(SDR_CCR) & (0x1U << 31))
		;

	mctl_enable_dllx();

#ifdef CONFIG_SUN4I
	/* set odt impendance divide ratio */
	reg_val = ((para->zq) >> 8) & 0xfffff;
	reg_val |= ((para->zq) & 0xff) << 20;
	reg_val |= (para->zq) & 0xf0000000;
	writel(reg_val, SDR_ZQCR0);
#endif

#ifdef CONFIG_SUN4I
	/* set I/O configure register */
	reg_val = 0x00cc0000;
	reg_val |= (para->odt_en) & 0x3;
	reg_val |= ((para->odt_en) & 0x3) << 30;
	writel(reg_val, SDR_IOCR);
#endif

	/* set refresh period */
	dramc_set_autorefresh_cycle(para->clock);

	/* set timing parameters */
	writel(para->tpr0, SDR_TPR0);
	writel(para->tpr1, SDR_TPR1);
	writel(para->tpr2, SDR_TPR2);

	/* set mode register */
	if (para->type == 3) {
		/* ddr3 */
		reg_val = 0x0;
#ifdef CONFIG_SUN5I
		reg_val |= 0x1000;
#endif
		reg_val |= (para->cas - 4) << 4;
		reg_val |= 0x5 << 9;
	} else if (para->type == 2) {
		/* ddr2 */
		reg_val = 0x2;
		reg_val |= para->cas << 4;
		reg_val |= 0x5 << 9;
	}
	writel(reg_val, SDR_MR);

	writel(para->emr1, SDR_EMR);
	writel(para->emr2, SDR_EMR2);
	writel(para->emr3, SDR_EMR3);

	/* set DQS window mode */
	reg_val = readl(SDR_CCR);
	reg_val |= 0x1U << 14;
	reg_val &= ~(0x1U << 17);
	writel(reg_val, SDR_CCR);

	/* initial external DRAM */
	reg_val = readl(SDR_CCR);
	reg_val |= 0x1U << 31;
	writel(reg_val, SDR_CCR);

	while (readl(SDR_CCR) & (0x1U << 31))
		;

	/* scan read pipe value */
	mctl_itm_enable();
	ret_val = dramc_scan_readpipe();

	if (ret_val < 0)
		return 0;

	/* configure all host port */
	mctl_configure_hostport();

	return dramc_get_dram_size();
}
