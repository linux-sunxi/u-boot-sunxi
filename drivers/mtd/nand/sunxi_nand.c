/*
 * sunxi_nand.c
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

static int read_offset = 0, write_offset = 0;
static int buffer_size = 8192 + 1024;
static char write_buffer[8192 + 1024] __attribute__((aligned(4)));
static char read_buffer[8192 + 1024] __attribute__((aligned(4)));
static struct nand_ecclayout sunxi_ecclayout;
static int program_column = -1, program_page = -1;

static void print_nand_clock(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	debug("============= nand clock ==============\n");
	debug("nand_sclk_cfg=%08x ahb_gate0=%08x\n", 
		  readl(&ccm->nand_sclk_cfg), readl(&ccm->ahb_gate0));
}

static void print_nand_gpio(void)
{
	struct sunxi_gpio *pio =
	    &((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[2];
	debug("============= nand gpio ===============\n");
	debug("cfg0=%08x cfg1=%08x cfg2=%08x\n", 
		  readl(&pio->cfg[0]), readl(&pio->cfg[1]), readl(&pio->cfg[2]));
}

/////////////////////////////////////////////////////////////////
// NFC
//

static void nfc_select_chip(struct mtd_info *mtd, int chip)
{
	uint32_t ctl;
	// A10 has 8 CE pin to support 8 flash chips
    ctl = readl(NFC_REG_CTL);
    ctl &= ~NFC_CE_SEL;
	ctl |= ((chip & 7) << 24);
    writel(ctl, NFC_REG_CTL);
}

static void nfc_cmdfunc(struct mtd_info *mtd, unsigned command, int column,
						int page_addr)
{
	int i;
	uint32_t cfg = command;
	int read_size, write_size, do_enable_ecc = 0;
	int addr_cycle, wait_rb_flag, byte_count, sector_count;
	addr_cycle = wait_rb_flag = byte_count = sector_count = 0;

	//debug("command %x ...\n", command);
	wait_cmdfifo_free();

	// switch to AHB
	writel(readl(NFC_REG_CTL) & ~NFC_RAM_METHOD, NFC_REG_CTL);

	switch (command) {
	case NAND_CMD_RESET:
	case NAND_CMD_ERASE2:
		break;
	case NAND_CMD_READID:
		addr_cycle = 1;
		// read 8 byte ID
		byte_count = 8;
		break;
	case NAND_CMD_PARAM:
		addr_cycle = 1;
		byte_count = 1024;
		wait_rb_flag = 1;
		break;
	case NAND_CMD_RNDOUT:
		addr_cycle = 2;
		writel(0xE0, NFC_REG_RCMD_SET);
		byte_count = mtd->oobsize;
		cfg |= NFC_SEQ | NFC_SEND_CMD2;
		wait_rb_flag = 1;
		break;
	case NAND_CMD_READOOB:
	case NAND_CMD_READ0:
		if (command == NAND_CMD_READOOB) {
			cfg = NAND_CMD_READ0;
			// sector num to read
			sector_count = 1024 / 1024;
			read_size = 1024;
			// OOB offset
			column += mtd->writesize;
		}
		else {
			sector_count = mtd->writesize / 1024;
			read_size = mtd->writesize;
			do_enable_ecc = 1;
			debug("cmdfunc read %d %d\n", column, page_addr);
		}
			
		//access NFC internal RAM by DMA bus
		writel(readl(NFC_REG_CTL) | NFC_RAM_METHOD, NFC_REG_CTL);
		// if the size is smaller than NFC_REG_SECTOR_NUM, read command won't finish
		// does that means the data read out (by DMA through random data output) hasn't finish?
		_dma_config_start(0, NFC_REG_IO_DATA, (__u32)read_buffer, read_size);
		addr_cycle = 5;
		// RAM0 is 1K size
		byte_count =1024;
		wait_rb_flag = 1;
		// 0x30 for 2nd cycle of read page
		// 0x05+0xe0 is the random data output command
		writel(0x00e00530, NFC_REG_RCMD_SET);
		// NFC_SEND_CMD1 for the command 1nd cycle enable
		// NFC_SEND_CMD2 for the command 2nd cycle enable
		// NFC_SEND_CMD3 & NFC_SEND_CMD4 for NFC_READ_CMD0 & NFC_READ_CMD1
		cfg |= NFC_SEND_CMD2 | NFC_DATA_SWAP_METHOD;
		// 3 - ?
		// 2 - page command
		// 1 - spare command?
		// 0 - normal command
		cfg |= 2 << 30;
		break;
	case NAND_CMD_ERASE1:
		addr_cycle = 3;
		//debug("cmdfunc earse block %d\n", page_addr);
		break;
	case NAND_CMD_SEQIN:	
		program_column = column;
		program_page = page_addr;
		write_offset = 0;
		return;
	case NAND_CMD_PAGEPROG:
		cfg = NAND_CMD_SEQIN;
		addr_cycle = 5;
		column = program_column;
		page_addr = program_page;
		debug("cmdfunc pageprog: %d %d\n", column, page_addr);

		// for write OOB
		if (column == mtd->writesize) {
			sector_count = 1024 /1024;
			write_size = 1024;
		}
		else if (column == 0) {
			sector_count = mtd->writesize / 1024;
			do_enable_ecc = 1;
			write_size = mtd->writesize;
			for (i = 0; i < sector_count; i++)
				writel(*((unsigned int *)(write_buffer + mtd->writesize) + i), NFC_REG_USER_DATA(i));
		}
		else {
			error("program unsupported column %d %d\n", column, page_addr);
			return;
		}

		//access NFC internal RAM by DMA bus
		writel(readl(NFC_REG_CTL) | NFC_RAM_METHOD, NFC_REG_CTL);
		_dma_config_start(1, (__u32)write_buffer, NFC_REG_IO_DATA, write_size);
		// RAM0 is 1K size
		byte_count =1024;
		writel(0x00008510, NFC_REG_WCMD_SET);
		cfg |= NFC_SEND_CMD2 | NFC_DATA_SWAP_METHOD | NFC_ACCESS_DIR;
		cfg |= 2 << 30;
		if (column != 0) {
			debug("cmdfunc program %d %d with %x %x %x\n", column, page_addr, 
					 write_buffer[0], write_buffer[1], write_buffer[2]);
		}
		break;
	case NAND_CMD_STATUS:
		byte_count = 1;
		break;
	default:
		error("unknown command\n");
		return;
	}

	// address cycle
	if (addr_cycle) {
		uint32_t low = 0;
		uint32_t high = 0;
		switch (addr_cycle) {
		case 2:
			low = column & 0xffff;
			break;
		case 3:
			low = page_addr & 0xffffff;
			break;
		case 5:
			high = (page_addr >> 16) & 0xff;
		case 4:
			low = (column & 0xffff) | (page_addr << 16);
			break;
		}
		writel(low, NFC_REG_ADDR_LOW);
		writel(high, NFC_REG_ADDR_HIGH);
		cfg |= NFC_SEND_ADR;
		cfg |= ((addr_cycle - 1) << 16);
	}

	// command will wait until the RB ready to mark finish?
	if (wait_rb_flag)
		cfg |= NFC_WAIT_FLAG;

	// will fetch data
	if (byte_count) {
		cfg |= NFC_DATA_TRANS;
		writel(byte_count, NFC_REG_CNT);
	}

	// set sectors
	if (sector_count)
		writel(sector_count, NFC_REG_SECTOR_NUM);

	// enable ecc
	if (do_enable_ecc)
		enable_ecc(1);

	// send command
	cfg |= NFC_SEND_CMD1;
	writel(cfg, NFC_REG_CMD);

	switch (command) {
	case NAND_CMD_READ0:
	case NAND_CMD_READOOB:
	case NAND_CMD_PAGEPROG:
		_wait_dma_end();
		break;
	}

	// wait command send complete
	wait_cmdfifo_free();
	wait_cmd_finish();

	// reset will wait for RB ready
	switch (command) {
	case NAND_CMD_RESET:
		// wait rb0 ready
		select_rb(0);
		while (!check_rb_ready(0));
		// wait rb1 ready
		select_rb(1);
		while (!check_rb_ready(1));
		// select rb 0 back
		select_rb(0);
		break;
	case NAND_CMD_READ0:
		for (i = 0; i < sector_count; i++)
			*((unsigned int *)(read_buffer + mtd->writesize) + i) = readl(NFC_REG_USER_DATA(i));
		break;
	}

	if (do_enable_ecc)
		disable_ecc();

	//debug("done\n");

	// read write offset
	read_offset = 0;
}

static uint8_t nfc_read_byte(struct mtd_info *mtd)
{
	return readb(NFC_RAM0_BASE + read_offset++);
}

static int nfc_dev_ready(struct mtd_info *mtd)
{
	return check_rb_ready(0);
}

static void nfc_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	if (write_offset + len > buffer_size) {
		error("write too much offset=%d len=%d buffer size=%d\n",
				 write_offset, len, buffer_size);
		return;
	}
	memcpy(write_buffer + write_offset, buf, len);
	write_offset += len;
}

static void nfc_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	if (read_offset + len > buffer_size) {
		error("read too much offset=%d len=%d buffer size=%d\n", 
				 read_offset, len, buffer_size);
		return;
	}
	memcpy(buf, read_buffer + read_offset, len);
	read_offset += len;
}

static int get_chip_status(struct mtd_info *mtd)
{
	struct nand_chip *nand = mtd->priv;
	nand->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);
	return nand->read_byte(mtd);
}

// For erase and program command to wait for chip ready
static int nfc_wait(struct mtd_info *mtd, struct nand_chip *chip)
{
	while (!check_rb_ready(0));
	return get_chip_status(mtd);
}

static void nfc_ecc_hwctl(struct mtd_info *mtd, int mode)
{

}

static int nfc_ecc_calculate(struct mtd_info *mtd, const uint8_t *dat, uint8_t *ecc_code)
{
	return 0;
}

static int nfc_ecc_correct(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
	return check_ecc(mtd->writesize / 1024);
}

//////////////////////////////////////////////////////////////////////////////////
// 1K mode for SPL read/write

struct save_1k_mode {
	uint32_t ctl;
	uint32_t ecc_ctl;
	uint32_t spare_area;
};

static void enter_1k_mode(struct save_1k_mode *save)
{
	uint32_t ctl;

	ctl = readl(NFC_REG_CTL);
	save->ctl = ctl;
	ctl &= ~NFC_PAGE_SIZE;
	writel(ctl, NFC_REG_CTL);
	
	ctl = readl(NFC_REG_ECC_CTL);
	save->ecc_ctl = ctl;
	set_ecc_mode(8);

	ctl = readl(NFC_REG_SPARE_AREA);
	save->spare_area = ctl;
	writel(1024, NFC_REG_SPARE_AREA);
}

static void exit_1k_mode(struct save_1k_mode *save)
{
	writel(save->ctl, NFC_REG_CTL);
	writel(save->ecc_ctl, NFC_REG_ECC_CTL);
	writel(save->spare_area, NFC_REG_SPARE_AREA);
}

void nfc_read_page1k(uint32_t page_addr, void *buff)
{
	struct save_1k_mode save;
	uint32_t cfg = NAND_CMD_READ0 | NFC_SEQ | NFC_SEND_CMD1 | NFC_DATA_TRANS | NFC_SEND_ADR | 
		NFC_SEND_CMD2 | ((5 - 1) << 16) | NFC_WAIT_FLAG | NFC_DATA_SWAP_METHOD | (2 << 30);

	nfc_select_chip(NULL, 0);

	wait_cmdfifo_free();

	enter_1k_mode(&save);

	writel(readl(NFC_REG_CTL) | NFC_RAM_METHOD, NFC_REG_CTL);
	_dma_config_start(0, NFC_REG_IO_DATA, (uint32_t)buff, 1024);

	writel(page_addr << 16, NFC_REG_ADDR_LOW);
	writel(page_addr >> 16, NFC_REG_ADDR_HIGH);
	writel(1024, NFC_REG_CNT);
	writel(0x00e00530, NFC_REG_RCMD_SET);
	writel(1, NFC_REG_SECTOR_NUM);

	enable_random();

	enable_ecc(1);

	writel(cfg, NFC_REG_CMD);

	_wait_dma_end();
	wait_cmdfifo_free();
	wait_cmd_finish();

	disable_ecc();
	check_ecc(1);

	disable_random();

	exit_1k_mode(&save);

	nfc_select_chip(NULL, -1);
}

void nfc_write_page1k(uint32_t page_addr, void *buff)
{
	struct save_1k_mode save;
	uint32_t cfg = NAND_CMD_SEQIN | NFC_SEQ | NFC_SEND_CMD1 | NFC_DATA_TRANS | NFC_SEND_ADR | 
		NFC_SEND_CMD2 | ((5 - 1) << 16) | NFC_WAIT_FLAG | NFC_DATA_SWAP_METHOD | NFC_ACCESS_DIR | 
		(2 << 30);

	nfc_select_chip(NULL, 0);

	wait_cmdfifo_free();

	enter_1k_mode(&save);

	writel(readl(NFC_REG_CTL) | NFC_RAM_METHOD, NFC_REG_CTL);
	_dma_config_start(1, (uint32_t)buff, NFC_REG_IO_DATA, 1024);

	writel(page_addr << 16, NFC_REG_ADDR_LOW);
	writel(page_addr >> 16, NFC_REG_ADDR_HIGH);
	writel(1024, NFC_REG_CNT);
	writel(0x00008510, NFC_REG_WCMD_SET);
	writel(1, NFC_REG_SECTOR_NUM);

	enable_random();

	enable_ecc(1);

	writel(cfg, NFC_REG_CMD);

	_wait_dma_end();
	wait_cmdfifo_free();
	wait_cmd_finish();

	disable_ecc();

	disable_random();

	exit_1k_mode(&save);

	nfc_select_chip(NULL, -1);
}

//////////////////////////////////////////////////////////////////////////////////////

static void print_nand_nfc(void)
{
	debug("=============== nand bfc ===============\n");
	debug("CTL=%08x ST=%08x INT=%08x TIMING_CTL=%08x TIMING_CFG=%08x\n",
		  readl(NFC_REG_CTL), readl(NFC_REG_ST), readl(NFC_REG_INT), 
		  readl(NFC_REG_TIMING_CTL), readl(NFC_REG_TIMING_CFG));
	debug("ADDR_LOW=%08x ADDR_HIGH=%08x SECTOR_NUM=%08x CNT=%08x\n",
		  readl(NFC_REG_ADDR_LOW), readl(NFC_REG_ADDR_HIGH), 
		  readl(NFC_REG_SECTOR_NUM), readl(NFC_REG_CNT));
	debug("CMD=%08x RCMD=%08x WCMD=%08x ECC_CTL=%08x ECC_ST=%08x\n",
		  readl(NFC_REG_CMD), readl(NFC_REG_RCMD_SET), readl(NFC_REG_WCMD_SET),
		  readl(NFC_REG_ECC_CTL), readl(NFC_REG_ECC_ST));
	debug("SPARE_AREA=%08x\n", readl(NFC_REG_SPARE_AREA));
}

static void print_nand_regs(void)
{
	print_nand_clock();
	print_nand_gpio();
	print_nand_nfc();
}

int board_nand_init(struct nand_chip *nand)
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

	//print_nand_regs();

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
	nfc_cmdfunc(NULL, NAND_CMD_RESET, -1, -1);
	// read nand chip id
	nfc_cmdfunc(NULL, NAND_CMD_READID, 0, -1);
	for (i = 0; i < 8; i++)
		id[i] = nfc_read_byte(NULL);

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
		error("Flash chip page shift out of range %d\n", (int)chip_param->page_shift);
		return -EINVAL;
	}
	// 0 for 1K
	ctl |= (((int)chip_param->page_shift - 10) & 0xf) << 8;
	writel(ctl, NFC_REG_CTL);

	writel(0xff, NFC_REG_TIMING_CFG);
	writel(1U << chip_param->page_shift, NFC_REG_SPARE_AREA);

	// disable random
	disable_random();

	// setup ECC layout
	sunxi_ecclayout.eccbytes = 0;
	sunxi_ecclayout.oobavail = (1U << chip_param->page_shift) / 1024 * 4 - 2;
	sunxi_ecclayout.oobfree->offset = 1;
	sunxi_ecclayout.oobfree->length = (1U << chip_param->page_shift) / 1024 * 4 - 2;
	nand->ecc.layout = &sunxi_ecclayout;
	nand->ecc.size = (1U << chip_param->page_shift);
	nand->ecc.bytes = 0;
	nand->ecc.strength = 40; /*TODO: move in proper place and put proper value*/

	// set buffer size
	buffer_size = (1U << chip_param->page_shift) + 1024;

	// setup DMA
	dma_hdle = DMA_Request(DMAC_DMATYPE_DEDICATED);
	if (dma_hdle == 0) {
		error("request DMA fail\n");
		return -ENODEV;
	}
	print_nand_dma(dma_hdle);

	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.hwctl = nfc_ecc_hwctl;
	nand->ecc.calculate = nfc_ecc_calculate;
	nand->ecc.correct = nfc_ecc_correct;
	nand->select_chip = nfc_select_chip;
	nand->dev_ready = nfc_dev_ready;
	nand->cmdfunc = nfc_cmdfunc;
	nand->read_byte = nfc_read_byte;
	nand->read_buf = nfc_read_buf;
	nand->write_buf = nfc_write_buf;
	nand->waitfunc = nfc_wait;
	nand->bbt_options = NAND_BBT_USE_FLASH;

	debug("board_nand_init finish\n");

	//print_nand_regs();

	return 0;
}

