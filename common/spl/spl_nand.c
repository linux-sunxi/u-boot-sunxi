/*
 * Copyright (C) 2011
 * Corscience GmbH & Co. KG - Simon Schwarz <schwarz@corscience.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <config.h>
#include <spl.h>
#include <asm/io.h>
#include <nand.h>
#include <image.h>
#include <packimg.h>

#ifdef CONFIG_SUNXI

extern int sunxi_nand_spl_page_size;
extern int sunxi_nand_spl_block_size;

int nand_spl_isbad(uint32_t offs);
void nand_spl_read(uint32_t offs, int size, void *dst);
/*
static void load_uimage(struct image_header *header, uint32_t offs)
{
	char *name;
	uint32_t load_addr, size;

	nand_spl_load_image(offs, CONFIG_SYS_NAND_PAGE_SIZE, (void *)header);
	if (image_get_magic(header) != IH_MAGIC) {
		error("image at flash offset %x has no signature\n", offs);
		return;
	}

	load_addr = image_get_load(header);
	size = image_get_data_size(header);
	name = image_get_name(header);
	debug("load image %s from flash offset %x to memory %x size %x\n",
		  name, offs, load_addr, size);

	nand_spl_load_image(offs, size + sizeof(struct image_header), 
						load_addr - sizeof(struct image_header));
}
*/

static uint32_t calc_crc(void *buff, int size)
{
	int i;
	uint32_t ret = 0;
	for (i = 0; i < size; i += 4)
		ret += *(uint32_t *)(buff + i);
	return ret;
}

static int block_isbad(char *record, uint32_t start, uint32_t offs)
{
	int ret;
	int i = (offs - start) / CONFIG_SYS_NAND_BLOCK_SIZE;
	if (record[i] == 0) {
		ret = nand_spl_isbad(offs);
		if (ret)
			record[i] = 1;
		else
			record[i] = 2;
	}
	else {
		ret = record[i] == 1 ? 1 : 0;
	}
	return ret;
}

static void read_skip_bad(char *record, uint32_t start, 
						  uint32_t offs, uint32_t image_size, void *dst)
{
	int size = image_size;
	uint32_t to, len, bound;

	while (size > 0) {
		if (block_isbad(record, start, offs)) {
			offs += CONFIG_SYS_NAND_BLOCK_SIZE;
			continue;
		}
			
		to = roundup(offs, CONFIG_SYS_NAND_BLOCK_SIZE);
		bound = (to == offs) ? CONFIG_SYS_NAND_BLOCK_SIZE : (to - offs);
		len = bound > size ? size : bound;
		nand_spl_read(offs, len, dst);
		offs += len;
		dst += len;
		size -= len;
	}
}

static int load_packimg(uint32_t start, uint32_t end, void *buff)
{
	int i;
	struct pack_header *ph;
	struct pack_entry *pe;
	uint32_t offs = start, crc;
	int nblocks = (end - start) / CONFIG_SYS_NAND_BLOCK_SIZE;
	char record[nblocks];
	memset(record, 0, nblocks);

	while (offs < end) {
		read_skip_bad(record, start, offs, CONFIG_SYS_NAND_PAGE_SIZE, buff);
		ph = buff;
		pe = buff + sizeof(*ph);

		// check valid header
		if (ph->magic != PACK_MAGIC)
			goto next_block;
		crc = calc_crc(pe, ph->nentry * sizeof(*pe));
		if (ph->crc != crc)
			goto next_block;

		// load all entries
		for (i = 0; i < ph->nentry; i++) {
			read_skip_bad(record, start, offs + pe[i].offset, pe[i].size, (void *)pe[i].ldaddr);
			crc = calc_crc((void *)pe[i].ldaddr, pe[i].size);
			if (pe[i].crc != crc)
				goto next_block;
		}

		debug("load packimg at %x success\n", offs);
		return 0;

	next_block:
		error("invalid packimg at offset %x\n", offs);
		offs += CONFIG_SYS_NAND_BLOCK_SIZE;
	}

	error("load packimg from %x to %x fail\n", start, end);
	return -1;
}

#endif

void spl_nand_load_image(void)
{
	struct image_header *header;
	int *src __attribute__((unused));
	int *dst __attribute__((unused));

	debug("spl: nand - using hw ecc\n");
	nand_init();

	/*use CONFIG_SYS_TEXT_BASE as temporary storage area */
	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE);
#ifdef CONFIG_SPL_OS_BOOT
	if (!spl_start_uboot()) {

#ifdef CONFIG_SUNXI
		if (load_packimg(CONFIG_SUNXI_PACKIMG_START, CONFIG_SUNXI_PACKIMG_END, (void *)CONFIG_SYS_TEXT_BASE))
			goto uboot;
#else
		/*
		 * load parameter image
		 * load to temp position since nand_spl_load_image reads
		 * a whole block which is typically larger than
		 * CONFIG_CMD_SPL_WRITE_SIZE therefore may overwrite
		 * following sections like BSS
		 */
		nand_spl_load_image(CONFIG_CMD_SPL_NAND_OFS,
			CONFIG_CMD_SPL_WRITE_SIZE,
			(void *)CONFIG_SYS_TEXT_BASE);
		/* copy to destintion */
		for (dst = (int *)CONFIG_SYS_SPL_ARGS_ADDR,
				src = (int *)CONFIG_SYS_TEXT_BASE;
				src < (int *)(CONFIG_SYS_TEXT_BASE +
				CONFIG_CMD_SPL_WRITE_SIZE);
				src++, dst++) {
			writel(readl(src), dst);
		}
#endif

		/* load linux */
		nand_spl_load_image(CONFIG_SYS_NAND_SPL_KERNEL_OFFS,
			sizeof(*header), (void *)header);
		spl_parse_image_header(header);
		if (header->ih_os == IH_OS_LINUX) {
			/* happy - was a linux */
			nand_spl_load_image(CONFIG_SYS_NAND_SPL_KERNEL_OFFS,
				spl_image.size, (void *)spl_image.load_addr);
			nand_deselect();
			return;
		} else {
			puts("The Expected Linux image was not "
				"found. Please check your NAND "
				"configuration.\n");
			puts("Trying to start u-boot now...\n");
		}
	}
#endif
#ifdef CONFIG_SUNXI
uboot:
#endif
#ifdef CONFIG_NAND_ENV_DST
	nand_spl_load_image(CONFIG_ENV_OFFSET,
		sizeof(*header), (void *)header);
	spl_parse_image_header(header);
	nand_spl_load_image(CONFIG_ENV_OFFSET, spl_image.size,
		(void *)spl_image.load_addr);
#ifdef CONFIG_ENV_OFFSET_REDUND
	nand_spl_load_image(CONFIG_ENV_OFFSET_REDUND,
		sizeof(*header), (void *)header);
	spl_parse_image_header(header);
	nand_spl_load_image(CONFIG_ENV_OFFSET_REDUND, spl_image.size,
		(void *)spl_image.load_addr);
#endif
#endif
	/* Load u-boot */
	nand_spl_load_image(CONFIG_SYS_NAND_U_BOOT_OFFS,
		sizeof(*header), (void *)header);
	spl_parse_image_header(header);
	nand_spl_load_image(CONFIG_SYS_NAND_U_BOOT_OFFS,
		spl_image.size, (void *)spl_image.load_addr);
	nand_deselect();
}
