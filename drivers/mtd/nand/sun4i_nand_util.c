#include <common.h>
#include <asm/arch/nand_bsp.h>
#include <nand.h>


/*
**************************************************
 * nand_write_skip_bad:
 *
 * Write image to NAND flash.
 *
 * @param nand  	NAND device
 * @param offset	offset in flash
 * @param length	buffer length
 * @param buffer   buffer to read from
 * @param flags	flags modifying the behaviour of the write to NAND
 * @return		0 in case of success
 **************************************************
 */
int sun4i_nand_read(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags){
			
	unsigned int nSectNum,  nSectorCnt;

	nSectNum   = (unsigned int)(offset / 512);
	nSectorCnt = (unsigned int )(*length / 512);
	return NAND_LogicRead(nSectNum, nSectorCnt, buffer);
}

int sun4i_nand_write(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags){
	unsigned int nSectNum,  nSectorCnt;

	nSectNum   = (unsigned int)(offset / 512);
	nSectorCnt = (unsigned int )(*length / 512);
	return NAND_LogicWrite(nSectNum, nSectorCnt, buffer);
}
