#ifndef __NAND_NDFC_H__
#define  __NAND_NDFC_H__

//#include "nand_oal.h"

#define SUCESS	0
#define FAIL	-1;
#define BADBLOCK -2

typedef struct cmd_list{
	struct	cmd_list *next;
	__u8	*addr;	
	__u8	addr_cycle;	
	__u8	data_fetch_flag;
	__u8	main_data_fetch;
	__u8	wait_rb_flag;	
	__u32 	bytecnt;
	__u32	value;	
}NFC_CMD_LIST;

typedef struct NFC_init_info{
	__u8	bus_width;// bus width 8 bit
	__u8	rb_sel; // ready busy 
	__u8	ce_ctl; // chip select
	__u8	ce_ctl1;
	__u8	pagesize; // 1024 ,2048 ,
	__u8	serial_access_mode; // SAM0 SAM1
	__u8    ddr_type;
	__u8	debug;
}NFC_INIT_INFO;

__s32 NFC_ReadRetryInit(__u32 read_retry_type);
__s32 NFC_ReadRetryExit(__u32 read_retry_type);
__s32 NFC_GetDefaultParam(__u32 chip, __u8 *defautl_value, __u32 read_retry_type);
__s32 NFC_SetDefaultParam(__u32 chip, __u8 *defautl_value, __u32 read_retry_type);
__s32 NFC_ReadRetry(__u32 chip, __u32 retry_count, __u32 read_retry_type);
__s32 NFC_LSBEnable(__u32 chip, __u32 read_retry_type);
__s32 NFC_LSBDisable(__u32 chip, __u32 read_retry_type);
__s32 NFC_LSBInit(__u32 read_retry_type);
__s32 NFC_LSBExit(__u32 read_retry_type);
__s32 NFC_SetRandomSeed(__u32 random_seed);
__s32 NFC_RandomEnable(void);
__s32 NFC_RandomDisable(void);
__s32 NFC_Init(NFC_INIT_INFO * nand_info);
void NFC_Exit(void);
__s32 NFC_Read(NFC_CMD_LIST * rcmd, void * mainbuf, void * sparebuf, __u8 dma_wait_mode, __u8 page_mode);
__s32 NFC_Read_1K(NFC_CMD_LIST * rcmd, void * mainbuf, void * sparebuf, __u8 dma_wait_mode, __u8 page_mode);
__s32 NFC_Read_Seq(NFC_CMD_LIST * rcmd, void * mainbuf, void * sparebuf, __u8 dma_wait_mode, __u8 page_mode);
__s32 NFC_Read_Spare(NFC_CMD_LIST * rcmd, void * mainbuf, void * sparebuf, __u8 dma_wait_mode, __u8 page_mode);
__s32 NFC_Write(NFC_CMD_LIST * wcmd, void * mainbuf, void * sparebuf, __u8 dma_wait_mode, __u8 rb_wait_mode, __u8 page_mode);
__s32 NFC_Write_Seq(NFC_CMD_LIST * wcmd, void * mainbuf, void * sparebuf, __u8 dma_wait_mode, __u8 rb_wait_mode, __u8 page_mode);
__s32 NFC_Write_1K(NFC_CMD_LIST * wcmd, void * mainbuf, void * sparebuf, __u8 dma_wait_mode, __u8 rb_wait_mode, __u8 page_mode);
__s32 NFC_Erase(NFC_CMD_LIST * ecmd, __u8 rb_wait_mode);
__s32 NFC_CopyBackRead(NFC_CMD_LIST * crcmd);
__s32 NFC_CopyBackWrite(NFC_CMD_LIST * cwcmd, __u8 rb_wait_mode);
__s32 NFC_GetId(NFC_CMD_LIST * idcmd, __u8 * idbuf);
__s32 NFC_GetUniqueId(NFC_CMD_LIST * idcmd, __u8 * idbuf);
__s32 NFC_SelectChip(__u32 chip);
__s32 NFC_DeSelectChip(__u32 chip);
__s32 NFC_SelectRb(__u32 rb);
__s32 NFC_DeSelectRb(__u32 rb);
__s32 NFC_GetStatus(NFC_CMD_LIST * scmd);
__s32 NFC_CheckRbReady(__u32 rb);
__s32 NFC_ChangMode(NFC_INIT_INFO * nand_info);
__s32 NFC_SetEccMode(__u8 ecc_mode);
__s32 NFC_ResetChip(NFC_CMD_LIST * reset_cmd);
__u32 NFC_QueryINT(void);
void NFC_EnableInt(__u8 minor_int);
void NFC_DisableInt(__u8 minor_int);

#endif
