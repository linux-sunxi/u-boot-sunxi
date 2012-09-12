/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON driver sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : dma.c
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include <asm/arch/dma_i.h>
#include <asm/arch/dma.h>
#include <common.h>

sw_dma_channal_set_t  dma_channal[CFG_SW_DMA_NORMAL_MAX + CFG_SW_DMA_DEDICATE_MAX] =
{
    {0, -1, (sw_dma_t)CFS_SW_DMA_NORMAL0  , 0                                           },
    {0, -1, (sw_dma_t)CFS_SW_DMA_NORMAL1  , 0                                           },
    {0, -1, (sw_dma_t)CFS_SW_DMA_NORMAL2  , 0                                           },
    {0, -1, (sw_dma_t)CFS_SW_DMA_NORMAL3  , 0                                           },
    {0, -1, (sw_dma_t)CFS_SW_DMA_NORMAL4  , 0                                           },
    {0, -1, (sw_dma_t)CFS_SW_DMA_NORMAL5  , 0                                           },
    {0, -1, (sw_dma_t)CFS_SW_DMA_NORMAL6  , 0                                           },
    {0, -1, (sw_dma_t)CFS_SW_DMA_NORMAL7  , 0                                           },
    {0, -1, (sw_dma_t)CFG_SW_DMA_DEDICATE0, (sw_dma_other_t)CFG_SW_DMA_DEDICATE0_OTHER  },
    {0, -1, (sw_dma_t)CFG_SW_DMA_DEDICATE1, (sw_dma_other_t)CFG_SW_DMA_DEDICATE1_OTHER  },
    {0, -1, (sw_dma_t)CFG_SW_DMA_DEDICATE2, (sw_dma_other_t)CFG_SW_DMA_DEDICATE2_OTHER  },
    {0, -1, (sw_dma_t)CFG_SW_DMA_DEDICATE3, (sw_dma_other_t)CFG_SW_DMA_DEDICATE3_OTHER  },
    {0, -1, (sw_dma_t)CFG_SW_DMA_DEDICATE4, (sw_dma_other_t)CFG_SW_DMA_DEDICATE4_OTHER  },
    {0, -1, (sw_dma_t)CFG_SW_DMA_DEDICATE5, (sw_dma_other_t)CFG_SW_DMA_DEDICATE5_OTHER  },
    {0, -1, (sw_dma_t)CFG_SW_DMA_DEDICATE6, (sw_dma_other_t)CFG_SW_DMA_DEDICATE6_OTHER  },
    {0, -1, (sw_dma_t)CFG_SW_DMA_DEDICATE7, (sw_dma_other_t)CFG_SW_DMA_DEDICATE7_OTHER  }
};

void sunxi_dma_exit(void)
{
    struct sunxi_dma_regs *dma_int = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;

	dma_int->inten = 0;
	dma_int->intsts = 0xffffffff;
}
/*
****************************************************************************************************
*
*             DMAC_RequestDma
*
*  Description:
*       request dma
*
*  Parameters:
*		type	0: normal timer
*				1: special timer
*  Return value:
*		dma handler
*		if 0, fail
****************************************************************************************************
*/
uint sunxi_dma_request(uint dmatype)
{
    uint   i;

    if(dmatype == DMAC_DMATYPE_NORMAL)
    {
        for(i=0;i<CFG_SW_DMA_NORMAL_MAX;i++)
        {
            if(!dma_channal[i].used)
            {
                dma_channal[i].used = 1;
                dma_channal[i].channalNo = i;

                return (uint)&dma_channal[i];
            }
        }
    }
    else if(dmatype == DMAC_DMATYPE_DEDICATED)
    {
        for(i=CFG_SW_DMA_NORMAL_MAX;i<CFG_SW_DMA_NORMAL_MAX + CFG_SW_DMA_DEDICATE_MAX;i++)
        {
            if(!dma_channal[i].used)
            {
                dma_channal[i].used = 1;
                dma_channal[i].channalNo = i;

                return (uint)&dma_channal[i];
            }
        }
    }

    return 0;
}
/*
****************************************************************************************************
*
*             DMAC_ReleaseDma
*
*  Description:
*       release dma
*
*  Parameters:
*       hDma	dma handler
*
*  Return value:
*		EPDK_OK/FAIL
****************************************************************************************************
*/
int sunxi_dma_release(uint hdma)
{
    sw_dma_channal_set_t * pdma = (sw_dma_channal_set_t *)hdma;

    /* stop dma                 */
    pdma->channal->config &= 0x7fffffff; /* stop dma */
    /* free dma handle          */
    pdma->used = 0;
    pdma->channalNo = -1;

    return 0;
}
/*
****************************************************************************************************
*
*             TMRC_SettingDMA
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*		pArg    *(pArg + 0)         ctrl
*               *(pArg + 1)         page size
*               *(pArg + 2)         page step
*               *(pArg + 3)         comity & block count
*
*  Return value:
*		EPDK_OK/FAIL
*
**********************************************************************************************************************
*/
int sunxi_dma_setting(uint hdma, void *cfg)
{
	sw_dma_channal_set_t    * pdma = (sw_dma_channal_set_t *)hdma;
	__dma_setting_t         * arg  = (__dma_setting_t      *)cfg;
	uint                   value;

    if(pdma->other)
    {
        __ddma_config_t    ddma_arg = {0};

        ddma_arg.src_drq_type     = arg->cfg.src_drq_type;
        ddma_arg.src_addr_type    = arg->cfg.src_addr_type;
        ddma_arg.src_burst_length = arg->cfg.src_burst_length;
        ddma_arg.src_data_width   = arg->cfg.src_data_width;

        ddma_arg.dst_drq_type     = arg->cfg.dst_drq_type;
        ddma_arg.dst_addr_type    = arg->cfg.dst_addr_type;
        ddma_arg.dst_burst_length = arg->cfg.dst_burst_length;
        ddma_arg.dst_data_width   = arg->cfg.dst_data_width;

        ddma_arg.continuous_mode  = arg->cfg.continuous_mode;

        value = *(uint *)&ddma_arg;
        pdma->channal->config       = value & (~0x80000000);
        pdma->other->page_size      = arg->pgsz;
        pdma->other->page_step      = arg->pgstp;
        pdma->other->comity_counter = arg->cmt_blk_cnt;
    }
    else
    {
        __ndma_config_t    ndma_arg = {0};

        ndma_arg.src_drq_type     = arg->cfg.src_drq_type;
        ndma_arg.src_addr_type    = arg->cfg.src_addr_type;
        ndma_arg.src_burst_length = arg->cfg.src_burst_length;
        ndma_arg.src_data_width   = arg->cfg.src_data_width;

        ndma_arg.dst_drq_type     = arg->cfg.dst_drq_type;
        ndma_arg.dst_addr_type    = arg->cfg.dst_addr_type;
        ndma_arg.dst_burst_length = arg->cfg.dst_burst_length;
        ndma_arg.dst_data_width   = arg->cfg.dst_data_width;

        ndma_arg.continuous_mode  = arg->cfg.continuous_mode;
        ndma_arg.wait_state       = arg->cfg.wait_state;

        value = *(uint *)&ndma_arg;
        pdma->channal->config       = value & (~0x80000000);
    }

    return 0;
}

/*
**********************************************************************************************************************
*
*             TMRC_StartTmr
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*
*  Return value:
*		EPDK_OK/FAIL
*
****************************************************************************************************
*/
int sunxi_dma_start(uint hdma, uint saddr, uint daddr, uint bytes)
{
	sw_dma_channal_set_t  * pdma = (sw_dma_channal_set_t *)hdma;

    pdma->channal->src_addr  = saddr;
    pdma->channal->dst_addr  = daddr;
    pdma->channal->bytes     = bytes;
    pdma->channal->config   |= 0x80000000;   /* start dma */

    return 0;
}
/*
**********************************************************************************************************************
*
*             DMAC_StopDma
*
*  Description:
*       stop dma
*
*  Parameters:
*       hDma	dma handler
*
*  Return value:
*		EPDK_OK/FAIL
*
**********************************************************************************************************************
*/
int sunxi_dma_stop(uint hdma)
{
	sw_dma_channal_set_t    * pdma = (sw_dma_channal_set_t *)hdma;

    pdma->channal->config &= 0x7fffffff; /* stop dma */

    return 0;
}
/*
**********************************************************************************************************************
*
*             eGon2_QueryDMAStatus
*
*  Description:
*
*
*  Parameters:
*       hDma	dma handler
*
*  Return value:
*		EPDK_OK/FAIL
*
**********************************************************************************************************************
*/
int sunxi_dma_querystatus(uint hdma)
{
    sw_dma_channal_set_t    * pdma = (sw_dma_channal_set_t *)hdma;

    return (pdma->channal->config >> 31) & 0x01;
}





