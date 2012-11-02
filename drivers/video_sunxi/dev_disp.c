#include "dev_disp.h"
#include <asm/arch/timer.h>


fb_info_t g_fbi;
__disp_drv_t g_disp_drv;

#define __user 
#define HZ 100
#define EFAULT 1

static __u32    lcd_flow_cnt[2] = {0};
static __bool   lcd_op_finished[2] = {0};
struct timer_list lcd_timer[2];
static __bool   lcd_open_start[2] = {0};
static unsigned int gbuffer[4096];
static __u32 suspend_output_type[2] = {0,0};
static __u32 suspend_status = 0;//0:normal; suspend_status&1 != 0:in early_suspend; suspend_status&2 != 0:in suspend;


__s32 copy_from_user(void *dest, void* src, __u32 size)
{
    memcpy(dest, src, size);
	return 0;
}

__s32 copy_to_user(void *dest, void* src, __u32 size)
{
    memcpy(dest, src, size);
	return 0;
}


__s32 disp_delay_ms(__u32 ms)
{
    /* todo */
    udelay(ms*1000);
    return 0;   
}
// [before][step_0][delay_0][step_1][delay_1]......[step_n-2][delay_n-2][step_n-1][delay_n-1][after]
void DRV_lcd_open_callback(void *parg)
{
    __lcd_flow_t *flow;
    __u32 sel = (__u32)parg;
    __s32 i = lcd_flow_cnt[sel]++;

    flow = BSP_disp_lcd_get_open_flow(sel);

	if(i < flow->func_num)
    {
    	debug("de timer delay %d\n", flow->func[i].delay);
    	flow->func[i].func(sel);
        if(flow->func[i].delay == 0)
        {
            DRV_lcd_open_callback((void*)sel);
        }
        else
        {
			lcd_timer[sel].data = sel;
			lcd_timer[sel].expires = flow->func[i].delay;
			lcd_timer[sel].function = DRV_lcd_open_callback;
			add_timer(&lcd_timer[sel]);
    	}
    }
    else if(i == flow->func_num)
    {
        BSP_disp_lcd_open_after(sel);
        lcd_op_finished[sel] = 1;
    }
}

__s32 DRV_lcd_open(__u32 sel)
{
    lcd_flow_cnt[sel] = 0;
    lcd_op_finished[sel] = 0;
	lcd_open_start[sel] = 1;
     
	init_timer(&lcd_timer[sel]);

    BSP_disp_lcd_open_before(sel);
    DRV_lcd_open_callback((void*)sel);

    return 0;
}
__bool DRV_lcd_check_open_finished(__u32 sel)
{
	if(lcd_open_start[sel] == 1)
	{
	    if(lcd_op_finished[sel])
	    {
	        del_timer(&lcd_timer[sel]);
	    }
		return lcd_op_finished[sel];
	}

	return 1;
}


/*
__s32 DRV_lcd_open(__u32 sel)
{
    __u32 i = 0;
    __lcd_flow_t *flow;

	if(g_disp_drv.b_lcd_open[sel] == 0)
	{
	    BSP_disp_lcd_open_before(sel);

	    flow = BSP_disp_lcd_get_open_flow(sel);
	    for(i=0; i<flow->func_num; i++)
	    {
	        __u32 timeout = flow->func[i].delay;

	        flow->func[i].func(sel);

	    	disp_delay_ms(timeout);
	    }

	    BSP_disp_lcd_open_after(sel);

		g_disp_drv.b_lcd_open[sel] = 1;
	}

    return 0;
}
*/

__s32 DRV_lcd_close(__u32 sel)
{
    __u32 i = 0;
    __lcd_flow_t *flow;

	if(g_disp_drv.b_lcd_open[sel] == 1)
	{
	    BSP_disp_lcd_close_befor(sel);

	    flow = BSP_disp_lcd_get_close_flow(sel);
	    for(i=0; i<flow->func_num; i++)
	    {
	        __u32 timeout = flow->func[i].delay;

	        flow->func[i].func(sel);

	    	disp_delay_ms(timeout);

	    }

	    BSP_disp_lcd_close_after(sel);

		g_disp_drv.b_lcd_open[sel] = 0;
	}
    return 0;
}


__s32 disp_int_process(__u32 sel)
{
    /* to do */
    return 0;
}

__s32 DRV_DISP_Init(void)
{
    __disp_bsp_init_para para;
    
    printf("====display init =====\n");

    memset(&para, 0, sizeof(__disp_bsp_init_para));
    para.base_image0    = 0x01e60000;
    para.base_image1    = 0x01e40000;
    para.base_scaler0   = 0x01e00000;
    para.base_scaler1   = 0x01e20000;
    para.base_lcdc0     = 0x01c0c000;
    para.base_lcdc1     = 0x01c17000;
    para.base_tvec0     = 0x01c0a000;
    para.base_tvec1     = 0x01c1b000;
    para.base_ccmu      = 0x01c20000;
    para.base_sdram     = 0x01c01000;
    para.base_pioc      = 0x01c20800;
    para.base_pwm       = 0x01c20c00;
	para.disp_int_process    = disp_int_process;

	memset(&g_disp_drv, 0, sizeof(__disp_drv_t));
    
    BSP_disp_init(&para);
    BSP_disp_open();
    
    DRV_lcd_open(0);
    //board_display_device_open();

    return 0;
}

__s32 DRV_DISP_Exit(void)
{
    //BSP_disp_close();
	del_timer(lcd_timer[0]);
	del_timer(lcd_timer[1]);

	BSP_disp_exit(g_disp_drv.exit_mode);
	
    return 0;
}



long disp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned long karg[4];
	unsigned long ubuffer[4] = {0};
	__s32 ret = 0;

	if (copy_from_user((void*)karg,(void __user*)arg,4*sizeof(unsigned long)))
	{
		__wrn("copy_from_user fail %d \n",__LINE__);
		return -EFAULT;
	}

	ubuffer[0] = *(unsigned long*)karg;
	ubuffer[1] = (*(unsigned long*)(karg+1));
	ubuffer[2] = (*(unsigned long*)(karg+2));
	ubuffer[3] = (*(unsigned long*)(karg+3));

    if(cmd < DISP_CMD_FB_REQUEST)
    {
        if((ubuffer[0] != 0) && (ubuffer[0] != 1))
        {
            __wrn("para err in disp_ioctl, cmd = 0x%x,screen id = %d\n", cmd, (int)ubuffer[0]);
            return -1;
        }
    }
    if(suspend_status & 2)
    {
        __wrn("ioctl:%x fail when in suspend!\n", cmd);
        return -1;
    }
    
#if 0
    if(cmd!=DISP_CMD_TV_GET_INTERFACE && cmd!=DISP_CMD_HDMI_GET_HPD_STATUS && cmd!=DISP_CMD_GET_OUTPUT_TYPE 
    	&& cmd!=DISP_CMD_SCN_GET_WIDTH && cmd!=DISP_CMD_SCN_GET_HEIGHT
    	&& cmd!=DISP_CMD_VIDEO_SET_FB && cmd!=DISP_CMD_VIDEO_GET_FRAME_ID)
    {
        OSAL_PRINTF("cmd:0x%x,%ld,%ld\n",cmd, ubuffer[0], ubuffer[1]);
    }
#endif

    switch(cmd)
    {
    //----disp global----
    	case DISP_CMD_SET_BKCOLOR:
	    {
	        __disp_color_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[1],sizeof(__disp_color_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
		    ret = BSP_disp_set_bk_color(ubuffer[0], &para);
		    break;
	    }

    	case DISP_CMD_SET_COLORKEY:
    	{
    	    __disp_colorkey_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[1],sizeof(__disp_colorkey_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_set_color_key(ubuffer[0], &para);
		    break;
		}

    	case DISP_CMD_SET_PALETTE_TBL:
    	    if((ubuffer[1] == 0) || ((int)ubuffer[3] <= 0))
    	    {
    	        __wrn("para invalid in disp ioctrl DISP_CMD_SET_PALETTE_TBL,buffer:0x%x, size:0x%x\n", (unsigned int)ubuffer[1], (unsigned int)ubuffer[3]);
    	        return -1;
    	    }
    		if(copy_from_user(gbuffer, (void __user *)ubuffer[1],ubuffer[3]))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_set_palette_table(ubuffer[0], (__u32 *)gbuffer, ubuffer[2], ubuffer[3]);
    		break;

    	case DISP_CMD_GET_PALETTE_TBL:
    	    if((ubuffer[1] == 0) || ((int)ubuffer[3] <= 0))
    	    {
    	        __wrn("para invalid in disp ioctrl DISP_CMD_GET_PALETTE_TBL,buffer:0x%x, size:0x%x\n", (unsigned int)ubuffer[1], (unsigned int)ubuffer[3]);
    	        return -1;
    	    }
    		ret = BSP_disp_get_palette_table(ubuffer[0], (__u32 *)gbuffer, ubuffer[2], ubuffer[3]);
    		if(copy_to_user((void __user *)ubuffer[1], gbuffer,ubuffer[3]))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;

    	case DISP_CMD_START_CMD_CACHE:
    		ret = BSP_disp_cmd_cache(ubuffer[0]);
    		break;

    	case DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE:
    		ret = BSP_disp_cmd_submit(ubuffer[0]);
    		break;

    	case DISP_CMD_GET_OUTPUT_TYPE:
    		ret =  BSP_disp_get_output_type(ubuffer[0]);
    		break;

    	case DISP_CMD_SCN_GET_WIDTH:
    		ret = BSP_disp_get_screen_width(ubuffer[0]);
    		break;

    	case DISP_CMD_SCN_GET_HEIGHT:
    		ret = BSP_disp_get_screen_height(ubuffer[0]);
    		break;

    	case DISP_CMD_SET_GAMMA_TABLE:
    	    if((ubuffer[1] == 0) || ((int)ubuffer[2] <= 0))
    	    {
    	        __wrn("para invalid in disp ioctrl DISP_CMD_SET_GAMMA_TABLE,buffer:0x%x, size:0x%x\n", (unsigned int)ubuffer[1], (unsigned int)ubuffer[2]);
    	        return -1;
    	    }
    		if(copy_from_user(gbuffer, (void __user *)ubuffer[1],ubuffer[2]))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_set_gamma_table(ubuffer[0], (__u32 *)gbuffer, ubuffer[2]);
    		break;

    	case DISP_CMD_GAMMA_CORRECTION_ON:
    		ret = BSP_disp_gamma_correction_enable(ubuffer[0]);
    		break;

    	case DISP_CMD_GAMMA_CORRECTION_OFF:
    		ret = BSP_disp_gamma_correction_disable(ubuffer[0]);
    		break;

        case DISP_CMD_SET_BRIGHT:
            ret = BSP_disp_set_bright(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_GET_BRIGHT:
            ret = BSP_disp_get_bright(ubuffer[0]);
    		break;

        case DISP_CMD_SET_CONTRAST:
            ret = BSP_disp_set_contrast(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_GET_CONTRAST:
            ret = BSP_disp_get_contrast(ubuffer[0]);
    		break;

        case DISP_CMD_SET_SATURATION:
            ret = BSP_disp_set_saturation(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_GET_SATURATION:
            ret = BSP_disp_get_saturation(ubuffer[0]);
    		break;

        case DISP_CMD_SET_HUE:
            ret = BSP_disp_set_hue(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_GET_HUE:
            ret = BSP_disp_get_hue(ubuffer[0]);
    		break;

        case DISP_CMD_ENHANCE_ON:
            ret = BSP_disp_enhance_enable(ubuffer[0], 1);
    		break;

        case DISP_CMD_ENHANCE_OFF:
            ret = BSP_disp_enhance_enable(ubuffer[0], 0);
    		break;

        case DISP_CMD_GET_ENHANCE_EN:
            ret = BSP_disp_get_enhance_enable(ubuffer[0]);
    		break;

    	case DISP_CMD_CAPTURE_SCREEN:
    	    ret = BSP_disp_capture_screen(ubuffer[0], (__disp_capture_screen_para_t *)ubuffer[1]);
    	    break;

        case DISP_CMD_SET_SCREEN_SIZE:
            ret = BSP_disp_set_screen_size(ubuffer[0], (__disp_rectsz_t*)ubuffer[1]);
            break;

        case DISP_CMD_DE_FLICKER_ON:
            ret = BSP_disp_de_flicker_enable(ubuffer[0], 1);
            break;

        case DISP_CMD_DE_FLICKER_OFF:
            ret = BSP_disp_de_flicker_enable(ubuffer[0], 0);
            break;

    //----layer----
    	case DISP_CMD_LAYER_REQUEST:
    		ret = BSP_disp_layer_request(ubuffer[0], (__disp_layer_work_mode_t)ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_RELEASE:
    		ret = BSP_disp_layer_release(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_OPEN:
    		ret = BSP_disp_layer_open(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_CLOSE:
    		ret = BSP_disp_layer_close(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_SET_FB:
    	{
    	    __disp_fb_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_fb_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_layer_set_framebuffer(ubuffer[0], ubuffer[1], &para);
    		//DRV_disp_wait_cmd_finish(ubuffer[0]);
    		break;
    	}

    	case DISP_CMD_LAYER_GET_FB:
    	{
    	    __disp_fb_t para;

    		ret = BSP_disp_layer_get_framebuffer(ubuffer[0], ubuffer[1], &para);
    		if(copy_to_user((void __user *)ubuffer[2], &para,sizeof(__disp_fb_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    	case DISP_CMD_LAYER_SET_SRC_WINDOW:
    	{
    	    __disp_rect_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_rect_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_layer_set_src_window(ubuffer[0],ubuffer[1], &para);
    		//DRV_disp_wait_cmd_finish(ubuffer[0]);
    		break;
        }

    	case DISP_CMD_LAYER_GET_SRC_WINDOW:
    	{
    	    __disp_rect_t para;

    		ret = BSP_disp_layer_get_src_window(ubuffer[0],ubuffer[1], &para);
    		if(copy_to_user((void __user *)ubuffer[2], &para, sizeof(__disp_rect_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    	case DISP_CMD_LAYER_SET_SCN_WINDOW:
    	{
    	    __disp_rect_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_rect_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_layer_set_screen_window(ubuffer[0],ubuffer[1], &para);
    		//DRV_disp_wait_cmd_finish(ubuffer[0]);
    		break;
        }

    	case DISP_CMD_LAYER_GET_SCN_WINDOW:
    	{
    	    __disp_rect_t para;

    		ret = BSP_disp_layer_get_screen_window(ubuffer[0],ubuffer[1], &para);
    		if(copy_to_user((void __user *)ubuffer[2], &para, sizeof(__disp_rect_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    	case DISP_CMD_LAYER_SET_PARA:
    	{
    	    __disp_layer_info_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_layer_info_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_layer_set_para(ubuffer[0], ubuffer[1], &para);
    		//DRV_disp_wait_cmd_finish(ubuffer[0]);
    		break;
        }

    	case DISP_CMD_LAYER_GET_PARA:
    	{
    	    __disp_layer_info_t para;

    		ret = BSP_disp_layer_get_para(ubuffer[0], ubuffer[1], &para);
    		if(copy_to_user((void __user *)ubuffer[2],&para, sizeof(__disp_layer_info_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    	case DISP_CMD_LAYER_TOP:
    		ret = BSP_disp_layer_set_top(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_BOTTOM:
    		ret = BSP_disp_layer_set_bottom(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_ALPHA_ON:
    		ret = BSP_disp_layer_alpha_enable(ubuffer[0], ubuffer[1], 1);
    		break;

    	case DISP_CMD_LAYER_ALPHA_OFF:
    		ret = BSP_disp_layer_alpha_enable(ubuffer[0], ubuffer[1], 0);
    		break;

    	case DISP_CMD_LAYER_SET_ALPHA_VALUE:
    		ret = BSP_disp_layer_set_alpha_value(ubuffer[0], ubuffer[1], ubuffer[2]);
    		//DRV_disp_wait_cmd_finish(ubuffer[0]);
    		break;

    	case DISP_CMD_LAYER_CK_ON:
    		ret = BSP_disp_layer_colorkey_enable(ubuffer[0], ubuffer[1], 1);
    		break;

    	case DISP_CMD_LAYER_CK_OFF:
    		ret = BSP_disp_layer_colorkey_enable(ubuffer[0], ubuffer[1], 0);
    		break;

    	case DISP_CMD_LAYER_SET_PIPE:
    		ret = BSP_disp_layer_set_pipe(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

    	case DISP_CMD_LAYER_GET_ALPHA_VALUE:
    		ret = BSP_disp_layer_get_alpha_value(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_GET_ALPHA_EN:
    		ret = BSP_disp_layer_get_alpha_enable(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_GET_CK_EN:
    		ret = BSP_disp_layer_get_colorkey_enable(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_GET_PRIO:
    		ret = BSP_disp_layer_get_piro(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LAYER_GET_PIPE:
    		ret = BSP_disp_layer_get_pipe(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_SMOOTH:
            ret = BSP_disp_layer_set_smooth(ubuffer[0], ubuffer[1],(__disp_video_smooth_t) ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_SMOOTH:
            ret = BSP_disp_layer_get_smooth(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_BRIGHT:
            ret = BSP_disp_layer_set_bright(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_BRIGHT:
            ret = BSP_disp_layer_get_bright(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_CONTRAST:
            ret = BSP_disp_layer_set_contrast(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_CONTRAST:
            ret = BSP_disp_layer_get_contrast(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_SATURATION:
            ret = BSP_disp_layer_set_saturation(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_SATURATION:
            ret = BSP_disp_layer_get_saturation(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_HUE:
            ret = BSP_disp_layer_set_hue(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_HUE:
            ret = BSP_disp_layer_get_hue(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_ENHANCE_ON:
            ret = BSP_disp_layer_enhance_enable(ubuffer[0], ubuffer[1], 1);
    		break;

        case DISP_CMD_LAYER_ENHANCE_OFF:
            ret = BSP_disp_layer_enhance_enable(ubuffer[0], ubuffer[1], 0);
    		break;

        case DISP_CMD_LAYER_GET_ENHANCE_EN:
            ret = BSP_disp_layer_get_enhance_enable(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_VPP_ON:
            ret = BSP_disp_layer_vpp_enable(ubuffer[0], ubuffer[1], 1);
    		break;

        case DISP_CMD_LAYER_VPP_OFF:
            ret = BSP_disp_layer_vpp_enable(ubuffer[0], ubuffer[1], 0);
    		break;

        case DISP_CMD_LAYER_GET_VPP_EN:
            ret = BSP_disp_layer_get_vpp_enable(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_LUMA_SHARP_LEVEL:
            ret = BSP_disp_layer_set_luma_sharp_level(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_LUMA_SHARP_LEVEL:
            ret = BSP_disp_layer_get_luma_sharp_level(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_CHROMA_SHARP_LEVEL:
            ret = BSP_disp_layer_set_chroma_sharp_level(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_CHROMA_SHARP_LEVEL:
            ret = BSP_disp_layer_get_chroma_sharp_level(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_WHITE_EXTEN_LEVEL:
            ret = BSP_disp_layer_set_white_exten_level(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_WHITE_EXTEN_LEVEL:
            ret = BSP_disp_layer_get_white_exten_level(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_LAYER_SET_BLACK_EXTEN_LEVEL:
            ret = BSP_disp_layer_set_black_exten_level(ubuffer[0], ubuffer[1], ubuffer[2]);
    		break;

        case DISP_CMD_LAYER_GET_BLACK_EXTEN_LEVEL:
            ret = BSP_disp_layer_get_black_exten_level(ubuffer[0], ubuffer[1]);
    		break;

    //----scaler----
    	case DISP_CMD_SCALER_REQUEST:
    		ret = BSP_disp_scaler_request();
    		break;

    	case DISP_CMD_SCALER_RELEASE:
    		ret = BSP_disp_scaler_release(ubuffer[1]);
    		break;

    	case DISP_CMD_SCALER_EXECUTE:
    	{
    	    __disp_scaler_para_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_scaler_para_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_scaler_start(ubuffer[1],&para);
    		break;
        }

        case DISP_CMD_SCALER_EXECUTE_EX:
    	{
    	    __disp_scaler_para_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_scaler_para_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_scaler_start_ex(ubuffer[1],&para);
    		break;
        }

    //----hwc----
    	case DISP_CMD_HWC_OPEN:
    		ret =  BSP_disp_hwc_enable(ubuffer[0], 1);
    		break;

    	case DISP_CMD_HWC_CLOSE:
    		ret =  BSP_disp_hwc_enable(ubuffer[0], 0);
    		break;

    	case DISP_CMD_HWC_SET_POS:
    	{
    	    __disp_pos_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[1],sizeof(__disp_pos_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_hwc_set_pos(ubuffer[0], &para);
    		break;
        }

    	case DISP_CMD_HWC_GET_POS:
    	{
    	    __disp_pos_t para;

    		ret = BSP_disp_hwc_get_pos(ubuffer[0], &para);
    		if(copy_to_user((void __user *)ubuffer[1],&para, sizeof(__disp_pos_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    	case DISP_CMD_HWC_SET_FB:
    	{
    	    __disp_hwc_pattern_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[1],sizeof(__disp_hwc_pattern_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_hwc_set_framebuffer(ubuffer[0], &para);
    		break;
        }

    	case DISP_CMD_HWC_SET_PALETTE_TABLE:
			if((ubuffer[1] == 0) || ((int)ubuffer[3] <= 0))
            {
                __wrn("para invalid in display ioctrl DISP_CMD_HWC_SET_PALETTE_TABLE,buffer:0x%x, size:0x%x\n", (unsigned int)ubuffer[1], (unsigned int)ubuffer[3]);
                return -1;
            }
    		if(copy_from_user(gbuffer, (void __user *)ubuffer[1],ubuffer[3]))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_hwc_set_palette(ubuffer[0], (void*)gbuffer, ubuffer[2], ubuffer[3]);
    		break;


    //----video----
    	case DISP_CMD_VIDEO_START:
    		ret = BSP_disp_video_start(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_VIDEO_STOP:
    		ret = BSP_disp_video_stop(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_VIDEO_SET_FB:
    	{
    	    __disp_video_fb_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_video_fb_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_video_set_fb(ubuffer[0], ubuffer[1], &para);
    		break;
        }

        case DISP_CMD_VIDEO_GET_FRAME_ID:
            ret = BSP_disp_video_get_frame_id(ubuffer[0], ubuffer[1]);
    		break;

        case DISP_CMD_VIDEO_GET_DIT_INFO:
        {
            __disp_dit_info_t para;

            ret = BSP_disp_video_get_dit_info(ubuffer[0], ubuffer[1],&para);
    		if(copy_to_user((void __user *)ubuffer[2],&para, sizeof(__disp_dit_info_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    //----lcd----
    	case DISP_CMD_LCD_ON:
    		ret = DRV_lcd_open(ubuffer[0]);
            if(suspend_status != 0)
            {
                suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_LCD;
            }
    		break;

    	case DISP_CMD_LCD_OFF:
    		ret = DRV_lcd_close(ubuffer[0]);
            if(suspend_status != 0)
            {
                suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_NONE;
            }
    		break;

    	case DISP_CMD_LCD_SET_BRIGHTNESS:
    		ret = BSP_disp_lcd_set_bright(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LCD_GET_BRIGHTNESS:
    		ret = BSP_disp_lcd_get_bright(ubuffer[0]);
    		break;

    	case DISP_CMD_LCD_CPUIF_XY_SWITCH:
    		ret = BSP_disp_lcd_xy_switch(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_LCD_SET_SRC:
    		ret = BSP_disp_lcd_set_src(ubuffer[0], (__disp_lcdc_src_t)ubuffer[1]);
    		break;

        case DISP_CMD_LCD_USER_DEFINED_FUNC:
            ret = BSP_disp_lcd_user_defined_func(ubuffer[0], ubuffer[1], ubuffer[2], ubuffer[3]);
            break;

	//----pwm----
        case DISP_CMD_PWM_SET_PARA:
            ret = pwm_set_para(ubuffer[0], (__pwm_info_t *)ubuffer[1]);
            break;

        case DISP_CMD_PWM_GET_PARA:
            ret = pwm_get_para(ubuffer[0], (__pwm_info_t *)ubuffer[1]);
            break;


    //----tv----
    	case DISP_CMD_TV_ON:
    		ret = BSP_disp_tv_open(ubuffer[0]);
            if(suspend_status != 0)
            {
                suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_TV;
            }
    		break;

    	case DISP_CMD_TV_OFF:
    		ret = BSP_disp_tv_close(ubuffer[0]);
            if(suspend_status != 0)
            {
                suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_NONE;
            }
    		break;

    	case DISP_CMD_TV_SET_MODE:
    		ret = BSP_disp_tv_set_mode(ubuffer[0], (__disp_tv_mode_t)ubuffer[1]);
    		break;

    	case DISP_CMD_TV_GET_MODE:
    		ret = BSP_disp_tv_get_mode(ubuffer[0]);
    		break;

    	case DISP_CMD_TV_AUTOCHECK_ON:
    		ret = BSP_disp_tv_auto_check_enable(ubuffer[0]);
    		break;

    	case DISP_CMD_TV_AUTOCHECK_OFF:
    		ret = BSP_disp_tv_auto_check_disable(ubuffer[0]);
    		break;

    	case DISP_CMD_TV_GET_INTERFACE:
    	    if(suspend_status != 0)
    	    {
    	        ret = DISP_TV_NONE;
    	    }
    	    else
    	    {
    		    ret = BSP_disp_tv_get_interface(ubuffer[0]);
            }
    		break;

    	case DISP_CMD_TV_SET_SRC:
    		ret = BSP_disp_tv_set_src(ubuffer[0], (__disp_lcdc_src_t)ubuffer[1]);
    		break;

        case DISP_CMD_TV_GET_DAC_STATUS:
            if(suspend_status != 0)
            {
                ret = 0;
            }
            else
            {
                ret =  BSP_disp_tv_get_dac_status(ubuffer[0], ubuffer[1]);
            }
            break;

        case DISP_CMD_TV_SET_DAC_SOURCE:
            ret =  BSP_disp_tv_set_dac_source(ubuffer[0], ubuffer[1], (__disp_tv_dac_source)ubuffer[2]);
            break;

        case DISP_CMD_TV_GET_DAC_SOURCE:
            ret =  BSP_disp_tv_get_dac_source(ubuffer[0], ubuffer[1]);
            break;

    //----hdmi----
    	case DISP_CMD_HDMI_ON:
    		ret = BSP_disp_hdmi_open(ubuffer[0]);
            if(suspend_status != 0)
            {
                suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_HDMI;
            }
    		break;

    	case DISP_CMD_HDMI_OFF:
    		ret = BSP_disp_hdmi_close(ubuffer[0]);
            if(suspend_status != 0)
            {
                suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_NONE;
            }
    		break;

    	case DISP_CMD_HDMI_SET_MODE:
    		ret = BSP_disp_hdmi_set_mode(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_HDMI_GET_MODE:
    		ret = BSP_disp_hdmi_get_mode(ubuffer[0]);
    		break;

    	case DISP_CMD_HDMI_GET_HPD_STATUS:
    	    if(suspend_status != 0)
    	    {
    	        ret = 0;
    	    }
    	    else
    	    {
    	        ret = BSP_disp_hdmi_get_hpd_status(ubuffer[0]);
    	    }
    		break;

    	case DISP_CMD_HDMI_SUPPORT_MODE:
    		ret = BSP_disp_hdmi_check_support_mode(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_HDMI_SET_SRC:
    		ret = BSP_disp_hdmi_set_src(ubuffer[0], (__disp_lcdc_src_t)ubuffer[1]);
    		break;

    //----vga----
    	case DISP_CMD_VGA_ON:
    		ret = BSP_disp_vga_open(ubuffer[0]);
            if(suspend_status != 0)
            {
                suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_VGA;
            }
    		break;

    	case DISP_CMD_VGA_OFF:
    		ret = BSP_disp_vga_close(ubuffer[0]);
            if(suspend_status != 0)
            {
                suspend_output_type[ubuffer[0]] = DISP_OUTPUT_TYPE_NONE;
            }
    		break;

    	case DISP_CMD_VGA_SET_MODE:
    		ret = BSP_disp_vga_set_mode(ubuffer[0], (__disp_vga_mode_t)ubuffer[1]);
    		break;

    	case DISP_CMD_VGA_GET_MODE:
    		ret = BSP_disp_vga_get_mode(ubuffer[0]);
    		break;

    	case DISP_CMD_VGA_SET_SRC:
    		ret = BSP_disp_vga_set_src(ubuffer[0], (__disp_lcdc_src_t)ubuffer[1]);
    		break;

    //----sprite----
    	case DISP_CMD_SPRITE_OPEN:
    		ret = BSP_disp_sprite_open(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_CLOSE:
    		ret = BSP_disp_sprite_close(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_SET_FORMAT:
    		ret = BSP_disp_sprite_set_format(ubuffer[0], (__disp_pixel_fmt_t)ubuffer[1], (__disp_pixel_seq_t)ubuffer[2]);
    		break;

    	case DISP_CMD_SPRITE_GLOBAL_ALPHA_ENABLE:
    		ret = BSP_disp_sprite_alpha_enable(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_GLOBAL_ALPHA_DISABLE:
    		ret = BSP_disp_sprite_alpha_disable(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_GET_GLOBAL_ALPHA_ENABLE:
    		ret = BSP_disp_sprite_get_alpha_enable(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_SET_GLOBAL_ALPHA_VALUE:
    		ret = BSP_disp_sprite_set_alpha_vale(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_GET_GLOBAL_ALPHA_VALUE:
    		ret = BSP_disp_sprite_get_alpha_value(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_SET_ORDER:
    		ret = BSP_disp_sprite_set_order(ubuffer[0], ubuffer[1],ubuffer[2]);
    		break;

    	case DISP_CMD_SPRITE_GET_TOP_BLOCK:
    		ret = BSP_disp_sprite_get_top_block(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_GET_BOTTOM_BLOCK:
    		ret = BSP_disp_sprite_get_bottom_block(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_SET_PALETTE_TBL:
            if((ubuffer[1] == 0) || ((int)ubuffer[3] <= 0))
            {
                __wrn("para invalid in display ioctrl DISP_CMD_SPRITE_SET_PALETTE_TBL,buffer:0x%x, size:0x%x\n", (unsigned int)ubuffer[1], (unsigned int)ubuffer[3]);
                return -1;
            }
    		if(copy_from_user(gbuffer, (void __user *)ubuffer[1],ubuffer[3]))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret =  BSP_disp_sprite_set_palette_table(ubuffer[0], (__u32 * )gbuffer,ubuffer[2],ubuffer[3]);
    		break;

    	case DISP_CMD_SPRITE_GET_BLOCK_NUM:
    		ret = BSP_disp_sprite_get_block_number(ubuffer[0]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_REQUEST:
    	{
    	    __disp_sprite_block_para_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[1],sizeof(__disp_sprite_block_para_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_sprite_block_request(ubuffer[0], &para);
    		break;
        }

    	case DISP_CMD_SPRITE_BLOCK_RELEASE:
    		ret = BSP_disp_sprite_block_release(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_SET_SCREEN_WINDOW:
    	{
    	    __disp_rect_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_rect_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_sprite_block_set_screen_win(ubuffer[0], ubuffer[1],&para);
    		break;
        }

    	case DISP_CMD_SPRITE_BLOCK_GET_SCREEN_WINDOW:
    	{
    	    __disp_rect_t para;

    		ret = BSP_disp_sprite_block_get_srceen_win(ubuffer[0], ubuffer[1],&para);
    		if(copy_to_user((void __user *)ubuffer[2],&para, sizeof(__disp_rect_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    	case DISP_CMD_SPRITE_BLOCK_SET_SOURCE_WINDOW:
    	{
    	    __disp_rect_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_rect_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_sprite_block_set_src_win(ubuffer[0], ubuffer[1],&para);
    		break;
        }

    	case DISP_CMD_SPRITE_BLOCK_GET_SOURCE_WINDOW:
    	{
    	    __disp_rect_t para;

    		ret = BSP_disp_sprite_block_get_src_win(ubuffer[0], ubuffer[1],&para);
    		if(copy_to_user((void __user *)ubuffer[2],&para, sizeof(__disp_rect_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    	case DISP_CMD_SPRITE_BLOCK_SET_FB:
    	{
    	    __disp_fb_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_fb_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_sprite_block_set_framebuffer(ubuffer[0], ubuffer[1],&para);
    		break;
        }

    	case DISP_CMD_SPRITE_BLOCK_GET_FB:
    	{
    	    __disp_fb_t para;

    		ret = BSP_disp_sprite_block_get_framebufer(ubuffer[0], ubuffer[1],&para);
    		if(copy_to_user((void __user *)ubuffer[2],&para, sizeof(__disp_fb_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }

    	case DISP_CMD_SPRITE_BLOCK_SET_TOP:
    		ret = BSP_disp_sprite_block_set_top(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_SET_BOTTOM:
    		ret = BSP_disp_sprite_block_set_bottom(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_GET_PREV_BLOCK:
    		ret = BSP_disp_sprite_block_get_pre_block(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_GET_NEXT_BLOCK:
    		ret = BSP_disp_sprite_block_get_next_block(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_GET_PRIO:
    		ret = BSP_disp_sprite_block_get_prio(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_OPEN:
    		ret = BSP_disp_sprite_block_open(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_CLOSE:
    		ret = BSP_disp_sprite_block_close(ubuffer[0], ubuffer[1]);
    		break;

    	case DISP_CMD_SPRITE_BLOCK_SET_PARA:
    	{
    	    __disp_sprite_block_para_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[2],sizeof(__disp_sprite_block_para_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
    		ret = BSP_disp_sprite_block_set_para(ubuffer[0], ubuffer[1],&para);
    		break;
        }

    	case DISP_CMD_SPRITE_BLOCK_GET_PARA:
    	{
    	    __disp_sprite_block_para_t para;

    		ret = BSP_disp_sprite_block_get_para(ubuffer[0], ubuffer[1],&para);
    		if(copy_to_user((void __user *)ubuffer[2],&para, sizeof(__disp_sprite_block_para_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
    		break;
        }
#ifdef __LINUX_OSAL__
	//----framebuffer----
    	case DISP_CMD_FB_REQUEST:
    	{
    	    __disp_fb_create_para_t para;

    		if(copy_from_user(&para, (void __user *)ubuffer[1],sizeof(__disp_fb_create_para_t)))
    		{
    		    __wrn("copy_from_user fail %d \n",__LINE__);
    			return  -EFAULT;
    		}
			ret = Display_Fb_Request(ubuffer[0], &para);
			break;
        }

		case DISP_CMD_FB_RELEASE:
			ret = Display_Fb_Release(ubuffer[0]);
			break;

	    case DISP_CMD_FB_GET_PARA:
	    {
    	    __disp_fb_create_para_t para;

			ret = Display_Fb_get_para(ubuffer[0], &para);
    		if(copy_to_user((void __user *)ubuffer[1],&para, sizeof(__disp_fb_create_para_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
			break;
        }

	    case DISP_CMD_GET_DISP_INIT_PARA:
	    {
    	    __disp_init_t para;

			ret = Display_get_disp_init_para(&para);
    		if(copy_to_user((void __user *)ubuffer[0],&para, sizeof(__disp_init_t)))
    		{
    		    __wrn("copy_to_user fail\n");
    			return  -EFAULT;
    		}
			break;
        }
//----for test----
		case DISP_CMD_MEM_REQUEST:
			ret =  disp_mem_request(ubuffer[0],ubuffer[1]);
			break;

	
		case DISP_CMD_MEM_RELASE:
			ret =  disp_mem_release(ubuffer[0]);
			break;

		case DISP_CMD_MEM_SELIDX:
			g_disp_mm_sel = ubuffer[0];
			break;

		case DISP_CMD_MEM_GETADR:
			ret = g_disp_mm[ubuffer[0]].mem_start;
			break;

		case DISP_CMD_SUSPEND:
		{
		    //pm_message_t state;

			//ret = disp_suspend(0, state);
			break;
        }

		case DISP_CMD_RESUME:
			//ret = disp_resume(0);
			break;
#endif
		case DISP_CMD_SET_EXIT_MODE:
	    	ret =  g_disp_drv.exit_mode = ubuffer[0];
			break;

		case DISP_CMD_LCD_CHECK_OPEN_FINISH:
			ret = DRV_lcd_check_open_finished(ubuffer[0]);
			break;
		
        case DISP_CMD_PRINT_REG:
            ret = BSP_disp_print_reg(1, ubuffer[0]);
            break;

		default:
		    break;
    }

	return ret;
}


