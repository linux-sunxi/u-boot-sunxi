/*
 * Driver for NAND support, Rick Bronson
 * borrowed heavily from:
 * (c) 1999 Machine Vision Holdings, Inc.
 * (c) 1999, 2000 David Woodhouse <dwmw2@infradead.org>
 *
 * Ported 'dynenv' to 'nand env.oob' command
 * (C) 2010 Nanometrics, Inc.
 * 'dynenv' -- Dynamic environment offset in NAND OOB
 * (C) Copyright 2006-2007 OpenMoko, Inc.
 * Added 16-bit nand support
 * (C) 2004 Texas Instruments
 *
 * Copyright 2010 Freescale Semiconductor
 * The portions of this file whose copyright is held by Freescale and which
 * are not considered a derived work of GPL v2-only code may be distributed
 * and/or modified under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/boot_type.h>
#include <asm/arch/drv_display.h>
#define		BMP_MAGIC      (0x4D42)
#define		SUNXI_BMP_LIMIT (2 * 1024 *1024) 
#define		DISPALY_RESERVED_MEM (0x40000000 + (512 - 64 - 32) * 1024 * 1024 )

typedef struct tag_Picture
{
    void *Buffer;   		/* 存放图片数据     */
    __u32 BufferSize;   	/* buffer长度       */
    __u32 BitCount; 		/* 一个像素的bit数  */
    __u32 Width;    		/* 图片宽度         */
    __u32 Height;   		/* 图片高度         */
    __u32 RowSize;  		/* 图片一行的大小   */
}Picture_t;

/*bmp header struct*/
typedef struct
{
	__u16 bfType;                                   //bit map type
	__u32 bfSize;                                   //total file size
	__u16 bfReserved1;                              //reserved , must be 0
	__u16 bfReserved2;                              //reserved, must be 0
	__u32 bfOffBits;                                //full img offet
}
__attribute__ ((packed)) __bmp_file_head_t;

/*bmp info struct,windows*/
typedef struct
{
    __u32 biSize;                                   //bit map info size
    __u32 biWidth;                                  //img width with pixel unit
    __s32 biHeight;                                 //img height with pixel unit
    __u16 biPlanes;                                 //bit map planes,always 1
    __u16 biBitCount;                               //bit planes per pixel
    __u32 biCompression;                            //compress method
    __u32 biSizeImage;                              //img size with byte unit
    __u32 biXPelsPerMeter;                          //the horizontal resolution with pixel/metre unit
    __u32 biYPelsPerMeter;                          //the vertical resolution with pixel/metre unit
    __u32 biClrUsed;                                //the color numbers used by bmp
    __u32 biClrlmportant;                           //important color numbers
}
__attribute__ ((packed)) __bmp_info_header_t;

static int do_read_bmp_from_flash(char * argv[6])
{
	int ret = 0;
	char *const argvs[6] = {"fatload",argv[1],argv[2],argv[3],argv[4],NULL };
	ret	= do_fat_fsload(0,0,5,argv);
	if(ret){
		printf("do_fat_fsload fail in read_bmp_from_flash\n");
		return ret;
	}
	return 0;
}


/*
 * read_bmp_from_flash read bmp file from storage media
 * dev_flag:		storage media 0:nand  1:mmc 
 * dev_number:		devices number (if mmcblk0 the number is 0)
 * partition_number:	parttion number of device(the min valid number is 1)
 * addr:			the destination address in memory of bmpfile start
 * filename:		bmp file name in flash(must with path)
*/
static int read_bmp_from_flash(char dev_flag,char dev_number,char partition_number,unsigned int addr,char* filename)
{
	char * argv[6];
	int ret =0;
	char *const argv1[6] = { "fatload", "nand", "0", "40000000", "os_show/melis2.bmp", NULL }; 
	//uboot_spare_head.boot_data.storage_type
	argv[0]	= "fatload";
	argv[5]	= NULL;
	if(dev_flag)
		argv[1]	= "mmc";
	else
		argv[1] = "nand";

	if(partition_number != 0)
		sprintf(argv[2],"%u:%u",dev_number,partition_number);
	else
		sprintf(argv[2],"%u",dev_number);

	sprintf(argv[3],"%x",addr);
	argv[4]	= filename;
	ret = do_read_bmp_from_flash(argv);
	if(ret){
		printf("do_read_bmp_from_flash in read_bmp_from_flash\n");
		return ret;
	}
	return ret;
}

static char *parse_buffer;
static int sunxi_bmp_parse(u32 start,Picture_t *PictureInfo)
{
	__s32                   ret = -1;
    int						length;
    char					*tmp_buffer;
	__bmp_file_head_t 		*bmp_file_header;
	__bmp_info_header_t 	*bmp_info_header;

	bmp_file_header = (__bmp_file_head_t *)start;
	if(bmp_file_header->bfType != BMP_MAGIC){
		sunxi_printf("bad BMP_MAGIC!\n");
		goto _get_bmp_by_name_error;
    }
	bmp_info_header = (__bmp_info_header_t *)(start + sizeof(__bmp_file_head_t));
	PictureInfo->BitCount = bmp_info_header->biBitCount;
	PictureInfo->Height = (bmp_info_header->biHeight > 0) ?
							bmp_info_header->biHeight : (-bmp_info_header->biHeight);
    PictureInfo->Width = bmp_info_header->biWidth;
	PictureInfo->RowSize  = PictureInfo->BitCount * PictureInfo->Width / 8;
	PictureInfo->BufferSize = PictureInfo->Height * PictureInfo->RowSize;
	length = PictureInfo->BufferSize+sizeof(__bmp_file_head_t)+ sizeof(__bmp_info_header_t);
	/*
	parse_buffer = (char *)malloc(length);
	if(!parse_buffer){
		goto _get_bmp_by_name_error;
	}
	*/
	//memcpy(parse_buffer,(void *)start,length);
	PictureInfo->Buffer = (char *)malloc(PictureInfo->BufferSize);
	if(!PictureInfo->Buffer)
		goto _get_bmp_by_name_error;

	//tmp_buffer = parse_buffer + bmp_file_header->bfOffBits;
	tmp_buffer = (char *)start + bmp_file_header->bfOffBits;
	if(bmp_info_header->biHeight > 0){
        __u32 i, j;
        __s32 *src, *dest;

        dest = (int *)PictureInfo->Buffer;
        for(i=0; i<(bmp_info_header->biHeight); i++){
            src  = (int *)(tmp_buffer + (bmp_info_header->biHeight - i - 1) * (bmp_info_header->biWidth << 2));
            for(j=0;j<bmp_info_header->biWidth; j++){
                *dest ++ = *src ++;
            }
        }
    }else{
    	memcpy(PictureInfo->Buffer, tmp_buffer, PictureInfo->BufferSize);
    }
	ret = 0;
_get_bmp_by_name_error:
/*
	if(parse_bufferbuffer){
		free(parse_buffer);
    }
*/
	return ret;
}

static int disp_bmp_show(Picture_t *PictureInfo)
{
    __disp_layer_info_t layer_para;
    __u32 layer_hdl;
    __u32 screen_width, screen_height;
    __u32 arg[4];
    
	if(PictureInfo == NULL)
	{
		printf("null para \n");
		return -1;
	}
    arg[0] = 0;
    screen_width = disp_ioctl(NULL, DISP_CMD_SCN_GET_WIDTH, (void*)arg);
    screen_height = disp_ioctl(NULL, DISP_CMD_SCN_GET_HEIGHT, (void*)arg);
    memset(&layer_para, 0, sizeof(__disp_layer_info_t));
    layer_para.fb.addr[0]        = (__u32)PictureInfo->Buffer;
    layer_para.fb.size.width    = PictureInfo->Width;
    layer_para.fb.size.height   = PictureInfo->Height;
    layer_para.fb.mode          = DISP_MOD_INTERLEAVED;
    layer_para.fb.format        = (PictureInfo->BitCount == 24)? DISP_FORMAT_RGB888:DISP_FORMAT_ARGB8888;
    layer_para.fb.br_swap       = 0;
    layer_para.fb.seq           = DISP_SEQ_ARGB;
    layer_para.fb.b_trd_src     = 0;
    layer_para.fb.trd_mode      = 0;
    layer_para.ck_enable        = 0;
    layer_para.alpha_en         = 1; 
    layer_para.alpha_val        = 0xff;
    layer_para.pipe             = 0; 
    layer_para.src_win.x        = 0;
    layer_para.src_win.y        = 0;
    layer_para.src_win.width    = PictureInfo->Width;
    layer_para.src_win.height   = PictureInfo->Height;
    layer_para.scn_win.x        = (screen_width - PictureInfo->Width) / 2;
    layer_para.scn_win.y        = (screen_height - PictureInfo->Height) / 2;
    layer_para.scn_win.width    = PictureInfo->Width;
    layer_para.scn_win.height   = PictureInfo->Height;
    layer_para.b_trd_out        = 0;
    layer_para.out_trd_mode     = 0;
    
    arg[0] = 0;
    arg[1] = DISP_LAYER_WORK_MODE_NORMAL;
    layer_hdl = disp_ioctl(NULL, DISP_CMD_LAYER_REQUEST, (void*)arg);
    if(layer_hdl <= 0)
    {
        printf("DISP_CMD_LAYER_REQUEST, fail\n");
		return -1;
    }
    else
    {
        layer_para.mode = DISP_LAYER_WORK_MODE_NORMAL; 
        arg[0] = 0;
        arg[1] = layer_hdl;
        arg[2] = (__u32)&layer_para;
        arg[3] = 0;
        disp_ioctl(NULL,DISP_CMD_LAYER_SET_PARA,(void*)arg);

        arg[0] = 0;
        arg[1] = layer_hdl;
        arg[2] = 0;
        arg[3] = 0;
        disp_ioctl(NULL,DISP_CMD_LAYER_OPEN,(void*)arg);
        
        printf("show bmp ok\n");
    }

	arg[0] = DISP_REG_IMAGE0;
	disp_ioctl(NULL,DISP_CMD_PRINT_REG,(void*)arg);
    return 0;	
}


static int display_bmp(char dev_flag,char dev_number,char partition_number,char *file,unsigned int buff)
{
	int ret = 0;
	Picture_t PictureInfo;
	unsigned int addr;
	if(buff){
		printf("use buff\n");
		addr	= (unsigned int)buff;
	}else{
		printf("use reserved mem\n");
		addr	= (unsigned int)DISPALY_RESERVED_MEM;
	}
	ret = read_bmp_from_flash(dev_flag,dev_number,partition_number,addr,file);
	if(ret){
		sunxi_printf("read_bmp_from_flash fail\n");
		goto done;
	}
	ret = sunxi_bmp_parse(addr,&PictureInfo);
	if(ret){
		sunxi_printf("sunxi_bmp_parse fail\n");
		goto done;
	}

	ret	= disp_bmp_show(&PictureInfo);
	if(ret){
		sunxi_printf("disp_bmp_show fail\n");
		goto done;
	}
done:
	return ret;	
}




//-------------------------------------------------------------------------------
int do_sunxi_display(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	unsigned int addr,length;
	char *ep;
	char *file;
	char *buf=NULL;
	char storage_type,dev_number,partition_number,control;

	if(argc < 6){
		printf("sunxi_display <interface> <dev> <part>  <addr> <filename>\n");
		return 1;
	}

	length = SUNXI_BMP_LIMIT;
	if(!strcmp(argv[1],"mmc"))
		storage_type	= 1;
	else if(!strcmp(argv[1],"nand"))
		storage_type	= 0;
	else{
		printf("the media doesn`t support now!\n" );
		return 1;
	}

	dev_number			= (char)simple_strtoul(argv[2],&ep,16);
	partition_number	= (char)simple_strtoul(argv[3],&ep,16);
	addr				= (unsigned int)simple_strtoul(argv[4],&ep,16);
	control				= (char)simple_strtoul(argv[6],&ep,16);
	if(control){
		printf("reserved mode !\n");
		display_bmp(storage_type,dev_number,partition_number,argv[5],0);
	}else{
		if(addr<40000000){
			buf =(char *) malloc(length);
			display_bmp(storage_type,dev_number,partition_number,argv[5],(unsigned int)buf);
		}else{
			display_bmp(storage_type,dev_number,partition_number,argv[5],addr);
		}
	}
	if(buf)
		free(buf);
	return 0;
}

U_BOOT_CMD(
	sunxi_display, CONFIG_SYS_MAXARGS, 0, do_sunxi_display,
	"sunxi_flash sub-system",
	"sunxi_display <interface> <dev> <part>  <addr> <filename>\n"
);
