/*
 *
 *	based on code of fs/reiserfs/dev.c by
 *
 *	(C) Copyright 2003 - 2004
 *	Sysgo AG, <www.elinos.com>, Pavel Bartusek <pba@sysgo.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <common.h>
#include <config.h>
#include <zfs_common.h>

static block_dev_desc_t *zfs_block_dev_desc;
static disk_partition_t part_info;

int zfs_set_blk_dev(block_dev_desc_t *rbdd, int part)
{
	zfs_block_dev_desc = rbdd;

	if (part == 0) {
		/* disk doesn't use partition table */
		part_info.start = 0;
		part_info.size = rbdd->lba;
		part_info.blksz = rbdd->blksz;
	} else {
		if (get_partition_info(zfs_block_dev_desc, part, &part_info))
			return 0;
	}

	return part_info.size;
}

/* err */
int zfs_devread(int sector, int byte_offset, int byte_len, char *buf)
{
	short sec_buffer[SECTOR_SIZE/sizeof(short)];
	char *sec_buf = sec_buffer;
	unsigned block_len;

	/*
	 *	Check partition boundaries
	 */
	if ((sector < 0) ||
		((sector + ((byte_offset + byte_len - 1) >> SECTOR_BITS)) >=
		 part_info.size)) {
		/*		errnum = ERR_OUTSIDE_PART; */
		printf(" ** zfs_devread() read outside partition sector %d\n", sector);
		return 1;
	}

	/*
	 *	Get the read to the beginning of a partition.
	 */
	sector += byte_offset >> SECTOR_BITS;
	byte_offset &= SECTOR_SIZE - 1;

	debug(" <%d, %d, %d>\n", sector, byte_offset, byte_len);

	if (zfs_block_dev_desc == NULL) {
		printf("** Invalid Block Device Descriptor (NULL)\n");
		return 1;
	}

	if (byte_offset != 0) {
		/* read first part which isn't aligned with start of sector */
		if (zfs_block_dev_desc->block_read(zfs_block_dev_desc->dev,
										   part_info.start + sector, 1,
										   (unsigned long *) sec_buf) != 1) {
			printf(" ** zfs_devread() read error **\n");
			return 1;
		}
		memcpy(buf, sec_buf + byte_offset,
			   min(SECTOR_SIZE - byte_offset, byte_len));
		buf += min(SECTOR_SIZE - byte_offset, byte_len);
		byte_len -= min(SECTOR_SIZE - byte_offset, byte_len);
		sector++;
	}

	if (byte_len == 0)
		return 0;

	/*	read sector aligned part */
	block_len = byte_len & ~(SECTOR_SIZE - 1);

	if (block_len == 0) {
		u8 p[SECTOR_SIZE];

		block_len = SECTOR_SIZE;
		zfs_block_dev_desc->block_read(zfs_block_dev_desc->dev,
									   part_info.start + sector,
									   1, (unsigned long *)p);
		memcpy(buf, p, byte_len);
		return 0;
	}

	if (zfs_block_dev_desc->block_read(zfs_block_dev_desc->dev,
									   part_info.start + sector,
									   block_len / SECTOR_SIZE,
									   (unsigned long *) buf) !=
		block_len / SECTOR_SIZE) {
		printf(" ** zfs_devread() read error - block\n");
		return 1;
	}

	block_len = byte_len & ~(SECTOR_SIZE - 1);
	buf += block_len;
	byte_len -= block_len;
	sector += block_len / SECTOR_SIZE;

	if (byte_len != 0) {
		/* read rest of data which are not in whole sector */
		if (zfs_block_dev_desc->
			block_read(zfs_block_dev_desc->dev,
					   part_info.start + sector, 1,
					   (unsigned long *) sec_buf) != 1) {
			printf(" ** zfs_devread() read error - last part\n");
			return 1;
		}
		memcpy(buf, sec_buf, byte_len);
	}
	return 0;
}
