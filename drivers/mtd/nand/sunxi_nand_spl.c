/*
 * sunxi_nand_spl.c
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

#include "sunxi_nand.h"

int sunxi_nand_spl_page_size;
int sunxi_nand_spl_block_size;

static int nfc_isbad(uint32_t offs)
{
	uint32_t page_addr;
	uint32_t cfg = NAND_CMD_READ0 | NFC_SEQ | NFC_SEND_CMD1 | NFC_DATA_TRANS | NFC_SEND_ADR | 
		NFC_SEND_CMD2 | ((5 - 1) << 16) | NFC_WAIT_FLAG | (0 << 30);

	offs &= ~(sunxi_nand_spl_block_size - 1);
	page_addr = offs / sunxi_nand_spl_page_size;

	wait_cmdfifo_free();
	writel(readl(NFC_REG_CTL) & ~NFC_RAM_METHOD, NFC_REG_CTL);
	writel(sunxi_nand_spl_page_size | (page_addr << 16), NFC_REG_ADDR_LOW);
	writel(page_addr >> 16, NFC_REG_ADDR_HIGH);
	writel(2, NFC_REG_CNT);
	writel(1, NFC_REG_SECTOR_NUM);
	writel(NAND_CMD_READSTART, NFC_REG_RCMD_SET);
	writel(cfg, NFC_REG_CMD);
	wait_cmdfifo_free();
	wait_cmd_finish();
	if (readb(NFC_RAM0_BASE) != 0xff)
		return 1;
	return 0;
}

static void nfc_read_page(uint32_t offs, void *buff)
{
	uint32_t page_addr;
	uint32_t cfg = NAND_CMD_READ0 | NFC_SEND_CMD2 | NFC_DATA_SWAP_METHOD | NFC_SEND_CMD1 |
		NFC_SEND_ADR | ((5 - 1) << 16) | NFC_WAIT_FLAG | NFC_DATA_TRANS | (2 << 30);

	page_addr = offs / sunxi_nand_spl_page_size;

	wait_cmdfifo_free();
	writel(readl(NFC_REG_CTL) | NFC_RAM_METHOD, NFC_REG_CTL);
	_dma_config_start(0, NFC_REG_IO_DATA, (__u32)buff, sunxi_nand_spl_page_size);
	writel(page_addr << 16, NFC_REG_ADDR_LOW);
	writel(page_addr >> 16, NFC_REG_ADDR_HIGH);
	writel(0x00e00530, NFC_REG_RCMD_SET);
	writel(1024, NFC_REG_CNT);
	writel(sunxi_nand_spl_page_size / 1024, NFC_REG_SECTOR_NUM);
	enable_ecc(1);
	writel(cfg, NFC_REG_CMD);
	_wait_dma_end();
	wait_cmdfifo_free();
	wait_cmd_finish();
	disable_ecc();
	if (check_ecc(sunxi_nand_spl_page_size / 1024) < 0)
		error("can't correct bit error of page read at offset %x\n", offs);
}

static void nfc_reset(void)
{
	u32 cfg;

	wait_cmdfifo_free();
	cfg = NAND_CMD_RESET | NFC_SEND_CMD1;
	writel(cfg, NFC_REG_CMD);
	wait_cmdfifo_free();
	wait_cmd_finish();
	// wait rb0 ready
	select_rb(0);
	while (!check_rb_ready(0));
	// wait rb1 ready
	select_rb(1);
	while (!check_rb_ready(1));
	// select rb 0 back
	select_rb(0);
}

static void nfc_readid(uint8_t *id)
{
	u32 cfg;
	int i;

	wait_cmdfifo_free();
	writel(0, NFC_REG_ADDR_LOW);
	writel(0, NFC_REG_ADDR_HIGH);
	cfg = NAND_CMD_READID | NFC_SEND_ADR | NFC_DATA_TRANS | NFC_SEND_CMD1;
	writel(8, NFC_REG_CNT);
	writel(cfg, NFC_REG_CMD);
	wait_cmdfifo_free();
	wait_cmd_finish();

	for (i = 0; i < 8; i++)
		id[i] = readb(NFC_RAM0_BASE + i);
}

static void nfc_select_chip(int chip)
{
	uint32_t ctl;
	// A10 has 8 CE pin to support 8 flash chips
    ctl = readl(NFC_REG_CTL);
    ctl &= ~NFC_CE_SEL;
	ctl |= ((chip & 7) << 24);
    writel(ctl, NFC_REG_CTL);
}

static int nfc_init(void)
{
	u32 ctl;
	int i, j;
	uint8_t id[8];
	struct nand_chip_param *nand_chip_param, *chip_param = NULL;

	debug("board_nand_init start\n");

	// set init clock
	sunxi_nand_set_clock(NAND_MAX_CLOCK);

	// set gpio
	sunxi_nand_set_gpio();

	// reset NFC
	ctl = readl(NFC_REG_CTL);
	ctl |= NFC_RESET;
	writel(ctl, NFC_REG_CTL);
	while(readl(NFC_REG_CTL) & NFC_RESET);

	// enable NFC
	ctl = NFC_EN;
	writel(ctl, NFC_REG_CTL);

	// serial_access_mode = 1
	ctl = (1 << 8);
	writel(ctl, NFC_REG_TIMING_CTL);

	// reset nand chip
	nfc_reset();

	// read nand chip id
	nfc_readid(id);

	// find chip
	nand_chip_param = sunxi_get_nand_chip_param(id[0]);
	for (i = 0; nand_chip_param[i].id_len; i++) {
		int find = 1;
		for (j = 0; j < nand_chip_param[i].id_len; j++) {
			if (id[j] != nand_chip_param[i].id[j]) {
				find = 0;
				break;
			}
		}
		if (find) {
			chip_param = &nand_chip_param[i];
			debug("find nand chip in sunxi database\n");
			break;
		}
	}

	// not find
	if (chip_param == NULL) {
		error("can't find nand chip in sunxi database\n");
		return -ENODEV;
	}

	// set final NFC clock freq
	if (chip_param->clock_freq > 30)
		chip_param->clock_freq = 30;
	sunxi_nand_set_clock((int)chip_param->clock_freq * 1000000);
	debug("set final clock freq to %dMHz\n", (int)chip_param->clock_freq);

	// disable interrupt
	writel(0, NFC_REG_INT);
	// clear interrupt
	writel(readl(NFC_REG_ST), NFC_REG_ST);

	// set ECC mode
	ctl = readl(NFC_REG_ECC_CTL);
	ctl &= ~NFC_ECC_MODE;
	ctl |= (unsigned int)chip_param->ecc_mode << NFC_ECC_MODE_SHIFT;
	writel(ctl, NFC_REG_ECC_CTL);

	// enable NFC
	ctl = NFC_EN;

	// Page size
	if (chip_param->page_shift > 14 || chip_param->page_shift < 10) {
		error("Flash chip page shift out of range %d\n", chip_param->page_shift);
		return -EINVAL;
	}
	// 0 for 1K
	ctl |= ((chip_param->page_shift - 10) & 0xf) << 8;
	writel(ctl, NFC_REG_CTL);

	writel(0xff, NFC_REG_TIMING_CFG);
	writel(1U << chip_param->page_shift, NFC_REG_SPARE_AREA);

	// disable random
	disable_random();

	// record size
	sunxi_nand_spl_page_size = 1U << chip_param->page_shift;
	sunxi_nand_spl_block_size = 1U << (chip_param->page_per_block_shift + chip_param->page_shift);

	// setup DMA
	dma_hdle = DMA_Request(DMAC_DMATYPE_DEDICATED);
	if (dma_hdle == 0) {
		error("request DMA fail\n");
		return -ENODEV;
	}

	debug("board_nand_init finish\n");

	return 0;
}

int nand_spl_isbad(uint32_t offs)
{
	return nfc_isbad(offs);
}

void nand_spl_read(uint32_t offs, int size, void *dst)
{
	// offs must be page aligned
	while (size > 0) {
		nfc_read_page(offs, dst);
		offs += sunxi_nand_spl_page_size;
		dst += sunxi_nand_spl_page_size;
		size -= sunxi_nand_spl_page_size;
	}
}

int nand_spl_load_image(uint32_t offs, unsigned int image_size, void *dst)
{
	int size = image_size;
	uint32_t to, len, bound;

	debug("nand spl load image from %x to %x size %x\n", offs, dst, size);

	while (size > 0) {
		if (nand_spl_isbad(offs)) {
			debug("nand spl block %x is bad\n", offs);
			offs += sunxi_nand_spl_block_size;
			continue;
		}
			
		to = roundup(offs, sunxi_nand_spl_block_size);
		bound = (to == offs) ? sunxi_nand_spl_block_size : (to - offs);
		len = bound > size ? size : bound;
		nand_spl_read(offs, len, dst);
		offs += len;
		dst += len;
		size -= len;
	}

	return 0;
}

/* nand_init() - initialize data to make nand usable by SPL */
void nand_init(void)
{
	if (nfc_init())
		return;

	nfc_select_chip(0);
}

/* Unselect after operation */
void nand_deselect(void)
{

}


