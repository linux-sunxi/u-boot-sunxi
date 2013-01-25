/*
*************************************************************************************
*                         			eBsp
*					   Operation System Adapter Layer
*
*				(c) Copyright 2006-2010, All winners Co,Ld.
*							All	Rights Reserved
*
* File Name 	: OSAL_Clock.h
*
* Author 		: javen
*
* Description 	: 操作系统适配层
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	   	2010-09-07          1.0         create this word
*		holi			2010-12-03			1.1			实现了具体的接口
*************************************************************************************
*/

#ifndef  __OSAL_CLOCK_H__
#define  __OSAL_CLOCK_H__

#include "OSAL.h"

/* define module clock id       */
typedef enum __CCU_CLK_ID
{
    SYS_CLK_NONE=0,  /* invalid clock id                     */

    /* OSC */
    SYS_CLK_LOSC,    /* "sys_losc"   ,LOSC, 32768 hz clock   */
    SYS_CLK_HOSC,    /* "sys_hosc"   ,HOSC, 24Mhz clock      */

    /* PLL */
    SYS_CLK_PLL1,    /* "sys_pll1"   ,PLL1 clock             */
    SYS_CLK_PLL2,    /* "sys_pll2"   ,PLL2 clock             */
    SYS_CLK_PLL3,    /* "sys_pll3"   ,PLL3 clock             */
    SYS_CLK_PLL4,    /* "sys_pll4"   ,PLL4 clock             */
    SYS_CLK_PLL5,    /* "sys_pll5"   ,PLL5 clock             */
    SYS_CLK_PLL6,    /* "sys_pll6"   ,PLL6 clock,            */
    SYS_CLK_PLL7,    /* "sys_pll7"   ,PLL7 clock             */
    SYS_CLK_PLL8,    /* "sys_pll8"   ,PLL8 clock             */
    SYS_CLK_PLL9,    /* "sys_pll9"   ,PLL9 clock             */
    SYS_CLK_PLL10,   /* "sys_pll10"  ,PLL10 clock            */

    /* related PLL */
    SYS_CLK_PLL2X8,  /* "sys_pll2X8"   ,PLL2 8X clock        */
    SYS_CLK_PLL3X2,  /* "sys_pll3X2"   ,PLL3 X2 clock        */
    SYS_CLK_PLL6x2,  /* "sys_pll6X2"   ,PLL6 X2 clock        */
    SYS_CLK_PLL7X2,  /* "sys_pll7X2"   ,PLL7 X2 clock        */
    SYS_CLK_MIPIPLL, /* "sys_mipi_pll" ,MIPI PLL clock       */

    /* CPU & BUS */
    SYS_CLK_AC327,   /* "sys_ac327"  ,CPU clock              */
    SYS_CLK_AR100,   /* "sys_ar100"  ,CPU clock              */
    SYS_CLK_AXI,     /* "sys_axi"    ,AXI clock              */
    SYS_CLK_AHB0,    /* "sys_ahb0"   ,AHB0 clock             */
    SYS_CLK_AHB1,    /* "sys_ahb1"   ,AHB1 clock             */
    SYS_CLK_APB0,    /* "sys_apb0"   ,APB0 clock             */
    SYS_CLK_APB1,    /* "sys_apb1"   ,APB1 clock             */
    SYS_CLK_APB2,    /* "sys_apb2"   ,APB2 clock             */

    CCU_CLK_NULL,    /* null module clock id                 */

    /* module clock */
    MOD_CLK_NAND0,       /* "mod_nand0"                      */
    MOD_CLK_NAND1,       /* "mod_nand1"                      */
    MOD_CLK_SDC0,        /* "mod_sdc0"                       */
    MOD_CLK_SDC1,        /* "mod_sdc1"                       */
    MOD_CLK_SDC2,        /* "mod_sdc2"                       */
    MOD_CLK_SDC3,        /* "mod_sdc3"                       */
    MOD_CLK_TS,          /* "mod_ts"                         */
    MOD_CLK_SS,          /* "mod_ss"                         */
    MOD_CLK_SPI0,        /* "mod_spi0"                       */
    MOD_CLK_SPI1,        /* "mod_spi1"                       */
    MOD_CLK_SPI2,        /* "mod_spi2"                       */
    MOD_CLK_SPI3,        /* "mod_spi3"                       */
    MOD_CLK_I2S0,        /* "mod_i2s0"                       */
    MOD_CLK_I2S1,        /* "mod_i2s1"                       */
    MOD_CLK_SPDIF,       /* "mod_spdif"                      */
    MOD_CLK_USBPHY0,     /* "mod_usbphy0"                    */
    MOD_CLK_USBPHY1,     /* "mod_usbphy1"                    */
    MOD_CLK_USBPHY2,     /* "mod_usbphy2"                    */
    MOD_CLK_USBEHCI0,    /* "mod_usbehci0"                   */
    MOD_CLK_USBEHCI1,    /* "mod_usbehci1"                   */
    MOD_CLK_USBOHCI0,    /* "mod_usbohci0"                   */
    MOD_CLK_USBOHCI1,    /* "mod_usbohci1"                   */
    MOD_CLK_USBOHCI2,    /* "mod_usbohci2"                   */
    MOD_CLK_USBOTG,      /* "mod_usbotg"                     */
    MOD_CLK_MDFS,        /* "mod_mdfs"                       */
    MOD_CLK_DEBE0,       /* "mod_debe0"                      */
    MOD_CLK_DEBE1,       /* "mod_debe1"                      */
    MOD_CLK_DEFE0,       /* "mod_defe0"                      */
    MOD_CLK_DEFE1,       /* "mod_defe1"                      */
    MOD_CLK_DEMIX,       /* "mod_demp"                       */
    MOD_CLK_LCD0CH0,     /* "mod_lcd0ch0"                    */
    MOD_CLK_LCD0CH1,     /* "mod_lcd0ch1"                    */
    MOD_CLK_LCD1CH0,     /* "mod_lcd1ch0"                    */
    MOD_CLK_LCD1CH1,     /* "mod_lcd1ch1"                    */
    MOD_CLK_CSI0S,       /* "mod_csi0s"                      */
    MOD_CLK_CSI0M,       /* "mod_csi0m"                      */
    MOD_CLK_CSI1S,       /* "mod_csi1s"                      */
    MOD_CLK_CSI1M,       /* "mod_csi1m"                      */
    MOD_CLK_VE,          /* "mod_ve"                         */
    MOD_CLK_ADDA,        /* "mod_adda"                       */
    MOD_CLK_AVS,         /* "mod_avs"                        */
    MOD_CLK_HDMI,        /* "mod_hdmi"                       */
    MOD_CLK_HDMI_DDC,     /* "mod_hdmi_ddc"                   */
    MOD_CLK_PS,          /* "mod_ps"                         */
    MOD_CLK_MTCACC,      /* "mod_mtcacc"                     */
    MOD_CLK_MBUS0,       /* "mod_mbus0"                      */
    MOD_CLK_MBUS1,       /* "mod_mbus1"                      */
    MOD_CLK_DRAM,        /* "mod_dram"                       */
    MOD_CLK_MIPIDSIS,    /* "mod_mipidsis"                   */
    MOD_CLK_MIPIDSIP,    /* "mod_mipidsip"                   */
    MOD_CLK_MIPICSIS,    /* "mod_mipicsis"                   */
    MOD_CLK_MIPICSIP,    /* "mod_mipicsip"                   */
    MOD_CLK_IEPDRC0,     /* "mod_iepdrc0"                    */
    MOD_CLK_IEPDRC1,     /* "mod_iepdrc1"                    */
    MOD_CLK_IEPDEU0,     /* "mod_iepdeu0"                    */
    MOD_CLK_IEPDEU1,     /* "mod_iepdeu1"                    */
    MOD_CLK_GPUCORE,     /* "mod_gpucore"                    */
    MOD_CLK_GPUMEM,      /* "mod_gpumem"                     */
    MOD_CLK_GPUHYD,      /* "mod_gpuhyd"                     */
    MOD_CLK_TWI0,        /* "mod_twi0"                       */
    MOD_CLK_TWI1,        /* "mod_twi1"                       */
    MOD_CLK_TWI2,        /* "mod_twi2"                       */
    MOD_CLK_TWI3,        /* "mod_twi3"                       */
    MOD_CLK_UART0,       /* "mod_uart0"                      */
    MOD_CLK_UART1,       /* "mod_uart1"                      */
    MOD_CLK_UART2,       /* "mod_uart2"                      */
    MOD_CLK_UART3,       /* "mod_uart3"                      */
    MOD_CLK_UART4,       /* "mod_uart4"                      */
    MOD_CLK_UART5,       /* "mod_uart5"                      */
    MOD_CLK_GMAC,        /* "mod_gmac"                       */
    MOD_CLK_DMA,         /* "mod_dma"                        */
    MOD_CLK_HSTMR,       /* "mod_hstmr"                      */
    MOD_CLK_MSGBOX,      /* "mod_msgbox"                     */
    MOD_CLK_SPINLOCK,    /* "mod_spinlock"                   */
    MOD_CLK_LVDS,        /* "mod_lvds"                       */
    MOD_CLK_SMPTWD,      /* "smp_twd"                        */

    /* axi module gating */
    AXI_CLK_DRAM,        /* "axi_dram"                       */

    /* ahb module gating */
    AHB_CLK_MIPICSI,     /* "ahb_mipicsi"                    */
    AHB_CLK_MIPIDSI,     /* "ahb_mipidsi"                    */
    AHB_CLK_SS,          /* "ahb_ss"                         */
    AHB_CLK_DMA,         /* "ahb_dma"                        */
    AHB_CLK_SDMMC0,      /* "ahb_sdmmc0"                     */
    AHB_CLK_SDMMC1,      /* "ahb_sdmmc1"                     */
    AHB_CLK_SDMMC2,      /* "ahb_sdmmc2"                     */
    AHB_CLK_SDMMC3,      /* "ahb_sdmmc3"                     */
    AHB_CLK_NAND1,       /* "ahb_nand1"                      */
    AHB_CLK_NAND0,       /* "ahb_nand0"                      */
    AHB_CLK_SDRAM,       /* "ahb_sdram"                      */
    AHB_CLK_GMAC,        /* "ahb_gmac"                       */
    AHB_CLK_TS,          /* "ahb_ts"                         */
    AHB_CLK_HSTMR,       /* "ahb_hstmr"                      */
    AHB_CLK_SPI0,        /* "ahb_spi0"                       */
    AHB_CLK_SPI1,        /* "ahb_spi1"                       */
    AHB_CLK_SPI2,        /* "ahb_spi2"                       */
    AHB_CLK_SPI3,        /* "ahb_spi3"                       */
    AHB_CLK_OTG,         /* "ahb_otg"                        */
    AHB_CLK_EHCI0,       /* "ahb_ehci0"                      */
    AHB_CLK_EHCI1,       /* "ahb_ehci1"                      */
    AHB_CLK_OHCI0,       /* "ahb_ohci0"                      */
    AHB_CLK_OHCI1,       /* "ahb_ohci1"                      */
    AHB_CLK_OHCI2,       /* "ahb_ohci2"                      */
    AHB_CLK_VE,          /* "ahb_ve"                         */
    AHB_CLK_LCD0,        /* "ahb_lcd0"                       */
    AHB_CLK_LCD1,        /* "ahb_lcd1"                       */
    AHB_CLK_CSI0,        /* "ahb_csi0"                       */
    AHB_CLK_CSI1,        /* "ahb_csi1"                       */
    AHB_CLK_HDMI,       /* "ahb_hdmid"                      */
    AHB_CLK_DEBE0,       /* "ahb_debe0"                      */
    AHB_CLK_DEBE1,       /* "ahb_debe1"                      */
    AHB_CLK_DEFE0,       /* "ahb_defe0"                      */
    AHB_CLK_DEFE1,       /* "ahb_defe1"                      */
    AHB_CLK_MP,          /* "ahb_mp"                         */
    AHB_CLK_GPU,         /* "ahb_gpu"                        */
    AHB_CLK_MSGBOX,      /* "ahb_msgbox"                     */
    AHB_CLK_SPINLOCK,    /* "ahb_spinlock"                   */
    AHB_CLK_DEU0,        /* "ahb_deu0"                       */
    AHB_CLK_DEU1,        /* "ahb_deu1"                       */
    AHB_CLK_DRC0,        /* "ahb_drc0"                       */
    AHB_CLK_DRC1,        /* "ahb_drc1"                       */
    AHB_CLK_MTCACC,      /* "ahb_mtcacc"                     */

    /* apb module gating */
    APB_CLK_ADDA,        /* "apb_adda"                       */
    APB_CLK_SPDIF,       /* "apb_spdif"                      */
    APB_CLK_PIO,         /* "apb_pio"                        */
    APB_CLK_I2S0,        /* "apb_i2s0"                       */
    APB_CLK_I2S1,        /* "apb_i2s1"                       */
    APB_CLK_TWI0,        /* "apb_twi0"                       */
    APB_CLK_TWI1,        /* "apb_twi1"                       */
    APB_CLK_TWI2,        /* "apb_twi2"                       */
    APB_CLK_TWI3,        /* "apb_twi3"                       */
    APB_CLK_UART0,       /* "apb_uart0"                      */
    APB_CLK_UART1,       /* "apb_uart1"                      */
    APB_CLK_UART2,       /* "apb_uart2"                      */
    APB_CLK_UART3,       /* "apb_uart3"                      */
    APB_CLK_UART4,       /* "apb_uart4"                      */
    APB_CLK_UART5,       /* "apb_uart5"                      */

    /* dram module gating */
    DRAM_CLK_VE,         /* "dram_ve"                        */
    DRAM_CLK_CSI0,       /* "dram_csi0"                      */
    DRAM_CLK_CSI1,       /* "dram_csi1"                      */
    DRAM_CLK_TS,         /* "dram_ts"                        */
    DRAM_CLK_DRC0,       /* "dram_drc0"                      */
    DRAM_CLK_DRC1,       /* "dram_drc1"                      */
    DRAM_CLK_DEU0,       /* "dram_deu0"                      */
    DRAM_CLK_DEU1,       /* "dram_deu1"                      */
    DRAM_CLK_DEFE0,      /* "dram_defe0"                     */
    DRAM_CLK_DEFE1,      /* "dram_defe1"                     */
    DRAM_CLK_DEBE0,      /* "dram_debe0"                     */
    DRAM_CLK_DEBE1,      /* "dram_debe1"                     */
    DRAM_CLK_MP,         /* "dram_mp"                        */

    CCU_CLK_CNT          /* invalid id, for calc count       */

} __ccu_clk_id_e;

/*
*********************************************************************************************************
*                                   SET SOURCE CLOCK FREQUENCY
*
* Description: 
*		set source clock frequency;
*
* Arguments  : 
*		nSclkNo  	:	source clock number;
*       nFreq   	:	frequency, the source clock will change to;
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_SetSrcFreq(__u32 nSclkNo, __u32 nFreq);



/*
*********************************************************************************************************
*                                   GET SOURCE CLOCK FREQUENCY
*
* Description: 
*		get source clock frequency;
*
* Arguments  : 
*		nSclkNo  	:	source clock number need get frequency;
*
* Returns    : 
*		frequency of the source clock;
*
* Note       :
*********************************************************************************************************
*/
__u32 OSAL_CCMU_GetSrcFreq(__u32 nSclkNo);



/*
*********************************************************************************************************
*                                   OPEN MODULE CLK
* Description: 
*		open module clk;
*
* Arguments  : 
*		nMclkNo	:	number of module clock which need be open;
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__hdle OSAL_CCMU_OpenMclk(__s32 nMclkNo);


/*
*********************************************************************************************************
*                                    CLOSE MODULE CLK
* Description: 
*		close module clk;
*
* Arguments  : 
*		hMclk	:	handle
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__s32  OSAL_CCMU_CloseMclk(__hdle hMclk);

/*
*********************************************************************************************************
*                                   GET MODULE SRC
* Description: 
*		set module src;
*
* Arguments  : 
*		nMclkNo	:	number of module clock which need be open;
*       nSclkNo	:	call-back function for process clock change;
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_SetMclkSrc(__hdle hMclk, __u32 nSclkNo);





/*
*********************************************************************************************************
*                                  GET MODULE SRC
*
* Description: 
*		get module src;
*
* Arguments  : 
*		nMclkNo	:	handle of the module clock;
*
* Returns    : 
*		src no
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_GetMclkSrc(__hdle hMclk);




/*
*********************************************************************************************************
*                                   SET MODUEL CLOCK FREQUENCY
*
* Description: 
*		set module clock frequency;
*
* Arguments  : 
*		nSclkNo  :	number of source clock which the module clock will use;
*		nDiv     :	division for the module clock;
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_SetMclkDiv(__hdle hMclk, __s32 nDiv);



/*
*********************************************************************************************************
*                                   GET MODUEL CLOCK FREQUENCY
*
* Description: 
*		get module clock requency;
*
* Arguments  : 
*		hMclk    	:	module clock handle;
*
* Returns    : 
*		frequency of the module clock;
*
* Note       :
*********************************************************************************************************
*/
__u32 OSAL_CCMU_GetMclkDiv(__hdle hMclk);



/*
*********************************************************************************************************
*                                   MODUEL CLOCK ON/OFF
*
* Description: 
*		module clock on/off;
*
* Arguments  : 
*		nMclkNo		:	module clock handle;
*       bOnOff   	:	on or off;
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_MclkOnOff(__hdle hMclk, __s32 bOnOff);

__s32 OSAL_CCMU_MclkReset(__hdle hMclk, __s32 bReset);


/*
//第一版
__s32  esCLK_SetSrcFreq(__s32 nSclkNo, __u32 nFreq);
__u32  esCLK_GetSrcFreq(__s32 nSclkNo);

__hdle esCLK_OpenMclk(__s32 nMclkNo, __pCB_ClkCtl_t pCb);
__s32  esCLK_CloseMclk(__hdle hMclk);

__s32  esCLK_SetMclkSrc(__s32 nMclkNo, __s32 nSclkNo);
__s32  esCLK_GetMclkSrc(__s32 nMclkNo);

__s32  esCLK_SetMclkDiv(__s32 nMclkNo, __s32 nDiv);
__u32  esCLK_GetMclkDiv(__s32 nMclkNo);

__s32  esCLK_MclkOnOff(__s32 nMclkNo, __s32 bOnOff);

//======================================================================================

//第二版
__s32 esCLK_reg_cb(__s32 nMclkNo, __pCB_ClkCtl_t pCb);	//__hdle esCLK_OpenMclk(__s32 nMclkNo, __pCB_ClkCtl_t pCb);
__s32  esCLK_unreg_cb(__s32 nMclkNo);					//__s32  esCLK_CloseMclk(__hdle hMclk);

//------------------------------------------------------

					__s32  esCLK_SetSrcFreq(__s32 nSclkNo, __u32 nFreq);
					__u32  esCLK_GetSrcFreq(__s32 nSclkNo);


__hdle esCLK_OpenMclk(__s32 nMclkNo);
__s32  esCLK_CloseMclk(__hdle hMclk);



__s32  esCLK_SetMclkSrc(__hdle hMclk, __s32 nSclkNo);	//__s32  esCLK_SetMclkSrc(__s32 nMclkNo, __s32 nSclkNo);
__s32  esCLK_GetMclkSrc(__hdle hMclk);					//__s32  esCLK_GetMclkSrc(__s32 nMclkNo);

__s32  esCLK_SetMclkDiv(__hdle hMclk, __s32 nDiv);
__u32  esCLK_GetMclkDiv(__hdle hMclk);

__s32  esCLK_MclkOnOff(__hdle hMclk, __s32 bOnOff);


*/

#endif   //__OSAL_CLOCK_H__

