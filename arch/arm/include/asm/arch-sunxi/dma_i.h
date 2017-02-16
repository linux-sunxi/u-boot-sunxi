/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON driver sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : dmac_i.h
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
#ifndef	_DMAC_I_H_
#define	_DMAC_I_H_

#define CFG_SW_DMA_NORMAL_MAX       8
#define CFG_SW_DMA_DEDICATE_MAX     8

#define DMAC_REGS_BASE				0x01c02000


#define CFG_SW_DMA_NORMAL_BASE              (DMAC_REGS_BASE + 0x100              )
#define CFS_SW_DMA_NORMAL0                  (CFG_SW_DMA_NORMAL_BASE + 0x20 * 0   )
#define CFS_SW_DMA_NORMAL1                  (CFG_SW_DMA_NORMAL_BASE + 0x20 * 1   )
#define CFS_SW_DMA_NORMAL2                  (CFG_SW_DMA_NORMAL_BASE + 0x20 * 2   )
#define CFS_SW_DMA_NORMAL3                  (CFG_SW_DMA_NORMAL_BASE + 0x20 * 3   )
#define CFS_SW_DMA_NORMAL4                  (CFG_SW_DMA_NORMAL_BASE + 0x20 * 4   )
#define CFS_SW_DMA_NORMAL5                  (CFG_SW_DMA_NORMAL_BASE + 0x20 * 5   )
#define CFS_SW_DMA_NORMAL6                  (CFG_SW_DMA_NORMAL_BASE + 0x20 * 6   )
#define CFS_SW_DMA_NORMAL7                  (CFG_SW_DMA_NORMAL_BASE + 0x20 * 7   )

#define CFG_SW_DMA_DEDICATE_BASE            (DMAC_REGS_BASE + 0x300               )
#define CFG_SW_DMA_DEDICATE0                (CFG_SW_DMA_DEDICATE_BASE + 0x20 * 0 )
#define CFG_SW_DMA_DEDICATE1                (CFG_SW_DMA_DEDICATE_BASE + 0x20 * 1 )
#define CFG_SW_DMA_DEDICATE2                (CFG_SW_DMA_DEDICATE_BASE + 0x20 * 2 )
#define CFG_SW_DMA_DEDICATE3                (CFG_SW_DMA_DEDICATE_BASE + 0x20 * 3 )
#define CFG_SW_DMA_DEDICATE4                (CFG_SW_DMA_DEDICATE_BASE + 0x20 * 4 )
#define CFG_SW_DMA_DEDICATE5                (CFG_SW_DMA_DEDICATE_BASE + 0x20 * 5 )
#define CFG_SW_DMA_DEDICATE6                (CFG_SW_DMA_DEDICATE_BASE + 0x20 * 6 )
#define CFG_SW_DMA_DEDICATE7                (CFG_SW_DMA_DEDICATE_BASE + 0x20 * 7 )

#define CFG_SW_DMA_OTHER_BASE               (DMAC_REGS_BASE + 0x300 + 0x10       )
#define CFG_SW_DMA_DEDICATE0_OTHER          (CFG_SW_DMA_OTHER_BASE + 0x20 * 0    )
#define CFG_SW_DMA_DEDICATE1_OTHER          (CFG_SW_DMA_OTHER_BASE + 0x20 * 1    )
#define CFG_SW_DMA_DEDICATE2_OTHER          (CFG_SW_DMA_OTHER_BASE + 0x20 * 2    )
#define CFG_SW_DMA_DEDICATE3_OTHER          (CFG_SW_DMA_OTHER_BASE + 0x20 * 3    )
#define CFG_SW_DMA_DEDICATE4_OTHER          (CFG_SW_DMA_OTHER_BASE + 0x20 * 4    )
#define CFG_SW_DMA_DEDICATE5_OTHER          (CFG_SW_DMA_OTHER_BASE + 0x20 * 5    )
#define CFG_SW_DMA_DEDICATE6_OTHER          (CFG_SW_DMA_OTHER_BASE + 0x20 * 6    )
#define CFG_SW_DMA_DEDICATE7_OTHER          (CFG_SW_DMA_OTHER_BASE + 0x20 * 7    )

struct sw_dma
{
    volatile unsigned int config;           /* DMAÅäÖÃ²ÎÊý              */
    volatile unsigned int src_addr;         /* DMA´«ÊäÔ´µØÖ·            */
    volatile unsigned int dst_addr;         /* DMA´«ÊäÄ¿µÄµØÖ·          */
    volatile unsigned int bytes;            /* DMA´«Êä×Ö½ÚÊý            */
};

typedef volatile struct sw_dma *sw_dma_t;

struct sw_dma_other
{
    volatile unsigned int page_size;        /* DMA´«ÊäPAGE SIZE         */
    volatile unsigned int page_step;        /* DMA´«ÊäPAGE STEP         */
    volatile unsigned int comity_counter;   /* DMA´«Êäcomity counter    */
};

typedef volatile struct sw_dma_other *sw_dma_other_t;

typedef struct sw_dma_channal_set
{
    unsigned int            used;           /* DMAÊÇ·ñ±»Ê¹ÓÃ            */
      signed int            channalNo;      /* DMAÍ¨µÀ±àºÅ              */
    sw_dma_t                channal;        /* DMAÍ¨µÀ                  */
    sw_dma_other_t          other;          /* DMAÆäËüÉèÖÃ              */
}
sw_dma_channal_set_t;


typedef struct __ndma_config_set
{
    unsigned int      src_drq_type     : 5;            //Ô´µØÖ·´æ´¢ÀàÐÍ£¬ÈçDRAM, SPI,NANDµÈ£¬²Î¼û  __ndma_drq_type_t
    unsigned int      src_addr_type    : 1;            //Ô­µØÖ·ÀàÐÍ£¬ÈçµÝÔö£¬»òÕß²»±ä  0:µÝÔöÄ£Ê½  1:±£³Ö²»±ä
    unsigned int      src_secure       : 1;            //source secure  0:secure  1:not secure
    unsigned int      src_burst_length : 2;            //·¢ÆðÒ»´Îburst¿í¶È 0:1   1:4   2:8
    unsigned int      src_data_width   : 2;            //Êý¾Ý´«Êä¿í¶È£¬0:Ò»´Î´«Êä8bit£¬1:Ò»´Î´«Êä16bit£¬2:Ò»´Î´«Êä32bit£¬3:±£Áô
    unsigned int      reserved0        : 5;
    unsigned int      dst_drq_type     : 5;            //Ä¿µÄµØÖ·´æ´¢ÀàÐÍ£¬ÈçDRAM, SPI,NANDµÈ
    unsigned int      dst_addr_type    : 1;            //Ä¿µÄµØÖ·ÀàÐÍ£¬ÈçµÝÔö£¬»òÕß²»±ä  0:µÝÔöÄ£Ê½  1:±£³Ö²»±ä
    unsigned int      dst_secure       : 1;            //dest secure  0:secure  1:not secure
    unsigned int      dst_burst_length : 2;            //·¢ÆðÒ»´Îburst¿í¶È Ìî0¶ÔÓ¦ÓÚ1£¬Ìî1¶ÔÓ¦ÓÚ4,
    unsigned int      dst_data_width   : 2;            //Êý¾Ý´«Êä¿í¶È£¬0:Ò»´Î´«Êä8bit£¬1:Ò»´Î´«Êä16bit£¬2:Ò»´Î´«Êä32bit£¬3:±£Áô
    unsigned int      wait_state       : 3;            //µÈ´ýÊ±ÖÓ¸öÊý Ñ¡Ôñ·¶Î§´Ó0-7
    unsigned int      continuous_mode  : 1;            //Ñ¡ÔñÁ¬Ðø¹¤×÷Ä£Ê½ 0:´«ÊäÒ»´Î¼´½áÊø 1:·´¸´´«Êä£¬µ±Ò»´ÎDMA´«Êä½áÊøºó£¬ÖØÐÂ¿ªÊ¼´«Êä
    unsigned int      reserved1        : 1;
}
__ndma_config_t;

typedef struct __ddma_config_set
{
    unsigned int      src_drq_type     : 5;            //Ô´µØÖ·´æ´¢ÀàÐÍ£¬ÈçDRAM, SPI,NANDµÈ£¬²Î¼û  __ddma_src_type_t
    unsigned int      src_addr_type    : 2;            //Ô­µØÖ·ÀàÐÍ£¬ÈçµÝÔö£¬»òÕß²»±ä  0:µÝÔöÄ£Ê½  1:±£³Ö²»±ä  2:HÄ£Ê½  3:VÄ£Ê½
    unsigned int      src_burst_length : 2;            //·¢ÆðÒ»´Îburst¿í¶È Ìî0¶ÔÓ¦ÓÚ1£¬Ìî1¶ÔÓ¦ÓÚ4,
    unsigned int      src_data_width   : 2;            //Êý¾Ý´«Êä¿í¶È£¬0:Ò»´Î´«Êä8bit£¬1:Ò»´Î´«Êä16bit£¬2:Ò»´Î´«Êä32bit£¬3:±£Áô
    unsigned int      reserved0        : 5;
    unsigned int      dst_drq_type     : 5;            //Ä¿µÄµØÖ·´æ´¢ÀàÐÍ£¬ÈçDRAM, SPI,NANDµÈ, ²Î¼û  __ddma_dst_type_t
    unsigned int      dst_addr_type    : 2;            //Ä¿µÄµØÖ·ÀàÐÍ£¬ÈçµÝÔö£¬»òÕß²»±ä 0:µÝÔöÄ£Ê½  1:±£³Ö²»±ä  2:HÄ£Ê½  3:VÄ£Ê½
    unsigned int      dst_burst_length : 2;            //·¢ÆðÒ»´Îburst¿í¶È Ìî0¶ÔÓ¦ÓÚ1£¬Ìî1¶ÔÓ¦ÓÚ4,
    unsigned int      dst_data_width   : 2;            //Êý¾Ý´«Êä¿í¶È£¬0:Ò»´Î´«Êä8bit£¬1:Ò»´Î´«Êä16bit£¬2:Ò»´Î´«Êä32bit£¬3:±£Áô
    unsigned int      reserved1        : 3;
    unsigned int      continuous_mode  : 1;            //Ñ¡ÔñÁ¬Ðø¹¤×÷Ä£Ê½ 0:´«ÊäÒ»´Î¼´½áÊø 1:·´¸´´«Êä£¬µ±Ò»´ÎDMA´«Êä½áÊøºó£¬ÖØÐÂ¿ªÊ¼´«Êä
    unsigned int      reserved2        : 1;
}
__ddma_config_t;

#endif	/* _EGON2_DMAC_I_H_ */


