
#ifndef __EBSP_DISPLAY_H__
#define __EBSP_DISPLAY_H__

//#define __LINUX_OSAL__
//#define __MELIS_OSAL__
//#define __WINCE_OSAL__
//#define __BOOT_OSAL__
#define __UBOOT_OSAL__

#ifdef __LINUX_OSAL__
#include "linux/kernel.h"
#include "linux/mm.h"
#include <asm/uaccess.h>
#include <asm/memory.h>
#include <asm/unistd.h>
#include "linux/semaphore.h"
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <linux/fb.h>
#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()¡¢kthread_run()
#include <linux/err.h> //IS_ERR()¡¢PTR_ERR()
#include <linux/delay.h>
#include <linux/platform_device.h>
#include "asm-generic/int-ll64.h"
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <mach/sys_config.h>
#include <mach/clock.h>
#include <mach/aw_ccu.h>
#include <mach/system.h>
#include <linux/types.h>
#include <linux/timer.h>


typedef unsigned int __hdle;

#include <linux/drv_display.h>
#include "../OSAL/OSAL.h"

#if 1
#define OSAL_PRINTF(msg...) {printk(KERN_WARNING "[DISP] ");printk(msg);}
#define __inf(msg...)
#define __msg(msg...)
#define __wrn(msg...)       {printk(KERN_WARNING "[DISP WRN] file:%s,line:%d:    ",__FILE__,__LINE__);printk(msg);}
#define __here__
#else
#define OSAL_PRINTF(msg...) {printk(KERN_WARNING "[DISP] ");printk(msg);}
#define __inf(msg...)       {printk(KERN_WARNING "[DISP] ");printk(msg);}
#define __msg(msg...)       {printk(KERN_WARNING "[DISP] file:%s,line:%d:    ",__FILE__,__LINE__);printk(msg);}
#define __wrn(msg...)       {printk(KERN_WARNING "[DISP WRN] file:%s,line:%d:    ",__FILE__,__LINE__);printk(msg);}
#define __here__            {printk(KERN_WARNING "[DISP] file:%s,line:%d\n",__FILE__,__LINE__);}
#endif


#endif//end of define __LINUX_OSAL__

#ifdef __MELIS_OSAL__
#include "string.h"
#include "D:/winners/eBase/eBSP/BSP/sun_20/common_inc.h"
#endif

#ifdef __BOOT_OSAL__
#define OSAL_PRINTF wlibc_uprintf

#include "egon2.h"
#include "string.h"
#include "../OSAL/OSAL_De.h"
#endif


#ifdef __UBOOT_OSAL__
#include <common.h>
#include "../drv_display_sun4i.h"
#include <asm/arch/sys_config.h>

#define OSAL_PRINTF         
#define __inf               
#define __msg               
#define __wrn               
#define __here__     
typedef unsigned int __hdle;

#include "../OSAL/OSAL.h"

typedef enum __AW_CCU_SYS_CLK
{
    AW_SYS_CLK_NONE,    /* invalid clock id                     */

    AW_SYS_CLK_LOSC,    /* "losc"       ,LOSC, 32768 hz clock   */
    AW_SYS_CLK_HOSC,    /* "hosc"       ,HOSC, 24Mhz clock      */

    AW_SYS_CLK_PLL1,    /* "core_pll"   ,PLL1 clock             */
    AW_SYS_CLK_PLL2,    /* "audio_pll"  ,PLL2 clock             */
    AW_SYS_CLK_PLL2X8,  /* "audio_pllx8"  ,PLL2 8x clock        */
    AW_SYS_CLK_PLL3,    /* "video_pll0" ,PLL3 clock             */
    AW_SYS_CLK_PLL3X2,  /* "video_pll0x2" ,PLL3 2x clock        */
    AW_SYS_CLK_PLL4,    /* "ve_pll"     ,PLL4 clock             */
    AW_SYS_CLK_PLL5,    /* "sdram_pll"  ,PLL5 clock             */
    AW_SYS_CLK_PLL5M,   /* "sdram_pll_m",PLL5 M clock           */
    AW_SYS_CLK_PLL5P,   /* "sdram_pll_p",PLL5 P clock           */
    AW_SYS_CLK_PLL6,    /* "sata_pll"   ,PLL6 clock, just used
                           as source of sata_pll_m and sata_pll_2,
                           users should not use this clock dirctly
                        */
    AW_SYS_CLK_PLL7,    /* "video_pll1" ,PLL7 clock             */
    AW_SYS_CLK_PLL7X2,  /* "video_pll1x2" ,PLL7 2x clock        */
    AW_SYS_CLK_200M,    /* "200m_pll"   ,200Mhz clock           */

    AW_SYS_CLK_CPU,     /* "cpu"        ,CPU clock              */
    AW_SYS_CLK_AXI,     /* "axi"        ,AXI clock              */
    AW_SYS_CLK_AHB,     /* "ahb"        ,AHB clock              */
    AW_SYS_CLK_APB0,    /* "apb"        ,APB0 clock             */
    AW_SYS_CLK_APB1,    /* "apb1"       ,APB1 clock             */

    /* add by kevin, 2011-7-21 19:01 */
    AW_SYS_CLK_PLL6M,   /* "sata_pll_m" ,PLL6 M clock, just for SATA    */
    AW_SYS_CLK_PLL62,   /* "sata_pll_2" ,PLL6 2 clock, for module       */

    AW_SYS_CLK_CNT      /* invalid id, for calc count           */

} __aw_ccu_sys_clk_e;


/* define module clock id       */
typedef enum __AW_CCU_MOD_CLK
{
    AW_MOD_CLK_NONE,/* invalid clock id             */

    AW_MOD_CLK_NFC,         /* "nfc"            */
    AW_MOD_CLK_MSC,         /* "msc"            */
    AW_MOD_CLK_SDC0,        /* "sdc0"           */
    AW_MOD_CLK_SDC1,        /* "sdc1"           */
    AW_MOD_CLK_SDC2,        /* "sdc2"           */
    AW_MOD_CLK_SDC3,        /* "sdc3"           */
    AW_MOD_CLK_TS,          /* "ts"             */
    AW_MOD_CLK_SS,          /* "ss"             */
    AW_MOD_CLK_SPI0,        /* "spi0"           */
    AW_MOD_CLK_SPI1,        /* "spi1"           */
    AW_MOD_CLK_SPI2,        /* "spi2"           */
    AW_MOD_CLK_PATA,        /* "pata"           */
    AW_MOD_CLK_IR0,         /* "ir0"            */
    AW_MOD_CLK_IR1,         /* "ir1"            */
    AW_MOD_CLK_I2S,         /* "i2s"            */
    AW_MOD_CLK_AC97,        /* "ac97"           */
    AW_MOD_CLK_SPDIF,       /* "spdif"          */
    AW_MOD_CLK_KEYPAD,      /* "key_pad"        */
    AW_MOD_CLK_SATA,        /* "sata"           */
    AW_MOD_CLK_USBPHY,      /* "usb_phy"        */
    AW_MOD_CLK_USBPHY0,     /* "usb_phy0"       */
    AW_MOD_CLK_USBPHY1,     /* "usb_phy1"       */
    AW_MOD_CLK_USBPHY2,     /* "usb_phy2"       */
    AW_MOD_CLK_USBOHCI0,    /* "usb_ohci0"      */
    AW_MOD_CLK_USBOHCI1,    /* "usb_ohci1"      */
    AW_MOD_CLK_GPS,         /* "com"            */
    AW_MOD_CLK_SPI3,        /* "spi3"           */
    AW_MOD_CLK_DEBE0,       /* "de_image0"      */
    AW_MOD_CLK_DEBE1,       /* "de_image1"      */
    AW_MOD_CLK_DEFE0,       /* "de_scale0"      */
    AW_MOD_CLK_DEFE1,       /* "de_scale1"      */
    AW_MOD_CLK_DEMIX,       /* "de_mix"         */
    AW_MOD_CLK_LCD0CH0,     /* "lcd0_ch0"       */
    AW_MOD_CLK_LCD1CH0,     /* "lcd1_ch0"       */
    AW_MOD_CLK_CSIISP,      /* "csi_isp"        */
    AW_MOD_CLK_TVD,         /* "tvd"            */
    AW_MOD_CLK_LCD0CH1_S1,  /* "lcd0_ch1_s1"    */
    AW_MOD_CLK_LCD0CH1_S2,  /* "lcd0_ch1_s2"    */
    AW_MOD_CLK_LCD1CH1_S1,  /* "lcd1_ch1_s1"    */
    AW_MOD_CLK_LCD1CH1_S2,  /* "lcd1_ch1_s2"    */
    AW_MOD_CLK_CSI0,        /* "csi0"           */
    AW_MOD_CLK_CSI1,        /* "csi1"           */
    AW_MOD_CLK_VE,          /* "ve"             */
    AW_MOD_CLK_ADDA,        /* "audio_codec"    */
    AW_MOD_CLK_AVS,         /* "avs"            */
    AW_MOD_CLK_ACE,         /* "ace"            */
    AW_MOD_CLK_LVDS,        /* "lvds"           */
    AW_MOD_CLK_HDMI,        /* "hdmi"           */
    AW_MOD_CLK_MALI,        /* "mali"           */
    AW_MOD_CLK_TWI0,        /* "twi0"           */
    AW_MOD_CLK_TWI1,        /* "twi1"           */
    AW_MOD_CLK_TWI2,        /* "twi2"           */
    AW_MOD_CLK_CAN,         /* "can"            */
    AW_MOD_CLK_SCR,         /* "scr"            */
    AW_MOD_CLK_PS20,        /* "ps0"            */
    AW_MOD_CLK_PS21,        /* "ps1"            */
    AW_MOD_CLK_UART0,       /* "uart0"          */
    AW_MOD_CLK_UART1,       /* "uart1"          */
    AW_MOD_CLK_UART2,       /* "uart2"          */
    AW_MOD_CLK_UART3,       /* "uart3"          */
    AW_MOD_CLK_UART4,       /* "uart4"          */
    AW_MOD_CLK_UART5,       /* "uart5"          */
    AW_MOD_CLK_UART6,       /* "uart6"          */
    AW_MOD_CLK_UART7,       /* "uart7"          */

    /* clock gating for hang to AXI bus */
    AW_MOD_CLK_AXI_DRAM,    /* "axi_dram"       */

    /* clock gating for hang to AHB bus */
    AW_MOD_CLK_AHB_USB0,    /* "ahb_usb0"       */
    AW_MOD_CLK_AHB_EHCI0,   /* "ahb_ehci0"      */
    AW_MOD_CLK_AHB_OHCI0,   /* "ahb_ohci0"      */
    AW_MOD_CLK_AHB_SS,      /* "ahb_ss"         */
    AW_MOD_CLK_AHB_DMA,     /* "ahb_dma"        */
    AW_MOD_CLK_AHB_BIST,    /* "ahb_bist"       */
    AW_MOD_CLK_AHB_SDMMC0,  /* "ahb_sdc0"       */
    AW_MOD_CLK_AHB_SDMMC1,  /* "ahb_sdc1"       */
    AW_MOD_CLK_AHB_SDMMC2,  /* "ahb_sdc2"       */
    AW_MOD_CLK_AHB_SDMMC3,  /* "ahb_sdc3"       */
    AW_MOD_CLK_AHB_MS,      /* "ahb_msc"        */
    AW_MOD_CLK_AHB_NAND,    /* "ahb_nfc"        */
    AW_MOD_CLK_AHB_SDRAM,   /* "ahb_sdramc"     */
    AW_MOD_CLK_AHB_ACE,     /* "ahb_ace"        */
    AW_MOD_CLK_AHB_EMAC,    /* "ahb_emac"       */
    AW_MOD_CLK_AHB_TS,      /* "ahb_ts"         */
    AW_MOD_CLK_AHB_SPI0,    /* "ahb_spi0"       */
    AW_MOD_CLK_AHB_SPI1,    /* "ahb_spi1"       */
    AW_MOD_CLK_AHB_SPI2,    /* "ahb_spi2"       */
    AW_MOD_CLK_AHB_SPI3,    /* "ahb_spi3"       */
    AW_MOD_CLK_AHB_PATA,    /* "ahb_pata"       */
    AW_MOD_CLK_AHB_SATA,    /* "ahb_sata"       */
    AW_MOD_CLK_AHB_GPS,     /* "ahb_com"        */
    AW_MOD_CLK_AHB_VE,      /* "ahb_ve"         */
    AW_MOD_CLK_AHB_TVD,     /* "ahb_tvd"        */
    AW_MOD_CLK_AHB_TVE0,    /* "ahb_tve0"       */
    AW_MOD_CLK_AHB_TVE1,    /* "ahb_tve1"       */
    AW_MOD_CLK_AHB_LCD0,    /* "ahb_lcd0"       */
    AW_MOD_CLK_AHB_LCD1,    /* "ahb_lcd1"       */
    AW_MOD_CLK_AHB_CSI0,    /* "ahb_csi0"       */
    AW_MOD_CLK_AHB_CSI1,    /* "ahb_csi1"       */
    AW_MOD_CLK_AHB_HDMI,    /* "ahb_hdmi"       */
    AW_MOD_CLK_AHB_DEBE0,   /* "ahb_de_image0"  */
    AW_MOD_CLK_AHB_DEBE1,   /* "ahb_de_image1"  */
    AW_MOD_CLK_AHB_DEFE0,   /* "ahb_de_scale0"  */
    AW_MOD_CLK_AHB_DEFE1,   /* "ahb_de_scale1"  */
    AW_MOD_CLK_AHB_MP,      /* "ahb_de_mix"     */
    AW_MOD_CLK_AHB_MALI,    /* "ahb_mali"       */

    /* clock gating for hang APB bus */
    AW_MOD_CLK_APB_ADDA,    /* "apb_audio_codec"    */
    AW_MOD_CLK_APB_SPDIF,   /* "apb_spdif"          */
    AW_MOD_CLK_APB_AC97,    /* "apb_ac97"           */
    AW_MOD_CLK_APB_I2S,     /* "apb_i2s"            */
    AW_MOD_CLK_APB_PIO,     /* "apb_pio"            */
    AW_MOD_CLK_APB_IR0,     /* "apb_ir0"            */
    AW_MOD_CLK_APB_IR1,     /* "apb_ir1"            */
    AW_MOD_CLK_APB_KEYPAD,  /* "apb_key_pad"        */
    AW_MOD_CLK_APB_TWI0,    /* "apb_twi0"           */
    AW_MOD_CLK_APB_TWI1,    /* "apb_twi1"           */
    AW_MOD_CLK_APB_TWI2,    /* "apb_twi2"           */
    AW_MOD_CLK_APB_CAN,     /* "apb_can"            */
    AW_MOD_CLK_APB_SCR,     /* "apb_scr"            */
    AW_MOD_CLK_APB_PS20,    /* "apb_ps0"            */
    AW_MOD_CLK_APB_PS21,    /* "apb_ps1"            */
    AW_MOD_CLK_APB_UART0,   /* "apb_uart0"          */
    AW_MOD_CLK_APB_UART1,   /* "apb_uart1"          */
    AW_MOD_CLK_APB_UART2,   /* "apb_uart2"          */
    AW_MOD_CLK_APB_UART3,   /* "apb_uart3"          */
    AW_MOD_CLK_APB_UART4,   /* "apb_uart4"          */
    AW_MOD_CLK_APB_UART5,   /* "apb_uart5"          */
    AW_MOD_CLK_APB_UART6,   /* "apb_uart6"          */
    AW_MOD_CLK_APB_UART7,   /* "apb_uart7"          */

    /* clock gating for access dram */
    AW_MOD_CLK_SDRAM_VE,    /* "sdram_ve"           */
    AW_MOD_CLK_SDRAM_CSI0,  /* "sdram_csi0"         */
    AW_MOD_CLK_SDRAM_CSI1,  /* "sdram_csi1"         */
    AW_MOD_CLK_SDRAM_TS,    /* "sdram_ts"           */
    AW_MOD_CLK_SDRAM_TVD,   /* "sdram_tvd"          */
    AW_MOD_CLK_SDRAM_TVE0,  /* "sdram_tve0"         */
    AW_MOD_CLK_SDRAM_TVE1,  /* "sdram_tve1"         */
    AW_MOD_CLK_SDRAM_DEFE0, /* "sdram_de_scale0"    */
    AW_MOD_CLK_SDRAM_DEFE1, /* "sdram_de_scale1"    */
    AW_MOD_CLK_SDRAM_DEBE0, /* "sdram_de_image0"    */
    AW_MOD_CLK_SDRAM_DEBE1, /* "sdram_de_image1"    */
    AW_MOD_CLK_SDRAM_DEMP,  /* "sdram_de_mix"       */
    AW_MOD_CLK_SDRAM_ACE,   /* "sdram_ace"          */

    AW_MOD_CLK_AHB_EHCI1,   /* "ahb_ehci1"          */
    AW_MOD_CLK_AHB_OHCI1,   /* "ahb_ohci1"          */

    AW_MOD_CLK_CNT

} __aw_ccu_mod_clk_e;

#ifndef abs
#define abs(x) ((x&0x80000000)? (0-x):(x))
#endif
#endif

typedef struct
{
	__u32 base_image0;
	__u32 base_image1;
	__u32 base_scaler0;
	__u32 base_scaler1;
	__u32 base_lcdc0;
	__u32 base_lcdc1;
	__u32 base_tvec0;
	__u32 base_tvec1;
	__u32 base_pioc;
	__u32 base_sdram;
	__u32 base_ccmu;
	__u32 base_pwm;

	void (*tve_interrup) (__u32 sel);
	__s32 (*hdmi_set_mode)(__disp_tv_mode_t mode);
	__s32 (*Hdmi_open)(void);
	__s32 (*Hdmi_close)(void);
	__s32 (*hdmi_mode_support)(__disp_tv_mode_t mode);
	__s32 (*hdmi_get_HPD_status)(void);
	__s32 (*hdmi_set_pll)(__u32 pll, __u32 clk);
	__s32 (*disp_int_process)(__u32 sel);
}__disp_bsp_init_para;


extern __s32 BSP_disp_clk_on(__u32 type);
extern __s32 BSP_disp_clk_off(__u32 type);
extern __s32 BSP_disp_init(__disp_bsp_init_para * para);
extern __s32 BSP_disp_exit(__u32 mode);
extern __s32 BSP_disp_open(void);
extern __s32 BSP_disp_close(void);
extern __s32 BSP_disp_print_reg(__bool b_force_on, __u32 id);
extern __s32 BSP_disp_cmd_cache(__u32 sel);
extern __s32 BSP_disp_cmd_submit(__u32 sel);
extern __s32 BSP_disp_set_bk_color(__u32 sel, __disp_color_t *color);
extern __s32 BSP_disp_get_bk_color(__u32 sel, __disp_color_t *color);
extern __s32 BSP_disp_set_color_key(__u32 sel, __disp_colorkey_t *ck_mode);
extern __s32 BSP_disp_get_color_key(__u32 sel, __disp_colorkey_t *ck_mode);
extern __s32 BSP_disp_set_palette_table(__u32 sel, __u32 *pbuffer, __u32 offset, __u32 size);
extern __s32 BSP_disp_get_palette_table(__u32 sel, __u32 * pbuffer, __u32 offset,__u32 size);
extern __s32 BSP_disp_get_screen_height(__u32 sel);
extern __s32 BSP_disp_get_screen_width(__u32 sel);
extern __s32 BSP_disp_get_output_type(__u32 sel);
extern __s32 BSP_disp_get_frame_rate(__u32 sel);
extern __s32 BSP_disp_gamma_correction_enable(__u32 sel);
extern __s32 BSP_disp_gamma_correction_disable(__u32 sel);
extern __s32 BSP_disp_set_bright(__u32 sel, __u32 bright);
extern __s32 BSP_disp_get_bright(__u32 sel);
extern __s32 BSP_disp_set_contrast(__u32 sel, __u32 contrast);
extern __s32 BSP_disp_get_contrast(__u32 sel);
extern __s32 BSP_disp_set_saturation(__u32 sel, __u32 saturation);
extern __s32 BSP_disp_get_saturation(__u32 sel);
extern __s32 BSP_disp_set_hue(__u32 sel, __u32 hue);
extern __s32 BSP_disp_get_hue(__u32 sel);
extern __s32 BSP_disp_enhance_enable(__u32 sel, __bool enable);
extern __s32 BSP_disp_get_enhance_enable(__u32 sel);
extern __s32 BSP_disp_capture_screen(__u32 sel, __disp_capture_screen_para_t * para);
extern __s32 BSP_disp_set_screen_size(__u32 sel, __disp_rectsz_t * size);
extern __s32 BSP_disp_set_output_csc(__u32 sel, __disp_output_type_t type);
extern __s32 BSP_disp_de_flicker_enable(__u32 sel, __bool b_en);

extern __s32 BSP_disp_layer_request(__u32 sel, __disp_layer_work_mode_t mode);
extern __s32 BSP_disp_layer_release(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_open(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_close(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_set_framebuffer(__u32 sel, __u32 hid,__disp_fb_t *fbinfo);
extern __s32 BSP_disp_layer_get_framebuffer(__u32 sel, __u32 hid,__disp_fb_t*fbinfo);
extern __s32 BSP_disp_layer_set_src_window(__u32 sel, __u32 hid,__disp_rect_t *regn);
extern __s32 BSP_disp_layer_get_src_window(__u32 sel, __u32 hid,__disp_rect_t *regn);
extern __s32 BSP_disp_layer_set_screen_window(__u32 sel, __u32 hid,__disp_rect_t* regn);
extern __s32 BSP_disp_layer_get_screen_window(__u32 sel, __u32 hid,__disp_rect_t *regn);
extern __s32 BSP_disp_layer_set_para(__u32 sel, __u32 hid, __disp_layer_info_t * layer_para);
extern __s32 BSP_disp_layer_get_para(__u32 sel, __u32 hid, __disp_layer_info_t * layer_para);
extern __s32 BSP_disp_layer_set_top(__u32 sel, __u32  handle);
extern __s32 BSP_disp_layer_set_bottom(__u32 sel, __u32  handle);
extern __s32 BSP_disp_layer_set_alpha_value(__u32 sel, __u32 hid,__u8 value);
extern __s32 BSP_disp_layer_get_alpha_value(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_alpha_enable(__u32 sel, __u32 hid, __bool enable);
extern __s32 BSP_disp_layer_get_alpha_enable(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_set_pipe(__u32 sel, __u32 hid,__u8 pipe);
extern __s32 BSP_disp_layer_get_pipe(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_get_piro(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_colorkey_enable(__u32 sel, __u32 hid, __bool enable);
extern __s32 BSP_disp_layer_get_colorkey_enable(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_set_smooth(__u32 sel, __u32 hid, __disp_video_smooth_t  mode);
extern __s32 BSP_disp_layer_get_smooth(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_set_bright(__u32 sel, __u32 hid, __u32 bright);
extern __s32 BSP_disp_layer_set_contrast(__u32 sel, __u32 hid, __u32 contrast);
extern __s32 BSP_disp_layer_set_saturation(__u32 sel, __u32 hid, __u32 saturation);
extern __s32 BSP_disp_layer_set_hue(__u32 sel, __u32 hid, __u32 hue);
extern __s32 BSP_disp_layer_enhance_enable(__u32 sel, __u32 hid, __bool enable);
extern __s32 BSP_disp_layer_get_bright(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_get_contrast(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_get_saturation(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_get_hue(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_get_enhance_enable(__u32 sel, __u32 hid);

extern __s32 BSP_disp_layer_vpp_enable(__u32 sel, __u32 hid, __bool enable);
extern __s32 BSP_disp_layer_get_vpp_enable(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_set_luma_sharp_level(__u32 sel, __u32 hid, __u32 level);
extern __s32 BSP_disp_layer_get_luma_sharp_level(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_set_chroma_sharp_level(__u32 sel, __u32 hid, __u32 level);
extern __s32 BSP_disp_layer_get_chroma_sharp_level(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_set_white_exten_level(__u32 sel, __u32 hid, __u32 level);
extern __s32 BSP_disp_layer_get_white_exten_level(__u32 sel, __u32 hid);
extern __s32 BSP_disp_layer_set_black_exten_level(__u32 sel, __u32 hid, __u32 level);
extern __s32 BSP_disp_layer_get_black_exten_level(__u32 sel, __u32 hid);

extern __s32 BSP_disp_scaler_get_smooth(__u32 sel);
extern __s32 BSP_disp_scaler_set_smooth(__u32 sel, __disp_video_smooth_t  mode);
extern __s32 BSP_disp_scaler_request(void);
extern __s32 BSP_disp_scaler_release(__u32 handle);
extern __s32 BSP_disp_scaler_start(__u32 handle,__disp_scaler_para_t *scl);
extern __s32 BSP_disp_scaler_start_ex(__u32 handle,__disp_scaler_para_t *scl);

extern __s32 BSP_disp_hwc_enable(__u32 sel, __bool enable);
extern __s32 BSP_disp_hwc_set_pos(__u32 sel, __disp_pos_t *pos);
extern __s32 BSP_disp_hwc_get_pos(__u32 sel, __disp_pos_t *pos);
extern __s32 BSP_disp_hwc_set_framebuffer(__u32 sel, __disp_hwc_pattern_t *patmem);
extern __s32 BSP_disp_hwc_set_palette(__u32 sel, void *palette,__u32 offset, __u32 palette_size);

extern __s32 BSP_disp_video_set_fb(__u32 sel, __u32 hid, __disp_video_fb_t *in_addr);
extern __s32 BSP_disp_video_get_frame_id(__u32 sel, __u32 hid);
extern __s32 BSP_disp_video_get_dit_info(__u32 sel, __u32 hid, __disp_dit_info_t * dit_info);
extern __s32 BSP_disp_video_start(__u32 sel, __u32 hid);
extern __s32 BSP_disp_video_stop(__u32 sel, __u32 hid);

extern __s32 BSP_disp_lcd_open_before(__u32 sel);
extern __s32 BSP_disp_lcd_open_after(__u32 sel);
extern __lcd_flow_t * BSP_disp_lcd_get_open_flow(__u32 sel);
extern __s32 BSP_disp_lcd_close_befor(__u32 sel);
extern __s32 BSP_disp_lcd_close_after(__u32 sel);
extern __lcd_flow_t * BSP_disp_lcd_get_close_flow(__u32 sel);
extern __s32 BSP_disp_lcd_xy_switch(__u32 sel, __s32 mode);
extern __s32 BSP_disp_set_gamma_table(__u32 sel, __u32 *gamtbl_addr,__u32 gamtbl_size);
extern __s32 BSP_disp_lcd_set_bright(__u32 sel, __disp_lcd_bright_t  bright);
extern __s32 BSP_disp_lcd_get_bright(__u32 sel);
extern __s32 BSP_disp_lcd_set_src(__u32 sel, __disp_lcdc_src_t src);
extern __s32 LCD_PWM_EN(__u32 sel, __bool b_en);
extern __s32 LCD_BL_EN(__u32 sel, __bool b_en);
extern __s32 BSP_disp_lcd_user_defined_func(__u32 sel, __u32 para1, __u32 para2, __u32 para3);
extern __s32 pwm_set_para(__u32 channel, __pwm_info_t * pwm_info);
extern __s32 pwm_get_para(__u32 channel, __pwm_info_t * pwm_info);
extern __s32 BSP_disp_get_timming(__u32 sel, __disp_tcon_timing_t * tt);
extern __u32 BSP_disp_get_cur_line(__u32 sel);
extern __s32 BSP_disp_close_lcd_backlight(__u32 sel);

extern __s32 BSP_disp_tv_open(__u32 sel);
extern __s32 BSP_disp_tv_close(__u32 sel);
extern __s32 BSP_disp_tv_set_mode(__u32 sel, __disp_tv_mode_t tv_mod);
extern __s32 BSP_disp_tv_get_mode(__u32 sel);
extern __s32 BSP_disp_tv_get_interface(__u32 sel);
extern __s32 BSP_disp_tv_auto_check_enable(__u32 sel);
extern __s32 BSP_disp_tv_auto_check_disable(__u32 sel);
extern __s32 BSP_disp_tv_set_src(__u32 sel, __disp_lcdc_src_t src);
extern __s32 BSP_disp_tv_get_dac_status(__u32 sel, __u32 index);
extern __s32 BSP_disp_tv_set_dac_source(__u32 sel, __u32 index, __disp_tv_dac_source source);
extern __s32 BSP_disp_tv_get_dac_source(__u32 sel, __u32 index);

extern __s32 BSP_disp_hdmi_open(__u32 sel);
extern __s32 BSP_disp_hdmi_close(__u32 sel);
extern __s32 BSP_disp_hdmi_set_mode(__u32 sel, __disp_tv_mode_t  mode);
extern __s32 BSP_disp_hdmi_get_mode(__u32 sel);
extern __s32 BSP_disp_hdmi_check_support_mode(__u32 sel, __u8  mode);
extern __s32 BSP_disp_hdmi_get_hpd_status(__u32 sel);
extern __s32 BSP_disp_hdmi_set_src(__u32 sel, __disp_lcdc_src_t src);
extern __s32 BSP_disp_set_hdmi_func(__disp_hdmi_func * func);

extern __s32 BSP_disp_vga_open(__u32 sel);
extern __s32 BSP_disp_vga_close(__u32 sel);
extern __s32 BSP_disp_vga_set_mode(__u32 sel, __disp_vga_mode_t  mode);
extern __s32 BSP_disp_vga_get_mode(__u32 sel);
extern __s32 BSP_disp_vga_set_src(__u32 sel, __disp_lcdc_src_t src);

extern __s32 BSP_disp_sprite_init(__u32 sel);
extern __s32 BSP_disp_sprite_exit(__u32 sel);
extern __s32 BSP_disp_sprite_open(__u32 sel);
extern __s32 BSP_disp_sprite_close(__u32 sel);
extern __s32 BSP_disp_sprite_alpha_enable(__u32 sel);
extern __s32 BSP_disp_sprite_alpha_disable(__u32 sel);
extern __s32 BSP_disp_sprite_get_alpha_enable(__u32 sel);
extern __s32 BSP_disp_sprite_set_alpha_vale(__u32 sel, __u32 alpha);
extern __s32 BSP_disp_sprite_get_alpha_value(__u32 sel);
extern __s32 BSP_disp_sprite_set_format(__u32 sel, __disp_pixel_fmt_t format, __disp_pixel_seq_t pixel_seq);
extern __s32 BSP_disp_sprite_set_palette_table(__u32 sel, __u32 *buffer, __u32 offset, __u32 size);
extern __s32 BSP_disp_sprite_set_order(__u32 sel, __s32 hid,__s32 dst_hid);
extern __s32 BSP_disp_sprite_get_top_block(__u32 sel);
extern __s32 BSP_disp_sprite_get_bottom_block(__u32 sel);
extern __s32 BSP_disp_sprite_get_block_number(__u32 sel);
extern __s32 BSP_disp_sprite_block_request(__u32 sel, __disp_sprite_block_para_t *para);
extern __s32 BSP_disp_sprite_block_release(__u32 sel, __s32 hid);
extern __s32 BSP_disp_sprite_block_set_screen_win(__u32 sel, __s32 hid, __disp_rect_t * scn_win);
extern __s32 BSP_disp_sprite_block_get_srceen_win(__u32 sel, __s32 hid, __disp_rect_t * scn_win);
extern __s32 BSP_disp_sprite_block_set_src_win(__u32 sel, __s32 hid, __disp_rect_t * scn_win);
extern __s32 BSP_disp_sprite_block_get_src_win(__u32 sel, __s32 hid, __disp_rect_t * scn_win);
extern __s32 BSP_disp_sprite_block_set_framebuffer(__u32 sel, __s32 hid, __disp_fb_t * fb);
extern __s32 BSP_disp_sprite_block_get_framebufer(__u32 sel, __s32 hid,__disp_fb_t *fb);
extern __s32 BSP_disp_sprite_block_set_top(__u32 sel, __u32 hid);
extern __s32 BSP_disp_sprite_block_set_bottom(__u32 sel, __u32 hid);
extern __s32 BSP_disp_sprite_block_get_pre_block(__u32 sel, __u32 hid);
extern __s32 BSP_disp_sprite_block_get_next_block(__u32 sel, __u32 hid);
extern __s32 BSP_disp_sprite_block_get_prio(__u32 sel, __u32 hid);
extern __s32 BSP_disp_sprite_block_open(__u32 sel, __u32 hid);
extern __s32 BSP_disp_sprite_block_close(__u32 sel, __u32 hid);
extern __s32 BSP_disp_sprite_block_set_para(__u32 sel, __u32 hid,__disp_sprite_block_para_t *para);
extern __s32 BSP_disp_sprite_block_get_para(__u32 sel, __u32 hid,__disp_sprite_block_para_t *para);

#ifdef __LINUX_OSAL__
__s32 Display_set_fb_timming(__u32 sel);
#endif

#endif
