/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * (C) Copyright 2011
 * Texas Instruments, <www.ti.com>
 * Author: Vikram Pandita <vikram.pandita@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <sparse.h>
#include <mmc.h>
#include <nand.h>

#include <asm/arch/nand_bsp.h>
#include <asm/arch/boot_type.h>

#define SPARSE_HEADER_MAJOR_VER 1

#if 0
int mmc_compare(unsigned mmcc, unsigned char *src, unsigned offset, unsigned len)
{
	u8 data[512];

	while (len > 0) {
		if (mmc_read(mmcc, offset, data, 512) != 1) {
			printf("mmc read error offset %d\n", offset);
			return -1;
		}
		if (memcmp(data, src, 512)) {
			printf("mmc data mismatch offset %d\n", offset);
			return -1;
		}
		len -= 512;
		offset++;
		src += 512;
	}
	return 0;
}
#endif

int _unsparse(unsigned char *source, u32 offset, u32 partition_size,
	      unsigned id)
{
	sparse_header_t *header = (void*) source;
	u32 i, outlen = 0;

#ifdef DEBUG
	printf("partition size: 0x%x\n", partition_size);
	printf("sparse: write to device %d: 0x%x\n", id, offset);
#endif

	if ((header->total_blks * header->blk_sz) > partition_size) {
		printf("sparse: partition size %d MB limit: exceeded\n",
				partition_size/(1024*1024));
		return 1;
	}

	if (header->magic != SPARSE_HEADER_MAGIC) {
		printf("sparse: bad magic\n");
		return 1;
	}

	if ((header->major_version != SPARSE_HEADER_MAJOR_VER) ||
	    (header->file_hdr_sz != sizeof(sparse_header_t)) ||
	    (header->chunk_hdr_sz != sizeof(chunk_header_t))) {
		printf("sparse: incompatible format\n");
		return 1;
	}
	/* todo: ensure image will fit */

	/* Skip the header now */
	source += header->file_hdr_sz;

	for (i=0; i < header->total_chunks; i++) {
		unsigned int len = 0;
		int r;
		chunk_header_t *chunk = (void*) source;

		/* move to next chunk */
		source += sizeof(chunk_header_t);

		switch (chunk->chunk_type) {
		case CHUNK_TYPE_RAW:
			len = chunk->chunk_sz * header->blk_sz;

			if (chunk->total_sz != (len + sizeof(chunk_header_t))) {
				printf("sparse: bad chunk size for chunk %d, type Raw\n", i);
				return 1;
			}

			outlen += len;
			if (outlen > partition_size) {
				printf("sparse: RAW partition size %d MB limit: exceeded\n", partition_size/(1024*1024));
				return 1;
			}
#ifdef DEBUG
			printf("sparse: RAW blk=%d bsz=%d: write(offset=0x%x,len=0x%x)\n",
			       chunk->chunk_sz, header->blk_sz, offset, len);
#endif
			//if(!storage_type)
			//r = WRITE(&nand_info[0], offset, &len, source, 0);
			r = sunxi_flash_write(offset>>9, len>>9, source);
			if (r < 0) {
				printf("sparse: mmc write failed\n");
				return 1;
			}

			offset += len;
			source += len;
			break;

		case CHUNK_TYPE_DONT_CARE:
			if (chunk->total_sz != sizeof(chunk_header_t)) {
				printf("sparse: bogus DONT CARE chunk\n");
				return 1;
			}
			len = chunk->chunk_sz * header->blk_sz;
#ifdef DEBUG
			printf("sparse: DONT_CARE blk=0x%x bsz=0x%x: skip(offset=0x%x,len=0x%x)\n",
			       chunk->chunk_sz, header->blk_sz, offset, len);
#endif

			outlen += len;
			if (outlen > partition_size) {
				printf("sparse: DONT_CARE partition size %d MB limit: exceeded\n", partition_size/(1024*1024));
				return 1;
			}
			offset += len;
			break;

		default:
			printf("sparse: unknown chunk ID %04x\n", chunk->chunk_type);
			return 1;
		}

		printf("Writing %%%d ...\r", i * 100 / header->total_chunks);
	}

	printf("sparse: %d MB finished\n", outlen/(1024*1024));
	return 0;
}

u8 do_unsparse(unsigned char *source, u32 offset, u32 partition_size, char *slot_no)
{
	unsigned mmcc = simple_strtoul(slot_no, NULL, 16);
#ifdef DEBUG
	printf("do_unsparse storage_type = %d\n", storage_type);
#endif
	if (_unsparse(source, offset, partition_size, mmcc))
		return 1;

	printf("flash sparse ok!\n");
#if 0
	if (_unsparse(source, offset, partition_size, mmcc, mmc_compare))
		return 1;
#endif
	return 0;
}
