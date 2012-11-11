/*
********************************************************************************
*                                                   OSAL
*
*                                     (c) Copyright 2008-2009, Kevin China
*                                             				All Rights Reserved
*
* File    : OSAL_Clock.c
* By      : Sam.Wu
* Version : V1.00
* Date    : 2011/3/25 20:25 
* Description :  
* Update   :  date      author      version     notes    
********************************************************************************
*/
#include "OSAL_Clock.h"

#if 0
__s32 OSAL_CCMU_SetSrcFreq(__u32 nSclkNo, __u32 nFreq)
{
    //__inf("OSAL_CCMU_SetSrcFreq,%d,%d\n", nSclkNo, nFreq);

    if(nSclkNo == AW_SYS_CLK_PLL3)
    {
        sys_put_wvalue(0x01c20010,0x8010d000 | (nFreq/3000000));
    }
    else if(nSclkNo == AW_SYS_CLK_PLL7)
    {
        sys_put_wvalue(0x01c20030,0x8010d000 | (nFreq/3000000));
    }
    else if(nSclkNo == AW_SYS_CLK_PLL6)
    {
        __u32 N=0, K=2;
        __u32 value = 0;

        N = (nFreq + ((K * 24000000)/2)) / (K * 24000000);
        value = sys_get_wvalue(0x01c20028) & (~(0x1f<<8)) & (~(0x3<<4));
        sys_put_wvalue(0x01c20028,value | (1<<31) | (N<<8) | ((K-1)<<4));
    }
    return 0;
}


__u32 OSAL_CCMU_GetSrcFreq(__u32 nSclkNo)
{
    __u32 value = 0;

    //__inf("OSAL_CCMU_GetSrcFreq,%d\n",nSclkNo);

    if(nSclkNo == AW_SYS_CLK_PLL3)
    {
        value = sys_get_wvalue(0x01c20010)&0x7f;
    }
    else if(nSclkNo == AW_SYS_CLK_PLL7)
    {
        value = sys_get_wvalue(0x01c20030)&0x7f;
    }
    else if(nSclkNo == AW_SYS_CLK_PLL5P)
    {
        __u32 reg,N,K,P;

        reg = sys_get_wvalue(0x01c20020);
        N = (reg & (0x1f<<8))>>8;
        K = ((reg & (0x3<<4))>>4) + 1;
        P = (reg & (0x3<<16))>>16;
        if(P==0)
        {
            P = 1;
        }
        else if(P==1)
        {
            P = 2;
        }
        else if(P==2)
        {
            P = 4;
        }
        else
        {
            P = 8;
        }

        value = (24 * N * K)/P;
    }

    return value * 3000000;
}

__hdle OSAL_CCMU_OpenMclk(__s32 nMclkNo)
{
    return (__hdle)nMclkNo;
}

__s32  OSAL_CCMU_CloseMclk(__hdle hMclk)
{
    return 0;
}

__s32 OSAL_CCMU_SetMclkSrc(__hdle hMclk, __u32 nSclkNo)
{
    __u32 clk_src = 0;
    __u32 value = 0;

    //__inf("OSAL_CCMU_SetMclkSrc,%d,%d\n", hMclk, nSclkNo);

    if(hMclk == AW_MOD_CLK_LCD0CH0 || hMclk == AW_MOD_CLK_LCD1CH0 || hMclk == AW_MOD_CLK_LCD0CH1_S1 || hMclk == AW_MOD_CLK_LCD0CH1_S2
        || hMclk == AW_MOD_CLK_LCD1CH1_S1 || hMclk == AW_MOD_CLK_LCD1CH1_S2 || hMclk == AW_MOD_CLK_HDMI)
    {
        if(nSclkNo == AW_SYS_CLK_PLL3)
        {
            clk_src = 0;
        }
        else if(nSclkNo == AW_SYS_CLK_PLL7)
        {
            clk_src = 1;
        }
        else if(nSclkNo == AW_SYS_CLK_PLL3X2)
        {
            clk_src = 2;
        }
        else if(nSclkNo == AW_SYS_CLK_PLL7X2)
        {
            clk_src = 3;
        }
    }
    else if(hMclk == AW_MOD_CLK_DEBE0 || hMclk == AW_MOD_CLK_DEBE1 || hMclk == AW_MOD_CLK_DEFE0 || hMclk == AW_MOD_CLK_DEFE1)
    {
        if(nSclkNo == AW_SYS_CLK_PLL3)
        {
            clk_src = 0;
        }
        else if(nSclkNo == AW_SYS_CLK_PLL7)
        {
            clk_src = 1;
        }
        else if(nSclkNo == AW_SYS_CLK_PLL5P)
        {
            clk_src = 2;
        }
    }


    if(hMclk == AW_MOD_CLK_LCD0CH0)
    {
        value = sys_get_wvalue(0x01c20118) & (~(0x3<<24));
        sys_put_wvalue(0x01c20118, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH0)
    {
        value = sys_get_wvalue(0x01c2011c) & (~(0x3<<24));
        sys_put_wvalue(0x01c2011c, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_LCD0CH1_S1)
    {
        value = sys_get_wvalue(0x01c2012c) & (~(0x3<<24));
        sys_put_wvalue(0x01c2012c, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_LCD0CH1_S2)
    {
        value = sys_get_wvalue(0x01c2012c) & (~(0x3<<24));
        sys_put_wvalue(0x01c2012c, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH1_S1)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0x3<<24));
        sys_put_wvalue(0x01c20130, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH1_S2)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0x3<<24));
        sys_put_wvalue(0x01c20130, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_HDMI)
    {
        value = sys_get_wvalue(0x01c20150) & (~(0x3<<24));
        sys_put_wvalue(0x01c20150, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20104) & (~(0x3<<24));
        sys_put_wvalue(0x01c20104, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20108) & (~(0x3<<24));
        sys_put_wvalue(0x01c20108, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c2010c) & (~(0x3<<24));
        sys_put_wvalue(0x01c2010c, value | (clk_src<<24));
    }
    else if(hMclk == AW_MOD_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20110) & (~(0x3<<24));
        sys_put_wvalue(0x01c20110, value | (clk_src<<24));
    }
    return 0;
}

__s32 OSAL_CCMU_GetMclkSrc(__hdle hMclk)
{
    return 0;
}

__s32 OSAL_CCMU_SetMclkDiv(__hdle hMclk, __s32 nDiv)
{
    __u32 value = 0;

    //__inf("OSAL_CCMU_SetMclkDiv,%d,%d\n", hMclk, nDiv);

    if(hMclk == AW_MOD_CLK_LCD0CH1_S1)
    {
        value = sys_get_wvalue(0x01c2012c);
        if((value & 0xf) == (nDiv-1))
        {
            sys_put_wvalue(0x01c2012c, value & (~((1)<<11)));
        }else
        {
            sys_put_wvalue(0x01c2012c, value | ((1)<<11));
        }
    }
    else if(hMclk == AW_MOD_CLK_LCD0CH1_S2)
    {
        value = sys_get_wvalue(0x01c2012c) & (~(0xf<<0));
        sys_put_wvalue(0x01c2012c, value | ((nDiv-1)<<0));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH1_S1)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0xf<<0));
        sys_put_wvalue(0x01c20130, value | ((nDiv-1)<<0));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH1_S2)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0xf<<0));
        sys_put_wvalue(0x01c20130, value | ((nDiv-1)<<0));
    }
    else if(hMclk == AW_MOD_CLK_HDMI)
    {
        value = sys_get_wvalue(0x01c20150) & (~(0xf<<0));
        sys_put_wvalue(0x01c20150, value | ((nDiv-1)<<0));
    }
    else if(hMclk == AW_MOD_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20104) & (~(0xf<<0));
        sys_put_wvalue(0x01c20104, value | ((nDiv-1)<<0));
    }
    else if(hMclk == AW_MOD_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20108) & (~(0xf<<0));
        sys_put_wvalue(0x01c20108, value | ((nDiv-1)<<0));
    }
    else if(hMclk == AW_MOD_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c2010c) & (~(0xf<<0));
        sys_put_wvalue(0x01c2010c, value | ((nDiv-1)<<0));
    }
    else if(hMclk == AW_MOD_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20110) & (~(0xf<<0));
        sys_put_wvalue(0x01c20110, value | ((nDiv-1)<<0));
    }
    return 0;
}

__u32 OSAL_CCMU_GetMclkDiv(__hdle hMclk)
{
    return 0;
}

__s32 OSAL_CCMU_MclkOnOff(__hdle hMclk, __s32 bOnOff)
{
    __u32 value = 0;

    //__inf("OSAL_CCMU_MclkOnOff,%d,%d\n", hMclk, bOnOff);

    if(hMclk == AW_MOD_CLK_LCD0CH0)
    {
        value = sys_get_wvalue(0x01c20118) & (~(0x1<<31));
        sys_put_wvalue(0x01c20118, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH0)
    {
        value = sys_get_wvalue(0x01c2011c) & (~(0x1<<31));
        sys_put_wvalue(0x01c2011c, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_LCD0CH1_S1)
    {
        value = sys_get_wvalue(0x01c2012c) & (~(0x1<<15));
        sys_put_wvalue(0x01c2012c, value | (bOnOff<<15));
    }
    else if(hMclk == AW_MOD_CLK_LCD0CH1_S2)
    {
        value = sys_get_wvalue(0x01c2012c) & (~(0x1<<31));
        sys_put_wvalue(0x01c2012c, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH1_S1)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0x1<<15));
        sys_put_wvalue(0x01c20130, value | (bOnOff<<15));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH1_S2)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0x1<<31));
        sys_put_wvalue(0x01c20130, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_HDMI)
    {
        value = sys_get_wvalue(0x01c20150) & (~(0x1<<31));
        sys_put_wvalue(0x01c20150, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20104) & (~(0x1<<31));
        sys_put_wvalue(0x01c20104, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20108) & (~(0x1<<31));
        sys_put_wvalue(0x01c20108, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c2010c) & (~(0x1<<31));
        sys_put_wvalue(0x01c2010c, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20110) & (~(0x1<<31));
        sys_put_wvalue(0x01c20110, value | (bOnOff<<31));
    }
    else if(hMclk == AW_MOD_CLK_SDRAM_DEBE0)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<26));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<26));
    }
    else if(hMclk == AW_MOD_CLK_SDRAM_DEBE1)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<27));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<27));
    }
    else if(hMclk == AW_MOD_CLK_SDRAM_DEFE0)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<25));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<25));
    }
    else if(hMclk == AW_MOD_CLK_SDRAM_DEFE1)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<24));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<24));
    }
    else if(hMclk == AW_MOD_CLK_AHB_DEFE1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<15));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<15));
    }
    else if(hMclk == AW_MOD_CLK_AHB_DEFE0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<14));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<14));
    }
    else if(hMclk == AW_MOD_CLK_AHB_DEBE1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<13));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<13));
    }
    else if(hMclk == AW_MOD_CLK_AHB_DEBE0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<12));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<12));
    }
    else if(hMclk == AW_MOD_CLK_AHB_HDMI)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<11));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<11));
    }
    else if(hMclk == AW_MOD_CLK_AHB_LCD1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<5));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<5));
    }
    else if(hMclk == AW_MOD_CLK_AHB_LCD0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<4));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<4));
    }
    else if(hMclk == AW_MOD_CLK_AHB_TVE1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<3));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<3));
    }
    else if(hMclk == AW_MOD_CLK_AHB_TVE0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<2));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<2));
    }
    return 0;
}

__s32 OSAL_CCMU_MclkReset(__hdle hMclk, __s32 bReset)
{
    __u32 value = 0;

    //__inf("OSAL_CCMU_MclkReset,%d,%d\n", hMclk, bReset);

    bReset = 1-bReset;

    if(hMclk == AW_MOD_CLK_LCD0CH0)
    {
        value = sys_get_wvalue(0x01c20118) & (~(0x1<<30));
        sys_put_wvalue(0x01c20118, value | (bReset<<30));
    }
    else if(hMclk == AW_MOD_CLK_LCD1CH0)
    {
        value = sys_get_wvalue(0x01c2011c) & (~(0x1<<30));
        sys_put_wvalue(0x01c2011c, value | (bReset<<30));
    }
    else if(hMclk == AW_MOD_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20104) & (~(0x1<<30));
        sys_put_wvalue(0x01c20104, value | (bReset<<30));
    }
    else if(hMclk == AW_MOD_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20108) & (~(0x1<<30));
        sys_put_wvalue(0x01c20108, value | (bReset<<30));
    }
    else if(hMclk == AW_MOD_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c2010c) & (~(0x1<<30));
        sys_put_wvalue(0x01c2010c, value | (bReset<<30));
    }
    else if(hMclk == AW_MOD_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20110) & (~(0x1<<30));
        sys_put_wvalue(0x01c20110, value | (bReset<<30));
    }
    else if(hMclk == AW_MOD_CLK_LVDS)
    {
        value = sys_get_wvalue(0x01c2014c) & (~(0x1<<0));
        sys_put_wvalue(0x01c2014c, value | (bReset<<0));
    }
    return 0;
}

#else
typedef __u32 CSP_CCM_sysClkNo_t;
__s32 OSAL_CCMU_SetSrcFreq( CSP_CCM_sysClkNo_t nSclkNo, __u32 nFreq )
{
    return 0;
}

__u32 OSAL_CCMU_GetSrcFreq( CSP_CCM_sysClkNo_t nSclkNo )
{
    return 0;
}

__hdle OSAL_CCMU_OpenMclk( __s32 nMclkNo )
{
    return 0;
}

__s32 OSAL_CCMU_CloseMclk( __hdle hMclk )
{
    return 0;
}

__s32 OSAL_CCMU_SetMclkSrc( __hdle hMclk, CSP_CCM_sysClkNo_t nSclkNo )
{
    return 0;
}

__s32 OSAL_CCMU_GetMclkSrc( __hdle hMclk )
{
    return 0;
}

__s32 OSAL_CCMU_SetMclkDiv( __hdle hMclk, __s32 nDiv )
{
    return 0;
}

__u32 OSAL_CCMU_GetMclkDiv( __hdle hMclk )
{
    return 0;
}

__s32 OSAL_CCMU_MclkOnOff( __hdle hMclk, __s32 bOnOff )
{
    return 0;
}

__s32 OSAL_CCMU_MclkReset(__hdle hMclk, __s32 bReset)
{
    return 0;
}
#endif
