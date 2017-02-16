/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
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

#include "nfc_i.h"

__u32 nand_reg_address = 0;

__u32 	pagesize;
__hdle 	dma_hdle;
volatile __u32 irq_value;


__u8 read_retry_reg_adr[READ_RETRY_MAX_REG_NUM];
__u8 read_retry_default_val[8][READ_RETRY_MAX_REG_NUM];
__s16 read_retry_val[READ_RETRY_MAX_CYCLE][READ_RETRY_MAX_REG_NUM];
__u8 read_retry_mode;
__u8 read_retry_cycle;
__u8 read_retry_reg_num;

__u8 lsb_mode_reg_adr[LSB_MODE_MAX_REG_NUM];
__u8 lsb_mode_default_val[LSB_MODE_MAX_REG_NUM];
__u8 lsb_mode_val[LSB_MODE_MAX_REG_NUM];
__u8 lsb_mode_reg_num;

void nfc_repeat_mode_enable(void)
{
    __u32 reg_val;
   

	reg_val = NFC_READ_REG(NFC_REG_CTL);
	if(((reg_val>>18)&0x3)>1)   //ddr type
	{
    	reg_val |= 0x1<<20;
    	NFC_WRITE_REG(NFC_REG_CTL, reg_val);
    }

}

void nfc_repeat_mode_disable(void)
{
    __u32 reg_val;
	
    reg_val = NFC_READ_REG(NFC_REG_CTL);
	if(((reg_val>>18)&0x3)>1)   //ddr type
	{
    	reg_val &= (~(0x1<<20));
    	NFC_WRITE_REG(NFC_REG_CTL, reg_val);
    }
}

/*******************wait nfc********************************************/
__s32 _wait_cmdfifo_free(void)
{
	__s32 timeout = 0xffff;

	while ( (timeout--) && (NFC_READ_REG(NFC_REG_ST) & NFC_CMD_FIFO_STATUS) );
	if (timeout <= 0)
		return -ERR_TIMEOUT;
	return 0;
}

__s32 _wait_cmd_finish(void)
{
	__s32 timeout = 0xffff;
	while( (timeout--) && !(NFC_READ_REG(NFC_REG_ST) & NFC_CMD_INT_FLAG) );
	if (timeout <= 0)
		return -ERR_TIMEOUT;

	NFC_WRITE_REG(NFC_REG_ST, NFC_READ_REG(NFC_REG_ST) & NFC_CMD_INT_FLAG);
	return 0;
}

void _dma_config_start(__u32 rw, __u32 src_addr, __u32 dst_addr, __u32 len)
{
	__dma_setting_t   dma_param;

	if(rw)  //write
	{
		if(src_addr & 0xC0000000) //DRAM
			dma_param.cfg.src_drq_type = DMAC_CFG_SRC_TYPE_D_SDRAM;
		else  //SRAM
			dma_param.cfg.src_drq_type = DMAC_CFG_SRC_TYPE_D_SRAM;
		dma_param.cfg.src_addr_type = DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;  //linemode
		dma_param.cfg.src_burst_length = DMAC_CFG_SRC_4_BURST;  //burst mode
		dma_param.cfg.src_data_width = DMAC_CFG_SRC_DATA_WIDTH_32BIT;  //32bit

		dma_param.cfg.dst_drq_type = DMAC_CFG_DEST_TYPE_NFC;
		dma_param.cfg.dst_addr_type = DMAC_CFG_DEST_ADDR_TYPE_IO_MODE; //IO mode
		dma_param.cfg.dst_burst_length = DMAC_CFG_DEST_4_BURST; // burst mode
		dma_param.cfg.dst_data_width = DMAC_CFG_DEST_DATA_WIDTH_32BIT; //32 bit

		dma_param.cfg.wait_state = DMAC_CFG_WAIT_1_DMA_CLOCK; // invalid value
		dma_param.cfg.continuous_mode = DMAC_CFG_CONTINUOUS_DISABLE; //no continous

		dma_param.cmt_blk_cnt =  0x7f077f07; //commit register

	}
	else //read
	{
		dma_param.cfg.src_drq_type = DMAC_CFG_SRC_TYPE_NFC;
		dma_param.cfg.src_addr_type = DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;  //IO mode
		dma_param.cfg.src_burst_length = DMAC_CFG_SRC_4_BURST;  //burst mode
		dma_param.cfg.src_data_width = DMAC_CFG_SRC_DATA_WIDTH_32BIT;  //32bit

		if(dst_addr & 0xC0000000) //DRAM
			dma_param.cfg.dst_drq_type = DMAC_CFG_DEST_TYPE_D_SDRAM;
		else  //SRAM
			dma_param.cfg.dst_drq_type = DMAC_CFG_DEST_TYPE_D_SRAM;
		dma_param.cfg.dst_addr_type = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE; //line mode
		dma_param.cfg.dst_burst_length = DMAC_CFG_DEST_4_BURST; // burst mode
		dma_param.cfg.dst_data_width = DMAC_CFG_DEST_DATA_WIDTH_32BIT; //32 bit

		dma_param.cfg.wait_state = DMAC_CFG_WAIT_1_DMA_CLOCK; // invalid value
		dma_param.cfg.continuous_mode = DMAC_CFG_CONTINUOUS_DISABLE; //no continous

		dma_param.cmt_blk_cnt =  0x7f077f07; //commit register
	}

	NAND_SettingDMA(dma_hdle, (void *)&dma_param);

	NAND_StartDMA(dma_hdle, src_addr, dst_addr, len);
}


__s32 _wait_dma_end(void)
{
	__s32 timeout = 0xffff;

	while( (timeout--) && ( NAND_QueryDmaStat(dma_hdle)) );
	if (timeout <= 0)
		return -ERR_TIMEOUT;

	return 0;
}

__s32 _reset(void)
{
	__u32 cfg;

	__s32 timeout = 0xffff;

	/*reset NFC*/
	cfg = NFC_READ_REG(NFC_REG_CTL);
	cfg |= NFC_RESET;
	NFC_WRITE_REG(NFC_REG_CTL, cfg);
	//waiting reset operation end
	while((timeout--) && (NFC_READ_REG(NFC_REG_CTL) & NFC_RESET));
	if (timeout <= 0)
		return -ERR_TIMEOUT;

	return 0;
}

/***************ecc function*****************************************/
__s32 _check_ecc(__u32 eblock_cnt)
{
	__u32 i;
	__u32 ecc_mode;
	__u32 max_ecc_bit_cnt = 16;
	__u32 cfg;
	__u8 ecc_cnt[16];

	ecc_mode = (NFC_READ_REG(NFC_REG_ECC_CTL)>>12)&0xf;
	if(ecc_mode == 0)
		max_ecc_bit_cnt = 16;
	if(ecc_mode == 1)
		max_ecc_bit_cnt = 24;
	if(ecc_mode == 2)
		max_ecc_bit_cnt = 28;
	if(ecc_mode == 3)
		max_ecc_bit_cnt = 32;
	if(ecc_mode == 4)
		max_ecc_bit_cnt = 40;
	if(ecc_mode == 5)
		max_ecc_bit_cnt = 48;
	if(ecc_mode == 6)
		max_ecc_bit_cnt = 56;
    if(ecc_mode == 7)
		max_ecc_bit_cnt = 60;
    if(ecc_mode == 8)
		max_ecc_bit_cnt = 64;

	//check ecc errro
	cfg = NFC_READ_REG(NFC_REG_ECC_ST)&0xffff;
	for (i = 0; i < eblock_cnt; i++)
	{
		if (cfg & (1<<i))
				return -ERR_ECC;
	}

	//check ecc limit
	cfg = NFC_READ_REG(NFC_REG_ECC_CNT0);
	ecc_cnt[0] = (__u8)((cfg>>0)&0xff);
	ecc_cnt[1] = (__u8)((cfg>>8)&0xff);
	ecc_cnt[2] = (__u8)((cfg>>16)&0xff);
	ecc_cnt[3] = (__u8)((cfg>>24)&0xff);

	cfg = NFC_READ_REG(NFC_REG_ECC_CNT1);
	ecc_cnt[4] = (__u8)((cfg>>0)&0xff);
	ecc_cnt[5] = (__u8)((cfg>>8)&0xff);
	ecc_cnt[6] = (__u8)((cfg>>16)&0xff);
	ecc_cnt[7] = (__u8)((cfg>>24)&0xff);

	cfg = NFC_READ_REG(NFC_REG_ECC_CNT2);
	ecc_cnt[8] = (__u8)((cfg>>0)&0xff);
	ecc_cnt[9] = (__u8)((cfg>>8)&0xff);
	ecc_cnt[10] = (__u8)((cfg>>16)&0xff);
	ecc_cnt[11] = (__u8)((cfg>>24)&0xff);

	cfg = NFC_READ_REG(NFC_REG_ECC_CNT3);
	ecc_cnt[12] = (__u8)((cfg>>0)&0xff);
	ecc_cnt[13] = (__u8)((cfg>>8)&0xff);
	ecc_cnt[14] = (__u8)((cfg>>16)&0xff);
	ecc_cnt[15] = (__u8)((cfg>>24)&0xff);

	for (i = 0; i < eblock_cnt; i++)
	{
		if((max_ecc_bit_cnt - 4) <= ecc_cnt[i])
			return ECC_LIMIT;
	}

	return 0;
}

void _disable_ecc(void)
{
	__u32 cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	cfg &= ( (~NFC_ECC_EN)&0xffffffff );
	NFC_WRITE_REG(NFC_REG_ECC_CTL, cfg);
}

void _enable_ecc(__u32 pipline)
{
	__u32 cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	if (pipline ==1 )
		cfg |= NFC_ECC_PIPELINE;
	else
		cfg &= ((~NFC_ECC_PIPELINE)&0xffffffff);


	/*after erased, all data is 0xff, but ecc is not 0xff,
			so ecc asume it is right*/
	//if random open, disable exception
	if(cfg&(0x1<<9))
	    cfg &= (~(0x1<<4));
	else
	    cfg |= (1 << 4);

	//cfg |= (1 << 1); 16 bit ecc

	cfg |= NFC_ECC_EN;
	NFC_WRITE_REG(NFC_REG_ECC_CTL, cfg);
}


void _set_addr(__u8 *addr, __u8 cnt)
{
	__u32 i;
	__u32 addr_low = 0;
	__u32 addr_high = 0;

	for (i = 0; i < cnt; i++){
		if (i < 4)
			addr_low |= (addr[i] << (i*8) );
		else
			addr_high |= (addr[i] << ((i - 4)*8));
	}

	NFC_WRITE_REG(NFC_REG_ADDR_LOW, addr_low);
	NFC_WRITE_REG(NFC_REG_ADDR_HIGH, addr_high);
}

__s32 _read_in_page_mode(NFC_CMD_LIST  *rcmd,void *mainbuf,void *sparebuf,__u8 dma_wait_mode)
{
	__s32 ret,ret1;
	__s32 i;
	__u32 cfg;
	NFC_CMD_LIST *cur_cmd,*read_addr_cmd;
	__u32 read_data_cmd,random_read_cmd0,random_read_cmd1;

	ret = 0;
	read_addr_cmd = rcmd;
	cur_cmd = rcmd;
	cur_cmd = cur_cmd->next;
	random_read_cmd0 = cur_cmd->value;
	cur_cmd = cur_cmd->next;
	random_read_cmd1 = cur_cmd->value;
	cur_cmd = cur_cmd->next;
	read_data_cmd = cur_cmd->value;

	//access NFC internal RAM by DMA bus
	NFC_WRITE_REG(NFC_REG_CTL, (NFC_READ_REG(NFC_REG_CTL)) | NFC_RAM_METHOD);

	/*set dma and run*/
//	/*sdram*/
//	if (NFC_IS_SDRAM((uint)mainbuf))
//		attr = 0x2810293;
//	/*sram*/
//	else
//		attr = 0x2800293;
	_dma_config_start(0, NAFC_REGS_BASE + NFC_REG_o_IO_DATA, (__u32)mainbuf, pagesize);

	/*wait cmd fifo free*/
	ret = _wait_cmdfifo_free();
	if (ret)
		return ret;

	/*set NFC_REG_CNT*/
	NFC_WRITE_REG(NFC_REG_CNT,1024);

	/*set NFC_REG_RCMD_SET*/
	cfg = 0;
	cfg |= (read_data_cmd & 0xff);
	cfg |= ((random_read_cmd0 & 0xff) << 8);
	cfg |= ((random_read_cmd1 & 0xff) << 16);
	NFC_WRITE_REG(NFC_REG_RCMD_SET, cfg);

	/*set NFC_REG_SECTOR_NUM*/
	NFC_WRITE_REG(NFC_REG_SECTOR_NUM, pagesize/1024);

	/*set addr*/
	_set_addr(read_addr_cmd->addr,read_addr_cmd->addr_cycle);

	/*set NFC_REG_CMD*/
	cfg  = 0;
	cfg |= read_addr_cmd->value;
	/*set sequence mode*/
	//cfg |= 0x1<<25;
	cfg |= ( (read_addr_cmd->addr_cycle - 1) << 16);
	cfg |= (NFC_SEND_ADR | NFC_DATA_TRANS | NFC_SEND_CMD1 | NFC_SEND_CMD2 | NFC_WAIT_FLAG | NFC_DATA_SWAP_METHOD);
	cfg |= ((__u32)0x2 << 30);//page command

	if (pagesize/1024 == 1)
		cfg |= NFC_SEQ;

	/*enable ecc*/
	_enable_ecc(1);
	NFC_WRITE_REG(NFC_REG_CMD,cfg);

    NAND_WaitDmaFinish();//

	/*wait cmd fifo free and cmd finish*/
	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();
	if (ret){
		_disable_ecc();
		return ret;
	}
	/*get user data*/
	for (i = 0; i < pagesize/1024;  i++){
		*(((__u32*) sparebuf)+i) = NFC_READ_REG(NFC_REG_USER_DATA(i));
	}

	/*ecc check and disable ecc*/
	ret = _check_ecc(pagesize/1024);
	_disable_ecc();

	/*if dma mode is wait*/
	if(0 == dma_wait_mode){
		ret1 = _wait_dma_end();
		if (ret1)
			return ret1;
	}

	return ret;
}

/*******************************************************************************
*								NFC_Read
*
* Description 	: read some sectors data from flash in single plane mode.
* Arguments	: *rcmd	-- the read command sequence list head。
*			  *mainbuf	-- point to data buffer address, 	it must be four bytes align.
*                     *sparebuf	-- point to spare buffer address.
*                     dma_wait_mode	-- how to deal when dma start, 0 = wait till dma finish,
							    1 = dma interrupt was set and now sleep till interrupt occurs.
*			  page_mode  -- 0 = normal command, 1 = page mode
* Returns		: 0 = success.
			  1 = success & ecc limit.
			  -1 = too much ecc err.
* Notes		:  if align page data required，page command mode is used., if the commands do
			   not fetch data，ecc is not neccesary.
********************************************************************************/
__s32 NFC_Read(NFC_CMD_LIST  *rcmd, void *mainbuf, void *sparebuf, __u8 dma_wait_mode,__u8 page_mode )
{

	__s32 ret ;


	ret = _read_in_page_mode(rcmd, mainbuf,sparebuf, dma_wait_mode);

	/*switch to ahb*/
	NFC_WRITE_REG(NFC_REG_CTL, (NFC_READ_REG(NFC_REG_CTL)) & (~NFC_RAM_METHOD));




	return ret;
}


/*finish the comand list */
__s32 nfc_set_cmd_register(NFC_CMD_LIST *cmd)
{
	__u32 cfg;
	__s32 ret;

	NFC_CMD_LIST *cur_cmd = cmd;
	while(cur_cmd != NULL){
		/*wait cmd fifo free*/
		ret = _wait_cmdfifo_free();
		if (ret)
			return ret;

		cfg = 0;
		/*set addr*/
		if (cur_cmd->addr_cycle){
			_set_addr(cur_cmd->addr,cur_cmd->addr_cycle);
			cfg |= ( (cur_cmd->addr_cycle - 1) << 16);
			cfg |= NFC_SEND_ADR;
		}

		/*set NFC_REG_CMD*/
		/*set cmd value*/
		cfg |= cur_cmd->value;
		/*set sequence mode*/
		//cfg |= 0x1<<25;
		/*wait rb?*/
		if (cur_cmd->wait_rb_flag){
			cfg |= NFC_WAIT_FLAG;
		}
		if (cur_cmd->data_fetch_flag){
			NFC_WRITE_REG(NFC_REG_CTL, (NFC_READ_REG(NFC_REG_CTL)) & (~NFC_RAM_METHOD));
			cfg |= NFC_DATA_TRANS;
			NFC_WRITE_REG(NFC_REG_CNT, cur_cmd->bytecnt);
		}
		/*send command*/
		cfg |= NFC_SEND_CMD1;
		NFC_WRITE_REG(NFC_REG_CMD, cfg);
		cur_cmd = cur_cmd ->next;
	}
	return 0;
}

__s32 NFC_SetRandomSeed(__u32 random_seed)
{
	__u32 cfg;


	  cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	  cfg &= 0x0000ffff;
	  cfg |= (random_seed<<16);
	  NFC_WRITE_REG(NFC_REG_ECC_CTL,cfg);

	return 0;
}

__s32 NFC_RandomEnable(void)
{
	__u32 cfg;


	cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	cfg |= (0x1<<9);
	NFC_WRITE_REG(NFC_REG_ECC_CTL,cfg);


	return 0;
}

__s32 NFC_RandomDisable(void)
{
	__u32 cfg;


	cfg = NFC_READ_REG(NFC_REG_ECC_CTL);
	cfg &= (~(0x1<<9));
	NFC_WRITE_REG(NFC_REG_ECC_CTL,cfg);


	return 0;
}



/*******************************************************************************
*								NFC_GetId
*
* Description 	: get chip id.
* Arguments	: *idcmd	-- the get id command sequence list head.

* Returns		: 0 = success.
			  -1 = fail.
* Notes		:
********************************************************************************/
__s32 NFC_GetId(NFC_CMD_LIST  *idcmd ,__u8 *idbuf)
{
	__u32 i;
	__s32 ret;


    nfc_repeat_mode_enable();
	ret = nfc_set_cmd_register(idcmd);
	if (ret){

		return ret;
	}

	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();

	/*get 5 bytes id value*/
	for (i = 0; i < 5; i++){
		*(idbuf + i) = NFC_READ_RAM_B(NFC_RAM0_BASE+i);
	}

    nfc_repeat_mode_disable();

	return ret;
}

__s32 NFC_GetUniqueId(NFC_CMD_LIST  *idcmd ,__u8 *idbuf)
{
	__u32 i;
	__s32 ret;

    nfc_repeat_mode_enable();

	ret = nfc_set_cmd_register(idcmd);
	if (ret){

		return ret;
	}

	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();

	/*get 5 bytes id value*/
	for (i = 0; i < 32; i++){
		*(idbuf + i) = NFC_READ_RAM_B(NFC_RAM0_BASE+i);
	}

    nfc_repeat_mode_disable();

	return ret;
}

/*******************************************************************************
*								NFC_GetStatus
*
* Description 	: get status.
* Arguments	: *scmd	-- the get status command sequence list head.

* Returns		: status result
* Notes		: some cmd must be sent with addr.
********************************************************************************/
__s32 NFC_GetStatus(NFC_CMD_LIST  *scmd)
{
	__s32 ret;

	nfc_repeat_mode_enable();
	ret = nfc_set_cmd_register(scmd);
	if (ret){

		return ret;
	}

	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();
	if(ret){

		return ret;
	}

    nfc_repeat_mode_disable();

	return (NFC_READ_RAM_B(NFC_RAM0_BASE));

}
/*******************************************************************************
*								NFC_ResetChip
*
* Description 	: reset nand flash.
* Arguments	: *reset_cmd	-- the reset command sequence list head.

* Returns		: sucess or fail
* Notes		:
********************************************************************************/
__s32 NFC_ResetChip(NFC_CMD_LIST *reset_cmd)

{
	__s32 ret;


	ret = nfc_set_cmd_register(reset_cmd);
	if (ret){

		return ret;
	}
	ret = _wait_cmdfifo_free();
	ret |= _wait_cmd_finish();

	return ret;
}

/*******************************************************************************
*								NFC_SelectChip
*
* Description 	: enable chip ce.
* Arguments	: chip	-- chip no.

* Returns		: 0 = sucess -1 = fail
* Notes		:
********************************************************************************/
__s32 NFC_SelectChip( __u32 chip)
{
	__u32 cfg;


  cfg = NFC_READ_REG(NFC_REG_CTL);
  cfg &= ( (~NFC_CE_SEL) & 0xffffffff);
  cfg |= ((chip & 0x7) << 24);
  NFC_WRITE_REG(NFC_REG_CTL,cfg);


	return 0;
}

/*******************************************************************************
*								NFC_SelectRb
*
* Description 	: select rb.
* Arguments	: rb	-- rb no.

* Returns		: 0 = sucess -1 = fail
* Notes		:
********************************************************************************/
__s32 NFC_SelectRb( __u32 rb)
{
	__s32 cfg;


	  cfg = NFC_READ_REG(NFC_REG_CTL);
	  cfg &= ( (~NFC_RB_SEL) & 0xffffffff);
	  cfg |= ((rb & 0x1) << 3);
	  NFC_WRITE_REG(NFC_REG_CTL,cfg);

	  return 0;

}





__s32 NFC_DeSelectChip( __u32 chip)
{





	return 0;
}

__s32 NFC_DeSelectRb( __u32 rb)
{





	return 0;
}


/*******************************************************************************
*								NFC_CheckRbReady
*
* Description 	: check rb if ready.
* Arguments	: rb	-- rb no.

* Returns		: 0 = sucess -1 = fail
* Notes		:
********************************************************************************/

__s32 NFC_CheckRbReady( __u32 rb)
{
	__s32 ret;
	__u32 cfg = NFC_READ_REG(NFC_REG_ST);


	cfg &= (NFC_RB_STATE0 << (rb & 0x3));

	if (cfg)
		ret = 0;
	else
		ret = -1;

	return ret;
}

/*******************************************************************************
*								NFC_ChangeMode
*
* Description 	: change serial access mode when clock change.
* Arguments	: nand_info -- structure with flash bus width,pagesize ,serial access mode and other configure parametre

* Returns		: 0 = sucess -1 = fail
* Notes		: NFC must be reset before seial access mode changes.
********************************************************************************/
__s32 NFC_ChangMode(NFC_INIT_INFO *nand_info )
{
	__u32 cfg;

	pagesize = nand_info->pagesize * 512;

	/*reset nfc*/
	_reset();

	/*set NFC_REG_CTL*/
	cfg = 0;
	cfg |= NFC_EN;
	cfg |= ( (nand_info->bus_width & 0x1) << 2);
	cfg |= ( (nand_info->ce_ctl & 0x1) << 6);
	cfg |= ( (nand_info->ce_ctl1 & 0x1) << 7);
	if(nand_info->pagesize == 2 )            /*  1K  */
	   cfg |= ( 0x0 << 8 );
	else if(nand_info->pagesize == 4 )       /*  2K  */
	   cfg |= ( 0x1 << 8 );
	else if(nand_info->pagesize == 8 )       /*  4K  */
	   cfg |= ( 0x2 << 8 );
    else if(nand_info->pagesize == 16 )       /*  8K  */
	   cfg |= ( 0x3 << 8 );
	else if(nand_info->pagesize == 32 )       /*  16K  */
	   cfg |= ( 0x4 << 8 );
	else                                      /* default 4K */
	   cfg |= ( 0x2 << 8 );
	cfg |= ((nand_info->ddr_type & 0x3) << 18);   //set ddr type 
	cfg |= ((nand_info->debug & 0x1) << 31);
	NFC_WRITE_REG(NFC_REG_CTL,cfg);

	/*set NFC_TIMING */
	cfg = 0;
	if((nand_info->ddr_type & 0x3) == 0)
	    cfg |=((nand_info->serial_access_mode & 0x1) & 0xf)<<8;
	else if((nand_info->ddr_type & 0x3) == 2)
	{
	    cfg |= 0x3f;
	    cfg |= 0x3<<8;
    }
    else if((nand_info->ddr_type & 0x3) == 3)
	{
	    cfg |= 0x1f;
	    cfg |= 0x2<<8;
	}
	NFC_WRITE_REG(NFC_REG_TIMING_CTL,cfg);
	NFC_WRITE_REG(NFC_REG_TIMING_CFG,0xff);
	/*set NFC_SPARE_AREA */
	NFC_WRITE_REG(NFC_REG_SPARE_AREA, pagesize);

	return 0;
}

__s32 NFC_SetEccMode(__u8 ecc_mode)
{
    __u32 cfg = NFC_READ_REG(NFC_REG_ECC_CTL);


    cfg &=	((~NFC_ECC_MODE)&0xffffffff);
    cfg |= (NFC_ECC_MODE & (ecc_mode<<12));

	NFC_WRITE_REG(NFC_REG_ECC_CTL, cfg);

	return 0;
}
/*******************************************************************************
*								NFC_Init
*
* Description 	: init hardware, set NFC, set TIMING, request dma .
* Arguments	: nand_info -- structure with flash bus width,pagesize ,serial access mode and other configure parametre

* Returns		: 0 = sucess -1 = fail
* Notes		: .
********************************************************************************/
__s32 NFC_Init(NFC_INIT_INFO *nand_info )
{
	__s32 ret;

    //SET NAND FLASH GPIO
    NAND_GetPin();
    //SET NAND FLASH CLOCK
    NAND_OpenAHBClock();
    if( NAND_SetClock(10) )
    {
        return -1;
    }
    //--------------------------------------

    nand_reg_address = NFC_REG_BASE_ADDR;

	NFC_SetEccMode(0);

	/*init nand control machine*/
	ret = NFC_ChangMode( nand_info);

	/*request special dma*/
	dma_hdle = NAND_RequestDMA(DMAC_DMATYPE_DEDICATED);
	if (dma_hdle == 0)
		return -1;
	return ret;

}

/*******************************************************************************
*								NFC_Exit
*
* Description 	: free hardware resource, free dma , disable NFC.
* Arguments	: nand_info -- structure with flash bus width,pagesize ,serial access mode and other configure parametre

* Returns		: 0 = sucess -1 = fail
* Notes		: .
********************************************************************************/
void NFC_Exit( void )
{
	__u32 cfg;
	/*disable NFC*/
	cfg = NFC_READ_REG(NFC_REG_CTL);
	cfg &= ( (~NFC_EN) & 0xffffffff);
	NFC_WRITE_REG(NFC_REG_CTL,cfg);

	/*free dma*/
	NAND_ReleaseDMA(dma_hdle);

    //CLOSE GPIO AND CLOCK
	NAND_ReleasePin();
	NAND_CloseAHBClock();
}

__s32 _vender_get_param(__u8 *para, __u8 *addr, __u32 count)
{
    __u32 i, cfg;
    __u32 cmd_r = 0;
    __s32 ret = 0;
    

    
    if(read_retry_mode <0x10) //hynix mode
    {
        cmd_r = 0x37;
    }
    else if((read_retry_mode >=0x10)&&(read_retry_mode <0x20)) //toshiba mode
    {

		return ret;
    }    
    
    for(i=0; i<count; i++)
	{
		_set_addr(&addr[i], 1);
        
        //set data cnt
		NFC_WRITE_REG(NFC_REG_CNT, 1);
        
		/*set NFC_REG_CMD*/
		cfg = cmd_r;
		cfg |= (NFC_SEND_ADR | NFC_DATA_TRANS | NFC_SEND_CMD1 );
		NFC_WRITE_REG(NFC_REG_CMD, cfg);

		ret = _wait_cmdfifo_free();
		ret |= _wait_cmd_finish();

		if(ret)
		{

			return ret;
		}
	
		*(para+i) = NFC_READ_RAM_B(NFC_RAM0_BASE);
	}
    

	return ret;   
}

__s32 _vender_set_param(__u8 *para, __u8 *addr, __u32 count)
{
    __u32 i, cfg;
    __u32 cmd_w=0xff, cmd_end=0xff, cmd_done0 =0xff, cmd_done1=0xff;
    __s32 ret = 0;
    

    
    if(read_retry_mode <0x10) //hynix mode
    {
        cmd_w = 0x36;
        cmd_end = 0x16;
        cmd_done0 = 0xff;
        cmd_done1 = 0xff;
    }
    else if((read_retry_mode >=0x10)&&(read_retry_mode <0x20)) //toshiba mode
    {
        cmd_w = 0x55;
        cmd_end = 0xff;
        cmd_done0 = 0x26;
        cmd_done1 = 0x5D;
        
    }
    else if((read_retry_mode >=0x20)&&(read_retry_mode <0x30)) //Samsung mode
    {
        cmd_w = 0xA1;
        cmd_end = 0xff;
        cmd_done0 = 0xff;
        cmd_done1 = 0xff;
        
    }     
    
    for(i=0; i<count; i++)
	{
	    if((read_retry_mode >=0x20)&&(read_retry_mode <0x30)) //samsung mode
	    {
	        /* send cmd to set param */
	        NFC_WRITE_RAM_B(NFC_RAM0_BASE, 0x00);
	        NFC_WRITE_RAM_B(NFC_RAM0_BASE+1, addr[i]);
    		NFC_WRITE_RAM_B(NFC_RAM0_BASE+2, para[i]);

    		NFC_WRITE_REG(NFC_REG_CNT, 3);
    		
    		/*set NFC_REG_CMD*/
    		cfg = cmd_w;
    		cfg |= ( NFC_DATA_TRANS | NFC_ACCESS_DIR | NFC_SEND_CMD1);
    		nfc_repeat_mode_enable();
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);
    		nfc_repeat_mode_disable();
	    }
	    else //hynix & toshiba mode
	    {
	        /* send cmd to set param */
    		NFC_WRITE_RAM_B(NFC_RAM0_BASE, para[i]);
    		_set_addr(&addr[i], 1);
    		NFC_WRITE_REG(NFC_REG_CNT, 1);
    		
    		/*set NFC_REG_CMD*/
    		cfg = cmd_w;
    		cfg |= (NFC_SEND_ADR | NFC_DATA_TRANS | NFC_ACCESS_DIR | NFC_SEND_CMD1);
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);
	    }	

		ret = _wait_cmdfifo_free();
		ret |= _wait_cmd_finish();

		if(ret)
		{

			return ret;
		}
		
		/* send cmd to end */
		if(cmd_end != 0xff)
		{
		    /*set NFC_REG_CMD*/
    		cfg = cmd_end;
    		cfg |= ( NFC_SEND_CMD1);
    		NFC_WRITE_REG(NFC_REG_CMD, cfg);
    
    		ret = _wait_cmdfifo_free();
    		ret |= _wait_cmd_finish();
    		
    		if(ret)
    		{

    			return ret;
    		}
		}
		
	}
	
	if(cmd_done0!=0xff)
	{
	    /*set NFC_REG_CMD*/
		cfg = cmd_done0;
		cfg |= ( NFC_SEND_CMD1);
		NFC_WRITE_REG(NFC_REG_CMD, cfg);

		ret = _wait_cmdfifo_free();
		ret |= _wait_cmd_finish();
		
		if(ret)
		{

			return ret;
		}
	}
	
	if(cmd_done1!=0xff)
	{
	    /*set NFC_REG_CMD*/
		cfg = cmd_done1;
		cfg |= ( NFC_SEND_CMD1);
		NFC_WRITE_REG(NFC_REG_CMD, cfg);

		ret = _wait_cmdfifo_free();
		ret |= _wait_cmd_finish();
		
		if(ret)
		{

			return ret;
		}
	}
	
	
    

	return ret;   
}

__s32 _vender_pre_condition(void)
{
    __u32 i, cfg;
    __u32 cmd[2]= {0x5c, 0xc5};
    __s32 ret = 0;
    

    if((read_retry_mode>=0x10)&&(read_retry_mode<0x20))  //toshiba mode
    {
        for(i=0;i<2;i++)
        {
        	/*set NFC_REG_CMD*/
        	cfg = cmd[i];
        	cfg |= (NFC_SEND_CMD1);
        	NFC_WRITE_REG(NFC_REG_CMD, cfg);
        
        	ret = _wait_cmdfifo_free();
        	ret |= _wait_cmd_finish();
        	
        	if (ret)
        	{

        		return ret;
        	}
        }
    }

	
	return ret;
}


//for offset from defaul value
__s32 NFC_ReadRetry(__u32 chip, __u32 retry_count, __u32 read_retry_type)
{
    __u32 i;
    __s32 ret=0;
    __s16 temp_val;
    __u8 param[READ_RETRY_MAX_REG_NUM];
    __u32 nand_clk_bak;
    
	if(retry_count >read_retry_cycle)
		return -1;
		
    if(read_retry_mode<0x10)  //for hynix read retry mode
    {
        if(retry_count == 0)
    	    ret = _vender_set_param(&read_retry_default_val[chip][0], &read_retry_reg_adr[0], read_retry_reg_num);
    	else
    	{
    	    for(i=0; i<read_retry_reg_num; i++)
        	{
        	    
        	    temp_val = (read_retry_default_val[chip][i] + read_retry_val[retry_count-1][i]);
        	    if(temp_val >255)
        	        temp_val = 0xff;
        	    else if(temp_val <0)
    				temp_val = 0;
    			else
        	        temp_val &= 0xff;
        	    
        	    param[i] = (__u8)temp_val;    
        	    
        	}
    
    		//fix 0
    		if(read_retry_mode == 0)
    		{
    			if(retry_count == 10)
        	    	param[1] = 0;
    		}
    		else if(read_retry_mode == 1)
    		{
    			if((retry_count >=2)&&(retry_count<=6))
    				param[0] = 0;
    			
    			if((retry_count == 5)||(retry_count == 6))
        	    	param[1] = 0;
    		}
        	
        	ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);    	    
    	}
        
    }
    else if((read_retry_mode>=0x10)&&(read_retry_mode<0x20))  //for toshiba readretry mode
    {
        nand_clk_bak = NAND_GetClock();
        NAND_SetClock(10);
        
        if(retry_count == 1)
            _vender_pre_condition();
        
        for(i=0; i<read_retry_reg_num; i++)
            param[i] = (__u8)read_retry_val[retry_count-1][i];
            
        ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);
        
        NAND_SetClock(nand_clk_bak);
    }
    else if((read_retry_mode>=0x20)&&(read_retry_mode<0x30))
    {
        for(i=0; i<read_retry_reg_num; i++)
            param[i] = (__u8)read_retry_val[retry_count][i];
            
        ret =_vender_set_param(&param[0], &read_retry_reg_adr[0], read_retry_reg_num);
    }
	
	return ret;
}

__s32 NFC_ReadRetryInit(__u32 read_retry_type)
{
	__u32 i,j;
	__s16 para0[6][4] = {	{0x00,  0x06,  0x0A,  0x06},
    						{0x00, -0x03, -0x07, -0x08},
    						{0x00, -0x06, -0x0D, -0x0F},
    						{0x00, -0x0B, -0x14, -0x17},
    						{0x00,  0x00, -0x1A, -0x1E},
    						{0x00,  0x00, -0x20, -0x25}
					};
	__s16 para1[6][4] = {	{0x00,  0x06,  0x0a,  0x06},
    						{0x00, -0x03, -0x07, -0x08},
    						{0x00, -0x06, -0x0d, -0x0f},
    						{0x00, -0x09, -0x14, -0x17},
    						{0x00,  0x00, -0x1a, -0x1e},
    						{0x00,  0x00, -0x20, -0x25}

					};
    __s16 para0x10[5] = {0x04, 0x7c, 0x78, 0x74, 0x08};
    __s16 para0x20[15][4] ={{0x00, 0x00, 0x00, 0x00},    //0
                         {0x05, 0x0A, 0x00, 0x00},    //1
                         {0x28, 0x00, 0xEC, 0xD8},    //2
                         {0xED, 0xF5, 0xED, 0xE6},    //3
                         {0x0A, 0x0F, 0x05, 0x00},    //4
                         {0x0F, 0x0A, 0xFB, 0xEC},    //5
                         {0xE8, 0xEF, 0xE8, 0xDC},    //6
                         {0xF1, 0xFB, 0xFE, 0xF0},    //7
                         {0x0A, 0x00, 0xFB, 0xEC},    //8
                         {0xD0, 0xE2, 0xD0, 0xC2},    //9
                         {0x14, 0x0F, 0xFB, 0xEC},    //10
                         {0xE8, 0xFB, 0xE8, 0xDC},    //11
                         {0x1E, 0x14, 0xFB, 0xEC},    //12
                         {0xFB, 0xFF, 0xFB, 0xF8},    //13
                         {0x07, 0x0C, 0x02, 0x00}     //14
	                    };
	//init
	read_retry_mode = (read_retry_type>>16)&0xff;
	read_retry_cycle =(read_retry_type>>8)&0xff;
	read_retry_reg_num = (read_retry_type>>0)&0xff;

	if(read_retry_mode == 0)  //mode0
	{
		read_retry_reg_adr[0] = 0xAC;
		read_retry_reg_adr[1] = 0xAD;
		read_retry_reg_adr[2] = 0xAE;
		read_retry_reg_adr[3] = 0xAF;

		//set read retry level
		for(i=0;i<read_retry_cycle;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = para0[i][j];
			}
		}

	}
	else if(read_retry_mode == 1) //mode1
	{
		read_retry_reg_adr[0] = 0xA7;
		read_retry_reg_adr[1] = 0xAD;
		read_retry_reg_adr[2] = 0xAE;
		read_retry_reg_adr[3] = 0xAF;

		//set read retry level
		for(i=0;i<read_retry_cycle;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = para1[i][j];
			}
			
		}
		
		//set lsb mode
		lsb_mode_reg_num = 5;
		
		lsb_mode_reg_adr[0] = 0xa4;
		lsb_mode_reg_adr[1] = 0xa5;
		lsb_mode_reg_adr[2] = 0xb0;
		lsb_mode_reg_adr[3] = 0xb1;
		lsb_mode_reg_adr[4] = 0xc9;
		
		lsb_mode_val[0] = 0x25;
		lsb_mode_val[1] = 0x25;
		lsb_mode_val[2] = 0x25;
		lsb_mode_val[3] = 0x25;
		lsb_mode_val[4] = 0x1;
	}
	else if(read_retry_mode == 0x10)  //mode0x10  toshiba readretry mode0
	{
	    read_retry_reg_adr[0] = 0x04;
		read_retry_reg_adr[1] = 0x05;
		read_retry_reg_adr[2] = 0x06;
		read_retry_reg_adr[3] = 0x07;
	   
	    //set read retry level
		for(i=0;i<read_retry_cycle;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = para0x10[i];
			}			
		}
	   
	}
	else if(read_retry_mode == 0x20)  //mode0x10  Samsung mode0
	{
	    read_retry_reg_adr[0] = 0xA7;
		read_retry_reg_adr[1] = 0xA4;
		read_retry_reg_adr[2] = 0xA5;
		read_retry_reg_adr[3] = 0xA6;
	   
	    //set read retry level
		for(i=0;i<read_retry_cycle+1;i++)
		{
			for(j=0; j<read_retry_reg_num;j++)
			{
				read_retry_val[i][j] = para0x20[i][j];
			}
			
		}
	}
	
	return 0;
}

__s32 NFC_GetDefaultParam(__u32 chip,__u8* default_value, __u32 read_retry_type)
{
    __s32 ret; 
    __u32 i; 
    
    
    if(read_retry_mode<0x10)  //hynix read retry mode
    {
        ret =_vender_get_param(&read_retry_default_val[chip][0], &read_retry_reg_adr[0], read_retry_reg_num);
        for(i=0; i<read_retry_reg_num; i++)
        {
            default_value[i] = read_retry_default_val[chip][i];
        }
        
    	return ret;
	}
    else
    {
        return 0;
    }
}

__s32 NFC_SetDefaultParam(__u32 chip,__u8* default_value,__u32 read_retry_type)
{
    __s32 ret; 
    __u32 i; 
    
    if(read_retry_mode<0x10)  //hynix read retry mode
    {
        for(i=0; i<read_retry_reg_num; i++)
        {
            default_value[i] = read_retry_default_val[chip][i];
        }
        ret =_vender_set_param(&read_retry_default_val[chip][0], &read_retry_reg_adr[0], read_retry_reg_num);
        
    	return ret;
    }
    else if((read_retry_mode>=0x20)&&(read_retry_mode<0x30))  //samsung read retry mode
    {
        for(i=0; i<read_retry_reg_num; i++)
        {
            default_value[i] = (__u8)read_retry_val[0][i];;
        }
        ret =_vender_set_param(default_value, &read_retry_reg_adr[0], read_retry_reg_num);
        
    	return ret;
    }
    else
    {
        return 0;
    }
        
    
}

__s32 NFC_ReadRetryExit(__u32 read_retry_type)
{

	return 0;
}
