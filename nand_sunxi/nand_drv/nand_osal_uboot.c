/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm boot sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : nand_osal_boot0.c
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
#include  <common.h>
#include  <malloc.h>
#include  <asm/arch/dma.h>

#define   CCMU_REGS_BASE    0x01c20000

int OSAL_printf(const char * str, ...);
/*
****************************************************************************************************
*
*             OSAL_DmaInit
*
*  Description:
*       dma controller init
*
*  Parameters:
*
*  Return value:
*		EPDK_OK/FAIL
****************************************************************************************************
*/
int NAND_DmaInit(void)
{
    return 0;
}
/*
****************************************************************************************************
*
*             OSAL_DmaExit
*
*  Description:
*       dma controller exit
*
*  Parameters:
*
*  Return value:
*		EPDK_OK/FAIL
****************************************************************************************************
*/
int NAND_DmaExit(void)
{
    return 0;
}
/*
****************************************************************************************************
*
*             OSAL_DmaRequest
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
unsigned int NAND_RequestDMA(unsigned int dmatype)
{
    return (unsigned int)DMA_Request(dmatype);
}
/*
****************************************************************************************************
*
*             OSAL_DmaRelease
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
int NAND_ReleaseDMA(unsigned int hDma)
{
	/* stop dma                 */
    DMA_Stop(hDma);

    return DMA_Release(hDma);
}
/*
****************************************************************************************************
*
*             OSAL_DmaConfig
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
int NAND_SettingDMA(unsigned int hDMA, void * arg)
{
	return DMA_Setting(hDMA, (__dma_setting_t *)arg);
}
/*
**********************************************************************************************************************
*
*             OSAL_DmaStart
*
*  Description:
*
*
*  Parameters:
*
*
*  Return value:
*
*
****************************************************************************************************
*/
int NAND_StartDMA(unsigned int hDMA, unsigned int saddr, unsigned int daddr, unsigned int bytes)
{
    if((saddr & 0x01c03000) == 0x01c03000)
    {
        //这是读操作，读的时候刷新目的地址，原则就是刷新DRAM(SRAM)
        flush_cache(daddr, bytes);
    }
    else
    {
    	//这是写操作，写的时候刷新源地址，原则就是刷新DRAM(SRAM)
        flush_cache(saddr, bytes);
    }
    return DMA_Start(hDMA, saddr, daddr, bytes);
}

/*
**********************************************************************************************************************
*
*             OSAL_DmaRestart
*
*  Description:
*       start dma
*
*  Parameters:
*       hDma	dma handler
*
*  Return value:
*		EPDK_OK/FAIL
*
**********************************************************************************************************************
*/
int NAND_RestartDMA(unsigned int hDma)
{
    return DMA_Restart(hDma);
}
/*
**********************************************************************************************************************
*
*             OSAL_DmaStop
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
int NAND_StopDMA(unsigned int hDma)
{
    return DMA_Stop(hDma);
}
/*
**********************************************************************************************************************
*                                       OSAL_DmaQueryStatus
*
* Description: This function is used to query interrupt pending and clear pending bits if some pending is be set
*
* Arguments  : hDma         dma handle
*
* Returns    : bit31~24     main interrupt no
*              bit23~16     sub interrupt no
*              bit1         dma end interrupt flag
*              bit0         dma half end interrupt flag
**********************************************************************************************************************
*/
unsigned int NAND_QueryDmaStat(unsigned int hDma)
{
    return DMA_QueryStatus(hDma);
}


void NAND_WaitDmaFinish(void)
{

}



void NAND_OpenAHBClock(void)
{
    *(volatile unsigned int *)(CCMU_REGS_BASE + 0x60) |= 1 << 13;
}

void NAND_CloseAHBClock(void)
{
    *(volatile unsigned int *)(CCMU_REGS_BASE + 0x60) &= ~(1 << 13);
}


/*
**********************************************************************************************************************
*
*             NAND_GetCmuClk
*
*  Description:
*
*
*  Parameters:
*
*
*  Return value:
*
*
**********************************************************************************************************************
*/
__u32 NAND_GetCmuClk(void)
{
	unsigned int reg_val;
	unsigned int div_p, factor_n;
	unsigned int factor_k, factor_m;
	unsigned int clock;

	reg_val  = *(volatile unsigned int *)(0x01c20000 + 0x20);
	div_p    = (reg_val >> 16) & 0x3;
	factor_n = (reg_val >> 8) & 0x1f;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	factor_m = ((reg_val >> 0) & 0x3) + 1;

	clock = 24 * factor_n * factor_k/div_p/factor_m;

	return clock;
}
/*
**********************************************************************************************************************
*
*             NAND_GetCmuClk
*
*  Description:
*
*
*  Parameters:
*
*
*  Return value:
*
*
**********************************************************************************************************************
*/
int NAND_SetClock(unsigned int nand_max_clock)
{
	unsigned int edo_clk, cmu_clk;
	unsigned int cfg;
	unsigned int nand_clk_divid_ratio;

	/*set nand clock*/
	//edo_clk = (nand_max_clock > 20)?(nand_max_clock-10):nand_max_clock;
	edo_clk = nand_max_clock * 2;

    cmu_clk = NAND_GetCmuClk( );
	nand_clk_divid_ratio = cmu_clk / edo_clk;
	if (cmu_clk % edo_clk)
			nand_clk_divid_ratio++;
	if (nand_clk_divid_ratio){
		if (nand_clk_divid_ratio > 16)
			nand_clk_divid_ratio = 15;
		else
			nand_clk_divid_ratio--;
	}
	/*set nand clock gate on*/
	cfg = *(volatile __u32 *)(CCMU_REGS_BASE + 0x80);

	/*gate on nand clock*/
	cfg |= (1U << 31);
	/*take cmu pll as nand src block*/
	cfg &= ~(0x3 << 24);
	cfg |=  (0x2 << 24);
	//set divn = 0
	cfg &= ~(0x03 << 12);

	/*set ratio*/
	cfg &= ~(0x0f << 0);
	cfg |= (nand_clk_divid_ratio & 0xf) << 0;

	*(volatile unsigned int *)(CCMU_REGS_BASE + 0x80) = cfg;

	return 0;
}

__s32 NAND_GetClock(void)
{
	__u32 cmu_clk;
	__u32 cfg;
	__u32 nand_max_clock, nand_clk_divid_ratio;

	/*set nand clock*/
    cmu_clk = NAND_GetCmuClk( );

    /*set nand clock gate on*/
	cfg = *(volatile __u32 *)(CCMU_REGS_BASE + 0x80);
    nand_clk_divid_ratio = ((cfg)&0xf) +1;
    nand_max_clock = cmu_clk/(2*nand_clk_divid_ratio);

    return nand_max_clock;


}

void   NAND_GetPin(void)
{
	*(volatile uint *)(0x01c20800 + 0x48) = 0x22222222;
	*(volatile uint *)(0x01c20800 + 0x4C) = 0x22222222;
	*(volatile uint *)(0x01c20800 + 0x50) = 0x2222222;

}


void NAND_ReleasePin(void)
{

}

void * OSAL_PhyAlloc(unsigned int Size)
{
	return malloc(Size);
}

void OSAL_PhyFree(void *pAddr, unsigned int Size)
{
	free(pAddr);
}

void  OSAL_IrqUnLock(unsigned int  p)
{
    ;
}
void  OSAL_IrqLock  (unsigned int *p)
{
    ;
}

void *OSAL_io_remap(unsigned int base_addr, unsigned int size)
{
    return (void *)base_addr;
}
/*
**********************************************************************************************************************
*
*             OSAL_printf
*
*  Description:  用户可以自行设定是否需要打印
*
*
*  Parameters:
*
*
*  Return value:
*
*
**********************************************************************************************************************
*/
int OSAL_printf(const char * str, ...)
{
#ifdef DEBUG
    printf(str);
#endif
    return 0;
}
