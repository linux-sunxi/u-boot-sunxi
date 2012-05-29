/*
 *	GRUB  --  GRand Unified Bootloader
 *	Copyright (C) 1999,2000,2001,2002,2003,2004,2009
 *	Free Software Foundation, Inc.
 *	Copyright 2007 Sun Microsystems, Inc.
 *
 *	GRUB is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	GRUB is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <malloc.h>
#include <linux/stat.h>
#include <linux/time.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>
#include "zfs_common.h"

#include <zfs/zfs.h>
#include <zfs/zio.h>
#include <zfs/dnode.h>
#include <zfs/uberblock_impl.h>
#include <zfs/vdev_impl.h>
#include <zfs/zio_checksum.h>
#include <zfs/zap_impl.h>
#include <zfs/zap_leaf.h>
#include <zfs/zfs_znode.h>
#include <zfs/dmu.h>
#include <zfs/dmu_objset.h>
#include <zfs/dsl_dir.h>
#include <zfs/dsl_dataset.h>

void
fletcher_2(const void *buf, uint64_t size, grub_zfs_endian_t endian,
		   zio_cksum_t *zcp)
{
	const uint64_t *ip = buf;
	const uint64_t *ipend = ip + (size / sizeof(uint64_t));
	uint64_t a0, b0, a1, b1;

	for (a0 = b0 = a1 = b1 = 0; ip < ipend; ip += 2) {
		a0 += grub_zfs_to_cpu64(ip[0], endian);
		a1 += grub_zfs_to_cpu64(ip[1], endian);
		b0 += a0;
		b1 += a1;
	}

	zcp->zc_word[0] = grub_cpu_to_zfs64(a0, endian);
	zcp->zc_word[1] = grub_cpu_to_zfs64(a1, endian);
	zcp->zc_word[2] = grub_cpu_to_zfs64(b0, endian);
	zcp->zc_word[3] = grub_cpu_to_zfs64(b1, endian);
}

void
fletcher_4(const void *buf, uint64_t size, grub_zfs_endian_t endian,
		   zio_cksum_t *zcp)
{
	const uint32_t *ip = buf;
	const uint32_t *ipend = ip + (size / sizeof(uint32_t));
	uint64_t a, b, c, d;

	for (a = b = c = d = 0; ip < ipend; ip++) {
		a += grub_zfs_to_cpu32(ip[0], endian);
		b += a;
		c += b;
		d += c;
	}

	zcp->zc_word[0] = grub_cpu_to_zfs64(a, endian);
	zcp->zc_word[1] = grub_cpu_to_zfs64(b, endian);
	zcp->zc_word[2] = grub_cpu_to_zfs64(c, endian);
	zcp->zc_word[3] = grub_cpu_to_zfs64(d, endian);
}

