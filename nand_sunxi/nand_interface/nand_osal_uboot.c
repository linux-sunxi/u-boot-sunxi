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

int NAND_Print(const char * str, ...)
{
#ifdef DEBUG
    printf(str);
#endif
    return 0;
}

__u32 _Getpll6Clk(void)
{
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_k, div_m;
	__u32 clock;

	reg_val  = *(volatile __u32 *)(0x01c20000 + 0x28);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	div_m = ((reg_val >> 0) & 0x3) + 1;

	clock = 24 * factor_n * factor_k/2;
	NAND_Print("pll6 clock is %dM\n", clock);
	if(clock!=600)
		NAND_Print("pll6 clock rate error!!!!!!!\n");

	return 600;
}



int NAND_ClkRequest(__u32 nand_index)
{
	__u32 cfg;
	__u32 m, n;
	__u32 clk_src;

	clk_src = 1;
	if(clk_src)
		  NAND_Print("NAND_ClkRequest, select pll6: %dM\n", _Getpll6Clk());

	if(nand_index == 0)
	{
		//10M
		if(clk_src == 0)
		{
			m = 0;
			n = 0;
		}
		else
		{
			m = 14;
			n = 1;
		}


		/*set nand clock gate on*/
		cfg = 0;

		/*gate on nand clock*/
		cfg |= (1U << 31);
		/*take pll6 as nand src block*/
		cfg |=  ((clk_src&0x3) << 24);

		//set divn = 0
		cfg |= (n&0x3)<<16;
		cfg |= (m&0xf)<<0;

		*(volatile __u32 *)(0x01c20000 + 0x80) = cfg;

		//open ahb
		cfg = *(volatile __u32 *)(0x01c20000 + 0x60);
		cfg |= (0x1<<13);
		*(volatile __u32 *)(0x01c20000 + 0x60) = cfg;

		//reset
		cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
		cfg &= (~(0x1<<13));
		*(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

		cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
		cfg |= (0x1<<13);
		*(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

		cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
		cfg &= (~(0x1<<13));
		*(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

		cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
		cfg |= (0x1<<13);
		*(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

		NAND_Print("NAND_ClkRequest, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));
		NAND_Print("Reg 0x01c20060: 0x%x\n", *(volatile __u32 *)(0x01c20060));
		NAND_Print("Reg 0x01c202c0: 0x%x\n", *(volatile __u32 *)(0x01c202c0));
	}
	else
	{
		//10M
		if(clk_src == 0)
		{
			m = 0;
			n = 0;
		}
		else
		{
			m = 14;
			n = 1;
		}

		/*set nand clock gate on*/
		cfg = 0;

		/*gate on nand clock*/
		cfg |= (1U << 31);
		/*take pll6 as nand src block*/
		cfg |=  ((clk_src&0x3) << 24);
		//set divn = 0
		cfg |= (n&0x3)<<16;
		cfg |= (m&0xf)<<0;

		*(volatile __u32 *)(0x01c20000 + 0x84) = cfg;

		//open ahb
		cfg = *(volatile __u32 *)(0x01c20000 + 0x60);
		cfg |= (0x1<<12);
		*(volatile __u32 *)(0x01c20000 + 0x60) = cfg;

		//open reset
		cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
		cfg &= (~(0x1<<12));
		*(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

		cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
		cfg |= (0x1<<12);
		*(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

		cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
		cfg &= (~(0x1<<12));
		*(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

		cfg = *(volatile __u32 *)(0x01c20000 + 0x2c0);
		cfg |= (0x1<<12);
		*(volatile __u32 *)(0x01c20000 + 0x2c0) = cfg;

		NAND_Print("NAND_ClkRequest, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20084: 0x%x\n", *(volatile __u32 *)(0x01c20084));
		NAND_Print("Reg 0x01c20060: 0x%x\n", *(volatile __u32 *)(0x01c20060));
		NAND_Print("Reg 0x01c202c0: 0x%x\n", *(volatile __u32 *)(0x01c202c0));
	}


	return 0;
}


void NAND_ClkRelease(void)
{
    return ;
}

#if 0
int NAND_AHBEnable(void)
{
    *(volatile __u32 *)(CCMU_REGS_BASE + 0x60) |= 1U << 13;
    return 0;
}
#endif

#if 0
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
#endif

#if 0
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
#endif
int NAND_WaitDmaFinish(void)
{
    return 0;
}

int NAND_SetClk(__u32 nand_index, __u32 nand_clock)
{
	__u32 edo_clk, pll6_clk;
	__u32 cfg;
	__u32 nand_clk_divid_ratio;
	__u32 m = 0, n = 0;
	__u32 clk_src;


	clk_src = 1;

	if(clk_src == 0)
	{
	}
	else
	{
		edo_clk = nand_clock * 2;
		if(edo_clk <= 20)  //10M
		{
			n =  1;
			m = 14;
		}
		else if((edo_clk >20)&&(edo_clk <= 40))  //20M
		{
			n =  0;
			m = 14;
		}
		else if((edo_clk >40)&&(edo_clk <= 50))  //25M
		{
			n =  0;
			m = 11;
		}
		else if((edo_clk >50)&&(edo_clk <= 60))  //30M
		{
			n = 0;
			m = 9;
		}
		else //40M
		{
			n = 0;
			m = 7;
		}
	}


	if(nand_index == 0)
	{
		/*set nand clock*/
		/*set nand clock gate on*/
		cfg = *(volatile __u32 *)(0x01c20000 + 0x80);
		cfg &= (~(0x03 << 16));
		cfg &= (~(0xf));
		cfg |= ((n&0x3)<<16);
		cfg |= ((m&0xf));

		*(volatile __u32 *)(0x01c20000 + 0x80) = cfg;
		NAND_Print("NAND_SetClk, nand_index: 0x%x, clock: %d\n", nand_index, nand_clock);
		NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));

	}
	else
	{
		/*set nand clock*/
		/*set nand clock gate on*/
		cfg = *(volatile __u32 *)(0x01c20000 + 0x84);
		cfg &= (~(0x03 << 16));
		cfg &= (~(0xf));
		cfg |= ((n&0x3)<<16);
		cfg |= ((m&0xf));

		*(volatile __u32 *)(0x01c20000 + 0x84) = cfg;
		NAND_Print("NAND_SetClk, nand_index: 0x%x, clock: %d\n", nand_index, nand_clock);
		NAND_Print("Reg 0x01c20084: 0x%x\n", *(volatile __u32 *)(0x01c20084));

	}

	return 0;
}




int NAND_GetClk(__u32 nand_index)
{
	__u32 pll6_clk;
	__u32 cfg;
	__u32 nand_max_clock, nand_clk_divid_ratio;
	__u32 m, n;

	if(nand_index == 0)
	{
		/*set nand clock*/
	    pll6_clk = _Getpll6Clk();

	    /*set nand clock gate on*/
		cfg = *(volatile __u32 *)(0x01c20000 + 0x80);
	    m = ((cfg)&0xf) +1;
	    n = ((cfg>>16)&0x3);
	    nand_max_clock = pll6_clk/(2*(1<<n)*(m+1));
		NAND_Print("NAND_GetClk, nand_index: 0x%x, nand_clk: %dM\n", nand_index, nand_max_clock);
		NAND_Print("Reg 0x01c20080: 0x%x\n", *(volatile __u32 *)(0x01c20080));


	}
	else
	{
		/*set nand clock*/
	    pll6_clk = _Getpll6Clk();

	    /*set nand clock gate on*/
		cfg = *(volatile __u32 *)(0x01c20000 + 0x84);
	    m = ((cfg)&0xf) +1;
	    n = ((cfg>>16)&0x3);
	    nand_max_clock = pll6_clk/(2*(1<<n)*(m+1));
		NAND_Print("NAND_GetClk, nand_index: 0x%x, nand_clk: %dM\n", nand_index, nand_max_clock);
		NAND_Print("Reg 0x01c20084: 0x%x\n", *(volatile __u32 *)(0x01c20084));
	}

	return nand_max_clock;
}

#if 1
__s32 NAND_CleanFlushDCacheRegion(__u32 buff_addr, __u32 len)
{
    flush_cache(buff_addr, len);
    return 0;
}
#else
__s32 NAND_CleanFlushDCacheRegion(unsigned int buff_addr, int len)
{
	//这是写操作，写的时候刷新源地址，原则就是刷新DRAM(SRAM)
	CleanFlushDCacheRegion((void *)buff_addr, len);

    return 0;
}
#endif


void NAND_PIORequest(__u32 nand_index)
{
	__u32 cfg;

	if(nand_index == 0)
	{
		*(volatile __u32 *)(0x01c20800 + 0x48) = 0x22222222;
		*(volatile __u32 *)(0x01c20800 + 0x4c) = 0x22222222;
		//*(volatile __u32 *)(0x01c20800 + 0x50) = 0x22222222;
		cfg = *(volatile __u32 *)(0x01c20800 + 0x54);
		cfg &= (~0xfff);
		cfg |= 0x222;
		*(volatile __u32 *)(0x01c20800 + 0x54) = cfg;
		NAND_Print("NAND_PIORequest, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20848: 0x%x\n", *(volatile __u32 *)(0x01c20848));
		NAND_Print("Reg 0x01c2084c: 0x%x\n", *(volatile __u32 *)(0x01c2084c));
		//NAND_Print("Reg 0x01c20850: 0x%x\n", *(volatile __u32 *)(0x01c20850));
		NAND_Print("Reg 0x01c20854: 0x%x\n", *(volatile __u32 *)(0x01c20854));
	}
	else if(nand_index == 1)
	{
		*(volatile __u32 *)(0x01c20800 + 0x50) = 0x33333333;
		*(volatile __u32 *)(0x01c20800 + 0xfc) = 0x22222222;
		cfg = *(volatile __u32 *)(0x01c20800 + 0x100);
		cfg &= (~0xf);
		cfg |= (0x2);
		*(volatile __u32 *)(0x01c20800 + 0x100) = cfg;
		cfg = *(volatile __u32 *)(0x01c20800 + 0x108);
		cfg &= (~0xf<<20);
		cfg |= (0x22<<20);
		*(volatile __u32 *)(0x01c20800 + 0x108) = cfg;

		NAND_Print("NAND_PIORequest, nand_index: 0x%x\n", nand_index);
		NAND_Print("Reg 0x01c20850: 0x%x\n", *(volatile __u32 *)(0x01c20850));
		NAND_Print("Reg 0x01c208fc: 0x%x\n", *(volatile __u32 *)(0x01c208fc));
		NAND_Print("Reg 0x01c20900: 0x%x\n", *(volatile __u32 *)(0x01c20900));
		NAND_Print("Reg 0x01c20908: 0x%x\n", *(volatile __u32 *)(0x01c20908));
	}
	else
	{

	}

}


void NAND_PIORelease(__u32 nand_index)
{
	return;
}

void NAND_Memset(void* pAddr, unsigned char value, unsigned int len)
{
    memset(pAddr, value, len);
}

void NAND_Memcpy(void* pAddr_dst, void* pAddr_src, unsigned int len)
{
    memcpy(pAddr_dst, pAddr_src, len);
}

void * NAND_Malloc(unsigned int Size)
{
	return malloc(Size);
}

void NAND_Free(void *pAddr, unsigned int Size)
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
	return buff_addr;
}


__u32 NAND_DMASingleUnmap(__u32 rw, __u32 buff_addr, __u32 len)
{
	return buff_addr;
}

__u32 NAND_VA_TO_PA(__u32 buff_addr)
{
    return buff_addr;
}



__u32 NAND_GetIOBaseAddrCH0(void)
{
	return 0x01c03000;
}

__u32 NAND_GetIOBaseAddrCH1(void)
{
	return 0x01c05000;
}
