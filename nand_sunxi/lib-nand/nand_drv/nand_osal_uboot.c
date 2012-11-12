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



int NAND_ClkRequest(void)
{
    return 0;
}

void NAND_ClkRelease(void)
{
    return ;
}

int NAND_AHBEnable(void)
{
    *(volatile __u32 *)(CCMU_REGS_BASE + 0x60) |= 1U << 13;
    return 0;
}

int NAND_ClkEnable(void)
{
     *(volatile __u32 *)(CCMU_REGS_BASE + 0x80) |= 1U<< 31;

     return 0;
}

void NAND_AHBDisable(void)
{
    *(volatile __u32 *)(CCMU_REGS_BASE + 0x60) &= ~(1U << 13);
}



void NAND_ClkDisable(void)
{
    *(volatile __u32 *)(CCMU_REGS_BASE + 0x80) &= ~(1U << 31);
}

__u32 _GetCmuClk(void)
{
	__u32 reg_val;
	__u32 div_p, factor_n;
	__u32 factor_k, factor_m;
	__u32 clock;
#ifndef CONFIG_SUN6I_FPGA
	reg_val  = *(volatile unsigned int *)(0x01c20000 + 0x20);
	div_p    = (reg_val >> 16) & 0x3;
	factor_n = (reg_val >> 8) & 0x1f;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	factor_m = ((reg_val >> 0) & 0x3) + 1;

	clock = 24 * factor_n * factor_k/div_p/factor_m;
#else
	clock = 600;
#endif
	return clock;
}

int NAND_WaitDmaFinish(void)
{
    return 0;
}

int NAND_SetClk(unsigned int nand_clock)
{
	__u32 edo_clk, cmu_clk;
	__u32 cfg;
	__u32 nand_clk_divid_ratio;

	/*set nand clock*/
	//edo_clk = (nand_max_clock > 20)?(nand_max_clock-10):nand_max_clock;
	edo_clk = nand_clock * 2;

    cmu_clk = _GetCmuClk( );
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

	*(volatile __u32 *)(CCMU_REGS_BASE + 0x80) = cfg;

	return 0;
}

int NAND_GetClk(void)
{
	__u32 cmu_clk;
	__u32 cfg;
	__u32 nand_max_clock, nand_clk_divid_ratio;

	/*set nand clock*/
    cmu_clk = _GetCmuClk( );

    /*set nand clock gate on*/
	cfg = *(volatile __u32 *)(CCMU_REGS_BASE + 0x80);
    nand_clk_divid_ratio = ((cfg)&0xf) +1;
    nand_max_clock = cmu_clk/(2*nand_clk_divid_ratio);

    return nand_max_clock;


}

__s32 NAND_CleanFlushDCacheRegion(__u32 buff_addr, __u32 len)
{
    flush_cache(buff_addr, len);
    return 0;
}

void NAND_PIORequest(void)
{
	*(volatile uint *)(0x01c20800 + 0x48) = 0x22222222;
	*(volatile uint *)(0x01c20800 + 0x4C) = 0x22222222;
	*(volatile uint *)(0x01c20800 + 0x50) = 0x2222222;

}


void NAND_PIORelease(void)
{

}

void * NAND_Malloc(unsigned int Size)
{
	return malloc(Size);
}

void NAND_Free(void *pAddr, unsigned int Size)
{
//	free(pAddr);
}

void  OSAL_IrqUnLock(unsigned int  p)
{
    ;
}
void  OSAL_IrqLock  (unsigned int *p)
{
    ;
}

int NAND_WaitRbReady(void)
{
    return 0;
}

void *NAND_IORemap(unsigned int base_addr, unsigned int size)
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
int NAND_Print(const char * str, ...)
{
//#ifdef DEBUG
    printf(str);
//#endif
    return 0;
}
void NAND_EnRbInt(void)
{
    return ;
}

void NAND_ClearRbInt(void)
{
    return ;
}

void NAND_RbInterrupt(void)
{
    return ;
}

__u32 NAND_DMASingleMap(__u32 rw, __u32 buff_addr, __u32 len)
{
	return;
}

__u32 NAND_DMASingleUnmap(__u32 rw, __u32 buff_addr, __u32 len)
{
	return;
}

__u32 NAND_VA_TO_PA(__u32 buff_addr)
{
	return;
}
