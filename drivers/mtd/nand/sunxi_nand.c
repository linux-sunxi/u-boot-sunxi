/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 * Liu Shi <ivanliu@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/nand_bsp.h>
#include <nand.h>

int board_nand_init(struct nand_chip *nand) {

	NAND_Init();
	return 1;
}

int sunxi_nand_read_opts(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags) {

	unsigned int nSectNum,  nSectorCnt;

	nSectNum   = (unsigned int)(offset / 512);
	nSectorCnt = (unsigned int)(*length / 512);
#ifdef DEBUG
	printf("sunxi nand read: start sector %x counts %x\n", nSectNum, nSectorCnt);
#endif
	return NAND_LogicRead(nSectNum, nSectorCnt, buffer);
}

int sunxi_nand_write_opts(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags) {

	unsigned int nSectNum,  nSectorCnt;

	nSectNum   = (unsigned int)(offset / 512);
	nSectorCnt = (unsigned int)(*length / 512);
#ifdef DEBUG
	printf("sunxi nand write: start sector %x counts %x\n", nSectNum, nSectorCnt);
#endif
	return NAND_LogicWrite(nSectNum, nSectorCnt, buffer);
}

int sunxi_nand_erase_opts(nand_info_t *meminfo, const nand_erase_options_t *opts) {

	return 0;
}

/* There is a buffer in the nand logic layer, the function tells
 * the nand logical layer to write the logical data to physic nand
 */
int sunxi_nand_flush_opts(nand_info_t *meminfo) {

	LML_FlushPageCache();
	return 0;
}
