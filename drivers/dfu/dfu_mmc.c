/*
 * dfu.c -- DFU back-end routines
 *
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

enum dfu_mmc_op {
	DFU_OP_READ = 1,
	DFU_OP_WRITE,
};

static unsigned char __aligned(CONFIG_SYS_CACHELINE_SIZE)
				dfu_file_buf[CONFIG_SYS_DFU_MAX_FILE_SIZE];
static long dfu_file_buf_len;

static int mmc_block_op(enum dfu_mmc_op op, struct dfu_entity *dfu,
			u64 offset, void *buf, long *len)
{
	char cmd_buf[DFU_CMD_BUF_SIZE];
	u32 blk_start, blk_count;

	/*
	 * We must ensure that we work in lba_blk_size chunks, so ALIGN
	 * this value.
	 */
	*len = ALIGN(*len, dfu->data.mmc.lba_blk_size);

	blk_start = dfu->data.mmc.lba_start +
			(u32)lldiv(offset, dfu->data.mmc.lba_blk_size);
	blk_count = *len / dfu->data.mmc.lba_blk_size;
	if (blk_start + blk_count >
			dfu->data.mmc.lba_start + dfu->data.mmc.lba_size) {
		puts("Request would exceed designated area!\n");
		return -EINVAL;
	}

	sprintf(cmd_buf, "mmc %s %p %x %x",
		op == DFU_OP_READ ? "read" : "write",
		 buf, blk_start, blk_count);

	debug("%s: %s 0x%p\n", __func__, cmd_buf, cmd_buf);
	return run_command(cmd_buf, 0);
}

static int mmc_file_buffer(struct dfu_entity *dfu, void *buf, long *len)
{
	if (dfu_file_buf_len + *len > CONFIG_SYS_DFU_MAX_FILE_SIZE) {
		dfu_file_buf_len = 0;
		return -EINVAL;
	}

	/* Add to the current buffer. */
	memcpy(dfu_file_buf + dfu_file_buf_len, buf, *len);
	dfu_file_buf_len += *len;

	return 0;
}

static int mmc_file_op(enum dfu_mmc_op op, struct dfu_entity *dfu,
			void *buf, long *len)
{
	char cmd_buf[DFU_CMD_BUF_SIZE];
	char *str_env;
	int ret;

	switch (dfu->layout) {
	case DFU_FS_FAT:
		sprintf(cmd_buf, "fat%s mmc %d:%d 0x%x %s",
			op == DFU_OP_READ ? "load" : "write",
			dfu->data.mmc.dev, dfu->data.mmc.part,
			(unsigned int) buf, dfu->name);
		if (op == DFU_OP_WRITE)
			sprintf(cmd_buf + strlen(cmd_buf), " %lx", *len);
		break;
	case DFU_FS_EXT4:
		sprintf(cmd_buf, "ext4%s mmc %d:%d 0x%x /%s",
			op == DFU_OP_READ ? "load" : "write",
			dfu->data.mmc.dev, dfu->data.mmc.part,
			(unsigned int) buf, dfu->name);
		break;
	default:
		printf("%s: Layout (%s) not (yet) supported!\n", __func__,
		       dfu_get_layout(dfu->layout));
		return -1;
	}

	debug("%s: %s 0x%p\n", __func__, cmd_buf, cmd_buf);

	ret = run_command(cmd_buf, 0);
	if (ret) {
		puts("dfu: Read error!\n");
		return ret;
	}

	if (dfu->layout != DFU_RAW_ADDR && op == DFU_OP_READ) {
		str_env = getenv("filesize");
		if (str_env == NULL) {
			puts("dfu: Wrong file size!\n");
			return -1;
		}
		*len = simple_strtoul(str_env, NULL, 16);
	}

	return ret;
}

int dfu_write_medium_mmc(struct dfu_entity *dfu,
		u64 offset, void *buf, long *len)
{
	int ret = -1;

	switch (dfu->layout) {
	case DFU_RAW_ADDR:
		ret = mmc_block_op(DFU_OP_WRITE, dfu, offset, buf, len);
		break;
	case DFU_FS_FAT:
	case DFU_FS_EXT4:
		ret = mmc_file_buffer(dfu, buf, len);
		break;
	default:
		printf("%s: Layout (%s) not (yet) supported!\n", __func__,
		       dfu_get_layout(dfu->layout));
	}

	return ret;
}

int dfu_flush_medium_mmc(struct dfu_entity *dfu)
{
	int ret = 0;

	if (dfu->layout != DFU_RAW_ADDR) {
		/* Do stuff here. */
		ret = mmc_file_op(DFU_OP_WRITE, dfu, &dfu_file_buf,
				&dfu_file_buf_len);

		/* Now that we're done */
		dfu_file_buf_len = 0;
	}

	return ret;
}

int dfu_read_medium_mmc(struct dfu_entity *dfu, u64 offset, void *buf,
		long *len)
{
	int ret = -1;

	switch (dfu->layout) {
	case DFU_RAW_ADDR:
		ret = mmc_block_op(DFU_OP_READ, dfu, offset, buf, len);
		break;
	case DFU_FS_FAT:
	case DFU_FS_EXT4:
		ret = mmc_file_op(DFU_OP_READ, dfu, buf, len);
		break;
	default:
		printf("%s: Layout (%s) not (yet) supported!\n", __func__,
		       dfu_get_layout(dfu->layout));
	}

	return ret;
}

int dfu_fill_entity_mmc(struct dfu_entity *dfu, char *s)
{
	int dev, part;
	struct mmc *mmc;
	block_dev_desc_t *blk_dev;
	disk_partition_t partinfo;
	char *st;

	dfu->dev_type = DFU_DEV_MMC;
	st = strsep(&s, " ");
	if (!strcmp(st, "mmc")) {
		dfu->layout = DFU_RAW_ADDR;
		dfu->data.mmc.lba_start = simple_strtoul(s, &s, 16);
		dfu->data.mmc.lba_size = simple_strtoul(++s, &s, 16);
		dfu->data.mmc.lba_blk_size = get_mmc_blk_size(dfu->dev_num);
	} else if (!strcmp(st, "fat")) {
		dfu->layout = DFU_FS_FAT;
	} else if (!strcmp(st, "ext4")) {
		dfu->layout = DFU_FS_EXT4;
	} else if (!strcmp(st, "part")) {

		dfu->layout = DFU_RAW_ADDR;

		dev = simple_strtoul(s, &s, 10);
		s++;
		part = simple_strtoul(s, &s, 10);

		mmc = find_mmc_device(dev);
		if (mmc == NULL || mmc_init(mmc)) {
			printf("%s: could not find mmc device #%d!\n",
			       __func__, dev);
			return -ENODEV;
		}

		blk_dev = &mmc->block_dev;
		if (get_partition_info(blk_dev, part, &partinfo) != 0) {
			printf("%s: could not find partition #%d on mmc device #%d!\n",
			       __func__, part, dev);
			return -ENODEV;
		}

		dfu->data.mmc.lba_start = partinfo.start;
		dfu->data.mmc.lba_size = partinfo.size;
		dfu->data.mmc.lba_blk_size = partinfo.blksz;

	} else {
		printf("%s: Memory layout (%s) not supported!\n", __func__, st);
		return -ENODEV;
	}

	if (dfu->layout == DFU_FS_EXT4 || dfu->layout == DFU_FS_FAT) {
		dfu->data.mmc.dev = simple_strtoul(s, &s, 10);
		dfu->data.mmc.part = simple_strtoul(++s, &s, 10);
	}

	dfu->read_medium = dfu_read_medium_mmc;
	dfu->write_medium = dfu_write_medium_mmc;
	dfu->flush_medium = dfu_flush_medium_mmc;

	/* initial state */
	dfu->inited = 0;

	return 0;
}
