/*
**********************************************************************************************************************
*											        eGon
*						                     the Embedded System
*									       script parser sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : standby.c
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
#include "standby_i.h"
#include <asm/arch/ccmu.h>


//static  __u32  pll1_value = 0;
//static  __u32  pll2_value = 0;
//static  __u32  pll3_value = 0;
//static  __u32  pll4_value = 0;
//static  __u32  pll5_value = 0;
//static  __u32  pll6_value = 0;
//static  __u32  pll7_value = 0;
//static  __u32  pll8_value = 0;
//static  __u32  pll9_value = 0;
//static  __u32  pll10_value = 0;
//
//
//__s32 standby_clock_store(void)
//{
//	pll1_value  = CCMU_REG_PLL1_CTRL;
//	pll2_value  = CCMU_REG_PLL2_CTRL;
//	pll3_value  = CCMU_REG_PLL3_CTRL;
//	pll4_value  = CCMU_REG_PLL4_CTRL;
//	pll5_value  = CCMU_REG_PLL5_CTRL;
//	pll6_value  = CCMU_REG_PLL6_CTRL;
//	pll7_value  = CCMU_REG_PLL7_CTRL;
//	pll8_value  = CCMU_REG_PLL8_CTRL;
//	pll9_value  = CCMU_REG_PLL9_CTRL;
//	pll10_value = CCMU_REG_PLL10_CTRL;
//
//	return 0;
//}
//
//
//__s32 standby_clock_restore(void)
//{
//	CCMU_REG_PLL1_CTRL  = pll1_value;
//	CCMU_REG_PLL2_CTRL  = pll2_value;
//	CCMU_REG_PLL3_CTRL  = pll3_value;
//	CCMU_REG_PLL4_CTRL  = pll4_value;
//	CCMU_REG_PLL5_CTRL  = pll5_value;
//	CCMU_REG_PLL6_CTRL  = pll6_value;
//	CCMU_REG_PLL7_CTRL  = pll7_value;
//	CCMU_REG_PLL8_CTRL  = pll8_value;
//	CCMU_REG_PLL9_CTRL  = pll9_value;
//	CCMU_REG_PLL10_CTRL = pll10_value;
//
//	return 0;
//}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int standby_set_divd(int pll)
{
	__u32 reg_val;

	//config axi
	reg_val = readl(CCM_CPU_L2_AXI_CTRL);
	reg_val &= ~(0x03 << 0);
	reg_val |=  (0x02 << 0);
	writel(reg_val, CCM_CPU_L2_AXI_CTRL);
	//config ahb
	reg_val = readl(CCM_AHB1_APB1_CTRL);
	reg_val &= ~((0x03 << 12) | (0x03 << 8) | (0x03 << 4));
	reg_val |=   (0x02 << 12);
	if(pll <= 400)
	{
		;
	}
	else if(pll <= 800)
	{
		reg_val |= (1 << 4);
	}
	else if(pll <= 1600)
	{
		reg_val |= (2 << 4);
	}
	else
	{
		reg_val |= (3 << 4);
	}
	writel(reg_val, CCM_AHB1_APB1_CTRL);

	return 0;
}

__s32 standby_clock_to_24M(void)
{
	__u32 reg_val;
	int   i;

	standby_set_divd(24);

	reg_val = readl(CCM_CPU_L2_AXI_CTRL);
    reg_val &= ~(0x03 << 16);
    reg_val |=  (0x01 << 16);
    writel(reg_val, CCM_CPU_L2_AXI_CTRL);

	for(i=0; i<0x4000; i++);

	return 0;
}

__s32 standby_clock_to_pll1(void)
{
	__u32 pll1;
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_k, div_m;

	reg_val  = readl(CCM_PLL1_CPUX_CTRL);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	div_m = ((reg_val >> 0) & 0x3) + 1;

	pll1 = 24 * factor_n * factor_k/div_m;

	standby_set_divd(pll1);

	reg_val = readl(CCM_PLL1_CPUX_CTRL);
    reg_val &= ~(0x03 << 16);
    reg_val |=  (0x02 << 16);
    writel(reg_val, CCM_PLL1_CPUX_CTRL);

	return 0;
}


void standby_clock_plldisable(void)
{
	uint reg_val;

	reg_val = readl(CCM_PLL1_CPUX_CTRL);
	reg_val &= ~(1U << 31);
	writel(reg_val, CCM_PLL1_CPUX_CTRL);

	reg_val = readl(CCM_PLL3_VIDEO_CTRL);
	reg_val &= ~(1U << 31);
	writel(reg_val, CCM_PLL3_VIDEO_CTRL);

	reg_val = readl(CCM_PLL6_MOD_CTRL);
	reg_val &= ~(1U << 31);
	writel(reg_val, CCM_PLL6_MOD_CTRL);

	reg_val = readl(CCM_PLL7_VIDEO1_CTRL);
	reg_val &= ~(1U << 31);
	writel(reg_val, CCM_PLL7_VIDEO1_CTRL);
}

void standby_clock_pllenable(void)
{
	__u32 reg_val;

	reg_val = readl(CCM_PLL1_CPUX_CTRL);
	reg_val |= (1U << 31);
	writel(reg_val, CCM_PLL1_CPUX_CTRL);
	do
	{
		reg_val = readl(CCM_PLL1_CPUX_CTRL);
	}
	while(!(reg_val & (0x1 << 28)));


	reg_val = readl(CCM_PLL3_VIDEO_CTRL);
	reg_val |= (1U << 31);
	writel(reg_val, CCM_PLL3_VIDEO_CTRL);
	do
	{
		reg_val = readl(CCM_PLL3_VIDEO_CTRL);
	}
	while(!(reg_val & (0x1 << 28)));

	reg_val = readl(CCM_PLL6_MOD_CTRL);
	reg_val |= (1U << 31);
	writel(reg_val, CCM_PLL6_MOD_CTRL);
	do
	{
		reg_val = readl(CCM_PLL6_MOD_CTRL);
	}
	while(!(reg_val & (0x1 << 28)));

	reg_val = readl(CCM_PLL7_VIDEO1_CTRL);
	reg_val |= (1U << 31);
	writel(reg_val, CCM_PLL7_VIDEO1_CTRL);
	do
	{
		reg_val = readl(CCM_PLL7_VIDEO1_CTRL);
	}
	while(!(reg_val & (0x1 << 28)));
}

//void standby_clock_divsetto0(void)
//{
//	return ;
//}
//
//void standby_clock_divsetback(void)
//{
//	return ;
//}
//
//void standby_clock_drampll_ouput(int op)
//{
////	__u32 reg_val;
////
////	reg_val = CCMU_REG_PLL5_CTRL;
////	if(op)
////	{
////		reg_val |= 1 << 29;
////	}
////	else
////	{
////		reg_val &= ~(1 << 29);
////	}
////	CCMU_REG_PLL5_CTRL = reg_val;
////
////	return;
//}

/*
*********************************************************************************************************
*                           standby_tmr_enable_watchdog
*
*Description: enable watch-dog.
*
*Arguments  : none.
*
*Return     : none;
*
*Notes      :
*
*********************************************************************************************************
*/
//#define  TMR_WATCHDOG_MODE   (*(volatile unsigned int *)(0x1c20C00 + 0x94))

void standby_tmr_enable_watchdog(void)
{
    /* set watch-dog reset, timeout is 2 seconds */
//    TMR_WATCHDOG_MODE = (2<<3) | (1<<1);
//    /* enable watch-dog */
//    TMR_WATCHDOG_MODE |= (1<<0);
}


/*
*********************************************************************************************************
*                           standby_tmr_disable_watchdog
*
*Description: disable watch-dog.
*
*Arguments  : none.
*
*Return     : none;
*
*Notes      :
*
*********************************************************************************************************
*/
void standby_tmr_disable_watchdog(void)
{
    /* disable watch-dog reset */
//    TMR_WATCHDOG_MODE &= ~(1<<1);
//    /* disable watch-dog */
//    TMR_WATCHDOG_MODE &= ~(1<<0);
}

void standby_clock_apb1_to_source(int clock)
{
//	__u32 reg_val;
//
//	reg_val = CCMU_REG_APB1;
//	reg_val &= ~(0x03 << 24);
//	if(clock == 32000)
//	{
//		reg_val |= (2 << 24);
//	}
//	CCMU_REG_APB1 = reg_val;
}


void standby_clock_24m_op(int op)
{
//	__u32 reg_val1, reg_val2;
//
//	if(!op)
//	{
//		reg_val1 = CCMU_REG_OSC24M;
//		reg_val1 &= ~1;
//		CCMU_REG_OSC24M = reg_val1;
//
//		reg_val2 = CCMU_REG_OSC24M;
//		reg_val2 &= ~(0xffU << 24);
//		reg_val2 |=  (0xA7U << 24);
//		reg_val2 &= ~(0x01 << 16);
//		CCMU_REG_OSC24M = reg_val2;
//
//		reg_val1 = CCMU_REG_PLL5_CTRL;
//		reg_val1 &= ~(1 << 7);
//		CCMU_REG_PLL5_CTRL = reg_val1;
//
//		reg_val2 &= ~(0xffU << 24);
//		CCMU_REG_OSC24M = reg_val2;
//	}
//	else
//	{
//		int i;
//
//		reg_val1 = CCMU_REG_OSC24M;
//		reg_val1 &= ~(0xffU << 24);
//		reg_val1 |=  (0xA7U << 24);
//		reg_val1 |=  (0x01 << 16);
//		CCMU_REG_OSC24M = reg_val1;
//
//		reg_val2 = CCMU_REG_PLL5_CTRL;
//		reg_val2 |= 1 << 7;
//		CCMU_REG_PLL5_CTRL = reg_val2;
//
//		reg_val1 &= ~(0xffU << 24);
//		CCMU_REG_OSC24M = reg_val1;
//
//		for(i=0;i<100;i++);
//
//		reg_val1 = CCMU_REG_OSC24M;
//		reg_val1 |= 1;
//		CCMU_REG_OSC24M = reg_val1;
//		for(i=0;i<100;i++);
//	}

	return;
}

