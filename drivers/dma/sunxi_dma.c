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
#include <asm/arch/intc.h>


struct dma_irq_handler
{
	void                *m_data;
	void (*m_func)( void * data);
};

int    dma_int_count;
struct dma_irq_handler dma_int_f[CFG_SW_DMA_MAX];
static sunxi_dma_channal_t dma_channal_sets[CFG_SW_DMA_MAX];

static void sunxi_dma_int_func(void *p)
{
	int i;
	struct sunxi_dma_regs *dma_int = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;

	for(i=0;i<8;i++)
	{
		if(dma_int_f[i].m_func)
		{
			if(dma_int->intsts0 & (i * 2))
			{
				dma_int->intsts0 = (1 << (i * 20));
				dma_int_f[i].m_func(dma_int_f[i].m_data);
			}
		}
	}
	for(i=8;i<15;i++)
	{
		if(dma_int_f[i].m_func)
		{
			if(dma_int->intsts1 & (i * 2))
			{
				dma_int->intsts1 = (1 << (i * 20));
				dma_int_f[i].m_func(dma_int_f[i].m_data);
			}
		}
	}	
}


void sunxi_dma_init(void)
{
    struct sunxi_dma_regs *dma_int = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;

	dma_int->inten0 = 0;
	dma_int->inten1 = 0;

	dma_int->intsts0 = 0xffffffff;
	dma_int->intsts1 = 0xffffffff;

	memset(dma_channal_sets, 0, 16 * sizeof(sunxi_dma_channal_t));
	memset((void *)dma_int_f, 0, CFG_SW_DMA_MAX * sizeof(struct dma_irq_handler));
	dma_int_count = 0;
	irq_install_handler(AW_IRQ_DMA, sunxi_dma_int_func, 0);

	return ;
}

void sunxi_dma_exit(void)
{
    struct sunxi_dma_regs *dma_int = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;

	dma_int->inten0 = 0;
	dma_int->inten1 = 0;

	dma_int->intsts0 = 0xffffffff;
	dma_int->intsts1 = 0xffffffff;

	irq_free_handler(AW_IRQ_DMA);
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
    int   i;

	for(i=0;i<CFG_SW_DMA_MAX;i++)
    {
        if(dma_channal_sets[i].used == 0)
        {
            dma_channal_sets[i].used = 1;
			dma_channal_sets[i].num  = i;

            return (uint)&dma_channal_sets[i];
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
    int   num;
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;
	struct __sw_dma_control  *dma_control;

	if(!dma_channal->used)
	{
		return -1;
	}
	num = dma_channal->num;
	dma_control = (struct __sw_dma_control *)(CFG_SW_DMA_CHANNAL_BASE + num * 0x40);
	
	dma_control->enable = 0;
	dma_channal->used   = 0;
	dma_channal->num    = 0;
	
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
    int    num;
	uint   value;
	uint   commit_para;
	__dma_setting_t          *dma_sets = (__dma_setting_t *)cfg;
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;

	if(!dma_channal->used)
	{
		return -1;
	}
	num = dma_channal->num;

	value = *((uint *)&dma_sets->cfg);
	if(dma_sets->loop_mode)
	{
		dma_channal->dma_config.link = (uint )&dma_channal->dma_config;
 	}
	else
	{
		dma_channal->dma_config.link = SUNXI_DMA_LINK_NULL;
	}
	commit_para  = (dma_sets->wait_cyc & 0xff);
	commit_para |= (dma_sets->data_block_size & 0xff ) << 8;
	dma_channal->dma_config.commit_para = commit_para;
	dma_channal->dma_config.config = value;

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
    int   num;
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;
	struct __sw_dma_control  *dma_control;

	if(!dma_channal->used)
	{
		return -1;
	}
	num = dma_channal->num;

	dma_control = (struct __sw_dma_control *)(CFG_SW_DMA_CHANNAL_BASE + num * 0x40);
	
 	dma_channal->dma_config.source_addr = saddr;
 	dma_channal->dma_config.dest_addr   = daddr;
 	dma_channal->dma_config.byte_count  = bytes;

	dma_control->start_addr = (uint)&dma_channal->dma_config;
	dma_control->enable = 1;

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
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;
	struct __sw_dma_control  *dma_control;

	if(!dma_channal->used)
	{
		return -1;
	}
	dma_control = (struct __sw_dma_control *)(CFG_SW_DMA_CHANNAL_BASE + dma_channal->num * 0x40);
	
	dma_control->enable = 0;

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
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;
	struct sunxi_dma_regs    *dma_status;

	if(!dma_channal->used)
	{
		return -1;
	}
	dma_status = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;
	
	return (dma_status->status >> dma_channal->num) & 0x01;

}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_dma_install_int(uint hdma, interrupt_handler_t dma_int_func, void *p)
{
	int   dma_no;
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;
	struct sunxi_dma_regs    *dma_status;

	if(!dma_channal->used)
	{
		return -1;
	}
	dma_status = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;

	dma_no = dma_channal->num;
	if(dma_no < 8)
	{
		dma_status->intsts0 = (7 << dma_no);
	}
	else
	{
		dma_status->intsts1 = (7 << (dma_no - 8));
	}

	if(!dma_int_f[dma_no].m_func)
	{
		dma_int_f[dma_no].m_func = dma_int_func;
		dma_int_f[dma_no].m_data = p;
	}
	else
	{
		printf("this dma channel int is used already, you have to free it first\n");

		return -1;
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_dma_enable_int(uint hdma)
{
	int   dma_no;
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;
	struct sunxi_dma_regs    *dma_status;

	if(!dma_channal->used)
	{
		return -1;
	}
	dma_status = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;

	dma_no = dma_channal->num;
	if(dma_no < 8)
	{
	    if(dma_status->inten0 & (2 << dma_no))
	    {
	    	printf("%d dma int is avaible already\n", dma_no);

			return 0;
		}
		dma_status->inten0 |= (2 << dma_no);
	}
	else
	{
		if(dma_status->inten1 & (2 << (dma_no-8)))
	    {
	    	printf("%d dma int is avaible already\n", dma_no);

			return 0;
		}
		dma_status->inten1 |= (2 << (dma_no-8));
	}

	if(!dma_int_count)
	{
		irq_enable(AW_IRQ_DMA);
	}
	dma_int_count ++;

	return 0;	
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_dma_disaable_int(uint hdma)
{
	int   dma_no;
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;
	struct sunxi_dma_regs    *dma_status;

	if(!dma_channal->used)
	{
		return -1;
	}
	dma_status = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;

	dma_no = dma_channal->num;
	if(dma_no < 8)
	{
	    if(!(dma_status->inten0 & (2 << dma_no)))
	    {
	    	printf(" %d dma int is not used yet\n", dma_no);

			return 0;
		}
		dma_status->inten0 &= ~(2 << dma_no);
	}
	else
	{
		if(!(dma_status->inten1 & (2 << (dma_no-8))))
	    {
	    	printf(" %d dma int is not used yet\n", dma_no);

			return 0;
		}
		dma_status->inten1 &= ~(2 << (dma_no-8));
	}

	//enable golbal int
	dma_int_count --;
	if(!dma_int_count)
	{
		irq_disable(AW_IRQ_DMA);
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_dma_free_int(uint hdma)
{
	int   dma_no;
	sunxi_dma_channal_t      *dma_channal = (sunxi_dma_channal_t *)hdma;
	struct sunxi_dma_regs    *dma_status;

	if(!dma_channal->used)
	{
		return -1;
	}
	dma_status = (struct sunxi_dma_regs *)SUNXI_DMA_BASE;

	dma_no = dma_channal->num;
	if(dma_no < 8)
	{
		dma_status->intsts0 = (7 << dma_no);
	}
	else
	{
		dma_status->intsts1 = (7 << (dma_no - 8));
	}

	if(dma_int_f[dma_no].m_func)
	{
		dma_int_f[dma_no].m_func = NULL;
		dma_int_f[dma_no].m_data = NULL;
	}
	else
	{
		printf("this dma channel int is free, you do not need to free it again\n");

		return -1;
	}

	return 0;

}

