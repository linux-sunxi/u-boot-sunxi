#include <common.h>
#include <asm/arch/nand_bsp.h>
#include <nand.h>

int board_nand_init(struct nand_chip *nand)
{
	NAND_Init();
	return 1;
}

int sun4i_nand_read_opts(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags){

	unsigned int nSectNum,  nSectorCnt;

	nSectNum   = (unsigned int)(offset / 512);
	nSectorCnt = (unsigned int )(*length / 512);
	printf("sun4i nand read: start %x size %x\n", nSectNum, nSectorCnt);
	return NAND_LogicRead(nSectNum, nSectorCnt, buffer);
}

int sun4i_nand_write_opts(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags){
	unsigned int nSectNum,  nSectorCnt;

	nSectNum   = (unsigned int)(offset / 512);
	nSectorCnt = (unsigned int )(*length / 512);
	printf("sun4i nand write: start %x size %x\n", nSectNum, nSectorCnt);
	return NAND_LogicWrite(nSectNum, nSectorCnt, buffer);
}

int sun4i_nand_erase_opts(nand_info_t *meminfo, const nand_erase_options_t *opts){

	return 0;

}

/* There is a buffer in the nand logic layer, the function tells
 * the nand logical layer to write the logical data to physic nand
 */
int sun4i_nand_flush_opts(nand_info_t *meminfo){

	LML_FlushPageCache();
	return 0;
}
