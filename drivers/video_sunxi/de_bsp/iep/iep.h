
#ifndef __EBSP_IEP_H__
#define __EBSP_IEP_H__

#include "../bsp_display.h"

typedef enum
{
    IEP_DRC_MODE_UI,
    IEP_DRC_MODE_VIDEO,
}__iep_drc_mode_t;

typedef struct
{
    __bool                  b_interlace_out;
    __bool                  b_trd_out;
    __disp_3d_out_mode_t     trd_out_mode;
    __disp_rectsz_t         in_size; //single size
    __disp_rectsz_t         out_size;//single size
    __disp_rectsz_t         disp_size;//full size out
    __disp_cs_mode_t        csc_mode;
}__disp_frame_info_t;

//CMU:
extern __u32 IEP_CMU_Set_Reg_Base(__u32 sel, __u32 address);
extern __u32 IEP_CMU_Reg_Init(__u32 sel);
extern __s32 IEP_CMU_Enable(__u32 sel, __u32 enable);
extern __s32 IEP_CMU_Disable(__u32 sel);
extern __s32 IEP_CMU_Set_Data_Flow(__u8 sel, __u32 data_flow);
extern __s32 IEP_CMU_Set_Window(__u8 sel, __disp_rect_t *window);
extern __s32 IEP_CMU_Set_Imgsize(__u8 sel, __u32 width, __u32 height);
extern __s32 IEP_CMU_Set_Par(__u8 sel, __u32 hue, __u32 saturaion, __u32 brightness, __u32 mode);
extern __s32 IEP_CMU_Operation_In_Vblanking(__u32 sel);
extern __s32 IEP_CMU_Early_Suspend(__u32 sel);
extern __s32 IEP_CMU_Suspend(__u32 sel);
extern __s32 IEP_CMU_Resume(__u32 sel);
extern __s32 IEP_CMU_Late_Resume(__u32 sel);

//DRC
extern __s32 IEP_Drc_Init(__u32 sel);
extern __s32 IEP_Drc_Exit(__u32 sel);
extern __s32 IEP_Drc_Enable(__u32 sel, __u32 en);
extern __s32 IEP_Drc_Operation_In_Vblanking(__u32 sel);
extern __s32 IEP_Drc_Set_Reg_Base(__u32 sel, __u32 base);
extern __s32 IEP_Drc_Get_Reg_Base(__u32 sel);
extern __s32 IEP_Drc_Set_Winodw(__u32 sel, __disp_rect_t window);//full screen for default
extern __s32 IEP_Drc_Set_Mode(__u32 sel, __iep_drc_mode_t mode);
extern __s32 IEP_Drc_Early_Suspend(__u32 sel);//close clk
extern __s32 IEP_Drc_suspend(__u32 sel);//save register
extern __s32 IEP_Drc_Resume (__u32 sel);//restore register
extern __s32 IEP_Drc_Late_Resume(__u32 sel);//open clk
extern __s32 IEP_Drc_Set_Imgsize(__u32 sel, __u32 width, __u32 height);

//DEU
extern __s32 IEP_Deu_Enable(__u32 sel, __u32 enable);
extern __s32 IEP_Deu_Set_Luma_Sharpness_Level(__u32 sel, __u32 level);
extern __s32 IEP_Deu_Set_Chroma_Sharpness_Level(__u32 sel, __u32 level);
extern __s32 IEP_Deu_Set_White_Level_Extension(__u32 sel, __u32 level);
extern __s32 IEP_Deu_Set_Black_Level_Extension(__u32 sel, __u32 level);
extern __s32 IEP_Deu_Set_Ready(__u32 sel);
extern __s32 IEP_Deu_Set_Reg_base(__u32 sel, __u32 base);
extern __s32 IEP_Deu_Set_Winodw(__u32 sel, __disp_rect_t *window);
extern __s32 IEP_Deu_Output_Select(__u32 sel, __u32 be_ch);
extern __s32 IEP_Deu_Init(__u32 sel); 
extern __s32 IEP_Deu_Exit(__u32 sel);
extern __s32 IEP_Deu_Operation_In_Vblanking(__u32 sel);
extern __s32 IEP_Deu_Early_Suspend(__u32 sel);//close clk
extern __s32 IEP_Deu_suspend(__u32 sel);//save register
extern __s32 IEP_Deu_Resume (__u32 sel);//restore register
extern __s32 IEP_Deu_Late_Resume(__u32 sel);//open clk
extern __s32 IEP_Deu_Set_frameinfo(__u32 sel, __disp_frame_info_t frameinfo);


#endif
