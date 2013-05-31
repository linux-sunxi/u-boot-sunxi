/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
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
    volatile unsigned int config;           /* DMA配置参数              */
    volatile unsigned int src_addr;         /* DMA传输源地址            */
    volatile unsigned int dst_addr;         /* DMA传输目的地址          */
    volatile unsigned int bytes;            /* DMA传输字节数            */
};

typedef volatile struct sw_dma *sw_dma_t;

struct sw_dma_other
{
    volatile unsigned int page_size;        /* DMA传输PAGE SIZE         */
    volatile unsigned int page_step;        /* DMA传输PAGE STEP         */
    volatile unsigned int comity_counter;   /* DMA传输comity counter    */
};

typedef volatile struct sw_dma_other *sw_dma_other_t;

typedef struct sw_dma_channal_set
{
    unsigned int            used;           /* DMA是否被使用            */
      signed int            channalNo;      /* DMA通道编号              */
    sw_dma_t                channal;        /* DMA通道                  */
    sw_dma_other_t          other;          /* DMA其它设置              */
}
sw_dma_channal_set_t;


typedef struct __ndma_config_set
{
    unsigned int      src_drq_type     : 5;            //源地址存储类型，如DRAM, SPI,NAND等，参见  __ndma_drq_type_t
    unsigned int      src_addr_type    : 1;            //原地址类型，如递增，或者不变  0:递增模式  1:保持不变
    unsigned int      src_secure       : 1;            //source secure  0:secure  1:not secure
    unsigned int      src_burst_length : 2;            //发起一次burst宽度 0:1   1:4   2:8
    unsigned int      src_data_width   : 2;            //数据传输宽度，0:一次传输8bit，1:一次传输16bit，2:一次传输32bit，3:保留
    unsigned int      reserved0        : 5;
    unsigned int      dst_drq_type     : 5;            //目的地址存储类型，如DRAM, SPI,NAND等
    unsigned int      dst_addr_type    : 1;            //目的地址类型，如递增，或者不变  0:递增模式  1:保持不变
    unsigned int      dst_secure       : 1;            //dest secure  0:secure  1:not secure
    unsigned int      dst_burst_length : 2;            //发起一次burst宽度 填0对应于1，填1对应于4,
    unsigned int      dst_data_width   : 2;            //数据传输宽度，0:一次传输8bit，1:一次传输16bit，2:一次传输32bit，3:保留
    unsigned int      wait_state       : 3;            //等待时钟个数 选择范围从0-7
    unsigned int      continuous_mode  : 1;            //选择连续工作模式 0:传输一次即结束 1:反复传输，当一次DMA传输结束后，重新开始传输
    unsigned int      reserved1        : 1;
}
__ndma_config_t;

typedef struct __ddma_config_set
{
    unsigned int      src_drq_type     : 5;            //源地址存储类型，如DRAM, SPI,NAND等，参见  __ddma_src_type_t
    unsigned int      src_addr_type    : 2;            //原地址类型，如递增，或者不变  0:递增模式  1:保持不变  2:H模式  3:V模式
    unsigned int      src_burst_length : 2;            //发起一次burst宽度 填0对应于1，填1对应于4,
    unsigned int      src_data_width   : 2;            //数据传输宽度，0:一次传输8bit，1:一次传输16bit，2:一次传输32bit，3:保留
    unsigned int      reserved0        : 5;
    unsigned int      dst_drq_type     : 5;            //目的地址存储类型，如DRAM, SPI,NAND等, 参见  __ddma_dst_type_t
    unsigned int      dst_addr_type    : 2;            //目的地址类型，如递增，或者不变 0:递增模式  1:保持不变  2:H模式  3:V模式
    unsigned int      dst_burst_length : 2;            //发起一次burst宽度 填0对应于1，填1对应于4,
    unsigned int      dst_data_width   : 2;            //数据传输宽度，0:一次传输8bit，1:一次传输16bit，2:一次传输32bit，3:保留
    unsigned int      reserved1        : 3;
    unsigned int      continuous_mode  : 1;            //选择连续工作模式 0:传输一次即结束 1:反复传输，当一次DMA传输结束后，重新开始传输
    unsigned int      reserved2        : 1;
}
__ddma_config_t;

#endif	/* _EGON2_DMAC_I_H_ */

