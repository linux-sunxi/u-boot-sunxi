/*
 * (C) Copyright 2012
 *     wangflord@allwinnertech.com
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 */
#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/codec.h>
#include <asm/arch/dma.h>
#include <asm/arch/dma_i.h>

struct sunxi_codec_t    *sunxi_codec;
uint hdma = 0;
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_codec_init(void)
{
	struct sunxi_ccm_reg *ccm_reg = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	sunxi_codec = (struct sunxi_codec_t *)SUNXI_CODEC_BASE;

	//open codec clock
	ccm_reg->apb0_gate |= 1;
	ccm_reg->audio_codec_clk_cfg |= (1U << 31);
	//open dac
	sunxi_codec->dac_dpc = 0x80800000;
	sunxi_codec->dac_fifoc = 0x10400f11;
	sunxi_codec->adc_actl |= (1 << 4);
	sunxi_codec->dac_actl |= (0x3U<<30)|(0x1<<8)|(0x1<<6)|(0x3c);

   	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_codec_config(int samplerate, int samplebit, int channel)
{
	uint reg_value;
	struct sunxi_ccm_reg *ccm_reg = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	reg_value = sunxi_codec->dac_fifoc;
	reg_value &= ~((1 << 6) | (1 << 5) | (0x0eU << 29));

	if(samplebit == 24)
	{
		reg_value |= (1 << 5);
	}
	if(channel != 2)
	{
		reg_value |= (1 << 6);
	}

	if(samplerate%2000)
	{
		debug("sunxi codec samplerate as 44100\n");
		ccm_reg->pll2_cfg = 0;
	}
	else
	{
		debug("sunxi codec samplerate as 48000\n");
		ccm_reg->pll2_cfg = 0;
	}
	switch(samplerate)
	{
	    case 48000:
	    case 44100:
	    {
	        break;
	    }
	    case 32000:
	    {
	        reg_value |= (1 << 29);
	        break;
	    }
	    case 24000:
	    case 22050:
	    {
	        reg_value |= (2 << 29);
	        break;
	    }
	    case 16000:
	    {
	        reg_value |= (3 << 29);
	        break;
	    }
	    case 12000:
	    case 11025:
	    {
	        reg_value |= (4 << 29);
	        break;
	    }
	    case 8000:
	    {
	        reg_value |= (5 << 29);
	        break;
	    }
		default:
		{
			printf("sunxi codec config error : audio sample %d not supported\n", samplerate);
			return -1;
		}
	}
	sunxi_codec->dac_fifoc = reg_value;

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_codec_start(void *buffer, uint length, uint loop_mode)
{
	__dma_setting_t  dma_cfg;

	if(!hdma)
	{
		hdma = sunxi_dma_request(0);
		if(!hdma)
		{
			printf("sunxi codec error : request dma failed\n");

			return -1;
		}
	}

	dma_cfg.cfg.src_drq_type 	 = DMAC_CFG_DEST_TYPE_DRAM;
	dma_cfg.cfg.src_addr_type 	 = 0;		//源端地址不变
	dma_cfg.cfg.src_burst_length = 0;		//burst = 1
	dma_cfg.cfg.src_data_width   = 1;    	//一次传输16bit
	dma_cfg.cfg.dst_drq_type     = DMAC_CFG_SRC_TYPE_CODEC;
	dma_cfg.cfg.dst_addr_type    = 1;    	//目的端地址不变
	dma_cfg.cfg.dst_burst_length = 0;		//burst = 1
	dma_cfg.cfg.dst_data_width   = 1;    	//一次接收16bit

	dma_cfg.loop_mode = loop_mode;
	dma_cfg.data_block_size = 0;
	dma_cfg.wait_cyc = 0;

	sunxi_dma_setting(hdma, (void *)&dma_cfg);
	flush_cache((uint)buffer, length);
	sunxi_dma_start(hdma, (uint)buffer, (uint)(&sunxi_codec->dac_txdata), length);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_codec_stop(void)
{
	if(!hdma)
	{
		printf("sunxi codec error : cant stop without dma\n");

		return -1;
	}
	sunxi_dma_stop(hdma);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_codec_wink(void *buffer, uint length)
{
	if(!hdma)
	{
		printf("sunxi codec error : cant stop without dma\n");

		return -1;
	}
	sunxi_dma_stop(hdma);
	flush_cache((uint)buffer, length);
	sunxi_dma_start(hdma, (uint)buffer, (uint)(&sunxi_codec->dac_txdata), length);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int audio_codec_exit(void)
{
	struct sunxi_ccm_reg *ccm_reg = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	ccm_reg->apb0_gate &= ~1;
	ccm_reg->audio_codec_clk_cfg &= ~(1U << 31);

    return 0;
}

