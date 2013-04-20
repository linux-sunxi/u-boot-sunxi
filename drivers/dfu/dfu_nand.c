/*
 * dfu_nand.c -- DFU for NAND routines.
 *
 * Copyright (C) 2012-2013 Texas Instruments, Inc.
 *
 * Based on dfu_mmc.c which is:
 * Copyright (C) 2012 Samsung Electronics
 * author: Lukasz Majewski <l.majewski@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <div64.h>
#include <dfu.h>
#include <linux/mtd/mtd.h>
#include <jffs2/load_kernel.h>
#include <nand.h>

enum dfu_nand_op {
	DFU_OP_READ = 1,
	DFU_OP_WRITE,
};

static int nand_block_op(enum dfu_nand_op op, struct dfu_entity *dfu,
			u64 offset, void *buf, long *len)
{
	loff_t start, lim;
	size_t count, actual;
	int ret;
	nand_info_t *nand;

	/* if buf == NULL return total size of the area */
	if (buf == NULL) {
		*len = dfu->data.nand.size;
		return 0;
	}

	start = dfu->data.nand.start + offset + dfu->bad_skip;
	lim = dfu->data.nand.start + dfu->data.nand.size - start;
	count = *len;

	if (nand_curr_device < 0 ||
	    nand_curr_device >= CONFIG_SYS_MAX_NAND_DEVICE ||
	    !nand_info[nand_curr_device].name) {
		printf("%s: invalid nand device\n", __func__);
		return -1;
	}

	nand = &nand_info[nand_curr_device];

	if (op == DFU_OP_READ)
		ret = nand_read_skip_bad(nand, start, &count, &actual,
				lim, buf);
	else
		ret = nand_write_skip_bad(nand, start, &count, &actual,
				lim, buf, 0);

	if (ret != 0) {
		printf("%s: nand_%s_skip_bad call failed at %llx!\n",
		       __func__, op == DFU_OP_READ ? "read" : "write",
		       start);
		return ret;
	}

	/*
	 * Find out where we stopped writing data.  This can be deeper into
	 * the NAND than we expected due to having to skip bad blocks.  So
	 * we must take this into account for the next write, if any.
	 */
	if (actual > count)
		dfu->bad_skip += actual - count;

	return ret;
}

static inline int nand_block_write(struct dfu_entity *dfu,
		u64 offset, void *buf, long *len)
{
	return nand_block_op(DFU_OP_WRITE, dfu, offset, buf, len);
}

static inline int nand_block_read(struct dfu_entity *dfu,
		u64 offset, void *buf, long *len)
{
	return nand_block_op(DFU_OP_READ, dfu, offset, buf, len);
}

static int dfu_write_medium_nand(struct dfu_entity *dfu,
		u64 offset, void *buf, long *len)
{
	int ret = -1;

	switch (dfu->layout) {
	case DFU_RAW_ADDR:
		ret = nand_block_write(dfu, offset, buf, len);
		break;
	default:
		printf("%s: Layout (%s) not (yet) supported!\n", __func__,
		       dfu_get_layout(dfu->layout));
	}

	return ret;
}

static int dfu_read_medium_nand(struct dfu_entity *dfu, u64 offset, void *buf,
		long *len)
{
	int ret = -1;

	switch (dfu->layout) {
	case DFU_RAW_ADDR:
		ret = nand_block_read(dfu, offset, buf, len);
		break;
	default:
		printf("%s: Layout (%s) not (yet) supported!\n", __func__,
		       dfu_get_layout(dfu->layout));
	}

	return ret;
}

int dfu_fill_entity_nand(struct dfu_entity *dfu, char *s)
{
	char *st;
	int ret, dev, part;

	dfu->dev_type = DFU_DEV_NAND;
	st = strsep(&s, " ");
	if (!strcmp(st, "raw")) {
		dfu->layout = DFU_RAW_ADDR;
		dfu->data.nand.start = simple_strtoul(s, &s, 16);
		s++;
		dfu->data.nand.size = simple_strtoul(s, &s, 16);
	} else if (!strcmp(st, "part")) {
		char mtd_id[32];
		struct mtd_device *mtd_dev;
		u8 part_num;
		struct part_info *pi;

		dfu->layout = DFU_RAW_ADDR;

		dev = simple_strtoul(s, &s, 10);
		s++;
		part = simple_strtoul(s, &s, 10);

		sprintf(mtd_id, "%s%d,%d", "nand", dev, part - 1);
		printf("using id '%s'\n", mtd_id);

		mtdparts_init();

		ret = find_dev_and_part(mtd_id, &mtd_dev, &part_num, &pi);
		if (ret != 0) {
			printf("Could not locate '%s'\n", mtd_id);
			return -1;
		}

		dfu->data.nand.start = pi->offset;
		dfu->data.nand.size = pi->size;

	} else {
		printf("%s: Memory layout (%s) not supported!\n", __func__, st);
		return -1;
	}

	dfu->read_medium = dfu_read_medium_nand;
	dfu->write_medium = dfu_write_medium_nand;

	/* initial state */
	dfu->inited = 0;

	return 0;
}
