
#include "disp_display_i.h"
#include "disp_display.h"
#include "disp_clk.h"


#define CLK_ON 1
#define CLK_OFF 0
#define RST_INVAILD 0
#define RST_VAILD   1

#define CLK_DEBE0_AHB_ON	0x00000001
#define CLK_DEBE0_MOD_ON 	0x00000002
#define CLK_DEBE0_DRAM_ON	0x00000004
#define CLK_DEBE1_AHB_ON	0x00000010
#define CLK_DEBE1_MOD_ON 	0x00000020
#define CLK_DEBE1_DRAM_ON	0x00000040
#define CLK_DEFE0_AHB_ON	0x00000100
#define CLK_DEFE0_MOD_ON 	0x00000200
#define CLK_DEFE0_DRAM_ON	0x00000400
#define CLK_DEFE1_AHB_ON	0x00001000
#define CLK_DEFE1_MOD_ON 	0x00002000
#define CLK_DEFE1_DRAM_ON	0x00004000
#define CLK_LCDC0_AHB_ON	0x00010000
#define CLK_LCDC0_MOD0_ON  	0x00020000
#define CLK_LCDC0_MOD1_ON  	0x00040000	//represent lcd0-ch1-clk1 and lcd0-ch1-clk2
#define CLK_LCDC1_AHB_ON    0x00100000
#define CLK_LCDC1_MOD0_ON  	0x00200000
#define CLK_LCDC1_MOD1_ON  	0x00400000  //represent lcd1-ch1-clk1 and lcd1-ch1-clk2
#define CLK_TVENC0_AHB_ON	0x01000000
#define CLK_TVENC1_AHB_ON	0x02000000
#define CLK_HDMI_AHB_ON     0x10000000
#define CLK_HDMI_MOD_ON 	0x20000000
#define CLK_HDMI_MOD_DDC_ON 0x00000008
#define CLK_DSI_AHB_ON      0x40000000
#define CLK_DSI_MOD_ON      0x80000000

#define CLK_DEBE0_AHB_OFF	(~(CLK_DEBE0_AHB_ON	    ))
#define CLK_DEBE0_MOD_OFF 	(~(CLK_DEBE0_MOD_ON 	))
#define CLK_DEBE0_DRAM_OFF	(~(CLK_DEBE0_DRAM_ON	))
#define CLK_DEBE1_AHB_OFF	(~(CLK_DEBE1_AHB_ON	    ))
#define CLK_DEBE1_MOD_OFF 	(~(CLK_DEBE1_MOD_ON 	))
#define CLK_DEBE1_DRAM_OFF	(~(CLK_DEBE1_DRAM_ON	))
#define CLK_DEFE0_AHB_OFF	(~(CLK_DEFE0_AHB_ON	    ))
#define CLK_DEFE0_MOD_OFF 	(~(CLK_DEFE0_MOD_ON 	))
#define CLK_DEFE0_DRAM_OFF	(~(CLK_DEFE0_DRAM_ON	))
#define CLK_DEFE1_AHB_OFF	(~(CLK_DEFE1_AHB_ON	    ))
#define CLK_DEFE1_MOD_OFF 	(~(CLK_DEFE1_MOD_ON 	))
#define CLK_DEFE1_DRAM_OFF	(~(CLK_DEFE1_DRAM_ON	))
#define CLK_LCDC0_AHB_OFF	(~(CLK_LCDC0_AHB_ON	    ))
#define CLK_LCDC0_MOD0_OFF  (~(CLK_LCDC0_MOD0_ON  	))
#define CLK_LCDC0_MOD1_OFF  (~(CLK_LCDC0_MOD1_ON  	))
#define CLK_LCDC1_AHB_OFF   (~(CLK_LCDC1_AHB_ON     ))
#define CLK_LCDC1_MOD0_OFF  (~(CLK_LCDC1_MOD0_ON  	))
#define CLK_LCDC1_MOD1_OFF  (~(CLK_LCDC1_MOD1_ON  	))
#define CLK_TVENC0_AHB_OFF	(~(CLK_TVENC0_AHB_ON	))
#define CLK_TVENC1_AHB_OFF 	(~(CLK_TVENC1_AHB_ON 	))
#define CLK_HDMI_AHB_OFF    (~(CLK_HDMI_AHB_ON		))
#define CLK_HDMI_MOD_OFF 	(~(CLK_HDMI_MOD_ON 	    ))
#define CLK_DSI_AHB_OFF     (~(CLK_DSI_AHB_ON 	    ))
#define CLK_DSI_MOD_OFF     (~(CLK_DSI_MOD_ON 	    ))

//#define CLK_LVDS_MOD_OFF 	(~(CLK_LVDS_MOD_ON 		))

__hdle h_debe0ahbclk,h_debe0mclk,h_debe0dramclk;
__hdle h_debe1ahbclk,h_debe1mclk,h_debe1dramclk;
__hdle h_defe0ahbclk,h_defe0mclk,h_defe0dramclk;
__hdle h_defe1ahbclk,h_defe1mclk,h_defe1dramclk;
__hdle h_tvenc0ahbclk;
__hdle h_tvenc1ahbclk;
__hdle h_lcd0ahbclk,h_lcd0ch0mclk0,h_lcd0ch1mclk1;
__hdle h_lcd1ahbclk,h_lcd1ch0mclk0,h_lcd1ch1mclk1;
__hdle h_lvdsmclk;	//only for reset
__hdle h_dsiahbclk,h_dsimclk_s,h_dsimclk_p;
__hdle h_hdmiahbclk,h_hdmimclk,h_hdmimclk_ddc;

__u32 g_clk_status = 0x0;

#define RESET_OSAL

volatile __ccmu_mipi_pll_reg0040_t *MipiPllCtl;
volatile __ccmu_mipi_pll_bias_reg0240_t *MipipllBias;

extern __disp_dev_t         gdisp;
extern __panel_para_t		gpanel_info[2];

__disp_clk_tab clk_tab = //record tv/vga/hdmi mode clock requirement
{
//LCDx_CH1_CLK2, CLK2/CLK1,    HDMI_CLK,	   PLL_CLK	 ,     PLLX2 req	  //	TV_VGA_MODE 		//INDEX, FOLLOW enum order
	//TV mode and HDMI mode
   {{13500000	  ,	1	    , 	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_480I 			        //0x0
	{13500000	  ,	1	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_576I 			        //0x1
	{27000000	  ,	1	    ,	27000000,		270000000	,	0	},	 //    DISP_TV_MOD_480P 			        //0x2
	{27000000	  ,	1	    ,	27000000,		270000000	,	0	},	 //    DISP_TV_MOD_576P 			        //0x3
	{74250000	  ,	1	    ,	74250000,		297000000	,	0	},	 //    DISP_TV_MOD_720P_50HZ		        //0x4
	{74250000	  ,	1	    ,	74250000,		297000000	,	0	},	 //    DISP_TV_MOD_720P_60HZ		        //0x5
	{74250000	  ,	1	    ,	74250000,		297000000	,	0	},	 //    DISP_TV_MOD_1080I_50HZ	        //0x6
	{74250000	  ,	1	    ,	74250000,		297000000	,	0	},	 //    DISP_TV_MOD_1080I_60HZ	        //0x7
	{74250000	  ,	1	    ,	74250000,		297000000	,	0	},	 //    DISP_TV_MOD_1080P_24HZ	        //0x8
	{148500000	  ,	1	    ,  148500000, 		297000000	,	0	},	 //    DISP_TV_MOD_1080P_50HZ	        //0x9
	{148500000	  ,	1	    ,  148500000, 		297000000	,	0	},	 //    DISP_TV_MOD_1080P_60HZ	        //0xa
	{27000000	  ,	2	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_PAL			        //0xb
	{27000000	  ,	2	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_PAL_SVIDEO	        //0xc
	{		0	  ,	1	    ,	       0,		        0	,	0	},	 //    reserved  //0xd
	{27000000	  ,	2	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_NTSC 			        //0xe
	{27000000	  ,	2	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_NTSC_SVIDEO	        //0xf
	{		0	  ,	1	    ,	       0,		        0	,	0	},	 //    reserved    //0x10
	{27000000	  ,	2	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_PAL_M			        //0x11
	{27000000	  ,	2	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_PAL_M_SVIDEO 	        //0x12
	{		0	  ,	1	    ,	       0,		        0	,	0	},	 //    reserved   //0x13
	{27000000	  ,	2	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_PAL_NC		        //0x14
	{27000000	  ,	2	    ,	27000000,		297000000	,	0	},	 //    DISP_TV_MOD_PAL_NC_SVIDEO	        //0x15
	{		0	  ,	1	    ,	       0,		        0	,	0	},	 //    reserved  //0x16
	{148500000	  ,	1	    ,  148500000, 		297000000	,	0	},	 //    DISP_TV_MOD_1080P_24HZ_3D_FP    //0x17
	{148500000	  ,	1	    ,  148500000,		297000000	,	0	},	 //    DISP_TV_MOD_720P_50HZ_3D_FP 	 //0x18
	{148500000	  ,	1	    ,  148500000,		297000000	,	0	},	 //    DISP_TV_MOD_720P_60HZ_3D_FP 	 //0x19
	{74250000	  ,	1	    ,	74250000,		297000000	,	0	},	 //    DISP_TV_MOD_1080P_25HZ 		 //0x1a
	{74250000	  ,	1	    ,	74250000,		297000000	,	0	},	 //    DISP_TV_MOD_1080P_30HZ 		//0x1b
	{		0	  ,	1	    ,	       0,		        0	,	0	},	 //    reserved 					        //0x1c
	{		0	  ,	1	    ,	       0,		        0	,	0	}},  //    reserved 					        //0x1d
	//VGA mode
   {{147000000    , 1      ,   147000000, 		294000000   ,   0   },   //    DISP_VGA_H1680_V1050                // 0X0
	{106800000    , 1      ,   106800000, 		267000000   ,   1   },   //    DISP_VGA_H1440_V900                  // 0X1
	{ 86000000    , 1      ,	86000000,  		258000000   ,   0   },   //    DISP_VGA_H1360_V768                  // 0X2
	{108000000    , 1      ,   108000000, 		270000000   ,   1   },   //    DISP_VGA_H1280_V1024                // 0X3
	{ 65250000    , 1      ,	65250000,  		261000000   ,   0   },   //    DISP_VGA_H1024_V768                  // 0X4
	{ 39857143    , 1      ,	39857143,  		279000000 	,   0   },   //    DISP_VGA_H800_V600                   // 0X5
	{ 25090909    , 1      ,	25090909,  		276000000 	,   0   },   //    DISP_VGA_H640_V480                   // 0X6
	{        0    , 1      ,	       0,  		        0   ,   0   },   //    DISP_VGA_H1440_V900_RB           // 0X7
 	{        0    , 1      ,	       0,  		        0   ,   0   },   //    DISP_VGA_H1680_V1050_RB         // 0X8
	{138000000    , 1      ,   138000000, 		276000000   ,   0   },   //    DISP_VGA_H1920_V1080_RB         // 0X9
	{148500000    , 1      ,   148500000, 		297000000   ,   0   },   //    DISP_VGA_H1920_V1080              // 0xa
	{ 74250000	  ,	1	   ,    74250000, 		297000000	,	0	}}	 //    DISP_VGA_H1280_V720                // 0xb
	};

/*
* src_freq:   source pll frequence(hz)
* output_freq:MIPI pll frequence(hz)
* coef:       pll coefficient(n,k,m)
*/
__s32 disp_mipipll_calc_coefficient(__u32 src_freq, __u32 output_freq, __disp_ccmu_coef *coef)
{
	int m_ok=0, k_ok=0, n_ok=0;
	int output_curr = 0;
	int err_ok = output_freq;
	int m_max;
	int real_freq;
	int m,k,n;
	int err_curr;

	src_freq = src_freq / 1000000;
	output_freq = output_freq / 1000000;

	m_max = src_freq/30;

	for(m=1;m<m_max+1;m++)
		for(k=2;k<5;k++)
			for(n=1;n<17;n++)
			{
				output_curr = src_freq*n*k/m;
				if(output_curr>output_freq)
					err_curr = output_curr-output_freq;
				else
					err_curr = output_freq-output_curr;
				if(err_curr<err_ok)
				{
					m_ok = m;
					k_ok = k;
					n_ok = n;
					err_ok = err_curr;
				}
			}

    n_ok = n_ok & 0xf;
    k_ok = k_ok & 0x3;
    m_ok = m_ok & 0xf;
	coef->factor_n = n_ok;
	coef->factor_k = k_ok;
	coef->divider_m = m_ok;
	real_freq = src_freq * n_ok * k_ok / m_ok;

	return 0;
}

__s32 disp_mipipll_set_coefficient(__disp_ccmu_coef *coef)
{
    MipiPllCtl->FactorN = coef->factor_n-1;
    MipiPllCtl->FactorK = coef->factor_k-1;
    MipiPllCtl->FactorM = coef->divider_m-1;

    return 0;
}

__s32 disp_mipipll_init(void)
{
    MipiPllCtl = (__ccmu_mipi_pll_reg0040_t *)0x01c20040;
    MipipllBias = (__ccmu_mipi_pll_bias_reg0240_t *)0x01c20240;
    MipiPllCtl->PLLEn = 0;
    MipiPllCtl->Ldo1En = 0;
    MipiPllCtl->Ldo2En = 0;
    MipiPllCtl->Ldo1En = 1;
    MipiPllCtl->Ldo2En = 1;
    MipiPllCtl->PllSrc = 1; //pll7
    MipipllBias->pllvdd_ldo_out_ctrl = 0x7; //1.45v
    __msdelay(2);
    MipiPllCtl->PLLEn = 1;

    return 0;
}

__s32 disp_mipipll_enable(__u32 en)
{
    if(en)
    {
        MipiPllCtl->PLLEn = 1;
    }
    else
    {
        MipiPllCtl->PLLEn = 0;
    }

    return 0;
}
__s32 image_clk_init(__u32 sel)
{
	__u32 pll_freq;
    __u32 mclk_div;

    DE_INF("image%d clk_init, pll9\n", sel);
	if(sel == 0)
	{
		h_debe0ahbclk = OSAL_CCMU_OpenMclk(AHB_CLK_DEBE0);
		h_debe0mclk = OSAL_CCMU_OpenMclk(MOD_CLK_DEBE0);
		h_debe0dramclk = OSAL_CCMU_OpenMclk(DRAM_CLK_DEBE0);

		//NEW OSAL_clk reset
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_debe0mclk, RST_INVAILD);
#endif
		OSAL_CCMU_SetMclkSrc(h_debe0mclk, SYS_CLK_PLL10);

		pll_freq = OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL10);
        mclk_div = 1;
        while((pll_freq / mclk_div) > 300000000)
        {
            mclk_div ++;
        }
		OSAL_CCMU_SetMclkDiv(h_debe0mclk, mclk_div);

		OSAL_CCMU_MclkOnOff(h_debe0ahbclk, CLK_ON);
		//OSAL_CCMU_MclkOnOff(h_debe0dramclk, CLK_ON);
		//OSAL_CCMU_MclkOnOff(h_debe0dramclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_debe0mclk, CLK_ON);

		g_clk_status |= (CLK_DEBE0_AHB_ON | CLK_DEBE0_MOD_ON);
	}
	else if(sel == 1)
	{
		h_debe1ahbclk = OSAL_CCMU_OpenMclk(AHB_CLK_DEBE1);
		h_debe1mclk = OSAL_CCMU_OpenMclk(MOD_CLK_DEBE1);
		h_debe1dramclk = OSAL_CCMU_OpenMclk(DRAM_CLK_DEBE1);
#ifdef RESET_OSAL

        OSAL_CCMU_MclkReset(h_debe1mclk, RST_INVAILD);
#endif
        OSAL_CCMU_SetMclkSrc(h_debe1mclk, SYS_CLK_PLL10);//FIX CONNECT TO PLL10

		pll_freq = OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL10);
        mclk_div = 1;
        while((pll_freq / mclk_div) > 300000000)
        {
            mclk_div ++;
        }
		OSAL_CCMU_SetMclkDiv(h_debe1mclk, mclk_div);

		OSAL_CCMU_MclkOnOff(h_debe1ahbclk, CLK_ON);
		//OSAL_CCMU_MclkOnOff(h_debe1dramclk, CLK_ON);
		//OSAL_CCMU_MclkOnOff(h_debe1dramclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_debe1mclk, CLK_ON);

		g_clk_status |= (CLK_DEBE1_AHB_ON | CLK_DEBE1_MOD_ON);
	}
	return DIS_SUCCESS;

}


__s32 image_clk_exit(__u32 sel)
{
	if(sel == 0)
	{
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_debe0mclk, RST_VAILD);
#endif
		OSAL_CCMU_MclkOnOff(h_debe0ahbclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_debe0dramclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_debe0mclk, CLK_OFF);
		OSAL_CCMU_CloseMclk(h_debe0ahbclk);
		OSAL_CCMU_CloseMclk(h_debe0dramclk);
		OSAL_CCMU_CloseMclk(h_debe0mclk);

		g_clk_status &= (CLK_DEBE0_AHB_OFF & CLK_DEBE0_MOD_OFF & CLK_DEBE0_DRAM_OFF);
	}
	else if(sel == 1)
	{
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_debe1mclk, RST_VAILD);
#endif
		OSAL_CCMU_MclkOnOff(h_debe1ahbclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_debe1dramclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_debe1mclk, CLK_OFF);
		OSAL_CCMU_CloseMclk(h_debe1ahbclk);
		OSAL_CCMU_CloseMclk(h_debe1dramclk);
		OSAL_CCMU_CloseMclk(h_debe1mclk);

		g_clk_status &= (CLK_DEBE1_AHB_OFF & CLK_DEBE1_MOD_OFF & CLK_DEBE1_DRAM_OFF);
	}

	return DIS_SUCCESS;
}

__s32 image_clk_on(__u32 sel)
{
	if(sel == 0)
	{
		//need to comfirm : REGisters can be accessed if  be_mclk was close.
		OSAL_CCMU_MclkOnOff(h_debe0dramclk, CLK_ON);
		g_clk_status |= CLK_DEBE0_DRAM_ON;
	}
	else if(sel == 1)
	{
		OSAL_CCMU_MclkOnOff(h_debe1dramclk, CLK_ON);
		g_clk_status |= CLK_DEBE1_DRAM_ON;
	}
	return	DIS_SUCCESS;
}

__s32 image_clk_off(__u32 sel)
{
	if(sel == 0)
	{
		OSAL_CCMU_MclkOnOff(h_debe0dramclk, CLK_OFF);
		g_clk_status &= CLK_DEBE0_DRAM_OFF;
	}
	else if(sel == 1)
	{
		OSAL_CCMU_MclkOnOff(h_debe1dramclk, CLK_OFF);
		g_clk_status &= CLK_DEBE1_DRAM_OFF;
	}
	return	DIS_SUCCESS;
}

__s32 scaler_clk_init(__u32 sel)
{
    __u32 pll_freq;
    __u32 mclk_div;

    DE_INF("scaler %d clk init\n", sel);
	if(sel == 0)
	{
		h_defe0ahbclk = OSAL_CCMU_OpenMclk(AHB_CLK_DEFE0);
		h_defe0dramclk = OSAL_CCMU_OpenMclk(DRAM_CLK_DEFE0);
		h_defe0mclk = OSAL_CCMU_OpenMclk(MOD_CLK_DEFE0);
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_defe0mclk, RST_INVAILD);
#endif

		OSAL_CCMU_SetMclkSrc(h_defe0mclk, SYS_CLK_PLL10);	//FIX CONNECT TO  PLL10
		pll_freq = OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL10);
        mclk_div = 1;
        while((pll_freq / mclk_div) > 300000000)
        {
            mclk_div ++;
        }
		OSAL_CCMU_SetMclkDiv(h_defe0mclk, mclk_div);

		OSAL_CCMU_MclkOnOff(h_defe0ahbclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_defe0mclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_defe0mclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_defe0dramclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_defe0dramclk, CLK_OFF);

		g_clk_status |= CLK_DEFE0_AHB_ON;
	}
	else if(sel == 1)
	{
		h_defe1ahbclk = OSAL_CCMU_OpenMclk(AHB_CLK_DEFE1);
		h_defe1dramclk = OSAL_CCMU_OpenMclk(DRAM_CLK_DEFE1);
		h_defe1mclk = OSAL_CCMU_OpenMclk(MOD_CLK_DEFE1);
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_defe1mclk, RST_INVAILD);
#endif
		OSAL_CCMU_SetMclkSrc(h_defe1mclk, SYS_CLK_PLL10);	//FIX CONNECT PLL10
		pll_freq = OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL10);
        mclk_div = 1;
        while((pll_freq / mclk_div) > 300000000)
        {
            mclk_div ++;
        }
		OSAL_CCMU_SetMclkDiv(h_defe1mclk, mclk_div);

		OSAL_CCMU_MclkOnOff(h_defe1ahbclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_defe1mclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_defe1mclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_defe1dramclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_defe1dramclk, CLK_OFF);

		g_clk_status |= CLK_DEFE1_AHB_ON;
	}
		return DIS_SUCCESS;
}

__s32 scaler_clk_exit(__u32 sel)
{
	if(sel == 0)
	{
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_defe0mclk, RST_VAILD);
#endif
		OSAL_CCMU_MclkOnOff(h_defe0ahbclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_defe0dramclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_defe0mclk, CLK_OFF);
		OSAL_CCMU_CloseMclk(h_defe0ahbclk);
		OSAL_CCMU_CloseMclk(h_defe0dramclk);
		OSAL_CCMU_CloseMclk(h_defe0mclk);

		g_clk_status &= (CLK_DEFE0_AHB_OFF & CLK_DEFE0_MOD_OFF & CLK_DEFE0_DRAM_OFF);

	}
	else if(sel == 1)
	{
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_defe1mclk, RST_VAILD);
#endif
		OSAL_CCMU_MclkOnOff(h_defe1ahbclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_defe1dramclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_defe1mclk, CLK_OFF);
		OSAL_CCMU_CloseMclk(h_defe1ahbclk);
		OSAL_CCMU_CloseMclk(h_defe1dramclk);
		OSAL_CCMU_CloseMclk(h_defe1mclk);

		g_clk_status &= (CLK_DEFE1_AHB_OFF & CLK_DEFE1_MOD_OFF & CLK_DEFE1_DRAM_OFF);
	}

	return DIS_SUCCESS;
}

__s32 scaler_clk_on(__u32 sel)
{
	if(sel == 0)
	{
		OSAL_CCMU_MclkOnOff(h_defe0mclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_defe0dramclk, CLK_ON);

		g_clk_status |= ( CLK_DEFE0_MOD_ON | CLK_DEFE0_DRAM_ON);
	}
	else if(sel == 1)
	{
		OSAL_CCMU_MclkOnOff(h_defe1mclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_defe1dramclk, CLK_ON);

		g_clk_status |= ( CLK_DEFE1_MOD_ON | CLK_DEFE1_DRAM_ON);
	}
	return	DIS_SUCCESS;

}

__s32 scaler_clk_off(__u32 sel)
{
	if(sel == 0)
	{
		OSAL_CCMU_MclkOnOff(h_defe0mclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_defe0dramclk, CLK_OFF);

		g_clk_status &= ( CLK_DEFE0_MOD_OFF & CLK_DEFE0_DRAM_OFF);
	}
	else if(sel == 1)
	{
		OSAL_CCMU_MclkOnOff(h_defe1mclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_defe1dramclk, CLK_OFF);

		g_clk_status &= ( CLK_DEFE1_MOD_OFF & CLK_DEFE1_DRAM_OFF);
	}
	return	DIS_SUCCESS;

}

__s32 lcdc_clk_init(__u32 sel)
{
    DE_INF("lcd %d clk init\n", sel);
	if(sel == 0)
	{
		h_lcd0ahbclk   = OSAL_CCMU_OpenMclk(AHB_CLK_LCD0);
		h_lcd0ch0mclk0 = OSAL_CCMU_OpenMclk(MOD_CLK_LCD0CH0);
		h_lcd0ch1mclk1 = OSAL_CCMU_OpenMclk(MOD_CLK_LCD0CH1);

        OSAL_CCMU_SetMclkSrc(h_lcd0ch0mclk0, SYS_CLK_MIPIPLL);
		OSAL_CCMU_SetMclkSrc(h_lcd0ch1mclk1, SYS_CLK_PLL7);	//Default to Video  Pll1
		OSAL_CCMU_SetMclkDiv(h_lcd0ch1mclk1, 10);
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_lcd0ch0mclk0, RST_INVAILD);
#endif
		OSAL_CCMU_MclkOnOff(h_lcd0ahbclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_lcd0ch0mclk0, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_lcd0ch0mclk0, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_lcd0ch1mclk1, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_lcd0ch1mclk1, CLK_OFF);

		g_clk_status |= CLK_LCDC0_AHB_ON;
	}
	else if(sel == 1)
	{
		h_lcd1ahbclk   = OSAL_CCMU_OpenMclk(AHB_CLK_LCD1);
		h_lcd1ch0mclk0 = OSAL_CCMU_OpenMclk(MOD_CLK_LCD1CH0);
		h_lcd1ch1mclk1 = OSAL_CCMU_OpenMclk(MOD_CLK_LCD1CH1);

		OSAL_CCMU_SetMclkSrc(h_lcd1ch0mclk0, SYS_CLK_MIPIPLL);
		OSAL_CCMU_SetMclkSrc(h_lcd1ch1mclk1, SYS_CLK_PLL7);	//Default to Video  Pll1
		OSAL_CCMU_SetMclkDiv(h_lcd1ch1mclk1, 10);
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_lcd1ch0mclk0, RST_INVAILD);
#endif
		OSAL_CCMU_MclkOnOff(h_lcd1ahbclk, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_lcd1ch0mclk0, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_lcd1ch0mclk0, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_lcd1ch1mclk1, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_lcd1ch1mclk1, CLK_OFF);

		g_clk_status |= CLK_LCDC1_AHB_ON;
	}
	return DIS_SUCCESS;

}

__s32 lcdc_clk_exit(__u32 sel)
{
	if(sel == 0)
	{
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_lcd0ch0mclk0, RST_VAILD);
#endif
		OSAL_CCMU_MclkOnOff(h_lcd0ahbclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_lcd0ch0mclk0, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_lcd0ch1mclk1, CLK_OFF);
		OSAL_CCMU_CloseMclk(h_lcd0ahbclk);
		OSAL_CCMU_CloseMclk(h_lcd0ch0mclk0);
		OSAL_CCMU_CloseMclk(h_lcd0ch1mclk1);

		g_clk_status &= (CLK_LCDC0_AHB_OFF & CLK_LCDC0_MOD0_OFF & CLK_LCDC0_MOD1_OFF);
	}
	else if(sel == 1)
	{
#ifdef RESET_OSAL
		OSAL_CCMU_MclkReset(h_lcd1ch0mclk0, RST_VAILD);
#endif
		OSAL_CCMU_MclkOnOff(h_lcd1ahbclk, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_lcd1ch0mclk0, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_lcd1ch1mclk1, CLK_OFF);
		OSAL_CCMU_CloseMclk(h_lcd1ahbclk);
		OSAL_CCMU_CloseMclk(h_lcd1ch0mclk0);
		OSAL_CCMU_CloseMclk(h_lcd1ch1mclk1);

		g_clk_status &= (CLK_LCDC1_AHB_OFF & CLK_LCDC1_MOD0_OFF & CLK_LCDC1_MOD1_OFF);
	}
	return DIS_SUCCESS;
}

__s32 lcdc_clk_on(__u32 sel)
{
	if(sel == 0)
	{
		OSAL_CCMU_MclkOnOff(h_lcd0ch0mclk0, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_lcd0ch1mclk1, CLK_ON);

		g_clk_status |= (CLK_LCDC0_MOD0_ON | CLK_LCDC0_MOD1_ON);
	}
	else if(sel == 1)
	{
		OSAL_CCMU_MclkOnOff(h_lcd1ch0mclk0, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_lcd1ch1mclk1, CLK_ON);

		g_clk_status |= (CLK_LCDC1_MOD0_ON | CLK_LCDC1_MOD1_ON);
	}
	return	DIS_SUCCESS;

}

__s32 lcdc_clk_off(__u32 sel)
{
	if(sel == 0)
	{
		OSAL_CCMU_MclkOnOff(h_lcd0ch0mclk0, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_lcd0ch1mclk1, CLK_OFF);

		g_clk_status &= (CLK_LCDC0_MOD0_OFF & CLK_LCDC0_MOD1_OFF);
	}
	else if(sel == 1)
	{
		OSAL_CCMU_MclkOnOff(h_lcd1ch0mclk0, CLK_OFF);
		OSAL_CCMU_MclkOnOff(h_lcd1ch1mclk1, CLK_OFF);

		g_clk_status &= (CLK_LCDC1_MOD0_OFF & CLK_LCDC1_MOD1_OFF);
	}
	return	DIS_SUCCESS;

}

__s32 tve_clk_init(__u32 sel)
{
	DE_INF("tve %d clk init\n",sel);
    if(sel == 0)
	{
		h_tvenc0ahbclk = OSAL_CCMU_OpenMclk(AHB_CLK_TVE0);
		OSAL_CCMU_MclkOnOff(h_tvenc0ahbclk, CLK_ON);

		g_clk_status |= CLK_TVENC0_AHB_ON;
	}
	else if(sel == 1)
	{
		h_tvenc1ahbclk = OSAL_CCMU_OpenMclk(AHB_CLK_TVE1);
		OSAL_CCMU_MclkOnOff(h_tvenc1ahbclk, CLK_ON);

		g_clk_status |= CLK_TVENC1_AHB_ON;
	}
	return DIS_SUCCESS;
}


__s32 tve_clk_exit(__u32 sel)
{
	if(sel == 0)
	{
		OSAL_CCMU_MclkOnOff(h_tvenc0ahbclk, CLK_OFF);
		OSAL_CCMU_CloseMclk(h_tvenc0ahbclk);

		g_clk_status &= CLK_TVENC0_AHB_OFF;
	}
	else if(sel == 1)
	{
		OSAL_CCMU_MclkOnOff(h_tvenc1ahbclk, CLK_OFF);
		OSAL_CCMU_CloseMclk(h_tvenc1ahbclk);

		g_clk_status &= CLK_TVENC1_AHB_OFF;
	}
	return DIS_SUCCESS;
}

__s32 tve_clk_on(__u32 sel)
{
	return DIS_SUCCESS;
}

__s32 tve_clk_off(__u32 sel)
{
	return DIS_SUCCESS;
}

__s32 hdmi_clk_init(void)
{
	DE_INF("hdmi clk init\n");
    h_hdmiahbclk = OSAL_CCMU_OpenMclk(AHB_CLK_HDMI);
	h_hdmimclk   = OSAL_CCMU_OpenMclk(MOD_CLK_HDMI);
    h_hdmimclk_ddc = OSAL_CCMU_OpenMclk(MOD_CLK_HDMI_DDC);
#ifdef RESET_OSAL
	OSAL_CCMU_MclkReset(h_hdmimclk, RST_INVAILD);
#endif
	OSAL_CCMU_SetMclkSrc(h_hdmimclk, SYS_CLK_PLL7);
	OSAL_CCMU_SetMclkDiv(h_hdmimclk, 1);

	OSAL_CCMU_MclkOnOff(h_hdmiahbclk, CLK_ON);
    OSAL_CCMU_MclkOnOff(h_hdmimclk_ddc, CLK_ON);
	g_clk_status |= CLK_HDMI_AHB_ON | CLK_HDMI_MOD_DDC_ON;

	return DIS_SUCCESS;
}

__s32 hdmi_clk_exit(void)
{
#ifdef RESET_OSAL
	OSAL_CCMU_MclkReset(h_hdmimclk, RST_VAILD);
#endif
	OSAL_CCMU_MclkOnOff(h_hdmimclk, CLK_OFF);
	OSAL_CCMU_MclkOnOff(h_hdmiahbclk, CLK_OFF);
	OSAL_CCMU_CloseMclk(h_hdmiahbclk);
	OSAL_CCMU_CloseMclk(h_hdmimclk);

	g_clk_status &= (CLK_HDMI_AHB_OFF & CLK_HDMI_MOD_OFF);

	return DIS_SUCCESS;
}

__s32 hdmi_clk_on(void)
{
	OSAL_CCMU_MclkOnOff(h_hdmimclk, CLK_ON);

	g_clk_status |= CLK_HDMI_MOD_ON;

	return DIS_SUCCESS;
}

__s32 hdmi_clk_off(void)
{
	OSAL_CCMU_MclkOnOff(h_hdmimclk, CLK_OFF);

	g_clk_status &= CLK_HDMI_MOD_OFF;

	return DIS_SUCCESS;
}

__s32 lvds_clk_init(void)
{
	DE_INF("lvds clk init \n");
    h_lvdsmclk = OSAL_CCMU_OpenMclk(MOD_CLK_LVDS);
#ifdef RESET_OSAL
	OSAL_CCMU_MclkReset(h_lvdsmclk, RST_INVAILD);
#endif
	return DIS_SUCCESS;
}

__s32 lvds_clk_exit(void)
{
#ifdef RESET_OSAL
	OSAL_CCMU_MclkReset(h_lvdsmclk, RST_VAILD);
#endif
	OSAL_CCMU_CloseMclk(MOD_CLK_LVDS);

	return DIS_SUCCESS;
}

__s32 lvds_clk_on(void)
{
	return DIS_SUCCESS;
}

__s32 lvds_clk_off(void)
{
	return DIS_SUCCESS;
}

__s32 dsi_clk_init(void)
{
    DE_INF("dsi clk init\n");
    h_dsiahbclk= OSAL_CCMU_OpenMclk(AHB_CLK_MIPIDSI);
    h_dsimclk_s= OSAL_CCMU_OpenMclk(MOD_CLK_MIPIDSIS);
    h_dsimclk_p = OSAL_CCMU_OpenMclk(MOD_CLK_MIPIDSIP);

    OSAL_CCMU_SetMclkSrc(h_dsimclk_s, SYS_CLK_PLL7);
    OSAL_CCMU_SetMclkDiv(h_dsimclk_s, 1);
    OSAL_CCMU_SetMclkSrc(h_dsimclk_p, SYS_CLK_PLL7);
    OSAL_CCMU_SetMclkDiv(h_dsimclk_p, 2);
#ifdef RESET_OSAL
    OSAL_CCMU_MclkReset(h_dsimclk_s, RST_INVAILD);
    OSAL_CCMU_MclkReset(h_dsimclk_p, RST_INVAILD);
#endif
    OSAL_CCMU_MclkOnOff(h_dsiahbclk, CLK_ON);
    OSAL_CCMU_MclkOnOff(h_dsimclk_s, CLK_ON);
    OSAL_CCMU_MclkOnOff(h_dsimclk_s, CLK_OFF);
    OSAL_CCMU_MclkOnOff(h_dsimclk_p, CLK_ON);
    OSAL_CCMU_MclkOnOff(h_dsimclk_p, CLK_OFF);

    g_clk_status |= CLK_DSI_AHB_ON;

	return DIS_SUCCESS;
}

__s32 dsi_clk_exit(void)
{
#ifdef RESET_OSAL
    OSAL_CCMU_MclkReset(h_dsimclk_s, RST_VAILD);
#endif
    OSAL_CCMU_MclkOnOff(h_dsiahbclk, CLK_OFF);
    OSAL_CCMU_MclkOnOff(h_dsimclk_s, CLK_OFF);
    OSAL_CCMU_MclkOnOff(h_dsimclk_p, CLK_OFF);
    OSAL_CCMU_CloseMclk(h_dsiahbclk);
    OSAL_CCMU_CloseMclk(h_dsimclk_s);
    OSAL_CCMU_CloseMclk(h_dsimclk_p);

    g_clk_status &= (CLK_DSI_AHB_OFF & CLK_DSI_MOD_OFF);

	return DIS_SUCCESS;
}

__s32 dsi_clk_on(void)
{
	OSAL_CCMU_MclkOnOff(h_dsimclk_s, CLK_ON);
	OSAL_CCMU_MclkOnOff(h_dsimclk_p, CLK_ON);

	g_clk_status |= CLK_DSI_MOD_ON;

	return	DIS_SUCCESS;
}

__s32 dsi_clk_off(void)
{
	OSAL_CCMU_MclkOnOff(h_dsimclk_s, CLK_OFF);
	OSAL_CCMU_MclkOnOff(h_dsimclk_p, CLK_OFF);

	g_clk_status &= CLK_DSI_MOD_OFF;

	return	DIS_SUCCESS;
}

__s32 disp_pll_init(void)
{
	OSAL_CCMU_SetSrcFreq(SYS_CLK_PLL3, 297000000);
	OSAL_CCMU_SetSrcFreq(SYS_CLK_PLL7, 297000000);
    OSAL_CCMU_SetSrcFreq(SYS_CLK_PLL10,264000000);
    disp_mipipll_init();

	return DIS_SUCCESS;
}

/*
*********************************************************************************************************
*							LCD_PLL_Calc
*
* Description  :  Calculate PLL frequence and divider depend on all kinds of lcd panel
*
* Arguments   :  sel	<display channel>
*                            info   <panel information>
*                            divider   <divider pointer>
*
* Returns         : success	<frequence of pll >
*                            fail               <-1>
*
* Note               : 1.support hv/cpu/ttl panels which pixel frequence between 2MHz~297MHz
*                            2.support all lvds panels, when pll can't reach  (pixel clk x7),
*			    set pll to 381MHz(pllx1), which will depress the frame rate.
*********************************************************************************************************
*/
static __s32 LCD_PLL_Calc(__u32 sel, __panel_para_t * info, __u32 *divider)
{
	__u32 lcd_dclk_freq;	//Hz
	__s32 pll_freq = -1;

	lcd_dclk_freq = info->lcd_dclk_freq * 1000000;
	if (info->lcd_if == LCD_IF_HV || info->lcd_if == LCD_IF_CPU ||  (info->lcd_if == LCD_IF_EDP))// hv panel , CPU panel and	ttl panel
	{
         *divider = 6;
          pll_freq = lcd_dclk_freq * (*divider);
	}
	else if(info->lcd_if == LCD_IF_LVDS) // lvds panel
	{
	    *divider = 7;
         pll_freq = lcd_dclk_freq * (*divider);
	}
	else if(info->lcd_if == LCD_IF_DSI)//todo ?
	{
		__u32 lane = info->lcd_dsi_lane;
        __u32 bitwidth = 0;

        switch(info->lcd_dsi_format)
        {
            case LCD_DSI_FORMAT_RGB888:
                bitwidth = 24;
                break;
            case LCD_DSI_FORMAT_RGB666:
                bitwidth = 24;
                break;
            case LCD_DSI_FORMAT_RGB565:
                bitwidth = 16;
                break;
            case LCD_DSI_FORMAT_RGB666P://todo?
                bitwidth = 18;
                break;
        }

		*divider = bitwidth/lane;
        pll_freq = lcd_dclk_freq * (*divider);
	}
	return pll_freq;
}

/*
*********************************************************************************************************
*							disp_pll_assign
*
* Description  :  Select a video pll for the display device under configuration by specific rules
*
* Arguments   :  sel	<display channel>
*                            pll_clk   <required pll frequency of this display device >
*
* Returns         : success	<0:video pll0; 1:video pll1; 2:sata pll>
*                            fail               <-1>
*
* Note               : ASSIGNMENT RULES
*                            RULE1. video pll1(1x) work between [250,300]MHz, when no lcdc using video pll1 and required freq is in [250,300]MHz, choose video pll1;
*                            RULE2. when video pll1 used by another lcdc, but running frequency is equal to required frequency, choose video pll1;
*                            RULE3. when video pll1 used by another lcdc, and running frequency isNOT equal to required frequency, choose video pll0;
*                           	CONDICTION CAN'T BE HANDLE
*                            1.two lvds panel are both require a pll freq outside [250,300], and pll freq are different, the second panel will fail to assign.
*
*********************************************************************************************************
*/
static __s32 disp_pll_assign(__u32 sel, __u32 pll_clk)
{
	__u32 another_lcdc, another_pll_use_status;
	__s32 ret = -1;

	another_lcdc = (sel == 0)? 1:0;
	another_pll_use_status = gdisp.screen[another_lcdc].pll_use_status;

	if(pll_clk ==  297000000)
	{
		if((!(another_pll_use_status & VIDEO_PLL1_USED)) || (OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL7) == pll_clk))
		{
			ret = 1;
		}
		else if((!(another_pll_use_status & VIDEO_PLL0_USED)) || (OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL3) == pll_clk))
		{
			ret = 0;
		}
	}
	else if(pll_clk <= (600000000 * 2))
	{
		if((!(another_pll_use_status & VIDEO_PLL0_USED)) || (OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL3) == pll_clk))
		{
			ret = 0;
		}
		else if((!(another_pll_use_status & VIDEO_PLL1_USED)) || (OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL7) == pll_clk))
		{
			ret = 1;
		}
    }
#if 0
    else if(pll_clk <= (600000000 * 4))
    {
        ret = 2; //MIPI PLL
    }
#endif

    if(ret == -1)
    {
        DE_WRN("Can't assign PLL for screen%d, pll_clk:%d\n",sel, pll_clk);
    }

    DE_INF("====disp_pll_assign====: sel:%d,pll_clk:%d,pll_sel:%d\n", sel, pll_clk, ret);

    return ret;
}


/*
*********************************************************************************************************
*							disp_pll_set
*
* Description  :  Set clock control module
*
* Arguments   :  sel	<display channel>
*                            videopll_sel   	<sel pll>
*			pll_freq		<sel pll freq>
*			tve_freq		<lcdx_ch1_clk2 freq>
*			pre_scale		<lcdx_ch1_clk2/lcdx_ch1_ch1>
*			lcd_clk_div	<lcd panel clk div>
*			hdmi_freq		<hdmi module clk freq>
*			pll_2x		<pll 2x required>
*			type          	   	<display device type: tv/vga/hdmi/lcd>
*
* Returns         : success	<DIS_SUCCESS>
*                            fail               <>
*
* Note               :  none

*
*********************************************************************************************************
*/

static __s32 disp_pll_set(__u32 sel, __s32 videopll_sel, __u32 pll_freq, __u32 tve_freq, __s32 pre_scale,
					__u32 lcd_clk_div, __u32 hdmi_freq, __u32 pll_2x, __u32 type)
{
	__u32 videopll;
	__hdle h_lcdmclk0, h_lcdmclk1;
	__s32 pll_2x_req;
	__u32 lcdmclk1_div, hdmiclk_div;

	if(type == DISP_OUTPUT_TYPE_LCD)	//lcd panel
	{
	    if(videopll_sel == 2)//mipi pll
	    {
	        videopll = SYS_CLK_MIPIPLL;
	        //pll_freq = ((pll_freq + 12000000)/ 24000000) * 24000000;
	        //OSAL_CCMU_SetSrcFreq(AW_SYS_CLK_PLL6, pll_freq);
	    }
	    else//video pll0 or video pll1
	    {
    		pll_2x_req = (pll_freq>600000000)?1:0;
    		if(pll_2x_req)
    		{
    		    pll_freq /= 2;
    		}

	        //in 3M unit
	    	pll_freq = (pll_freq + 1500000)/3000000;
			pll_freq = pll_freq * 3000000;

    		videopll = 	(videopll_sel == 0)?SYS_CLK_PLL3:SYS_CLK_PLL7;
    		OSAL_CCMU_SetSrcFreq(videopll,pll_freq);
    		if(pll_2x_req)
    		{
                videopll = (videopll == SYS_CLK_PLL3)?SYS_CLK_PLL3X2:SYS_CLK_PLL7X2;
    		}
		}

		if(gpanel_info[sel].tcon_index == 0)	//tcon0 drive lcd panel
		{
			h_lcdmclk0 = (sel == 0)?h_lcd0ch0mclk0 : h_lcd1ch0mclk0;
			OSAL_CCMU_SetMclkSrc(h_lcdmclk0, videopll);
			tcon0_set_dclk_div(sel,lcd_clk_div);
            if(gpanel_info[sel].lcd_if == LCD_IF_DSI)//todo?  --dphy clk fix to 297/2M
            {
                OSAL_CCMU_SetMclkSrc(h_dsimclk_s, videopll);
                OSAL_CCMU_SetMclkDiv(h_dsimclk_s, 1);
                OSAL_CCMU_SetMclkSrc(h_dsimclk_p, SYS_CLK_PLL7);
                OSAL_CCMU_SetMclkDiv(h_dsimclk_p, 1);
            }
		}
		else									//tcon1 drive lcd panel
		{
			h_lcdmclk1 = (sel == 0)?h_lcd0ch1mclk1 : h_lcd1ch1mclk1;
			OSAL_CCMU_SetMclkSrc(h_lcdmclk1, videopll);
			OSAL_CCMU_SetMclkDiv(h_lcdmclk1, lcd_clk_div);
		}
	}
	else //tv/vga/hdmi
	{
	    __u32 pll_freq_used;

		pll_2x_req = pll_2x;
		videopll = 	(videopll_sel == 0)?SYS_CLK_PLL3:SYS_CLK_PLL7;
		OSAL_CCMU_SetSrcFreq(videopll,pll_freq);	//Set related Video Pll Frequency

		videopll = 	(videopll_sel == 0)?
			   		((pll_2x_req)?SYS_CLK_PLL3X2: SYS_CLK_PLL3):
					((pll_2x_req)?SYS_CLK_PLL7X2: SYS_CLK_PLL7);

		pll_freq_used = pll_freq * (pll_2x_req + 1);

		lcdmclk1_div = (pll_freq_used + (tve_freq / 2)) / tve_freq;
		hdmiclk_div = (pll_freq_used + (hdmi_freq / 2)) / hdmi_freq;

		h_lcdmclk1 = (sel == 0)?h_lcd0ch1mclk1 : h_lcd1ch1mclk1;
		OSAL_CCMU_SetMclkSrc(h_lcdmclk1, videopll);
		OSAL_CCMU_SetMclkDiv(h_lcdmclk1, lcdmclk1_div);

		if(type == DISP_OUTPUT_TYPE_HDMI && gdisp.screen[sel].hdmi_index == 0)	//hdmi internal mode
		{
			OSAL_CCMU_SetMclkSrc(h_hdmimclk, videopll);
			OSAL_CCMU_SetMclkDiv(h_hdmimclk, hdmiclk_div);

            if(gdisp.init_para.hdmi_set_pll != NULL)
            {
    			if((videopll == SYS_CLK_PLL3X2) || (videopll == SYS_CLK_PLL3))
    			{
    			    gdisp.init_para.hdmi_set_pll(0, pll_freq);
    			}
    			else
    			{
    			    gdisp.init_para.hdmi_set_pll(1, pll_freq);
    			}
			}
			else
			{
			    DE_WRN("gdisp.init_para.hdmi_set_pll is NULL\n");
			}
		}
	}

	return DIS_SUCCESS;
}

/*
*********************************************************************************************************
*							disp_clk_cfg
*
* Description  :  Config PLL and mclk depend on all kinds of display devices
*
* Arguments   :  sel	<display channel>
*                            type   <display device type: tv/vga/hdmi/lcd>
*                            mode   <display mode of tv/vga/hdmi: 480i, ntsc...>
*
* Returns         : success	<DIS_SUCCESS>
*                            fail               <DIS_FAIL>
*
* Note               : None.
*
*********************************************************************************************************
*/
__s32 disp_clk_cfg(__u32 sel, __u32 type, __u8 mode)
{	__u32 pll_freq = 297000000, tve_freq = 27000000;
	__u32 hdmi_freq = 74250000;
	__s32 videopll_sel, pre_scale = 1;
	__u32 lcd_clk_div = 0;
	__u32 pll_2x = 0;

	if(type == DISP_OUTPUT_TYPE_TV || type == DISP_OUTPUT_TYPE_HDMI)
	{
		pll_freq = clk_tab.tv_clk_tab[mode].pll_clk;
		tve_freq = clk_tab.tv_clk_tab[mode].tve_clk;
		pre_scale = clk_tab.tv_clk_tab[mode].pre_scale;
		hdmi_freq = clk_tab.tv_clk_tab[mode].hdmi_clk;
		pll_2x = clk_tab.tv_clk_tab[mode].pll_2x;
	}
	else if(type == DISP_OUTPUT_TYPE_VGA)
	{
		pll_freq = clk_tab.vga_clk_tab[mode].pll_clk;
		tve_freq = clk_tab.vga_clk_tab[mode].tve_clk;
		pre_scale = clk_tab.vga_clk_tab[mode].pre_scale;
		pll_2x = clk_tab.vga_clk_tab[mode].pll_2x;
	}
	else if(type == DISP_OUTPUT_TYPE_LCD)
	{
        __disp_ccmu_coef coef;

        pll_freq = LCD_PLL_Calc(sel, (__panel_para_t*)&gpanel_info[sel], &lcd_clk_div);
		pre_scale = 1;
        tcon0_set_dclk_div(sel,lcd_clk_div);

        disp_mipipll_calc_coefficient(297000000, pll_freq, &coef);
        disp_mipipll_set_coefficient(&coef);

        return DIS_SUCCESS;
	}
	else
	{
		return DIS_SUCCESS;
	}

	if ( (videopll_sel = disp_pll_assign(sel, pll_freq)) == -1)
	{
	    DE_WRN("===pll assign fail====\n");
		return DIS_FAIL;
	}

	disp_pll_set(sel, videopll_sel, pll_freq, tve_freq, pre_scale, lcd_clk_div, hdmi_freq, pll_2x, type);
	if(videopll_sel == 0)
	{
	    gdisp.screen[sel].pll_use_status |= VIDEO_PLL0_USED;
	}
	else if(videopll_sel == 1)
	{
	    gdisp.screen[sel].pll_use_status |= VIDEO_PLL1_USED;
	}

	return DIS_SUCCESS;
}

//type==1: open ahb clk and image mclk
//type==2: open all clk except ahb clk and image mclk
//type==3: open all clk
__s32 BSP_disp_clk_on(__u32 type)
{
    if(type & 1)
    {
//AHB CLK
    	if((g_clk_status & CLK_DEFE0_AHB_ON) == CLK_DEFE0_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe0ahbclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_DEFE1_AHB_ON) == CLK_DEFE1_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe1ahbclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_DEBE0_AHB_ON) == CLK_DEBE0_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe0ahbclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_DEBE1_AHB_ON) == CLK_DEBE1_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe1ahbclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_LCDC0_AHB_ON) == CLK_LCDC0_AHB_ON)	//OK?? REG wont clear?
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd0ahbclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_LCDC1_AHB_ON) == CLK_LCDC1_AHB_ON)	//OK?? REG wont clear?
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd1ahbclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_HDMI_AHB_ON) == CLK_HDMI_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_hdmiahbclk, CLK_ON);
    	}
        if((g_clk_status & CLK_DSI_AHB_ON) == CLK_DSI_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_dsiahbclk, CLK_ON);
    	}
    	//OSAL_CCMU_MclkOnOff(h_tveahbclk, CLK_ON);

//MODULE CLK
    	if((g_clk_status & CLK_DEBE0_MOD_ON) == CLK_DEBE0_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe0mclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_DEBE1_MOD_ON) == CLK_DEBE1_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe1mclk, CLK_ON);
    	}
	}

	if(type & 2)
	{
//DRAM CLK
    	if((g_clk_status & CLK_DEFE0_DRAM_ON) == CLK_DEFE0_DRAM_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe0dramclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_DEFE1_DRAM_ON) == CLK_DEFE1_DRAM_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe1dramclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_DEBE0_DRAM_ON) == CLK_DEBE0_DRAM_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe0dramclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_DEBE1_DRAM_ON) == CLK_DEBE1_DRAM_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe1dramclk, CLK_ON);
    	}

//MODULE CLK
    	if((g_clk_status & CLK_DEFE0_MOD_ON) == CLK_DEFE0_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe0mclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_DEFE1_MOD_ON) == CLK_DEFE1_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe1mclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_LCDC0_MOD0_ON) == CLK_LCDC0_MOD0_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd0ch0mclk0, CLK_ON);
    	}
    	if((g_clk_status & CLK_LCDC0_MOD1_ON) == CLK_LCDC0_MOD1_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd0ch1mclk1, CLK_ON);
    	}
    	if((g_clk_status & CLK_LCDC1_MOD0_ON) == CLK_LCDC1_MOD0_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd1ch0mclk0, CLK_ON);
    	}
    	if((g_clk_status & CLK_LCDC1_MOD1_ON) == CLK_LCDC1_MOD1_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd1ch1mclk1, CLK_ON);
    	}
    	if((g_clk_status & CLK_HDMI_MOD_ON) == CLK_HDMI_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_hdmimclk, CLK_ON);
    	}
    	if((g_clk_status & CLK_HDMI_MOD_DDC_ON) == CLK_HDMI_MOD_DDC_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_hdmimclk_ddc, CLK_ON);
    	}
    	if((g_clk_status & CLK_DSI_MOD_ON) == CLK_DSI_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_dsimclk_s, CLK_ON);
            OSAL_CCMU_MclkOnOff(h_dsimclk_p, CLK_ON);
    	}
    }

    if(type == 2)
    {
    	if((g_clk_status & CLK_DEBE0_MOD_ON) == CLK_DEBE0_MOD_ON)
    	{
        	__u32 pll_freq;
            __u32 mclk_div;

            pll_freq = OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL10);
            mclk_div = 1;
            while((pll_freq / mclk_div) > 300000000)
            {
                mclk_div ++;
            }
            OSAL_CCMU_SetMclkDiv(h_debe0mclk, mclk_div);
    	}
    	if((g_clk_status & CLK_DEBE1_MOD_ON) == CLK_DEBE1_MOD_ON)
    	{
            __u32 pll_freq;
            __u32 mclk_div;

            pll_freq = OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL10);
            mclk_div = 1;
            while((pll_freq / mclk_div) > 300000000)
            {
                mclk_div ++;
            }
            OSAL_CCMU_SetMclkDiv(h_debe1mclk, mclk_div);
    	}
    }

	return DIS_SUCCESS;
}

//type==1: close ahb clk and image mclk
//type==2: close all clk except ahb clk and image mclk
//type==3: close all clk
__s32 BSP_disp_clk_off(__u32 type)
{
    if(type & 1)
    {
//AHB CLK
    	if((g_clk_status & CLK_DEFE0_AHB_ON) == CLK_DEFE0_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe0ahbclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DEFE1_AHB_ON) == CLK_DEFE1_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe1ahbclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DEBE0_AHB_ON) == CLK_DEBE0_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe0ahbclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DEBE1_AHB_ON) == CLK_DEBE1_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe1ahbclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_LCDC0_AHB_ON) == CLK_LCDC0_AHB_ON)	//OK?? REG wont clear?
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd0ahbclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_LCDC1_AHB_ON) == CLK_LCDC1_AHB_ON)	//OK?? REG wont clear?
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd1ahbclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_HDMI_AHB_ON) == CLK_HDMI_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_hdmiahbclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DSI_AHB_ON) == CLK_DSI_AHB_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_dsiahbclk, CLK_OFF);
    	}
    	//OSAL_CCMU_MclkOnOff(h_tveahbclk, CLK_OFF);

//MODULE CLK
    	if((g_clk_status & CLK_DEBE0_MOD_ON) == CLK_DEBE0_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe0mclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DEBE1_MOD_ON) == CLK_DEBE1_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe1mclk, CLK_OFF);
    	}
	}

	if(type & 2)
	{
//DRAM CLK
    	if((g_clk_status & CLK_DEFE0_DRAM_ON) == CLK_DEFE0_DRAM_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe0dramclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DEFE1_DRAM_ON) == CLK_DEFE1_DRAM_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe1dramclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DEBE0_DRAM_ON) == CLK_DEBE0_DRAM_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe0dramclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DEBE1_DRAM_ON) == CLK_DEBE1_DRAM_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_debe1dramclk, CLK_OFF);
    	}

//MODULE CLK
    	if((g_clk_status & CLK_DEFE0_MOD_ON) == CLK_DEFE0_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe0mclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DEFE1_MOD_ON) == CLK_DEFE1_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_defe1mclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_LCDC0_MOD0_ON) == CLK_LCDC0_MOD0_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd0ch0mclk0, CLK_OFF);
    	}
    	if((g_clk_status & CLK_LCDC0_MOD1_ON) == CLK_LCDC0_MOD1_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd0ch1mclk1, CLK_OFF);
    	}
    	if((g_clk_status & CLK_LCDC1_MOD0_ON) == CLK_LCDC1_MOD0_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd1ch0mclk0, CLK_OFF);
    	}
    	if((g_clk_status & CLK_LCDC1_MOD1_ON) == CLK_LCDC1_MOD1_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_lcd1ch1mclk1, CLK_OFF);
    	}
    	if((g_clk_status & CLK_HDMI_MOD_ON) == CLK_HDMI_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_hdmimclk, CLK_OFF);
    	}
    	if((g_clk_status & CLK_HDMI_MOD_DDC_ON) == CLK_HDMI_MOD_DDC_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_hdmimclk_ddc, CLK_OFF);
    	}
    	if((g_clk_status & CLK_DSI_MOD_ON) == CLK_DSI_MOD_ON)
    	{
    		OSAL_CCMU_MclkOnOff(h_dsimclk_s, CLK_OFF);
            OSAL_CCMU_MclkOnOff(h_dsimclk_p, CLK_OFF);
    	}
    }

    if(type == 2)
    {
    	if((g_clk_status & CLK_DEBE0_MOD_ON) == CLK_DEBE0_MOD_ON)
    	{
    		OSAL_CCMU_SetMclkDiv(h_debe0mclk, 16);
    	}
    	if((g_clk_status & CLK_DEBE1_MOD_ON) == CLK_DEBE1_MOD_ON)
    	{
    		OSAL_CCMU_SetMclkDiv(h_debe1mclk, 16);
    	}
    }

	return DIS_SUCCESS;
}



