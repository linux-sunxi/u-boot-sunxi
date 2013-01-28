#include "disp_event.h"
#include "disp_display.h"
#include "disp_de.h"
#include "disp_video.h"
#include "disp_scaler.h"
#include "disp_iep.h"
#include "disp_lcd.h"

extern __panel_para_t gpanel_info[2];

__s32 BSP_disp_cmd_cache(__u32 sel)
{
    gdisp.screen[sel].cache_flag = TRUE;
    return DIS_SUCCESS;
}

__s32 BSP_disp_cmd_submit(__u32 sel)
{
    gdisp.screen[sel].cache_flag = FALSE;

    return DIS_SUCCESS;
}

__s32 BSP_disp_cfg_start(__u32 sel)
{
	gdisp.screen[sel].cfg_cnt++;

	return DIS_SUCCESS;
}

__s32 BSP_disp_cfg_finish(__u32 sel)
{
	gdisp.screen[sel].cfg_cnt--;

	return DIS_SUCCESS;
}

__s32 BSP_disp_vsync_event_enable(__u32 sel, __bool enable)
{
    gdisp.screen[sel].vsync_event_en = enable;

    return DIS_SUCCESS;
}
void LCD_vbi_event_proc(__u32 sel, __u32 tcon_index)
{
    __u32 cur_line = 0, start_delay = 0;
    __u32 i = 0;

    if(gdisp.screen[sel].vsync_event_en && gdisp.init_para.vsync_event)
    {
        gdisp.init_para.vsync_event(sel);
    }
	Video_Operation_In_Vblanking(sel, tcon_index);

    cur_line = TCON_get_cur_line(sel, tcon_index);
    start_delay = TCON_get_start_delay(sel, tcon_index);
    if(cur_line > start_delay-4)
	{
	    //DE_INF("int:%d,%d\n", cur_line,start_delay);
        if(gpanel_info[sel].lcd_fresh_mode == 0)//return while not  trigger mode
		{
		    return ;
        }
	}

    if(gdisp.screen[sel].LCD_CPUIF_ISR)
    {
    	(*gdisp.screen[sel].LCD_CPUIF_ISR)();
    }

    if(gdisp.screen[sel].cache_flag == FALSE && gdisp.screen[sel].cfg_cnt == 0)
    {
        for(i=0; i<2; i++)
        {
            if((gdisp.scaler[i].status & SCALER_USED) && (gdisp.scaler[i].screen_index == sel))
            {
                __u32 hid;

                hid = gdisp.scaler[i].layer_id;
                DE_SCAL_Set_Reg_Rdy(i);
                //DE_SCAL_Reset(i);
                //DE_SCAL_Start(i);
                disp_deu_set_frame_info(sel, IDTOHAND(hid));
                IEP_Deu_Operation_In_Vblanking(i);
                gdisp.scaler[i].b_reg_change = FALSE;
            }
            if(gdisp.scaler[i].b_close == TRUE)
            {
                Scaler_close(i);
                gdisp.scaler[i].b_close = FALSE;
            }
        }

        if(DISP_OUTPUT_TYPE_LCD == BSP_disp_get_output_type(sel))
        {
            IEP_Drc_Operation_In_Vblanking(sel);
        }
        DE_BE_Cfg_Ready(sel);
        IEP_CMU_Operation_In_Vblanking(sel);
		gdisp.screen[sel].have_cfg_reg = TRUE;
    }

#if 0
    cur_line = LCDC_get_cur_line(sel, tcon_index);

	if(cur_line > 5)
	{
    	DE_INF("%d\n", cur_line);
    }
#endif

    return ;
}

void LCD_line_event_proc(__u32 sel)
{
	if(gdisp.screen[sel].have_cfg_reg)
	{
	    gdisp.init_para.disp_int_process(sel);
	    gdisp.screen[sel].have_cfg_reg = FALSE;
	}
}
