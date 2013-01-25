#include "iep_deu.h"
#include "iep_deu_ebios.h"
#include "../../de/disp_clk.h"

extern __u32 deu_str_tab[512];
static __u32 g_deu_clk_status;
extern __u8 deu_lp_tab_s[5][5][5];
extern __u8 deu_lp_tab_l[5][5][5];

static __hdle h_deuahbclk0, h_deudramclk0, h_deumclk0, h_deuahbclk1, h_deudramclk1, h_deumclk1;

static __deu_t gdeu[2];	//DRC module parameters
static __u32 g_deu_status[2] = {0,0};

static __u32 *g_strtab_addr;

static __u8 *plptab;

#define CLK_ON 1
#define CLK_OFF 0
#define RST_INVAILD 0
#define RST_VAILD   1

#define ____SEPARATOR_DEU_CLK____
__s32 deu_clk_init(__u32 sel)
{
    __u32 pll_freq;
    __u32 mclk_div;

    DE_INF("deu %d clk init\n", sel);
	if(!sel)
	{
	    h_deuahbclk0 = OSAL_CCMU_OpenMclk(AHB_CLK_DEU0);
	    h_deudramclk0 = OSAL_CCMU_OpenMclk(DRAM_CLK_DEU0);
	    h_deumclk0 = OSAL_CCMU_OpenMclk(MOD_CLK_IEPDEU0);

		OSAL_CCMU_MclkReset(h_deumclk0, RST_INVAILD);
		
		OSAL_CCMU_SetMclkSrc(h_deumclk0, SYS_CLK_PLL10);	//FIX CONNECT TO  PLL10
		OSAL_CCMU_SetMclkDiv(h_deumclk0, 1);
        pll_freq = OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL10);
        mclk_div = 1;
        while((pll_freq / mclk_div) > 300000000)
        {
            mclk_div ++;
        }
		OSAL_CCMU_SetMclkDiv(h_deumclk0, mclk_div);
		
		OSAL_CCMU_MclkOnOff(h_deuahbclk0, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_deumclk0, CLK_ON);

		g_deu_clk_status  |= (CLK_DEU0_AHB_ON | CLK_DEU0_MOD_ON);
	}
	else
	{
		h_deuahbclk1 = OSAL_CCMU_OpenMclk(AHB_CLK_DEU1);
	    h_deudramclk1 = OSAL_CCMU_OpenMclk(DRAM_CLK_DEU1);
	    h_deumclk1 = OSAL_CCMU_OpenMclk(MOD_CLK_IEPDEU1);

		OSAL_CCMU_MclkReset(h_deumclk1, RST_INVAILD);
		OSAL_CCMU_SetMclkSrc(h_deumclk1, SYS_CLK_PLL10);	//FIX CONNECT TO PLL10
		OSAL_CCMU_SetMclkDiv(h_deumclk1, 1);
        pll_freq = OSAL_CCMU_GetSrcFreq(SYS_CLK_PLL10);
        mclk_div = 1;
        while((pll_freq / mclk_div) > 300000000)
        {
            mclk_div ++;
        }
		OSAL_CCMU_SetMclkDiv(h_deumclk1, mclk_div);
		
		OSAL_CCMU_MclkOnOff(h_deuahbclk1, CLK_ON);
		OSAL_CCMU_MclkOnOff(h_deumclk1, CLK_ON);

		g_deu_clk_status  |= (CLK_DEU1_AHB_ON | CLK_DEU1_MOD_ON);
	}
	return DIS_SUCCESS;
}

__s32 deu_clk_exit(__u32 sel)
{
	if(!sel)
	{
		OSAL_CCMU_MclkReset(h_deumclk0, RST_VAILD);

		if(g_deu_clk_status & CLK_DEU0_DRAM_ON)
		{
			OSAL_CCMU_MclkOnOff(h_deudramclk0, CLK_OFF);
		}

		if(g_deu_clk_status & CLK_DEU0_MOD_ON)
		{
			OSAL_CCMU_MclkOnOff(h_deumclk0, CLK_OFF);
		}

		if(g_deu_clk_status & CLK_DEU0_AHB_ON)
		{
			OSAL_CCMU_MclkOnOff(h_deuahbclk0, CLK_OFF);
		}

		OSAL_CCMU_CloseMclk(h_deuahbclk0);
	    OSAL_CCMU_CloseMclk(h_deudramclk0);
	    OSAL_CCMU_CloseMclk(h_deumclk0);

		g_deu_clk_status &= (CLK_DEU0_AHB_OFF & CLK_DEU0_MOD_OFF & CLK_DEU0_DRAM_OFF);
	}
	else
	{
		OSAL_CCMU_MclkReset(h_deumclk1, RST_VAILD);

		if(g_deu_clk_status & CLK_DEU1_DRAM_ON)
		{
			OSAL_CCMU_MclkOnOff(h_deudramclk1, CLK_OFF);
		}

		if(g_deu_clk_status & CLK_DEU1_MOD_ON)
		{
			OSAL_CCMU_MclkOnOff(h_deumclk1, CLK_OFF);
		}

		if(g_deu_clk_status & CLK_DEU1_AHB_ON)
		{
			OSAL_CCMU_MclkOnOff(h_deuahbclk1, CLK_OFF);
		}

		OSAL_CCMU_CloseMclk(h_deuahbclk1);
	    OSAL_CCMU_CloseMclk(h_deudramclk1);
	    OSAL_CCMU_CloseMclk(h_deumclk1);

		g_deu_clk_status &= (CLK_DEU1_AHB_OFF & CLK_DEU1_MOD_OFF & CLK_DEU1_DRAM_OFF);
	}
    return DIS_SUCCESS;
}

__s32 deu_clk_open(__u32 sel)
{
	if(!sel)
	{
		OSAL_CCMU_MclkOnOff(h_deudramclk0, CLK_ON);

		g_deu_clk_status |= (CLK_DEU0_DRAM_ON);
	}
	else
	{
		OSAL_CCMU_MclkOnOff(h_deudramclk1, CLK_ON);

		g_deu_clk_status |= (CLK_DEU1_DRAM_ON);
	}
	return DIS_SUCCESS;
}

__s32 deu_clk_close(__u32 sel)
{
	if(!sel)
	{
		OSAL_CCMU_MclkOnOff(h_deudramclk0, CLK_OFF);

		g_deu_clk_status &= (CLK_DEU0_DRAM_OFF);
	}
	else
	{
		OSAL_CCMU_MclkOnOff(h_deudramclk1, CLK_OFF);

		g_deu_clk_status &= (CLK_DEU1_DRAM_OFF);
	}
	return DIS_SUCCESS;
}

#define ____SEPARATOR_DEU_ALG____
__s32 DEU_ALG(__u32 sel)
{
	__disp_frame_info_t frameinfo;
	__u32 lpmode, dctimode;
	__u32 scalefact, filtertype = 0;
	__u32 deuwidth, deuheight;

	memcpy(&frameinfo, &gdeu[sel].frameinfo, sizeof(__disp_frame_info_t));

	if(frameinfo.b_trd_out == 1 && frameinfo.trd_out_mode == DISP_3D_OUT_MODE_LIRGB)
	{
		deuwidth = frameinfo.disp_size.width*2;
		deuheight = frameinfo.disp_size.height/2;
	}
	/*
	else if(frameinfo.b_interlace_out)
	{
		deuwidth = frameinfo.disp_size.width;
		deuheight = frameinfo.disp_size.height/2;
	}
	*/
	else
	{
		deuwidth = frameinfo.disp_size.width;
		deuheight = frameinfo.disp_size.height;
	}

	//1D/2D/DISABLE of LP and ENABLE/DISABLE DCTI
	if((frameinfo.trd_out_mode == DISP_3D_OUT_MODE_CI_1  || frameinfo.trd_out_mode == 	DISP_3D_OUT_MODE_CI_2  ||
	   frameinfo.trd_out_mode == DISP_3D_OUT_MODE_CI_3  || frameinfo.trd_out_mode ==  DISP_3D_OUT_MODE_CI_4) &&
	   frameinfo.b_trd_out == 1)
	{
		lpmode = 0;	//disable
		dctimode = 0;	//disable
	}
	else if((( frameinfo.trd_out_mode == DISP_3D_OUT_MODE_FA ) &&
			frameinfo.b_trd_out == 1)|| frameinfo.b_interlace_out == 1 || deuwidth > 2048)
	{
		lpmode = 1;	//1d lp
		dctimode = 1;	//1d dcti
	}
	else
	{
		lpmode = 2;	//2d lp
		dctimode = 1;	//1d dcti
	}

	scalefact = (frameinfo.out_size.width<<2)/frameinfo.in_size.width;	//scale factor X4

	if (scalefact<5)
	{
		filtertype = 0;
	}
	else if(scalefact>=5 && scalefact<7)
	{
		filtertype = 1;
	}
	else if(scalefact>=7 && scalefact<9)
	{
		filtertype = 2;
	}
	else if(scalefact>=9 && scalefact<11)
	{
		filtertype = 3;
	}
	else if(scalefact>=15)
	{
		filtertype = 4;
	}

	//set reg
	DEU_EBIOS_Set_Display_Size(sel, deuwidth, deuheight);
	DEU_EBIOS_LP_Enable(sel, ((lpmode==0)||(gdeu[sel].lumashplvl==0))?0:1);
	DEU_EBIOS_LP_Set_Mode(sel, lpmode-1);
	DEU_EBIOS_DCTI_Enable(sel, ((dctimode==0)||(gdeu[sel].chromashplvl==0))?0:1);


    DEU_EBIOS_LP_Set_Para(sel, gdeu[sel].lumashplvl, filtertype, plptab);
    DEU_EBIOS_DCTI_Set_Para(sel, gdeu[sel].chromashplvl);


    return DIS_SUCCESS;

}

#define ____SEPARATOR_DEU_BSP____

__s32 IEP_Deu_Enable(__u32 sel, __u32 enable)
{
	__u32 strtab_addr;

    strtab_addr =(__u32)g_strtab_addr;
	if(enable)
	{
		deu_clk_open(sel);
		DEU_EBIOS_Enable(sel, 1);

		//__inf("vir_addr:0x%08x, phy_addr:0x%08x\n", strtab_addr, (__u32)(OSAL_VAtoPA((void*)strtab_addr)));

		//virtual to physcal addr
		strtab_addr = (__u32)OSAL_VAtoPA((void*)strtab_addr);



		DEU_EBIOS_LP_Set_STR_Addr(sel, strtab_addr);
		DEU_EBIOS_LP_STR_Cfg_Rdy(sel);
		DEU_EBIOS_Set_Csc_Coeff(sel, gdeu[sel].frameinfo.csc_mode);
		DEU_EBIOS_Csc_Enable(sel, 1);
		g_deu_status[sel] |= DEU_USED;

	}
	else
	{
		DEU_EBIOS_Csc_Enable(sel, 0);
		DEU_EBIOS_Enable(sel, 0);

		g_deu_status[sel] |= DEU_NEED_CLOSED;

	}

    return DIS_SUCCESS;
}

__s32 IEP_Deu_Set_Luma_Sharpness_Level(__u32 sel, __u32 level)
{
	gdeu[sel].lumashplvl = level;

	return DIS_SUCCESS;
}

__s32 IEP_Deu_Set_Chroma_Sharpness_Level(__u32 sel, __u32 level)
{
	gdeu[sel].chromashplvl = level;

	return DIS_SUCCESS;
}

__s32 IEP_Deu_Set_White_Level_Extension(__u32 sel, __u32 level)
{
	DEU_EBIOS_WLE_Set_Para(sel,level);
	DEU_EBIOS_WLE_Enable(sel, (level==0)?0:1);

	gdeu[sel].wlelvl = level;

	return DIS_SUCCESS;
}

__s32 IEP_Deu_Set_Black_Level_Extension(__u32 sel, __u32 level)
{
	DEU_EBIOS_BLE_Set_Para(sel,level);
	DEU_EBIOS_BLE_Enable(sel, (level==0)?0:1);

	gdeu[sel].blelvl = level;

	return DIS_SUCCESS;
}

__s32 IEP_Deu_Set_Ready(__u32 sel)
{
	DEU_EBIOS_Cfg_Rdy(sel);

	return DIS_SUCCESS;
}

__s32 IEP_Deu_Set_Reg_base(__u32 sel, __u32 base)
{
	DEU_EBIOS_Set_Reg_Base(sel, base);

	return DIS_SUCCESS;

}

__s32 IEP_Deu_Set_Winodw(__u32 sel, __disp_rect_t *window)
{
 	__u32 top, bot, left, right;

	//convert rectangle to register
	top = window->y;
	bot = window->y + window->height - 1;
	left = window->x;
	right = window->x + window->width - 1;

	DEU_EBIOS_Set_Win_Para(sel, top, bot, left, right);

	return DIS_SUCCESS;
}

__s32 IEP_Deu_Output_Select(__u32 sel, __u32 be_ch)
{
	DEU_EBIOS_Set_Output_Chnl(sel, be_ch);

	return DIS_SUCCESS;

}

__s32 IEP_Deu_Init(__u32 sel)
{
	int ret;
	int value = 1;
	char primary_key[20];

	g_strtab_addr = (__u32 *)OSAL_PhyAlloc(512);//OSAL_PhyAlloc(512);
	memcpy(g_strtab_addr, deu_str_tab, 512);
	deu_clk_init(sel);

    sprintf(primary_key, "lcd%d_para", sel);
	ret = OSAL_Script_FetchParser_Data(primary_key, "deu_mode", &value, 1);
	if(ret < 0)
	{
		DE_WRN("deu_mode%d not exist.\n", sel);
		plptab = &deu_lp_tab_s[0][0][0];
	}
	else
	{
		DE_INF("deu_mode%d = %d.\n", sel, value);
		if(value > 1 || value < 0)
		{
			DE_WRN("deu_mode%d invalid.\n",sel);
			plptab = &deu_lp_tab_s[0][0][0];
		}
		else
		{
			plptab = (value == 1)? (&deu_lp_tab_l[0][0][0]):(&deu_lp_tab_s[0][0][0]);
		}
	}

	return DIS_SUCCESS;

}

__s32 IEP_Deu_Exit(__u32 sel)
{
	deu_clk_exit(sel);

    OSAL_PhyFree(g_strtab_addr, 512);

	return DIS_SUCCESS;

}

__s32 IEP_Deu_Operation_In_Vblanking(__u32 sel)
{
	if(g_deu_status[sel] & DEU_USED)
	{
		//function about setting level through frameinfo

		DEU_ALG(sel);

		DEU_EBIOS_Cfg_Rdy(sel);
	}

	if(g_deu_status[sel] & DEU_NEED_CLOSED)
	{
		g_deu_status[sel] &= (DEU_USED_MASK & DEU_NEED_CLOSED_MASK);
		deu_clk_close(sel);
	}

    return DIS_SUCCESS;
}

__s32 IEP_Deu_Early_Suspend(__u32 sel);//close clk

__s32 IEP_Deu_suspend(__u32 sel);//save register

__s32 IEP_Deu_Resume (__u32 sel);//restore register

__s32 IEP_Deu_Late_Resume(__u32 sel);//open clk

__s32 IEP_Deu_Set_frameinfo(__u32 sel, __disp_frame_info_t frameinfo)
{
   	memcpy(&gdeu[sel].frameinfo, &frameinfo, sizeof(__disp_frame_info_t));

    return DIS_SUCCESS;

}

