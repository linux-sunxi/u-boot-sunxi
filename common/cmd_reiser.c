/*
 * (C) Copyright 2003 - 2004
 * Sysgo Real-Time Solutions, AG <www.elinos.com>
 * Pavel Bartusek <pba@sysgo.com>
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
 *
 */

/*
 * Reiserfs support
 */
#include <common.h>
#include <config.h>
#include <command.h>
#include <image.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>
#include <reiserfs.h>
#include <part.h>

#ifndef CONFIG_DOS_PARTITION
#error DOS partition support must be selected
#endif

/* #define	REISER_DEBUG */

#ifdef	REISER_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

int do_reiserls (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *filename = "/";
	int dev, part;
	block_dev_desc_t *dev_desc=NULL;
	disk_partition_t info;

	if (argc < 3)
		return CMD_RET_USAGE;

	part = get_device_and_partition(argv[1], argv[2], &dev_desc, &info, 1);
	if (part < 0)
		return 1;

	if (argc == 4) {
	    filename = argv[3];
	}

	dev = dev_desc->dev;
	PRINTF("Using device %s %d:%d, directory: %s\n", argv[1], dev, part, filename);

	reiserfs_set_blk_dev(dev_desc, &info);

	if (!reiserfs_mount(info.size)) {
		printf ("** Bad Reiserfs partition or disk - %s %d:%d **\n",  argv[1], dev, part);
		return 1;
	}

	if (reiserfs_ls (filename)) {
		printf ("** Error reiserfs_ls() **\n");
		return 1;
	};

	return 0;
}

U_BOOT_CMD(
	reiserls,	4,	1,	do_reiserls,
	"list files in a directory (default /)",
	"<interface> <dev[:part]> [directory]\n"
	"    - list files from 'dev' on 'interface' in a 'directory'"
);

/******************************************************************************
 * Reiserfs boot command intepreter. Derived from diskboot
 */
int do_reiserload (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *filename = NULL;
	int dev, part;
	ulong addr = 0, filelen;
	disk_partition_t info;
	block_dev_desc_t *dev_desc = NULL;
	unsigned long count;
	char *addr_str;

	switch (argc) {
	case 3:
		addr_str = getenv("loadaddr");
		if (addr_str != NULL) {
			addr = simple_strtoul (addr_str, NULL, 16);
		} else {
			addr = CONFIG_SYS_LOAD_ADDR;
		}
		filename = getenv ("bootfile");
		count = 0;
		break;
	case 4:
		addr = simple_strtoul (argv[3], NULL, 16);
		filename = getenv ("bootfile");
		count = 0;
		break;
	case 5:
		addr = simple_strtoul (argv[3], NULL, 16);
		filename = argv[4];
		count = 0;
		break;
	case 6:
		addr = simple_strtoul (argv[3], NULL, 16);
		filename = argv[4];
		count = simple_strtoul (argv[5], NULL, 16);
		break;

	default:
		return CMD_RET_USAGE;
	}

	if (!filename) {
		puts ("\n** No boot file defined **\n");
		return 1;
	}

	part = get_device_and_partition(argv[1], argv[2], &dev_desc, &info, 1);
	if (part < 0)
		return 1;

	dev = dev_desc->dev;

	printf("Loading file \"%s\" from %s device %d%c%c\n",
		filename, argv[1], dev,
		part ? ':' : ' ', part ? part + '0' : ' ');

	reiserfs_set_blk_dev(dev_desc, &info);

	if (!reiserfs_mount(info.size)) {
		printf ("** Bad Reiserfs partition or disk - %s %d:%d **\n",  argv[1], dev, part);
		return 1;
	}

	filelen = reiserfs_open(filename);
	if (filelen < 0) {
		printf("** File not found %s\n", filename);
		return 1;
	}
	if ((count < filelen) && (count != 0)) {
	    filelen = count;
	}

	if (reiserfs_read((char *)addr, filelen) != filelen) {
		printf("\n** Unable to read \"%s\" from %s %d:%d **\n", filename, argv[1], dev, part);
		return 1;
	}

	/* Loading ok, update default load address */
	load_addr = addr;

	printf ("\n%ld bytes read\n", filelen);
	setenv_hex("filesize", filelen);

	return filelen;
}

U_BOOT_CMD(
	reiserload,	6,	0,	do_reiserload,
	"load binary file from a Reiser filesystem",
	"<interface> <dev[:part]> [addr] [filename] [bytes]\n"
	"    - load binary file 'filename' from 'dev' on 'interface'\n"
	"      to address 'addr' from dos filesystem"
);
