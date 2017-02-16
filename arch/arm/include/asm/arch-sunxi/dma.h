/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm driver sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : wboot_dma
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
#ifndef	_DMA_H_
#define	_DMA_H_

#define  DMA_HDLER_TYPE_CNT                     2
#define  DMAC_DMATYPE_NORMAL         			0
#define  DMAC_DMATYPE_DEDICATED      			1


#define  DMA_TRANSFER_HALF_INT       0
#define  DMA_TRANSFER_END_INT        1

#define  DMA_TRANSFER_UNLOOP_MODE   0
#define  DMA_TRANSFER_LOOP_MODE     1


//================================
//======    DMA ÅäÖÃ     =========
//================================

/* DMA »ù´¡ÅäÖÃ  */
#define DMAC_CFG_CONTINUOUS_ENABLE              (0x01)	//(0x01&lt;&lt;29)
#define DMAC_CFG_CONTINUOUS_DISABLE             (0x00)	//(0x01&lt;&lt;29)

//* DMA Ê±ÖÓ */
#define	DMAC_CFG_WAIT_1_DMA_CLOCK				(0x00)	//(0x00&lt;&lt;26)
#define	DMAC_CFG_WAIT_2_DMA_CLOCK				(0x01)	//(0x01&lt;&lt;26)
#define	DMAC_CFG_WAIT_3_DMA_CLOCK				(0x02)	//(0x02&lt;&lt;26)
#define	DMAC_CFG_WAIT_4_DMA_CLOCK				(0x03)	//(0x03&lt;&lt;26)
#define	DMAC_CFG_WAIT_5_DMA_CLOCK				(0x04)	//(0x04&lt;&lt;26)
#define	DMAC_CFG_WAIT_6_DMA_CLOCK				(0x05)	//(0x05&lt;&lt;26)
#define	DMAC_CFG_WAIT_7_DMA_CLOCK				(0x06)	//(0x06&lt;&lt;26)
#define	DMAC_CFG_WAIT_8_DMA_CLOCK				(0x07)	//(0x07&lt;&lt;26)

/* DMA ´«ÊäÄ¿µÄ¶Ë ÅäÖÃ */
/* DMA Ä¿µÄ¶Ë ´«Êä¿í¶È */
#define	DMAC_CFG_DEST_DATA_WIDTH_8BIT			(0x00)	//(0x00&lt;&lt;24)
#define	DMAC_CFG_DEST_DATA_WIDTH_16BIT			(0x01)	//(0x01&lt;&lt;24)
#define	DMAC_CFG_DEST_DATA_WIDTH_32BIT			(0x02)	//(0x02&lt;&lt;24)

/* DMA Ä¿µÄ¶Ë Í»·¢´«ÊäÄ£Ê½ */
#define	DMAC_CFG_DEST_1_BURST       			(0x00)	//(0x00&lt;&lt;23)
#define	DMAC_CFG_DEST_4_BURST		    		(0x01)	//(0x01&lt;&lt;23)

/* DMA Ä¿µÄ¶Ë µØÖ·±ä»¯Ä£Ê½ */
#define	DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE		(0x00)	//(0x00&lt;&lt;21)
#define	DMAC_CFG_DEST_ADDR_TYPE_IO_MODE 		(0x01)	//(0x01&lt;&lt;21)
#define	DMAC_CFG_DEST_ADDR_TYPE_HPAGE_MODE 		(0x02)	//(0x02&lt;&lt;21)
#define	DMAC_CFG_DEST_ADDR_TYPE_VPAGE_MODE 		(0x03)	//(0x03&lt;&lt;21)


/* DMA ´«ÊäÔ´¶Ë ÅäÖÃ */
/* DMA Ô´¶Ë ´«Êä¿í¶È */
#define	DMAC_CFG_SRC_DATA_WIDTH_8BIT			(0x00)	//(0x00&lt;&lt;8)
#define	DMAC_CFG_SRC_DATA_WIDTH_16BIT			(0x01)	//(0x01&lt;&lt;8)
#define	DMAC_CFG_SRC_DATA_WIDTH_32BIT			(0x02)	//(0x02&lt;&lt;8)

/* DMA Ô´¶Ë Í»·¢´«ÊäÄ£Ê½ */
#define	DMAC_CFG_SRC_1_BURST       				(0x00)	//(0x00&lt;&lt;7)
#define	DMAC_CFG_SRC_4_BURST		    		(0x01)	//(0x01&lt;&lt;7)

/* DMA Ô´¶Ë µØÖ·±ä»¯Ä£Ê½ */
#define	DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE		(0x00)	//(0x00&lt;&lt;5)
#define	DMAC_CFG_SRC_ADDR_TYPE_IO_MODE 			(0x01)	//(0x01&lt;&lt;5)
#define	DMAC_CFG_SRC_ADDR_TYPE_HPAGE_MODE 		(0x02)	//(0x02&lt;&lt;5)
#define	DMAC_CFG_SRC_ADDR_TYPE_VPAGE_MODE 		(0x03)	//(0x03&lt;&lt;5)


/* DMA ´«ÊäÄ¿µÄ¶Ë ÅäÖÃ */
/* DMA ´«ÊäÄ¿µÄ¶Ë NÐÍDMA Ä¿µÄÑ¡Ôñ */
#define	DMAC_CFG_DEST_TYPE_IR					(0x00)	//(0x00&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_SPDIF		    	(0x01)	//(0x01&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_IIS			    	(0x02)	//(0x02&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_AC97			    	(0x03)	//(0x03&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_SPI0				    (0x04)	//(0x04&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_SPI1				    (0x05)	//(0x05&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_SPI2				    (0x06)	//(0x06&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_UART0				(0x08)	//(0x08&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_UART1				(0x09)	//(0x09&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_UART2				(0x0a)	//(0x0a&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_UART3				(0x0b)	//(0x0b&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_AUDIO_DA				(0x0c)	//(0x0c&lt;&lt;16)

#define	DMAC_CFG_DEST_TYPE_NFC_DEBUG			(0x0f)	//(0x0f&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_N_SRAM 				(0x10)	//(0x10&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_N_SDRAM				(0x11)	//(0x11&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_UART4				(0x12)	//(0x12&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_UART5				(0x13)	//(0x13&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_UART6				(0x14)	//(0x14&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_UART7				(0x15)	//(0x15&lt;&lt;16)

/* DMA ´«ÊäÄ¿µÄ¶Ë DÐÍDMA Ä¿µÄÑ¡Ôñ */
#define	DMAC_CFG_DEST_TYPE_D_SRAM 				(0x00)	//(0x00&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_D_SDRAM				(0x01)	//(0x01&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_TCON0				(0x02)	//(0x02&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_NFC  		    	(0x03)	//(0x03&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_USB0			    	(0x04)	//(0x04&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_USB1			    	(0x05)	//(0x05&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_SDC1			    	(0x07)	//(0x07&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_SDC2 				(0x08)	//(0x08&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_SDC3 				(0x09)	//(0x09&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_MSC  				(0x0a)	//(0x0a&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_EMAC 				(0x0b)	//(0x0b&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_SS   				(0x0d)	//(0x0d&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_USB2			    	(0x0f)	//(0x0f&lt;&lt;16)
#define	DMAC_CFG_DEST_TYPE_ATA			    	(0x10)	//(0x10&lt;&lt;16)

/* DMA ´«ÊäÔ´¶Ë ÅäÖÃ */
/* DMA ´«ÊäÔ´¶Ë NÐÍDMA Ä¿µÄÑ¡Ôñ */
#define	DMAC_CFG_SRC_TYPE_IR					(0x00)	//(0x00&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_SPDIF		    	   	(0x01)	//(0x01&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_IIS			    	(0x02)	//(0x02&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_AC97			    	(0x03)	//(0x03&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_SPI0				    (0x04)	//(0x04&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_SPI1				    (0x05)	//(0x05&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_SPI2				    (0x06)	//(0x06&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_UART0				    (0x08)	//(0x08&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_UART1				    (0x09)	//(0x09&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_UART2				    (0x0a)	//(0x0a&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_UART3				    (0x0b)	//(0x0b&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_AUDIO 				(0x0c)	//(0x0c&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_TP     				(0x0d)	//(0x0d&lt;&lt;0)

#define	DMAC_CFG_SRC_TYPE_NFC_DEBUG			    (0x0f)	//(0x0f&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_N_SRAM 				(0x10)	//(0x10&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_N_SDRAM				(0x11)	//(0x11&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_UART4				    (0x12)	//(0x12&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_UART5				    (0x13)	//(0x13&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_UART6				    (0x14)	//(0x14&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_UART7				    (0x15)	//(0x15&lt;&lt;0)

/* DMA ´«ÊäÔ´¶Ë DÐÍDMA Ä¿µÄÑ¡Ôñ */
#define	DMAC_CFG_SRC_TYPE_D_SRAM 				(0x00)	//(0x00&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_D_SDRAM				(0x01)	//(0x01&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_TCON0				    (0x02)	//(0x02&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_NFC  		    	   	(0x03)	//(0x03&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_USB0			    	(0x04)	//(0x04&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_USB1			    	(0x05)	//(0x05&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_SDC1			    	(0x07)	//(0x07&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_SDC2 				    (0x08)	//(0x08&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_SDC3 				    (0x09)	//(0x09&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_MSC  				    (0x0a)	//(0x0a&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_EMAC 				    (0x0c)	//(0x0c&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_SS   				    (0x0e)	//(0x0e&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_USB2			    	(0x0f)	//(0x0f&lt;&lt;0)
#define	DMAC_CFG_SRC_TYPE_ATA			    	(0x10)	//(0x10&lt;&lt;0)


typedef enum
{
    NDMA_IR                      = 0,
    NDMA_SPDIF                      ,
    NDMA_IIS                        ,
    NDMA_AC97                       ,
    NDMA_SPI0                       ,
    NDMA_SPI1                       ,
    NDMA_UART0                   = 8,
    NDMA_UART1                      ,
    NDMA_UART2                      ,
    NDMA_UART3                      ,
    NDMA_AUDIO                      ,
    NDMA_TP                         ,
    NDMA_SRAM                   = 16,
    NDMA_SDRAM                      ,
    NDMA_
}
__ndma_drq_type_t;

typedef enum
{
    DDMA_SRC_SRAM                   = 0,
    DDMA_SRC_SDRAM                     ,
    DDMA_SRC_LCD                       ,
    DDMA_SRC_NAND                      ,
    DDMA_SRC_USB0                      ,
    DDMA_SRC_USB1                      ,
    DDMA_SRC_SD0                       ,
    DDMA_SRC_SD1                       ,
    DDMA_SRC_SD2                       ,
    DDMA_SRC_SD3                       ,
    DDMA_SRC_MS                        ,
    DDMA_SRC_EMAC                   = 0xc,
    DDMA_SRC_SS                     = 0xe,
    DDMA_SRC_
}
__ddma_src_type_t;

typedef enum
{
    DDMA_DST_SRAM                   = 0,
    DDMA_DST_SDRAM                     ,
    DDMA_DST_LCD                       ,
    DDMA_DST_NAND                      ,
    DDMA_DST_USB0                      ,
    DDMA_DST_USB1                      ,
    DDMA_DST_SD0                       ,
    DDMA_DST_SD1                       ,
    DDMA_DST_SD2                       ,
    DDMA_DST_SD3                       ,
    DDMA_DST_MS                        ,
    DDMA_DST_EMAC                   = 0xb,
    DDMA_DST_SS                     = 0xd,
    DDMA_DST_
}
__ddma_dst_type_t;

typedef struct  __dma_config_set
{
    unsigned int      src_drq_type     ; //Ô´µØÖ·´æ´¢ÀàÐÍ£¬ÈçDRAM, SPI,NANDµÈ£¬¸ù¾ÝÑ¡ÔñNDMA»òÕßDDMA, Ñ¡Ôñ __ndma_drq_type_t»òÕß __ddma_src_type_t
    unsigned int      src_addr_type    ; //Ô­µØÖ·ÀàÐÍ NDMAÏÂ 0:µÝÔöÄ£Ê½  1:±£³Ö²»±ä  DDMAÏÂ 0:µÝÔöÄ£Ê½  1:±£³Ö²»±ä  2:HÄ£Ê½  3:VÄ£Ê½
    unsigned int      src_burst_length ; //·¢ÆðÒ»´Îburst¿í¶È Ìî0¶ÔÓ¦ÓÚ1£¬Ìî1¶ÔÓ¦ÓÚ4,
    unsigned int      src_data_width   ; //Êý¾Ý´«Êä¿í¶È£¬0:Ò»´Î´«Êä8bit£¬1:Ò»´Î´«Êä16bit£¬2:Ò»´Î´«Êä32bit£¬3:±£Áô
    unsigned int      dst_drq_type     ; //Ô´µØÖ·´æ´¢ÀàÐÍ£¬ÈçDRAM, SPI,NANDµÈ£¬¸ù¾ÝÑ¡ÔñNDMA»òÕßDDMA, Ñ¡Ôñ __ndma_drq_type_t»òÕß __ddma_dst_type_t
    unsigned int      dst_addr_type    ; //Ô­µØÖ·ÀàÐÍ NDMAÏÂ 0:µÝÔöÄ£Ê½  1:±£³Ö²»±ä  DDMAÏÂ 0:µÝÔöÄ£Ê½  1:±£³Ö²»±ä  2:HÄ£Ê½  3:VÄ£Ê½
    unsigned int      dst_burst_length ; //·¢ÆðÒ»´Îburst¿í¶È Ìî0¶ÔÓ¦ÓÚ1£¬Ìî1¶ÔÓ¦ÓÚ4,
    unsigned int      dst_data_width   ; //Êý¾Ý´«Êä¿í¶È£¬0:Ò»´Î´«Êä8bit£¬1:Ò»´Î´«Êä16bit£¬2:Ò»´Î´«Êä32bit£¬3:±£Áô
    unsigned int      wait_state       ; //µÈ´ýÊ±ÖÓ¸öÊý Ñ¡Ôñ·¶Î§´Ó0-7£¬Ö»¶ÔNDMAÓÐÐ§
    unsigned int      continuous_mode  ; //Ñ¡ÔñÁ¬Ðø¹¤×÷Ä£Ê½ 0:´«ÊäÒ»´Î¼´½áÊø 1:·´¸´´«Êä£¬µ±Ò»´ÎDMA´«Êä½áÊøºó£¬ÖØÐÂ¿ªÊ¼´«Êä
}
__dma_config_t;

typedef struct 	__dma_setting_set
{
    __dma_config_t         cfg;	    	    //DMAÅäÖÃ²ÎÊý
    unsigned int           pgsz;            //DEÊ¹ÓÃ²ÎÊý£¬¿é¿½±´Ê¹ÓÃ
    unsigned int           pgstp;           //DEÊ¹ÓÃ²ÎÊý£¬¿é¿½±´Ê¹ÓÃ
    unsigned int           cmt_blk_cnt;     //DEÊ¹ÓÃ²ÎÊý£¬¿é¿½±´Ê¹ÓÃ
}__dma_setting_t;


extern    unsigned int 	DMA_Request			(unsigned int dmatype);
extern    int 			DMA_Release			(unsigned int hDma);
extern    int 			DMA_Setting			(unsigned int hDMA, void *cfg);
extern    int 			DMA_Start			(unsigned int hDMA, unsigned int saddr, unsigned int daddr, unsigned int bytes);
extern    int 			DMA_Stop			(unsigned int hDma);
extern    int 			DMA_Restart			(unsigned int hDma);
extern    int 			DMA_QueryStatus		(unsigned int hDma);
extern    int 			DMA_QueryChannalNo	(unsigned int hDma);

void print_nand_dma(unsigned int hDma);

#endif	//_DMA_H_

/* end of _DMA_H_ */


