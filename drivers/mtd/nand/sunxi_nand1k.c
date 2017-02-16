/*
 * sunxi_nand_common.c
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

static char rw_buff[1024];

int nand1k_read(char *buff, loff_t offs, size_t count)
{
	uint32_t len, page, offset;
	size_t size = 0;

	debug("nand1k read off=%llx count=%x\n", offs, count);

	if (offs > 128 * 1024 || offs < 0 || 
		count < 0 || count > 128 * 1024 || 
		offs + count > 128 * 1024) {
		error("nand1k is restricted to access the first 128 1K pages\n");
		return -EINVAL;
	}

	while (size < count) {
		page = offs / 1024;
		offset = offs % 1024;
		len = 1024 - offset;
		if (len > count - size)
			len = count - size;

		nfc_read_page1k(page, rw_buff);

		memcpy(buff, rw_buff + offset, len);

		size += len;
		offs += len;
		buff += len;
	}

    return size;
}

int nand1k_write(const char *buff, loff_t offs, size_t count)
{
	uint32_t ret;
	size_t size = 0;

	debug("nand1k write off=%llx count=%x\n", offs, count);

	if (offs > 128 * 1024 || offs < 0 || 
		count < 0 || count > 128 * 1024 || 
		offs + count > 128 * 1024) {
		error("nand1k is restricted to access the first 128 1K pages\n");
		return -EINVAL;
	}

	if ((offs & (1024 - 1)) || (count & (1024 - 1))) {
		error("nand1k can't write non-1K-aligned data\n");
		return -EINVAL;
	}

	while (size < count) {
		memcpy(rw_buff, buff, 1024);

		nfc_write_page1k(offs / 1024, rw_buff);
		
		size += 1024;
		offs += 1024;
		buff += 1024;
	}

    return size;
}




