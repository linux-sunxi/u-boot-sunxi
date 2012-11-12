/*
 * (C) Copyright 2002
 * Detlev Zundel, DENX Software Engineering, dzu@denx.de.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * BMP handling routines
 */

#include <common.h>
#include <bmp_layout.h>
#include <command.h>
#include <malloc.h>

typedef struct
{
	int x;
	int y;
	int bit;
	void *buffer;
}
sunxi_bmp_store_t;

static int sunxi_bmp_info (uint addr);
static int sunxi_bmp_decode(ulong addr, uint de_addr, sunxi_bmp_store_t *bmp_info);
static int sunxi_bmp_display(sunxi_bmp_store_t bmp_info);
/*
 * Allocate and decompress a BMP image using gunzip().
 *
 * Returns a pointer to the decompressed image data. Must be freed by
 * the caller after use.
 *
 * Returns NULL if decompression failed, or if the decompressed data
 * didn't contain a valid BMP signature.
 */

static int do_sunxi_bmp_info(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	uint addr;

	if(argc == 2)
	{
		/* use argument only*/
		addr = simple_strtoul(argv[1], NULL, 16);
		debug("bmp addr=%x\n", addr);
	}
	else if(argc == 3)
	{
		char  load_addr[8];
		char  filename[32];
		char *const bmp_argv[6] = { "fatload", "sunxi_flash", "0", load_addr, filename, NULL };

		addr = simple_strtoul(argv[1], NULL, 16);
		memcpy(load_addr, argv[1], 8);
		memset(filename, 0, 32);
		memcpy(filename, argv[2], strlen(argv[2]));
#ifdef DEBUG
	    int i;

		for(i=0;i<6;i++)
		{
	        printf("argv[%d] = %s\n", i, argv[i]);
		}
#endif
	    if(do_fat_fsload(0, 0, 5, bmp_argv))
		{
		   printf("sunxi bmp info error : unable to open bmp file %s\n", argv[2]);

		   return cmd_usage(cmdtp);
	    }
	}
	else
	{
		return cmd_usage(cmdtp);
	}

	return (sunxi_bmp_info(addr));
}

U_BOOT_CMD(
	sunxi_bmp_info,	3,	1,	do_sunxi_bmp_info,
	"manipulate BMP image data",
	"only one para : the address where the bmp stored\n"
);


static int do_sunxi_bmp_display(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	uint addr;
	uint de_addr;
	sunxi_bmp_store_t bmp_info;

	if(argc == 2)
	{
		/* use argument only*/
		addr = simple_strtoul(argv[1], NULL, 16);
		de_addr = SUNXI_DISPLAY_FRAME_BUFFER_ADDR;
	}
	else if(argc == 3)
	{
		addr = simple_strtoul(argv[1], NULL, 16);
		de_addr = simple_strtoul(argv[2], NULL, 16);
	}
	else if(argc == 4)
	{
		char  load_addr[8];
		char  filename[32];
		char *const bmp_argv[6] = { "fatload", "sunxi_flash", "0", load_addr, filename, NULL };

		addr = simple_strtoul(argv[1], NULL, 16);
		memcpy(load_addr, argv[1], 8);
		memset(filename, 0, 32);
		memcpy(filename, argv[3], strlen(argv[3]));
		de_addr = simple_strtoul(argv[2], NULL, 16);
#ifdef DEBUG
	    int i;

		for(i=0;i<6;i++)
		{
	        printf("argv[%d] = %s\n", i, argv[i]);
		}
#endif
	    if(do_fat_fsload(0, 0, 5, bmp_argv))
		{
		   printf("sunxi bmp info error : unable to open bmp file %s\n", argv[2]);

		   return cmd_usage(cmdtp);
	    }
	}
	else
	{
		return cmd_usage(cmdtp);
	}
	if(de_addr < 0x40000000)
	{
		de_addr = SUNXI_DISPLAY_FRAME_BUFFER_ADDR;
	}
	debug("bmp addr %x, display addr %x\n", addr, de_addr);

	if(!sunxi_bmp_decode(addr, de_addr, &bmp_info))
	{
		debug("decode bmp ok\n");

		return sunxi_bmp_display(bmp_info);
	}
	debug("decode bmp error\n");

	return -1;
}


U_BOOT_CMD(
	sunxi_bmp_display,	4,	1,	do_sunxi_bmp_display,
	"manipulate BMP image data",
	"sunxi_bmp_display addr [de addr]\n"
	"parameters 1 : the address where the bmp stored\n"
	"parameters 2 : option para, the address where the bmp display\n"
);

int sunxi_logo_display(void)
{
	sunxi_bmp_store_t bmp_info;

	char *const bmp_argv[6] = { "fatload", "sunxi_flash", "0", "40000000", "bootlogo.bmp", NULL };
    if(do_fat_fsload(0, 0, 5, bmp_argv))
	{
	   printf("sunxi bmp info error : unable to open logo file %s\n");

	   return -1;
    }
	if(!sunxi_bmp_decode(0x40000000, SUNXI_DISPLAY_FRAME_BUFFER_ADDR, &bmp_info))
	{
		debug("decode bmp ok\n");

		return sunxi_bmp_display(bmp_info);
	}

	return -1;
}
/*
 * Subroutine:  bmp_info
 *
 * Description: Show information about bmp file in memory
 *
 * Inputs:	addr		address of the bmp file
 *
 * Return:      None
 *
 */
static int sunxi_bmp_info(uint addr)
{
	bmp_image_t *bmp=(bmp_image_t *)addr;

	if((bmp->header.signature[0]!='B') || (bmp->header.signature[1]!='M'))
	{
		printf("this is not a bmp picture\n");

		return -1;
	}
	debug("bmp picture dectede\n");

	printf("Image size    : %d x %d\n", bmp->header.width, (bmp->header.height & 0x80000000) ? (-bmp->header.height):(bmp->header.height));
	printf("Bits per pixel: %d\n", bmp->header.bit_count);

	return(0);
}

/*
 * Subroutine:  bmp_display
 *
 * Description: Display bmp file located in memory
 *
 * Inputs:	addr		address of the bmp file
 *
 * Return:      None
 *
 */
static int sunxi_bmp_decode(ulong addr, uint de_addr, sunxi_bmp_store_t *bmp_info)
{
	char *tmp_buffer;
	char *bmp_data;
	bmp_image_t *bmp = (bmp_image_t *)addr;
	int x, y, bmp_bpix;

	if((bmp->header.signature[0]!='B') || (bmp->header.signature[1] !='M'))
	{
		printf("this is not a bmp picture\n");

		return -1;
	}
	debug("bmp dectece\n");

	bmp_bpix = bmp->header.bit_count/8;
	if((bmp_bpix != 3) && (bmp_bpix != 4))
	{
		printf("no support bmp picture without bpix 24 or 32\n");

		return -1;
	}
	debug("bmp bitcount %d\n", bmp->header.bit_count);
	x = bmp->header.width;
	y = (bmp->header.height & 0x80000000) ? (-bmp->header.height):(bmp->header.height);
	debug("bmp x = %x, bmp y = %x\n", x, y);

	tmp_buffer = (char *)de_addr;
	bmp_data = (char *)(addr + bmp->header.data_offset);
	if(bmp->header.height & 0x80000000)
    {
        memcpy(tmp_buffer, bmp_data, x * y * bmp_bpix);
    }
    else
    {
    	uint i, line_bytes;
        char *src;

		line_bytes = x * bmp_bpix;
		for(i=0; i<y; i++)
        {
        	src = bmp_data + (y - i - 1) * line_bytes;
        	memcpy(tmp_buffer, src, line_bytes);
            tmp_buffer += line_bytes;
        }
    }
    bmp_info->x = x;
    bmp_info->y = y;
    bmp_info->bit = bmp->header.bit_count;
    bmp_info->buffer = de_addr;

	return 0;
}

static int sunxi_bmp_display(sunxi_bmp_store_t bmp_info)
{
	debug("begin to set framebuffer\n");
	if(board_display_framebuffer_set(bmp_info.x, bmp_info.y, bmp_info.bit, (void *)bmp_info.buffer))
	{
		printf("sunxi bmp display error : set frame buffer error\n");

		return -2;
	}
	debug("begin to show layer\n");
	board_display_layer_show(0);
	debug("bmp display finish\n");

	return 0;
}

#define  BMP_TEST_MAX    11
int do_sunxi_bmp_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char  filename[32];
	char  *const name[11] = {"bat0.bmp", "bat1.bmp", "bat2.bmp", "bat3.bmp", "bat4.bmp", "bat5.bmp", "bat6.bmp",
		            "bat7.bmp", "bat8.bmp", "bat9.bmp", "bat10.bmp"};
	char *const bmp_argv[6] = { "fatload", "sunxi_flash", "0", "0x40000000", filename, NULL };
	int  i = 0;
	sunxi_bmp_store_t  bmp_store[11];

	for(i=0;i<BMP_TEST_MAX;i++)
	{
		memset(filename, 0, 32);
		memcpy(filename, name[i], strlen(name[i]));

		if(do_fat_fsload(0, 0, 5, bmp_argv))
		{
		   printf("sunxi bmp info error : unable to open bmp file %s\n", name[i]);

		   return -1;
	    }
	    if(sunxi_bmp_decode(0x40000000, (0x41000000 + i * 1024 * 1024), &bmp_store[i]))
	    {
	    	printf("sunxi bmp decode error : unable to decode %s\n", name[i]);

	    	return -1;
	    }
	}
	i = 0;
	if(board_display_framebuffer_set(bmp_store[0].x, bmp_store[0].y, bmp_store[0].bit, (void *)bmp_store[0].buffer))
	{
		printf("sunxi bmp display error : set frame buffer error\n");

		return -2;
	}
	board_display_layer_show(0);
	while(1)
	{
		__msdelay(2000);
		i ++;
		if(i >= BMP_TEST_MAX)
		{
			i = 0;
		}
		board_display_framebuffer_change(bmp_store[i].buffer);
	}

	return 0;
}


U_BOOT_CMD(
	sunxi_bmp_test,	1,	1,	do_sunxi_bmp_test,
	"bmp test\n",
	"test only\n"
);

