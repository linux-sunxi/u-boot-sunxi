/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some board init for the Allwinner A10-evb board.
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

#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <fastboot.h>

#include <asm/arch/nand_bsp.h>
#include <mmc.h>
#include <asm/arch/android_misc.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sunxi_mbr.h>
#include <asm/arch/boot_type.h>
#include <asm/arch/drv_display_sun4i.h>

DECLARE_GLOBAL_DATA_PTR;

static struct bootloader_message misc_message;

void fastboot_partition_init(void)
{
	fastboot_ptentry fb_part;
	int index, part_total;
	char partition_sets[256];
	char part_line[48];

	printf("--------fastboot partitions--------\n");
	part_total = sunxi_partition_get_total_num();
	printf("-total partitions:%d-\n", part_total);
	printf("%-12s  %-12s  %-12s\n", "-name-", "-start-", "-size-");
	memset(partition_sets, 0, 256);

	for(index = 0; index < part_total; index++)
	{
		sunxi_partition_get_name(index, &fb_part.name[0]);
		fb_part.start = sunxi_partition_get_offset(index) * 512;
		fb_part.length = sunxi_partition_get_size(index) * 512;
		fb_part.flags = 0;
		printf("%-12s: %-12x  %-12x\n", fb_part.name, fb_part.start, fb_part.length);
		fastboot_flash_add_ptn(&fb_part);

		memset(part_line, 0, 48);
		if(!uboot_spare_head.boot_data.storage_type)
		{
			sprintf(part_line, "%s@nand%c:", fb_part.name, 'a' + index);
		}
		else
		{
			if(!index)
			{
				sprintf(part_line, "%s@mmcblk0p2:", fb_part.name);
			}
			else
			{
				sprintf(part_line, "%s@mmcblk0p%d:", fb_part.name, 4 + index);
			}
		}
		strcat(partition_sets, part_line);
	}
	printf("-----------------------------------\n");

	setenv("partitions", partition_sets);
}

static struct bootloader_message misc_message;

int android_misc_flash_check(void) {

	u32   misc_offset = 0;
	char  buffer[2048];

	misc_offset = sunxi_partition_get_offset_byname("misc");
	if(!misc_offset)
	{
		puts("no misc partition is found\n");
		return 0;
	}
	memset(buffer, 0, 2048);
#ifdef DEBUG
	printf("misc_offset  : %d\n", (int )misc_offset);
#endif
	sunxi_flash_read(misc_offset, 2048/512, buffer);
	memcpy(&misc_message, buffer, sizeof(misc_message));
#ifdef DEBUG
	printf("misc.command  : %s\n", misc_message.command);
	printf("misc.status   : %s\n", misc_message.status);
	printf("misc.recovery : %s\n", misc_message.recovery);
#endif
	if(uboot_spare_head.boot_data.storage_type)
	{
		if(!strcmp(misc_message.command, "boot-recovery")) {
			/* there is a recovery command */
			puts("find boot recovery\n");
			setenv("bootcmd", "run setargs_mmc boot_recovery");
			puts("Recovery detected, will boot recovery\n");
			/* android recovery will clean the misc */
		}
		else{
			printf("bootcmd set setargs_mmc\n");
			setenv("bootcmd", "run setargs_mmc boot_normal");
		}
		if(!strcmp(misc_message.command, "boot-fastboot")) {
			/* there is a fastboot command */
			setenv("bootcmd", "run setargs_mmc boot_fastboot");
			puts("Fastboot detected, will enter fastboot\n");
			/* clean the misc partition ourself */
			memset(buffer, 0, 2048);
			sunxi_flash_write(misc_offset, 2048/512, buffer);
		}
	}
	else
	{
		if(!strcmp(misc_message.command, "boot-recovery")) {
			/* there is a recovery command */
			puts("find boot recovery\n");
			setenv("bootcmd", "run setargs_nand boot_recovery");
			puts("Recovery detected, will boot recovery\n");
			/* android recovery will clean the misc */
		}
		else{
			printf("bootcmd set setargs_nand\n");
			setenv("bootcmd", "run setargs_nand boot_normal");
		}

		if(!strcmp(misc_message.command, "boot-fastboot")) {
			/* there is a fastboot command */
			setenv("bootcmd", "run setargs_nand boot_fastboot");
			puts("Fastboot detected, will enter fastboot\n");
			/* clean the misc partition ourself */
			memset(buffer, 0, 2048);
			sunxi_flash_write(misc_offset, 2048/512, buffer);
		}
	}

	return 0;
}

int check_android_misc(void)
{
	debug("check_android_misc storage type = %d\n", uboot_spare_head.boot_data.storage_type);

	return android_misc_flash_check();
}

void set_boot_type_arg(void){

	if(uboot_spare_head.boot_data.storage_type){
		setenv("bootcmd", "run setargs_mmc boot_normal");
	}
	else{
		setenv("bootcmd", "run setargs_nand boot_normal");
	}
}

/* add board specific code here */
int board_init(void) {

	gd->bd->bi_arch_number = 3495;
	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100);
	debug("board_init storage_type = %d\n",uboot_spare_head.boot_data.storage_type);

	return 0;
}

/* Partition init must be after NAND init, so we put the fastboot
 * partition init here in the board late init.
 */
int board_late_init(void)
{
	fastboot_partition_init();
	set_boot_type_arg();
	check_android_misc();
	return 0;
}
void dram_init_banksize(void)
{
	/*
	 * We should init the Dram options, and kernel get it by tag.
	 */
	int dram_size;
	int ret;
	//gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	ret = script_parser_fetch("dram_para", "dram_para1", &dram_size, 1);
	if(ret)
	{
		dram_size &= 0xffff;
		if(dram_size)
		{
			gd->bd->bi_dram[0].size = dram_size * 1024 * 1024;
		}
		else
		{
			gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
		}
	}
	else
	{
		gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	}
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);

	return 0;
}

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	sunxi_mmc_init(bis->bi_card_num);

	return 0;
}

void board_mmc_pre_init(int card_num)
{
    bd_t *bd;

	bd = gd->bd;
	gd->bd->bi_card_num = card_num;
	mmc_initialize(bd);
    //gd->bd->bi_card_num = card_num;
}

int board_mmc_get_num(void)
{
    return gd->boot_card_num;
}


void board_mmc_set_num(int num)
{
    gd->boot_card_num = num;
}


int mmc_get_env_addr(struct mmc *mmc, u32 *env_addr) {

	*env_addr = sunxi_partition_get_offset_byname(CONFIG_SUNXI_ENV_PARTITION);
	return 0;
}
#endif

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	puts("Board: SUN6I\n");
	return 0;
}
#endif

int board_display_layer_open(void)
{
	char arg[4];

	arg[0] = 0;
	arg[1] = DISP_LAYER_WORK_MODE_NORMAL;

	gd->layer_hd = disp_ioctl(NULL, DISP_CMD_LAYER_REQUEST, (void*)arg);
	if(gd->layer_hd == 0)
	{
        printf("sunxi display error : display request layer failed\n");

        return -1;
	}

	return 0;
}

int board_display_layer_close(int status)
{
	uint  arg[4];

    arg[0] = gd->layer_hd;
    arg[1] = 0;
    arg[2] = 0;
	arg[3] = 0;
    if(disp_ioctl(NULL, DISP_CMD_LAYER_CLOSE, (void*)arg))
    {
        printf("sunxi display error : close layer failed\n");
    }

	if(gd->layer_para)
	{
		free((void *)gd->layer_para);
		gd->layer_para = NULL;
	}
	if(!status)		//close all
	{
		arg[0] = DISP_EXIT_MODE_CLEAN_PARTLY;
		disp_ioctl(NULL, DISP_CMD_SET_EXIT_MODE, (void*)arg);
	}
	else			//close lcd only
	{
		arg[0] = DISP_EXIT_MODE_CLEAN_ALL;
		disp_ioctl(NULL, DISP_CMD_SET_EXIT_MODE, (void*)arg);
	}

	return 0;
}

static int board_display_wait_lcd_open(void)
{
	int ret;
	int timedly = 5000;
	int check_time = timedly/50;
	uint arg[4];

	arg[0] = arg[1] = arg[2] = arg[3] = 0;
	do
	{
    	ret = disp_ioctl(NULL, DISP_CMD_LCD_CHECK_OPEN_FINISH, (void*)arg);
		if(ret == 1)		//open already
		{
			break;
		}
		else if(ret == -1)  //open falied
		{
			return -1;
		}
		__msdelay(50);
		check_time --;
		if(check_time <= 0)
		{
			return -1;
		}
	}
	while(1);
}

int board_display_layer_show(int display_source)
{
	__s32 ret = 0;
	uint  arg[4];

    if(!display_source)
    {
	    board_display_wait_lcd_open();
    }
	arg[0] = 0;
	arg[1] = gd->layer_hd;
	arg[2] = gd->layer_para;
	arg[3] = 0;
	disp_ioctl(NULL,DISP_CMD_LAYER_SET_PARA,(void*)arg);

	arg[0] = 0;
	arg[1] = gd->layer_hd;
	arg[2] = 0;
	arg[3] = 0;
	disp_ioctl(NULL,DISP_CMD_LAYER_OPEN,(void*)arg);

}


int board_display_framebuffer_set(int width, int height, int bitcount, void *buffer)
{
    __disp_layer_info_t *layer_para;
	__u32 screen_width, screen_height;
	__u32 arg[4];

	if(!gd->layer_para)
	{
		layer_para = (__disp_layer_info_t *)malloc(sizeof(__disp_layer_info_t));
		if(!layer_para)
		{
			printf("sunxi display error: unable to malloc memory for layer\n");

			return -1;
		}
	}
	else
	{
		layer_para = (__disp_layer_info_t *)gd->layer_para;
	}
	arg[0] = 0;
	screen_width = disp_ioctl(NULL, DISP_CMD_SCN_GET_WIDTH, (void*)arg);
	screen_height = disp_ioctl(NULL, DISP_CMD_SCN_GET_HEIGHT, (void*)arg);
	debug("screen_width =%d, screen_height =%d\n", screen_width, screen_height);
	memset((void *)layer_para, 0, sizeof(__disp_layer_info_t));
	layer_para->fb.addr[0]		= buffer;
	debug("frame buffer address %x\n", (uint)buffer);
	layer_para->fb.size.width	= width;
	layer_para->fb.size.height	= height;
	layer_para->fb.mode			= DISP_MOD_INTERLEAVED;
	layer_para->fb.format		= (bitcount == 24)? DISP_FORMAT_RGB888:DISP_FORMAT_ARGB8888;
	debug("bitcount = %d\n", bitcount);
	layer_para->fb.br_swap		= 0;
	layer_para->fb.seq			= DISP_SEQ_ARGB;
	layer_para->fb.b_trd_src 	= 0;
	layer_para->fb.trd_mode		= 0;
	layer_para->ck_enable		= 0;
	layer_para->mode            = DISP_LAYER_WORK_MODE_NORMAL;
	layer_para->alpha_en 		= 1;
	layer_para->alpha_val		= 0xff;
	layer_para->pipe 			= 0;
	layer_para->src_win.x		= 0;
	layer_para->src_win.y		= 0;
	layer_para->src_win.width	= width;
	layer_para->src_win.height	= height;
	layer_para->scn_win.x		= (screen_width - width) / 2;
	layer_para->scn_win.y		= (screen_height - height) / 2;
	layer_para->scn_win.width	= width;
	layer_para->scn_win.height	= height;
	layer_para->b_trd_out		= 0;
	layer_para->out_trd_mode 	= 0;

	debug("layer para = %x\n", (uint)layer_para);
	gd->layer_para = (uint)layer_para;
	debug("layer para = %x\n", gd->layer_para);

	return 0;
}

int board_display_framebuffer_change(void *buffer)
{
    uint arg[4];
	__disp_fb_t disp_fb;

	arg[0] = 0;
	arg[1] = gd->layer_hd;
	arg[2] = (uint)&disp_fb;
	arg[3] = 0;

	if(disp_ioctl(NULL, DISP_CMD_LAYER_GET_FB, (void*)arg))
	{
		printf("sunxi display error :set framebuffer failed\n");

		return -1;
	}
	disp_fb.addr[0] = (uint)buffer;
	arg[0] = 0;
    arg[1] = gd->layer_hd;
    arg[2] = (uint)&disp_fb;
    arg[3] = 0;
	debug("try to set framebuffer %x\n", (uint)buffer);
    if(disp_ioctl(NULL, DISP_CMD_LAYER_SET_FB, (void*)arg))
    {
        printf("sunxi display error :set framebuffer failed\n");

		return -1;
	}

	return 0;
}

int board_display_device_open(void)
{
#ifndef CONFIG_SUN6I_FPGA
	int  value = 1;
	int  ret;
	__u32 output_type = 0;
	__u32 output_mode = 0;
	__u32 auto_hpd = 0;
	__u32 err_count = 0;
	__u32 arg[4];
	int i;

	debug("De_OpenDevice\n");
//screen0_output_type
	if(script_parser_fetch("boot_disp", "output_type", &value, 1) < 0)
	{
		printf("fetch script data boot_disp.output_type fail\n");
		err_count ++;
		value = 0;
	}
	else
	{
		printf("boot_disp.output_type=%d\n", value);
	}

	if(value == 0)
	{
		output_type = DISP_OUTPUT_TYPE_NONE;
	}
	else if(value == 1)
	{
		output_type = DISP_OUTPUT_TYPE_LCD;
	}
	else if(value == 2)
	{
		output_type = DISP_OUTPUT_TYPE_TV;
	}
	else if(value == 3)
	{
		output_type = DISP_OUTPUT_TYPE_HDMI;
	}
	else if(value == 4)
	{
		output_type = DISP_OUTPUT_TYPE_VGA;
	}
	else
	{
		printf("invalid screen0_output_type %d\n", value);

		return -1;
	}
//screen0_output_mode
	if(script_parser_fetch("boot_disp", "output_mode", &value, 1) < 0)
	{
		printf("fetch script data boot_disp.output_mode fail\n");
		err_count ++;
		value = 0;
	}
	else
	{
		printf("boot_disp.output_mode=%d\n", value);
	}

	if(output_type == DISP_OUTPUT_TYPE_TV || output_type == DISP_OUTPUT_TYPE_HDMI)
	{
		output_mode = (__disp_tv_mode_t)value;
	}
	else if(output_type == DISP_OUTPUT_TYPE_VGA)
	{
		output_mode = (__disp_vga_mode_t)value;
	}

//auto hot plug detect
	if(script_parser_fetch("boot_disp", "auto_hpd", &value, 1) < 0)
	{
		printf("fetch script data boot_disp.auto_hpd fail\n");
		err_count ++;
		value = 0;
	}else
	{
		printf("boot_disp.auto_hpd=%d\n", value);
	}


	if(err_count == 3)//no boot_disp config
	{
		if(script_parser_fetch("lcd0_para", "lcd_used", &value, 1) < 0)
		{
			printf("fetch script data lcd0_para.lcd_used fail\n");
			value = 0;
		}else
		{
			printf("lcd0_para.lcd_used=%d\n", value);
		}

		value = 0;
		if(value == 1) //lcd available
		{
			output_type = DISP_OUTPUT_TYPE_LCD;
		}
		else
		{
			arg[0] = 0;
			arg[1] = 0;
			arg[2] = 0;
			ret = 0;
			for(i=0; (i<3)&&(ret==0); i++)
			{
				ret = disp_ioctl(NULL, DISP_CMD_HDMI_GET_HPD_STATUS, 0, (void*)arg);
			}
			if(ret == 1)
			{
				output_type = DISP_OUTPUT_TYPE_HDMI;
				//output_mode = (output_mode == -1)? DISP_TV_MOD_720P_50HZ:output_mode;
				output_mode = DISP_TV_MOD_720P_50HZ;
			}else
			{

				ret = 0;
				arg[0] = 0;
				arg[1] = 0;
				arg[2] = 0;
				for(i=0; (i<4)&&(ret==0); i++)
				{
					ret = disp_ioctl(NULL, DISP_CMD_TV_GET_INTERFACE, 0, (void*)arg);
					__msdelay(200);
				}
				printf("tv detect, ret = %d\n", ret);
				if((ret & DISP_TV_CVBS) == DISP_TV_CVBS)
				{
					output_type = DISP_OUTPUT_TYPE_TV;
					output_mode = DISP_TV_MOD_PAL;
					printf("cvbs plug\n");
				}else if((ret & DISP_TV_YPBPR) == DISP_TV_YPBPR)
				{
					output_type = DISP_OUTPUT_TYPE_TV;
					output_mode = DISP_TV_MOD_720P_50HZ;
					printf("ypbpr plug\n");
				}else
				{
					printf("no device plug\n");
					output_type = DISP_OUTPUT_TYPE_NONE;
				}
			}
		}

	}
	else//has boot_disp config
	{
		if(output_type == DISP_OUTPUT_TYPE_LCD)
		{

		}
		else if(auto_hpd == 1)
		{
			if(disp_ioctl(NULL, DISP_CMD_HDMI_GET_HPD_STATUS, 0, (void*)arg) == 1)
			{
				output_type = DISP_OUTPUT_TYPE_HDMI;
				//output_mode = (output_mode == -1)? DISP_TV_MOD_720P_50HZ:output_mode;
				output_mode = DISP_TV_MOD_720P_50HZ;
			}
			else
			{
				ret = disp_ioctl(NULL, DISP_CMD_TV_GET_INTERFACE, 0, (void*)arg);
				if((ret & DISP_TV_CVBS) == DISP_TV_CVBS)
				{
					output_type = DISP_OUTPUT_TYPE_TV;
					output_mode = DISP_TV_MOD_PAL;
				}
				else if((ret & DISP_TV_YPBPR) == DISP_TV_YPBPR)
				{
					if((output_type != DISP_OUTPUT_TYPE_VGA) && (output_type == DISP_OUTPUT_TYPE_TV))
					{
						 output_type = DISP_OUTPUT_TYPE_TV;
						 output_mode = DISP_TV_MOD_720P_50HZ;
					}
				}
				else
				{
					output_type = DISP_OUTPUT_TYPE_NONE;
				}
			}
		}
	}

	if(output_type == DISP_OUTPUT_TYPE_LCD)
	{
		debug("lcd open\n");
		arg[0] = 0;
		arg[1] = 0;
		arg[2] = 0;
		ret = disp_ioctl(NULL, DISP_CMD_LCD_ON, 0, (void*)arg);
		debug("lcd open,ret=%d\n",ret);
	}
	else if(output_type == DISP_OUTPUT_TYPE_TV)
	{
		debug("tv open\n");
		arg[0] = output_mode;
		arg[1] = 0;
		arg[2] = 0;
		disp_ioctl(NULL, DISP_CMD_TV_SET_MODE, 0, arg);
		ret = disp_ioctl(NULL, DISP_CMD_TV_ON, 0, NULL);
	}
	else if(output_type == DISP_OUTPUT_TYPE_HDMI)
	{
		debug("hdmi open\n");
		arg[0] = output_mode;
		arg[1] = 0;
		arg[2] = 0;
		disp_ioctl(NULL, DISP_CMD_HDMI_SET_MODE, 0, arg);
		ret = disp_ioctl(NULL, DISP_CMD_HDMI_ON, 0, NULL);
	}
	else if(output_type == DISP_OUTPUT_TYPE_VGA)
	{
		debug("vga open\n");
		arg[0] = output_mode;
		arg[1] = 0;
		arg[2] = 0;
		disp_ioctl(NULL, DISP_CMD_VGA_SET_MODE, 0, arg);
		ret = disp_ioctl(NULL, DISP_CMD_VGA_ON, 0, NULL);
	}

	return ret;
#else
	DRV_lcd_open(0);
#endif
	return 0;
}


