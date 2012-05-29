/*
 * ZFS filesystem implementation in Uboot by
 * Jorgen Lundman <lundman at lundman.net>
 *
 * zfsfs support
 * made from existing GRUB Sources by Sun, GNU and others.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ZFS_COMMON__
#define __ZFS_COMMON__

#define SECTOR_SIZE			0x200
#define SECTOR_BITS			9

#define grub_le_to_cpu16 le16_to_cpu
#define grub_be_to_cpu16 be16_to_cpu
#define grub_le_to_cpu32 le32_to_cpu
#define grub_be_to_cpu32 be32_to_cpu
#define grub_le_to_cpu64 le64_to_cpu
#define grub_be_to_cpu64 be64_to_cpu

#define grub_cpu_to_le64 cpu_to_le64
#define grub_cpu_to_be64 cpu_to_be64

enum zfs_errors {
	ZFS_ERR_NONE = 0,
	ZFS_ERR_NOT_IMPLEMENTED_YET = -1,
	ZFS_ERR_BAD_FS = -2,
	ZFS_ERR_OUT_OF_MEMORY = -3,
	ZFS_ERR_FILE_NOT_FOUND = -4,
	ZFS_ERR_BAD_FILE_TYPE = -5,
	ZFS_ERR_OUT_OF_RANGE = -6,
};

struct zfs_filesystem {

	/* Block Device Descriptor */
	block_dev_desc_t *dev_desc;
};


extern block_dev_desc_t *zfs_dev_desc;

struct device_s {
	uint64_t part_length;
};
typedef struct device_s *device_t;

struct zfs_file {
	device_t device;
	uint64_t size;
	void *data;
	uint64_t offset;
};

typedef struct zfs_file *zfs_file_t;

struct zfs_dirhook_info {
	int dir;
	int mtimeset;
	time_t mtime;
	time_t mtime2;
};




struct zfs_filesystem *zfsget_fs(void);
int init_fs(block_dev_desc_t *dev_desc);
void deinit_fs(block_dev_desc_t *dev_desc);
int zfs_open(zfs_file_t, const char *filename);
uint64_t zfs_read(zfs_file_t, char *buf, uint64_t len);
struct grub_zfs_data *zfs_mount(device_t);
int zfs_close(zfs_file_t);
int zfs_ls(device_t dev, const char *path,
		   int (*hook) (const char *, const struct zfs_dirhook_info *));
int zfs_devread(int sector, int byte_offset, int byte_len, char *buf);
int zfs_set_blk_dev(block_dev_desc_t *rbdd, int part);
void zfs_unmount(struct grub_zfs_data *data);
int lzjb_decompress(void *, void *, uint32_t, uint32_t);
#endif
