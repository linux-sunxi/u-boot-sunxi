#ifndef __NAND_NDFC_H__
#define  __NAND_NDFC_H__

//#include "nand_oal.h"

#define SUCESS	0
#define FAIL	-1;
#define BADBLOCK -2

typedef struct cmd_list{
	struct	cmd_list *next;
	unsigned char	*addr;
	unsigned char	addr_cycle;
	unsigned char	data_fetch_flag;
	unsigned char	main_data_fetch;
	unsigned char	wait_rb_flag;
	unsigned int 	bytecnt;
	unsigned int	value;
}NFC_CMD_LIST;

typedef struct NFC_init_info{
	unsigned char	bus_width;// bus width 8 bit
	unsigned char	rb_sel; // ready busy
	unsigned char	ce_ctl; // chip select
	unsigned char	ce_ctl1;
	unsigned char	pagesize; // 1024 ,2048 ,
	unsigned char	serial_access_mode; // SAM0 SAM1
	unsigned char	debug;
}NFC_INIT_INFO;

int NFC_ReadRetryInit(unsigned int read_retry_type);
int NFC_ReadRetryExit(unsigned int read_retry_type);
int NFC_GetDefaultParam(unsigned int chip, unsigned char *defautl_value, unsigned int read_retry_type);
int NFC_SetDefaultParam(unsigned int chip, unsigned char *defautl_value, unsigned int read_retry_type);
int NFC_ReadRetry(unsigned int chip, unsigned int retry_count, unsigned int read_retry_type);
int NFC_SetRandomSeed(unsigned int random_seed);
int NFC_RandomEnable(void);
int NFC_RandomDisable(void);
int NFC_Init(NFC_INIT_INFO * nand_info);
void NFC_Exit(void);
int NFC_Read(NFC_CMD_LIST * rcmd, void * mainbuf, void * sparebuf, unsigned char dma_wait_mode, unsigned char page_mode);
int NFC_Read_1K(NFC_CMD_LIST * rcmd, void * mainbuf, void * sparebuf, unsigned char dma_wait_mode, unsigned char page_mode);
int NFC_Read_Seq(NFC_CMD_LIST * rcmd, void * mainbuf, void * sparebuf, unsigned char dma_wait_mode, unsigned char page_mode);
int NFC_Write(NFC_CMD_LIST * wcmd, void * mainbuf, void * sparebuf, unsigned char dma_wait_mode, unsigned char rb_wait_mode, unsigned char page_mode);
int NFC_Write_Seq(NFC_CMD_LIST * wcmd, void * mainbuf, void * sparebuf, unsigned char dma_wait_mode, unsigned char rb_wait_mode, unsigned char page_mode);
int NFC_Write_1K(NFC_CMD_LIST * wcmd, void * mainbuf, void * sparebuf, unsigned char dma_wait_mode, unsigned char rb_wait_mode, unsigned char page_mode);
int NFC_Erase(NFC_CMD_LIST * ecmd, unsigned char rb_wait_mode);
int NFC_CopyBackRead(NFC_CMD_LIST * crcmd);
int NFC_CopyBackWrite(NFC_CMD_LIST * cwcmd, unsigned char rb_wait_mode);
int NFC_GetId(NFC_CMD_LIST * idcmd, unsigned char * idbuf);
int NFC_GetUniqueId(NFC_CMD_LIST * idcmd, unsigned char * idbuf);
int NFC_SelectChip(unsigned int chip);
int NFC_DeSelectChip(unsigned int chip);
int NFC_SelectRb(unsigned int rb);
int NFC_DeSelectRb(unsigned int rb);
int NFC_GetStatus(NFC_CMD_LIST * scmd);
int NFC_CheckRbReady(unsigned int rb);
int NFC_ChangMode(NFC_INIT_INFO * nand_info);
int NFC_SetEccMode(unsigned char ecc_mode);
int NFC_ResetChip(NFC_CMD_LIST * reset_cmd);
unsigned int NFC_QueryINT(void);
void NFC_EnableInt(unsigned char minor_int);
void NFC_DisableInt(unsigned char minor_int);

#endif
