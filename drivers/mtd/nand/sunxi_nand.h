/*
 * sunxi_nand.h
 *
 * Copyright (C) 2013 Qiang Yu <yuq825@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SUNXI_NAND_H
#define _SUNXI_NAND_H

#include <common.h>
#include <nand.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch/dma.h>
#include <asm/arch/clock.h>

#define NFC_REG_CTL               0x01c03000
#define NFC_REG_ST                0x01c03004
#define NFC_REG_INT               0x01c03008
#define NFC_REG_TIMING_CTL        0x01c0300c
#define NFC_REG_TIMING_CFG        0x01c03010
#define NFC_REG_ADDR_LOW          0x01c03014
#define NFC_REG_ADDR_HIGH         0x01c03018
#define NFC_REG_SECTOR_NUM        0x01c0301c
#define NFC_REG_CNT		          0x01c03020
#define NFC_REG_CMD		          0x01c03024
#define NFC_REG_RCMD_SET          0x01c03028
#define NFC_REG_WCMD_SET          0x01c0302c
#define NFC_REG_IO_DATA           0x01c03030
#define NFC_REG_ECC_CTL           0x01c03034
#define NFC_REG_ECC_ST            0x01c03038
#define NFC_REG_DEBUG             0x01c0303c
#define NFC_REG_ECC_CNT0          0x01c03040
#define NFC_REG_ECC_CNT1          0x01c03044
#define NFC_REG_ECC_CNT2          0x01c03048
#define NFC_REG_ECC_CNT3          0x01c0304c
#define NFC_REG_USER_DATA_BASE    0x01c03050
#define NFC_REG_USER_DATA(i)      ( NFC_REG_USER_DATA_BASE + 4 * i )
#define NFC_REG_SPARE_AREA        0x01c030a0
#define NFC_RAM0_BASE             0x01c03400

/*define bit use in NFC_CTL*/
#define NFC_EN					(1 << 0)
#define NFC_RESET				(1 << 1)
#define NFC_BUS_WIDYH			(1 << 2)
#define NFC_RB_SEL				(1 << 3)
#define NFC_CE_SEL				(7 << 24)
#define NFC_CE_CTL				(1 << 6)
#define NFC_CE_CTL1				(1 << 7)
#define NFC_PAGE_SIZE			(0xf << 8)
#define NFC_SAM					(1 << 12)
#define NFC_RAM_METHOD			(1 << 14)
#define NFC_DEBUG_CTL			(1 << 31)

/*define bit use in NFC_ST*/
#define NFC_RB_B2R				(1 << 0)
#define NFC_CMD_INT_FLAG		(1 << 1)
#define NFC_DMA_INT_FLAG		(1 << 2)
#define NFC_CMD_FIFO_STATUS		(1 << 3)
#define NFC_STA					(1 << 4)
#define NFC_NATCH_INT_FLAG		(1 << 5)
#define NFC_RB_STATE0			(1 << 8)
#define NFC_RB_STATE1			(1 << 9)
#define NFC_RB_STATE2			(1 << 10)
#define NFC_RB_STATE3			(1 << 11)

/*define bit use in NFC_INT*/
#define NFC_B2R_INT_ENABLE		(1 << 0)
#define NFC_CMD_INT_ENABLE		(1 << 1)
#define NFC_DMA_INT_ENABLE		(1 << 2)


/*define bit use in NFC_CMD*/
#define NFC_CMD_LOW_BYTE		(0xff << 0)
#define NFC_CMD_HIGH_BYTE		(0xff << 8)
#define NFC_ADR_NUM				(0x7 << 16)
#define NFC_SEND_ADR			(1 << 19)
#define NFC_ACCESS_DIR			(1 << 20)
#define NFC_DATA_TRANS			(1 << 21)
#define NFC_SEND_CMD1			(1 << 22)
#define NFC_WAIT_FLAG			(1 << 23)
#define NFC_SEND_CMD2			(1 << 24)
#define NFC_SEQ					(1 << 25)
#define NFC_DATA_SWAP_METHOD	(1 << 26)
#define NFC_ROW_AUTO_INC		(1 << 27)
#define NFC_SEND_CMD3           (1 << 28)
#define NFC_SEND_CMD4           (1 << 29)
#define NFC_CMD_TYPE			(3 << 30)

/* define bit use in NFC_RCMD_SET*/
#define NFC_READ_CMD			(0xff<< 0)
#define NFC_RANDOM_READ_CMD0 	(0xff << 8)
#define NFC_RANDOM_READ_CMD1 	(0xff << 16)

/*define bit use in NFC_WCMD_SET*/
#define NFC_PROGRAM_CMD			(0xff << 0)
#define NFC_RANDOM_WRITE_CMD	(0xff << 8)
#define NFC_READ_CMD0			(0xff << 16)
#define NFC_READ_CMD1	        (0xff << 24)

/*define bit use in NFC_ECC_CTL*/
#define NFC_ECC_EN				(1 << 0)
#define NFC_ECC_PIPELINE		(1 << 3)
#define NFC_ECC_EXCEPTION       (1 << 4)
#define NFC_ECC_BLOCK_SIZE		(1 << 5)
#define NFC_RANDOM_EN           (1 << 9 )
#define NFC_RANDOM_DIRECTION    (1 << 10 )
#define NFC_ECC_MODE_SHIFT      12
#define NFC_ECC_MODE			(0xf << NFC_ECC_MODE_SHIFT)
#define NFC_RANDOM_SEED         (0x7fff << 16)

#define NFC_IRQ_MAJOR		    13
/*cmd flag bit*/
#define NFC_PAGE_MODE  			0x1
#define NFC_NORMAL_MODE  		0x0

#define NFC_DATA_FETCH 			0x1
#define NFC_NO_DATA_FETCH 		0x0
#define NFC_MAIN_DATA_FETCH 	0x1
#define NFC_SPARE_DATA_FETCH	0X0
#define NFC_WAIT_RB				0x1
#define NFC_NO_WAIT_RB			0x0
#define NFC_IGNORE				0x0

#define NFC_INT_RB				0
#define NFC_INT_CMD				1
#define NFC_INT_DMA				2
#define NFC_INT_BATCh			5

struct nand_chip_param {
	unsigned char id[8];
	unsigned char id_len;
	unsigned char page_shift;
	unsigned char page_per_block_shift;
	unsigned char clock_freq; //the highest access frequence of the nand flash chip, based on MHz
	unsigned char ecc_mode;   //the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
};

struct nand_chip_param *sunxi_get_nand_chip_param(unsigned char mf);

extern int dma_hdle;

#define NAND_MAX_CLOCK (10 * 1000000)

void sunxi_nand_set_clock(int hz);
void sunxi_nand_set_gpio(void);

/////////////////////////////////////////////////////////////////
// Utils
//

static inline void wait_cmdfifo_free(void)
{
	int timeout = 0xffff;
	while ((timeout--) && (readl(NFC_REG_ST) & NFC_CMD_FIFO_STATUS));
	if (timeout <= 0) {
		error("wait_cmdfifo_free timeout\n");
	}
}

static inline void wait_cmd_finish(void)
{
	int timeout = 0xffff;
	while((timeout--) && !(readl(NFC_REG_ST) & NFC_CMD_INT_FLAG));
	if (timeout <= 0) {
		error("wait_cmd_finish timeout\n");
		return;
	}
	writel(NFC_CMD_INT_FLAG, NFC_REG_ST);
}

// 1 for ready, 0 for not ready
static inline int check_rb_ready(int rb)
{
	return (readl(NFC_REG_ST) & (NFC_RB_STATE0 << (rb & 0x3))) ? 1 : 0;
}

void select_rb(int rb);
void enable_random(void);
void disable_random(void);
void enable_ecc(int pipline);
int check_ecc(int eblock_cnt);
void set_ecc_mode(int mode);
void disable_ecc(void);
void _dma_config_start(__u32 rw, __u32 src_addr, __u32 dst_addr, __u32 len);
__s32 _wait_dma_end(void);

void nfc_read_page1k(uint32_t page_addr, void *buff);
void nfc_write_page1k(uint32_t page_addr, void *buff);

#endif

