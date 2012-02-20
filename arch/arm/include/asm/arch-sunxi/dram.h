/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Berg Xing <bergxing@allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Sunxi platform dram register definition.
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

#ifndef _SUNXI_DRAM_H
#define _SUNXI_DRAM_H

struct sunxi_dram_reg {

	u32 ccr;        /* 0x00 controller configuration register */
	u32 dcr;        /* 0x04 dram configuration register */
	u32 iocr;       /* 0x08 i/o configuration register */
	u32 csr;        /* 0x0c controller status register */
	u32 drr;        /* 0x10 dram refresh register */
	u32 tpr0;       /* 0x14 dram timing parameters register 0 */
	u32 tpr1;       /* 0x18 dram timing parameters register 1 */
	u32 tpr2;       /* 0x1c dram timing parameters register 2 */
	u32 gdllcr;     /* 0x20 global dll control register */
	u8  res0[0x28];
	u32 rslr0;      /* 0x4c rank system latency register */
	u32 rslr1;      /* 0x50 rank system latency register */
	u8 res1[0x8];
	u32 rdgr0;      /* 0x5c rank dqs gating register */
	u32 rdgr1;      /* 0x60 rank dqs gating register */
	u8  res2[0x34];
	u32 odtcr;      /* 0x98 odt configuration register */
	u32 dtr0;       /* 0x9c data training register 0 */
	u32 dtr1;       /* 0xa0 data training register 1 */
	u32 dtar;       /* 0xa4 data training address register */
	u32 zqcr0;      /* 0xa8 zq control register 0 */
	u32 zqcr1;      /* 0xac zq control register 1 */
	u32 zqsr;       /* 0xb0 zq status register */
	u32 idcr;       /* 0xb4 initializaton delay configure register */
	u8  res3[0x138];
	u32 mr;         /* 0x1f0 mode register */
	u32 emr;        /* 0x1f4 extended mode register */
	u32 emr2;       /* 0x1f8 extended mode register */
	u32 emr3;       /* 0x1fc extended mode register */
	u32 dllctr;     /* 0x200 dll control register */
	u32 dllcr[4];   /* 0x204 dll control register 0(byte 0) */
	                /* 0x208 dll control register 1(byte 1) */
	                /* 0x20c dll control register 2(byte 2) */
	                /* 0x210 dll control register 3(byte 3) */
	u8  res4[0x4];
	u32 dqtr0;      /* 0x218 dq timing register */
	u32 dqtr1;      /* 0x21c dq timing register */
	u32 dqtr2;      /* 0x220 dq timing register */
	u32 dqtr3;      /* 0x224 dq timing register */
	u32 dqstr;      /* 0x228 dqs timing register */
	u32 dqsbtr;     /* 0x22c dqsb timing register */
	u32 mcr;        /* 0x230 mode configure register */
	u8  res[0xc];
	u32 apr;        /* 0x240 arbiter period register */
	u32 pldtr;      /* 0x244 priority level data threshold register */
	u8  res5[0x8];
	u32 hpcr[32];   /* 0x250 host port configure register */
	u8  res6[0x10];
	u32 csel;       /* 0x2e0 controller select register */

};

struct dram_para {

	u32 clock;
	u32 type;
	u32 rank_num;
	u32 density;
	u32 io_width;
	u32 bus_width;
	u32 cas;
	u32 zq;
	u32 odt_en;
	u32 size;
	u32 tpr0;
	u32 tpr1;
	u32 tpr2;
	u32 tpr3;
	u32 tpr4;
	u32 tpr5;
	u32 emr1;
	u32 emr2;
	u32 emr3;
};

#define SDRAM_RST_PIN_HIGH        (1)
#define SDRAM_RST_PIN_LOW         (0)

#define DCLK_OUT_OFFSET           (15)
#define DRAM_CTRL_SELECT_MAGIC    (0x16237495)

#define MCR_ENABLE_MODE           (3)
#define MCR_MODE_NORMAL           (0)
#define MCR_DQ_OUT_MODE_HS        (3)
#define MCR_DQ_IN_MODE_HS         (3)
#define MCR_ADDR_OUT_MODE_HS      (3)
#define MCR_ADDR_IN_MODE_HS       (1)
#define MCR_DQ_HS_TURNON_DLY      (7)

#define CCR_INTF_TIMING_DISABLE   (1)
#define CCR_INTF_TIMING_ENABLE    (0)
#define CCR_INIT_CHIP             (1)
#define CCR_DAT_TRAIN_TRIG        (1)

#define DLL_ENABLE                (0)
#define DLL_DISABLE               (1)
#define DLL_RESET                 (1)

#define DCR_TYPE_DDR2             (0)
#define DCR_TYPE_DDR3             (1)
#define DCR_IO_WIDTH_8            (1)
#define DCR_IO_WIDTH_16           (2)
#define DCR_CHIP_DENSITY_256Mb    (0)
#define DCR_CHIP_DENSITY_512Mb    (1)
#define DCR_CHIP_DENSITY_1Gb      (2)
#define DCR_CHIP_DENSITY_2Gb      (3)
#define DCR_CHIP_DENSITY_4Gb      (4)
#define DCR_CHIP_DENSITY_8Gb      (5)
#define DCR_BUS_WIDTH_16          (1)
#define DCR_BUS_WIDTH_32          (3)
#define DCR_ONE_RANK              (0)
#define DCR_TWO_RANKS             (1)
#define DCR_CMD_ON_ALL_RANKS      (1)
#define DCR_INTERLEAVE_MODE       (1)

#define ZQCR0_IMP_DIV             (0x7b)

#define MR_BURST_LENGTH           (0)
#define MR_POWER_DOWN             (1)
#define MR_CAS_LATENCY            (2)
#define MR_WRITE_RECOVERY         (5)

#define DQS_GATE_ON               (1)
#define DQS_DRIFT_COMPENSATION    (0)

int sunxi_dram_init(void);

#endif /* _SUNXI_DRAM_H */
