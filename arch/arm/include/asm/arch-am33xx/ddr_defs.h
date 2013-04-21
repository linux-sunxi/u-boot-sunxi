/*
 * ddr_defs.h
 *
 * ddr specific header
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _DDR_DEFS_H
#define _DDR_DEFS_H

#include <asm/arch/hardware.h>
#include <asm/emif.h>

/* AM335X EMIF Register values */
#define VTP_CTRL_READY		(0x1 << 5)
#define VTP_CTRL_ENABLE		(0x1 << 6)
#define VTP_CTRL_START_EN	(0x1)
#define PHY_DLL_LOCK_DIFF	0x0
#define DDR_CKE_CTRL_NORMAL	0x1
#define PHY_EN_DYN_PWRDN	(0x1 << 20)

/* Micron MT47H128M16RT-25E */
#define MT47H128M16RT25E_EMIF_READ_LATENCY	0x100005
#define MT47H128M16RT25E_EMIF_TIM1		0x0666B3C9
#define MT47H128M16RT25E_EMIF_TIM2		0x243631CA
#define MT47H128M16RT25E_EMIF_TIM3		0x0000033F
#define MT47H128M16RT25E_EMIF_SDCFG		0x41805332
#define MT47H128M16RT25E_EMIF_SDREF		0x0000081a
#define MT47H128M16RT25E_DLL_LOCK_DIFF		0x0
#define MT47H128M16RT25E_RATIO			0x80
#define MT47H128M16RT25E_INVERT_CLKOUT		0x00
#define MT47H128M16RT25E_RD_DQS			0x12
#define MT47H128M16RT25E_WR_DQS			0x00
#define MT47H128M16RT25E_PHY_WRLVL		0x00
#define MT47H128M16RT25E_PHY_GATELVL		0x00
#define MT47H128M16RT25E_PHY_WR_DATA		0x40
#define MT47H128M16RT25E_PHY_FIFO_WE		0x80
#define MT47H128M16RT25E_PHY_RANK0_DELAY		0x1
#define MT47H128M16RT25E_IOCTRL_VALUE		0x18B

/* Micron MT41J128M16JT-125 */
#define MT41J128MJT125_EMIF_READ_LATENCY	0x06
#define MT41J128MJT125_EMIF_TIM1		0x0888A39B
#define MT41J128MJT125_EMIF_TIM2		0x26337FDA
#define MT41J128MJT125_EMIF_TIM3		0x501F830F
#define MT41J128MJT125_EMIF_SDCFG		0x61C04AB2
#define MT41J128MJT125_EMIF_SDREF		0x0000093B
#define MT41J128MJT125_ZQ_CFG			0x50074BE4
#define MT41J128MJT125_DLL_LOCK_DIFF		0x1
#define MT41J128MJT125_RATIO			0x40
#define MT41J128MJT125_INVERT_CLKOUT		0x1
#define MT41J128MJT125_RD_DQS			0x3B
#define MT41J128MJT125_WR_DQS			0x85
#define MT41J128MJT125_PHY_WR_DATA		0xC1
#define MT41J128MJT125_PHY_FIFO_WE		0x100
#define MT41J128MJT125_IOCTRL_VALUE		0x18B

/* Micron MT41J256M8HX-15E */
#define MT41J256M8HX15E_EMIF_READ_LATENCY	0x06
#define MT41J256M8HX15E_EMIF_TIM1		0x0888A39B
#define MT41J256M8HX15E_EMIF_TIM2		0x26337FDA
#define MT41J256M8HX15E_EMIF_TIM3		0x501F830F
#define MT41J256M8HX15E_EMIF_SDCFG		0x61C04B32
#define MT41J256M8HX15E_EMIF_SDREF		0x0000093B
#define MT41J256M8HX15E_ZQ_CFG			0x50074BE4
#define MT41J256M8HX15E_DLL_LOCK_DIFF		0x1
#define MT41J256M8HX15E_RATIO			0x40
#define MT41J256M8HX15E_INVERT_CLKOUT		0x1
#define MT41J256M8HX15E_RD_DQS			0x3B
#define MT41J256M8HX15E_WR_DQS			0x85
#define MT41J256M8HX15E_PHY_WR_DATA		0xC1
#define MT41J256M8HX15E_PHY_FIFO_WE		0x100
#define MT41J256M8HX15E_IOCTRL_VALUE		0x18B

/* Micron MT41K256M16HA-125E */
#define MT41K256M16HA125E_EMIF_READ_LATENCY	0x100007
#define MT41K256M16HA125E_EMIF_TIM1		0x0AAAD4DB
#define MT41K256M16HA125E_EMIF_TIM2		0x266B7FDA
#define MT41K256M16HA125E_EMIF_TIM3		0x501F867F
#define MT41K256M16HA125E_EMIF_SDCFG		0x61C05332
#define MT41K256M16HA125E_EMIF_SDREF		0xC30
#define MT41K256M16HA125E_ZQ_CFG		0x50074BE4
#define MT41K256M16HA125E_DLL_LOCK_DIFF		0x1
#define MT41K256M16HA125E_RATIO			0x80
#define MT41K256M16HA125E_INVERT_CLKOUT		0x0
#define MT41K256M16HA125E_RD_DQS		0x38
#define MT41K256M16HA125E_WR_DQS		0x44
#define MT41K256M16HA125E_PHY_WR_DATA		0x7D
#define MT41K256M16HA125E_PHY_FIFO_WE		0x94
#define MT41K256M16HA125E_IOCTRL_VALUE		0x18B

/* Micron MT41J512M8RH-125 on EVM v1.5 */
#define MT41J512M8RH125_EMIF_READ_LATENCY	0x06
#define MT41J512M8RH125_EMIF_TIM1		0x0888A39B
#define MT41J512M8RH125_EMIF_TIM2		0x26517FDA
#define MT41J512M8RH125_EMIF_TIM3		0x501F84EF
#define MT41J512M8RH125_EMIF_SDCFG		0x61C04BB2
#define MT41J512M8RH125_EMIF_SDREF		0x0000093B
#define MT41J512M8RH125_ZQ_CFG			0x50074BE4
#define MT41J512M8RH125_DLL_LOCK_DIFF		0x1
#define MT41J512M8RH125_RATIO			0x80
#define MT41J512M8RH125_INVERT_CLKOUT		0x0
#define MT41J512M8RH125_RD_DQS			0x3B
#define MT41J512M8RH125_WR_DQS			0x3C
#define MT41J512M8RH125_PHY_FIFO_WE		0xA5
#define MT41J512M8RH125_PHY_WR_DATA		0x74
#define MT41J512M8RH125_IOCTRL_VALUE		0x18B

/**
 * Configure DMM
 */
void config_dmm(const struct dmm_lisa_map_regs *regs);

/**
 * Configure SDRAM
 */
void config_sdram(const struct emif_regs *regs, int nr);

/**
 * Set SDRAM timings
 */
void set_sdram_timings(const struct emif_regs *regs, int nr);

/**
 * Configure DDR PHY
 */
void config_ddr_phy(const struct emif_regs *regs, int nr);

struct ddr_cmd_regs {
	unsigned int resv0[7];
	unsigned int cm0csratio;	/* offset 0x01C */
	unsigned int resv1[2];
	unsigned int cm0dldiff;		/* offset 0x028 */
	unsigned int cm0iclkout;	/* offset 0x02C */
	unsigned int resv2[8];
	unsigned int cm1csratio;	/* offset 0x050 */
	unsigned int resv3[2];
	unsigned int cm1dldiff;		/* offset 0x05C */
	unsigned int cm1iclkout;	/* offset 0x060 */
	unsigned int resv4[8];
	unsigned int cm2csratio;	/* offset 0x084 */
	unsigned int resv5[2];
	unsigned int cm2dldiff;		/* offset 0x090 */
	unsigned int cm2iclkout;	/* offset 0x094 */
	unsigned int resv6[3];
};

struct ddr_data_regs {
	unsigned int dt0rdsratio0;	/* offset 0x0C8 */
	unsigned int resv1[4];
	unsigned int dt0wdsratio0;	/* offset 0x0DC */
	unsigned int resv2[4];
	unsigned int dt0wiratio0;	/* offset 0x0F0 */
	unsigned int resv3;
	unsigned int dt0wimode0;	/* offset 0x0F8 */
	unsigned int dt0giratio0;	/* offset 0x0FC */
	unsigned int resv4;
	unsigned int dt0gimode0;	/* offset 0x104 */
	unsigned int dt0fwsratio0;	/* offset 0x108 */
	unsigned int resv5[4];
	unsigned int dt0dqoffset;	/* offset 0x11C */
	unsigned int dt0wrsratio0;	/* offset 0x120 */
	unsigned int resv6[4];
	unsigned int dt0rdelays0;	/* offset 0x134 */
	unsigned int dt0dldiff0;	/* offset 0x138 */
	unsigned int resv7[12];
};

/**
 * This structure represents the DDR registers on AM33XX devices.
 * We make use of DDR_PHY_BASE_ADDR2 to address the DATA1 registers that
 * correspond to DATA1 registers defined here.
 */
struct ddr_regs {
	unsigned int resv0[7];
	unsigned int cm0csratio;	/* offset 0x01C */
	unsigned int resv1[2];
	unsigned int cm0dldiff;		/* offset 0x028 */
	unsigned int cm0iclkout;	/* offset 0x02C */
	unsigned int resv2[8];
	unsigned int cm1csratio;	/* offset 0x050 */
	unsigned int resv3[2];
	unsigned int cm1dldiff;		/* offset 0x05C */
	unsigned int cm1iclkout;	/* offset 0x060 */
	unsigned int resv4[8];
	unsigned int cm2csratio;	/* offset 0x084 */
	unsigned int resv5[2];
	unsigned int cm2dldiff;		/* offset 0x090 */
	unsigned int cm2iclkout;	/* offset 0x094 */
	unsigned int resv6[12];
	unsigned int dt0rdsratio0;	/* offset 0x0C8 */
	unsigned int resv7[4];
	unsigned int dt0wdsratio0;	/* offset 0x0DC */
	unsigned int resv8[4];
	unsigned int dt0wiratio0;	/* offset 0x0F0 */
	unsigned int resv9;
	unsigned int dt0wimode0;	/* offset 0x0F8 */
	unsigned int dt0giratio0;	/* offset 0x0FC */
	unsigned int resv10;
	unsigned int dt0gimode0;	/* offset 0x104 */
	unsigned int dt0fwsratio0;	/* offset 0x108 */
	unsigned int resv11[4];
	unsigned int dt0dqoffset;	/* offset 0x11C */
	unsigned int dt0wrsratio0;	/* offset 0x120 */
	unsigned int resv12[4];
	unsigned int dt0rdelays0;	/* offset 0x134 */
	unsigned int dt0dldiff0;	/* offset 0x138 */
};

/**
 * Encapsulates DDR CMD control registers.
 */
struct cmd_control {
	unsigned long cmd0csratio;
	unsigned long cmd0csforce;
	unsigned long cmd0csdelay;
	unsigned long cmd0dldiff;
	unsigned long cmd0iclkout;
	unsigned long cmd1csratio;
	unsigned long cmd1csforce;
	unsigned long cmd1csdelay;
	unsigned long cmd1dldiff;
	unsigned long cmd1iclkout;
	unsigned long cmd2csratio;
	unsigned long cmd2csforce;
	unsigned long cmd2csdelay;
	unsigned long cmd2dldiff;
	unsigned long cmd2iclkout;
};

/**
 * Encapsulates DDR DATA registers.
 */
struct ddr_data {
	unsigned long datardsratio0;
	unsigned long datawdsratio0;
	unsigned long datawiratio0;
	unsigned long datagiratio0;
	unsigned long datafwsratio0;
	unsigned long datawrsratio0;
	unsigned long datauserank0delay;
	unsigned long datadldiff0;
};

/**
 * Configure DDR CMD control registers
 */
void config_cmd_ctrl(const struct cmd_control *cmd, int nr);

/**
 * Configure DDR DATA registers
 */
void config_ddr_data(const struct ddr_data *data, int nr);

/**
 * This structure represents the DDR io control on AM33XX devices.
 */
struct ddr_cmdtctrl {
	unsigned int resv1[1];
	unsigned int cm0ioctl;
	unsigned int cm1ioctl;
	unsigned int cm2ioctl;
	unsigned int resv2[12];
	unsigned int dt0ioctl;
	unsigned int dt1ioctl;
};

/**
 * Configure DDR io control registers
 */
void config_io_ctrl(unsigned long val);

struct ddr_ctrl {
	unsigned int ddrioctrl;
	unsigned int resv1[325];
	unsigned int ddrckectrl;
};

void config_ddr(unsigned int pll, unsigned int ioctrl,
		const struct ddr_data *data, const struct cmd_control *ctrl,
		const struct emif_regs *regs, int nr);

#endif  /* _DDR_DEFS_H */
