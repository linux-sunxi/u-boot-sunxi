/*
 * Copyright 2008 - 2009 (C) Wind River Systems, Inc.
 * Tom Rix <Tom.Rix@windriver.com>
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
 * Part of the rx_handler were copied from the Android project.
 * Specifically rx command parsing in the  usb_rx_data_complete
 * function of the file bootable/bootloader/legacy/usbloader/usbloader.c
 *
 * The logical naming of flash comes from the Android project
 * Thse structures and functions that look like fastboot_flash_*
 * They come from bootable/bootloader/legacy/libboot/flash.c
 *
 * This is their Copyright:
 *
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <asm/byteorder.h>
#include <common.h>
#include <command.h>
#include <nand.h>
#include <fastboot.h>
#include <environment.h>
#include <sparse.h>
#ifdef CONFIG_ALLWINNER
#include <asm/arch/boot_type.h>
#endif
#ifdef CONFIG_FASTBOOT

#define MY_DEBUG
/* Use do_reset for fastboot's 'reboot' command */
extern int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
/* Use do_nand for fastboot's flash commands */

extern int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[]);
extern int do_mmcops (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern env_t *env_ptr;

/* Use do_setenv and do_env_save to permenantly save data */
extern int do_env_save (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_env_set ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* Use do_bootm and do_go for fastboot's 'boot' command */
#ifdef CONFIG_CMD_BOOTM
extern int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif
extern int do_go (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

/* Forward decl */
static int tx_handler(void);
static int rx_handler (const unsigned char *buffer, unsigned int buffer_size);
static void reset_handler (void);

static struct cmd_fastboot_interface interface =
{
	.rx_handler            = rx_handler,
	.reset_handler         = reset_handler,
	.product_name          = NULL,
	.serial_no             = NULL,
	.nand_block_size       = 0,
	.transfer_buffer       = (unsigned char *)0xffffffff,
	.transfer_buffer_size  = 0,
};

static unsigned int download_size;
static unsigned int download_bytes;
static unsigned int download_bytes_unpadded;
static unsigned int download_error;
static unsigned int continue_booting;
static unsigned int upload_size;
static unsigned int upload_bytes;
static unsigned int upload_error;
#if defined(CONFIG_STORAGE_EMMC)
static unsigned int mmc_controller_no;
#endif

/* To support the Android-style naming of flash */
#define MAX_PTN 16
static fastboot_ptentry ptable[MAX_PTN];
static unsigned int pcount;
static int static_pcount = -1;

static void set_env(char *var, char *val)
{
	char *setenv[4]  = { "setenv", NULL, NULL, NULL, };

	setenv[1] = var;
	setenv[2] = val;

	do_env_set(NULL, 0, 3, setenv);
}

static void save_env(struct fastboot_ptentry *ptn,
		     char *var, char *val)
{
	char start[32], length[32];
	char ecc_type[32];

	char *lock[5]    = { "nand", "lock",   NULL, NULL, NULL, };
	char *unlock[5]  = { "nand", "unlock", NULL, NULL, NULL, };
	char *ecc[4]     = { "nand", "ecc",    NULL, NULL, };
	char *saveenv[2] = { "setenv", NULL, };

	lock[2] = unlock[2] = start;
	lock[3] = unlock[3] = length;

	set_env (var, val);

	/* Some flashing requires the nand's ecc to be set */
	ecc[2] = ecc_type;
	if ((ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) &&
	    (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC))	{
		/* Both can not be true */
		printf("Warning can not do hw and sw ecc for partition '%s'\n", ptn->name);
		printf("Ignoring these flags\n");
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) {
		sprintf(ecc_type, "hw");
		do_nand(NULL, 0, 3, ecc);
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC) {
		sprintf(ecc_type, "sw");
		do_nand(NULL, 0, 3, ecc);
	}
	sprintf(start, "0x%x", ptn->start);
	sprintf(length, "0x%x", ptn->length);

	/* This could be a problem is there is an outstanding lock */
	do_nand(NULL, 0, 4, unlock);
	do_env_save(NULL, 0, 1, saveenv);
	do_nand(NULL, 0, 4, lock);
}

static void save_block_values(struct fastboot_ptentry *ptn,
			      unsigned int offset,
			      unsigned int size)
{
	struct fastboot_ptentry *env_ptn;

	char var[64], val[32];
	char start[32], length[32];
	char ecc_type[32];

	char *lock[5]    = { "nand", "lock",   NULL, NULL, NULL, };
	char *unlock[5]  = { "nand", "unlock", NULL, NULL, NULL, };
	char *ecc[4]     = { "nand", "ecc",    NULL, NULL, };
	char *setenv[4]  = { "setenv", NULL, NULL, NULL, };
	char *saveenv[2] = { "setenv", NULL, };

	setenv[1] = var;
	setenv[2] = val;
	lock[2] = unlock[2] = start;
	lock[3] = unlock[3] = length;

	printf ("saving it..\n");

	if (size == 0)
	{
		/* The error case, where the variables are being unset */

		sprintf (var, "%s_nand_offset", ptn->name);
		sprintf (val, "");
		do_env_set (NULL, 0, 3, setenv);

		sprintf (var, "%s_nand_size", ptn->name);
		sprintf (val, "");
		do_env_set (NULL, 0, 3, setenv);
	}
	else
	{
		/* Normal case */

		sprintf (var, "%s_nand_offset", ptn->name);
		sprintf (val, "0x%x", offset);

		printf ("%s %s %s\n", setenv[0], setenv[1], setenv[2]);

		do_env_set (NULL, 0, 3, setenv);

		sprintf (var, "%s_nand_size", ptn->name);

		sprintf (val, "0x%x", size);

		printf ("%s %s %s\n", setenv[0], setenv[1], setenv[2]);

		do_env_set (NULL, 0, 3, setenv);
	}


	/* Warning :
	   The environment is assumed to be in a partition named 'enviroment'.
	   It is very possible that your board stores the enviroment
	   someplace else. */
	env_ptn = fastboot_flash_find_ptn("env");

	if (env_ptn)
	{
		/* Some flashing requires the nand's ecc to be set */
		ecc[2] = ecc_type;
		if ((env_ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) &&
		    (env_ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC))
		{
			/* Both can not be true */
			printf ("Warning can not do hw and sw ecc for partition '%s'\n", ptn->name);
			printf ("Ignoring these flags\n");
		}
		else if (env_ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC)
		{
			sprintf (ecc_type, "hw");
			do_nand (NULL, 0, 3, ecc);
		}
		else if (env_ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC)
		{
			sprintf (ecc_type, "sw");
			do_nand (NULL, 0, 3, ecc);
		}

		sprintf (start, "0x%x", env_ptn->start);
		sprintf (length, "0x%x", env_ptn->length);

		/* This could be a problem is there is an outstanding lock */
		do_nand (NULL, 0, 4, unlock);
	}

	do_env_save (NULL, 0, 1, saveenv);

	if (env_ptn)
	{
		do_nand (NULL, 0, 4, lock);
	}
}

static void reset_handler ()
{
	/* If there was a download going on, bail */
	download_size = 0;
	download_bytes = 0;
	download_bytes_unpadded = 0;
	download_error = 0;
	continue_booting = 0;
	upload_size = 0;
	upload_bytes = 0;
	upload_error = 0;
}

/* When save = 0, just parse.  The input is unchanged
   When save = 1, parse and do the save.  The input is changed */
static int parse_env(void *ptn, char *err_string, int save, int debug)
{
	int ret = 1;
	unsigned int sets = 0;
	unsigned int comment_start = 0;
	char *var = NULL;
	char *var_end = NULL;
	char *val = NULL;
	char *val_end = NULL;
	unsigned int i;

	char *buff = (char *)interface.transfer_buffer;
	unsigned int size = download_bytes_unpadded;

	/* The input does not have to be null terminated.
	   This will cause a problem in the corner case
	   where the last line does not have a new line.
	   Put a null after the end of the input.

	   WARNING : Input buffer is assumed to be bigger
	   than the size of the input */
	if (save)
		buff[size] = 0;

	for (i = 0; i < size; i++) {

		if (NULL == var) {

			/*
			 * Check for comments, comment ok only on
			 * mostly empty lines
			 */
			if (buff[i] == '#')
				comment_start = 1;

			if (comment_start) {
				if  ((buff[i] == '\r') ||
				     (buff[i] == '\n')) {
					comment_start = 0;
				}
			} else {
				if (!((buff[i] == ' ') ||
				      (buff[i] == '\t') ||
				      (buff[i] == '\r') ||
				      (buff[i] == '\n'))) {
					/*
					 * Normal whitespace before the
					 * variable
					 */
					var = &buff[i];
				}
			}

		} else if (((NULL == var_end) || (NULL == val)) &&
			   ((buff[i] == '\r') || (buff[i] == '\n'))) {

			/* This is the case when a variable
			   is unset. */

			if (save) {
				/* Set the var end to null so the
				   normal string routines will work

				   WARNING : This changes the input */
				buff[i] = '\0';

				save_env(ptn, var, val);

				if (debug)
					printf("Unsetting %s\n", var);
			}

			/* Clear the variable so state is parse is back
			   to initial. */
			var = NULL;
			var_end = NULL;
			sets++;
		} else if (NULL == var_end) {
			if ((buff[i] == ' ') ||
			    (buff[i] == '\t'))
				var_end = &buff[i];
		} else if (NULL == val) {
			if (!((buff[i] == ' ') ||
			      (buff[i] == '\t')))
				val = &buff[i];
		} else if (NULL == val_end) {
			if ((buff[i] == '\r') ||
			    (buff[i] == '\n')) {
				/* look for escaped cr or ln */
				if ('\\' == buff[i - 1]) {
					/* check for dos */
					if ((buff[i] == '\r') &&
					    (buff[i+1] == '\n'))
						buff[i + 1] = ' ';
					buff[i - 1] = buff[i] = ' ';
				} else {
					val_end = &buff[i];
				}
			}
		} else {
			sprintf(err_string, "Internal Error");

			if (debug)
				printf("Internal error at %s %d\n",
				       __FILE__, __LINE__);
			return 1;
		}
		/* Check if a var / val pair is ready */
		if (NULL != val_end) {
			if (save) {
				/* Set the end's with nulls so
				   normal string routines will
				   work.

				   WARNING : This changes the input */
				*var_end = '\0';
				*val_end = '\0';

				save_env(ptn, var, val);

				if (debug)
					printf("Setting %s %s\n", var, val);
			}

			/* Clear the variable so state is parse is back
			   to initial. */
			var = NULL;
			var_end = NULL;
			val = NULL;
			val_end = NULL;

			sets++;
		}
	}

	/* Corner case
	   Check for the case that no newline at end of the input */
	if ((NULL != var) &&
	    (NULL == val_end)) {
		if (save) {
			/* case of val / val pair */
			if (var_end)
				*var_end = '\0';
			/* else case handled by setting 0 past
			   the end of buffer.
			   Similar for val_end being null */
			save_env(ptn, var, val);

			if (debug) {
				if (var_end)
					printf("Trailing Setting %s %s\n", var, val);
				else
					printf("Trailing Unsetting %s\n", var);
			}
		}
		sets++;
	}
	/* Did we set anything ? */
	if (0 == sets)
		sprintf(err_string, "No variables set");
	else
		ret = 0;

	return ret;
}

static int saveenv_to_ptn(struct fastboot_ptentry *ptn, char *err_string)
{
	int ret = 1;
	int save = 0;
	int debug = 0;

	/* err_string is only 32 bytes
	   Initialize with a generic error message. */
	sprintf(err_string, "%s", "Unknown Error");

	/* Parse the input twice.
	   Only save to the enviroment if the entire input if correct */
	save = 0;
	if (0 == parse_env(ptn, err_string, save, debug)) {
		save = 1;
		ret = parse_env(ptn, err_string, save, debug);
	}
	return ret;
}

static void set_ptn_ecc(struct fastboot_ptentry *ptn)
{
	char ecc_type[32];
	char *ecc[4] = {"nand", "ecc", NULL, NULL, };

	/* Some flashing requires the nand's ecc to be set */
	ecc[2] = ecc_type;
	if ((ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) &&
	    (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC)) {
		/* Both can not be true */
		printf("Warning can not do hw and sw ecc for partition '%s'\n",
		       ptn->name);
		printf("Ignoring these flags\n");
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) {
		sprintf(ecc_type, "hw");
		do_nand(NULL, 0, 3, ecc);
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC) {
		sprintf(ecc_type, "sw");
		do_nand(NULL, 0, 3, ecc);
	}
}

#if 0
static int write_to_ptn(struct fastboot_ptentry *ptn)
{
	int ret = 1;
	char start[32], length[32];
	char wstart[32], wlength[32], addr[32];
	char write_type[32];
	int repeat, repeat_max;

	char *lock[5]   = { "nand", "lock",   NULL, NULL, NULL, };
	char *unlock[5] = { "nand", "unlock", NULL, NULL, NULL,	};
	char *write[6]  = { "nand", "write",  NULL, NULL, NULL, NULL, };
	char *erase[5]  = { "nand", "erase",  NULL, NULL, NULL, };

	lock[2] = unlock[2] = erase[2] = start;
	lock[3] = unlock[3] = erase[3] = length;

	write[1] = write_type;
	write[2] = addr;
	write[3] = wstart;
	write[4] = wlength;

	printf("flashing '%s'\n", ptn->name);

	/* Which flavor of write to use */
	if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_I)
		sprintf(write_type, "write.i");
#ifdef CFG_NAND_YAFFS_WRITE
	else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS)
		sprintf(write_type, "write.yaffs");
#endif
	else
		sprintf(write_type, "write");
#ifdef MY_DEBUG
	printf("%s %d\n", __FILE__, __LINE__);
#endif
	set_ptn_ecc(ptn);
#ifdef MY_DEBUG
	printf("%s %d\n", __FILE__, __LINE__);
#endif
	/* Some flashing requires writing the same data in multiple,
	   consecutive flash partitions */
	repeat_max = 1;
	if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_REPEAT_MASK) {
		if (ptn->flags &
		    FASTBOOT_PTENTRY_FLAGS_WRITE_CONTIGUOUS_BLOCK) {
			printf("Warning can not do both 'contiguous block' and 'repeat' writes for for partition '%s'\n", ptn->name);
			printf("Ignoring repeat flag\n");
		} else {
			repeat_max = ptn->flags &
				FASTBOOT_PTENTRY_FLAGS_REPEAT_MASK;
		}
	}
#ifdef MY_DEBUG
	printf("%s %d\n", __FILE__, __LINE__);
#endif

	/* Unlock the whole partition instead of trying to
	   manage special cases */
	sprintf(length, "0x%x", ptn->length * repeat_max);

	for (repeat = 0; repeat < repeat_max; repeat++) {
		sprintf(start, "0x%x", ptn->start + (repeat * ptn->length));
#ifdef MY_DEBUG
			printf("%s %d\n", __FILE__, __LINE__);
#endif

		do_nand(NULL, 0, 4, unlock);
#ifdef MY_DEBUG
	printf("%s %d\n", __FILE__, __LINE__);
#endif

		do_nand(NULL, 0, 4, erase);
#ifdef MY_DEBUG
			printf("%s %d\n", __FILE__, __LINE__);
#endif

		if ((ptn->flags &
		     FASTBOOT_PTENTRY_FLAGS_WRITE_NEXT_GOOD_BLOCK) &&
		    (ptn->flags &
		     FASTBOOT_PTENTRY_FLAGS_WRITE_CONTIGUOUS_BLOCK)) {
			/* Both can not be true */
			printf("Warning can not do 'next good block' and 'contiguous block' for partition '%s'\n", ptn->name);
			printf("Ignoring these flags\n");
		} else if (ptn->flags &
			   FASTBOOT_PTENTRY_FLAGS_WRITE_NEXT_GOOD_BLOCK) {
			/* Keep writing until you get a good block
			   transfer_buffer should already be aligned */
			if (interface.nand_block_size) {
				unsigned int blocks = download_bytes /
					interface.nand_block_size;
				unsigned int i = 0;
				unsigned int offset = 0;

				sprintf(wlength, "0x%x",
					interface.nand_block_size);
				while (i < blocks) {
					/* Check for overflow */
					if (offset >= ptn->length)
						break;

					/* download's address only advance
					   if last write was successful */
					sprintf(addr, "0x%x",
						interface.transfer_buffer +
						(i * interface.nand_block_size));

					/* nand's address always advances */
					sprintf(wstart, "0x%x",
						ptn->start + (repeat * ptn->length) + offset);

					ret = do_nand(NULL, 0, 5, write);
					if (ret)
						break;
					else
						i++;

					/* Go to next nand block */
					offset += interface.nand_block_size;
				}
			} else {
				printf("Warning nand block size can not be 0 when using 'next good block' for partition '%s'\n", ptn->name);
				printf("Ignoring write request\n");
			}
		} else if (ptn->flags &
			 FASTBOOT_PTENTRY_FLAGS_WRITE_CONTIGUOUS_BLOCK) {
			/* Keep writing until you get a good block
			   transfer_buffer should already be aligned */
			if (interface.nand_block_size) {
				if (0 == nand_curr_device) {
					nand_info_t *nand;
					unsigned long off;
					unsigned int ok_start;

					nand = &nand_info[nand_curr_device];

					printf("\nDevice %d bad blocks:\n",
					       nand_curr_device);

					/* Initialize the ok_start to the
					   start of the partition
					   Then try to find a block large
					   enough for the download */
					ok_start = ptn->start;

					/* It is assumed that the start and
					   length are multiples of block size */
					for (off = ptn->start;
					     off < ptn->start + ptn->length;
					     off += nand->erasesize) {
						if (nand_block_isbad(nand, off)) {
							/* Reset the ok_start
							   to the next block */
							ok_start = off +
								nand->erasesize;
						}

						/* Check if we have enough
						   blocks */
						if ((ok_start - off) >=
						    download_bytes)
							break;
					}

					/* Check if there is enough space */
					if (ok_start + download_bytes <=
					    ptn->start + ptn->length) {
						sprintf(addr,    "0x%x", interface.transfer_buffer);
						sprintf(wstart,  "0x%x", ok_start);
						sprintf(wlength, "0x%x", download_bytes);

						ret = do_nand(NULL, 0, 5, write);

						/* Save the results into an
						   environment variable on the
						   format
						   ptn_name + 'offset'
						   ptn_name + 'size'  */
						if (ret) {
							/* failed */
							save_block_values(ptn, 0, 0);
						} else {
							/* success */
							save_block_values(ptn, ok_start, download_bytes);
						}
					} else {
						printf("Error could not find enough contiguous space in partition '%s' \n", ptn->name);
						printf("Ignoring write request\n");
					}
				} else {
					/* TBD : Generalize flash handling */
					printf("Error only handling 1 NAND per board");
					printf("Ignoring write request\n");
				}
			} else {
				printf("Warning nand block size can not be 0 when using 'continuous block' for partition '%s'\n", ptn->name);
				printf("Ignoring write request\n");
			}
		} else {
			/* Normal case */
			sprintf(addr,    "0x%x", interface.transfer_buffer);
			sprintf(wstart,  "0x%x", ptn->start +
				(repeat * ptn->length));
			sprintf(wlength, "0x%x", download_bytes);
#ifdef CFG_NAND_YAFFS_WRITE
			if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS)
				sprintf(wlength, "0x%x",
					download_bytes_unpadded);
#endif

			ret = do_nand(NULL, 0, 5, write);

			if (0 == repeat) {
				if (ret) /* failed */
					save_block_values(ptn, 0, 0);
				else     /* success */
					save_block_values(ptn, ptn->start,
							  download_bytes);
			}
		}

		do_nand(NULL, 0, 4, lock);

		if (ret)
			break;
	}

	return ret;
}
#else
static int write_to_ptn(struct fastboot_ptentry *ptn)
{
	void *addr;
	u32   block_start, nblock;

	addr        = (void *)interface.transfer_buffer;
    block_start = ptn->start/512;
	nblock      = download_bytes/512;
#if 1
	printf("fastboot download_size = %x\n", download_bytes);
	printf("fastboot block start = %x\n", block_start);
	printf("fastboot      nblock = %x\n", nblock);
	printf("fastboot        addr = %x\n", (u32)addr);
#endif
	return sunxi_flash_write(block_start, nblock, addr);
}
#endif

#if 0
static int erase_ptn(struct fastboot_ptentry *ptn)
{
	void *addr  = (void *)FASTBOOT_ERASE_BUFFER;
	u32   start = ptn->start/512;
	u32   unerased_size;
	u32   nblock = FASTBOOT_ERASE_BUFFER_SIZE/512;

	unerased_size = ptn->length;
	memset(addr, 0xff, FASTBOOT_ERASE_BUFFER_SIZE);
	while(unerased_size >= FASTBOOT_ERASE_BUFFER_SIZE)
	{
		if(sunxi_flash_write(start, nblock, addr))
		{
			return -1;
		}
		start += nblock;
		unerased_size -= FASTBOOT_ERASE_BUFFER_SIZE;
	}
	if(unerased_size)
	{
		if(unerased_size & 0x1ff)
		{
			unerased_size += 0x1ff;
		}
		if(sunxi_flash_write(start, unerased_size/512, addr))
		{
			return -1;
		}
	}

	return 0;
}
#else
static int erase_ptn(struct fastboot_ptentry *ptn)
{
	return 0;
}
#endif

static int tx_handler(void)
{
	if (upload_size) {

		int bytes_written;
		bytes_written = fastboot_tx(interface.transfer_buffer +
					    upload_bytes, upload_size -
					    upload_bytes);
		if (bytes_written > 0) {

			upload_bytes += bytes_written;
			/* Check if this is the last */
			if (upload_bytes == upload_size) {

				/* Reset upload */
				upload_size = 0;
				upload_bytes = 0;
				upload_error = 0;
			}
		}
	}
	return upload_error;
}

static int rx_handler (const unsigned char *buffer, unsigned int buffer_size)
{
	int ret = 1;
#ifdef DEBUG
	//printf("fast boot rx handle storage_type is %d\n",storage_type);
#endif
	/* Use 65 instead of 64
	   null gets dropped
	   strcpy's need the extra byte */
	char response[65];

	if (download_size)
	{
		/* Something to download */
		if (buffer_size)
		{
			/* Handle possible overflow */
			unsigned int transfer_size =
				download_size - download_bytes;

			if (buffer_size < transfer_size)
				transfer_size = buffer_size;

			/* Save the data to the transfer buffer */
			memcpy (interface.transfer_buffer + download_bytes,
				buffer, transfer_size);

			download_bytes += transfer_size;

			/* Check if transfer is done */
			if (download_bytes >= download_size) {
				/* Reset global transfer variable,
				   Keep download_bytes because it will be
				   used in the next possible flashing command */
				download_size = 0;
				/* The download buffer and cmd buffer is the same one
				   since the partition name ends without a trailing 0 byte
				   we need to clear the buffer after download finished
				   for the next possible command. */
				memset(buffer, 0, transfer_size);

				if (download_error) {
					/* There was an earlier error */
					sprintf(response, "ERROR");
				} else {
					/* Everything has transferred,
					   send the OK response */
					sprintf(response, "OKAY");
				}
				fastboot_tx_status(response, strlen(response));

				printf ("\ndownloading of %d MB finished\n",
					download_bytes >> 20);

#if 0 /* We don't need to pad */
#if defined(CONFIG_STORAGE_NAND)
				/* Pad to block length
				   In most cases, padding the download to be
				   block aligned is correct. The exception is
				   when the following flash writes to the oob
				   area.  This happens when the image is a
				   YAFFS image.  Since we do not know what
				   the download is until it is flashed,
				   go ahead and pad it, but save the true
				   size in case if should have
				   been unpadded */
				download_bytes_unpadded = download_bytes;
				if (interface.nand_block_size)
				{
					if (download_bytes %
					    interface.nand_block_size)
					{
						unsigned int pad = interface.nand_block_size - (download_bytes % interface.nand_block_size);
						unsigned int i;

						for (i = 0; i < pad; i++)
						{
							if (download_bytes >= interface.transfer_buffer_size)
								break;

							interface.transfer_buffer[download_bytes] = 0;
							download_bytes++;
						}
					}
				}
#endif
#endif /* #if 0 */
			}

			/* Provide some feedback */
			if (download_bytes &&
			    0 == (download_bytes %
				  (16 * interface.nand_block_size)))
			{
				/* Some feeback that the
				   download is happening */
				if (download_error)
					printf("X\n");
				else
					printf("downloading %d MB ...\r", download_bytes >> 20);
			}
		}
		else
		{
			/* Ignore empty buffers */
			printf ("Warning empty download buffer\n");
			printf ("Ignoring\n");
		}
		ret = 0;
	}
	else
	{
		/* A command */

		/* Cast to make compiler happy with string functions */
		const char *cmdbuf = (char *) buffer;

		/* Generic failed response */
		sprintf(response, "FAIL");

		/* reboot
		   Reboot the board. */

		if(memcmp(cmdbuf, "reboot", 6) == 0)
		{
#ifdef MY_DEBUG
			printf("reboot\n");
#endif
			sprintf(response,"OKAY");
			fastboot_tx_status(response, strlen(response));
			udelay (1000000); /* 1 sec */

			do_reset (NULL, 0, 0, NULL);

			/* This code is unreachable,
			   leave it to make the compiler happy */
			return 0;
		}

		/* getvar
		   Get common fastboot variables
		   Board has a chance to handle other variables */
		if(memcmp(cmdbuf, "getvar:", 7) == 0)
		{
#ifdef MY_DEBUG
			printf("getvar\n");
#endif
			strcpy(response,"OKAY");

			if(!strcmp(cmdbuf + strlen("version"), "version"))
			{
				strcpy(response + 4, FASTBOOT_VERSION);
			}
			else if(!strcmp(cmdbuf + strlen("product"), "product"))
			{
				if (interface.product_name)
					strcpy(response + 4, interface.product_name);

			} else if(!strcmp(cmdbuf + strlen("serialno"), "serialno")) {
				if (interface.serial_no)
					strcpy(response + 4, interface.serial_no);

			} else if(!strcmp(cmdbuf + strlen("downloadsize"), "downloadsize")) {
				if (interface.transfer_buffer_size)
					sprintf(response + 4, "%08x", interface.transfer_buffer_size);
			}
			else
			{
				fastboot_getvar(cmdbuf + 7, response + 4);
			}
			ret = 0;

		}

		/* erase
		   Erase a register flash partition
		   Board has to set up flash partitions */

		if(memcmp(cmdbuf, "erase:", 6) == 0)
		{
			struct fastboot_ptentry *ptn;

			ptn = fastboot_flash_find_ptn(cmdbuf + 6);
			if(ptn == 0)
			{
				printf("FAIL: partition does not exist\n");
				sprintf(response, "FAILpartition does not exist");
			}
			else
			{
				int status;

				status = erase_ptn(ptn);
				if (status)
				{
					printf("FAIL: failed to erase partition\n");
					sprintf(response,"FAILfailed to erase partition");
				}
				else
				{
					printf("partition '%s' erased\n", ptn->name);
					sprintf(response, "OKAY");
				}
			}
			ret = 0;
		}
		/* download
		   download something ..
		   What happens to it depends on the next command after data */

		if(memcmp(cmdbuf, "download:", 9) == 0) {
#ifdef MY_DEBUG
			printf("download\n");
#endif

			/* save the size */
			download_size = simple_strtoul (cmdbuf + 9, NULL, 16);
			/* Reset the bytes count, now it is safe */
			download_bytes = 0;
			/* Reset error */
			download_error = 0;
#ifdef MY_DEBUG
			printf("download_size = %d\n", download_size);
#endif

			printf ("Starting download of %d MB\n", download_size >> 20);

			if (0 == download_size)
			{
				/* bad user input */
				sprintf(response, "FAILdata invalid size");
			}
			else if (download_size > interface.transfer_buffer_size)
			{
				/* set download_size to 0 because this is an error */
				download_size = 0;
				sprintf(response, "FAILdata too large, larger than buffer");
			}
			else
			{
				/* The default case, the transfer fits
				   completely in the interface buffer */
				sprintf(response, "DATA%08x", download_size);
			}
			ret = 0;
		}

		/* boot
		   boot what was downloaded

		   WARNING WARNING WARNING

		   This is not what you expect.
		   The fastboot client does its own packaging of the
		   kernel.  The layout is defined in the android header
		   file bootimage.h.  This layeout is copiedlooks like this,

		   **
		   ** +-----------------+
		   ** | boot header     | 1 page
		   ** +-----------------+
		   ** | kernel          | n pages
		   ** +-----------------+
		   ** | ramdisk         | m pages
		   ** +-----------------+
		   ** | second stage    | o pages
		   ** +-----------------+
		   **

		   We only care about the kernel.
		   So we have to jump past a page.

		   What is a page size ?
		   The fastboot client uses 2048

		   The is the default value of

		   CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE

		*/

		if(memcmp(cmdbuf, "boot", 4) == 0) {
#ifdef MY_DEBUG
			printf("boot\n");
#endif

			if ((download_bytes) &&
			    (CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE < download_bytes))
			{
				char start[32];
				char *bootm[3] = { "bootm", NULL, NULL, };
				char *go[3]    = { "go",    NULL, NULL, };

				/*
				 * Use this later to determine if a command line was passed
				 * for the kernel.
				 */
				struct fastboot_boot_img_hdr *fb_hdr =
					(struct fastboot_boot_img_hdr *) interface.transfer_buffer;

				/* Skip the mkbootimage header */
				image_header_t *hdr =
					(image_header_t *)
					&interface.transfer_buffer[CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE];

				bootm[1] = go[1] = start;
				sprintf (start, "0x%x", hdr);

				/* Execution should jump to kernel so send the response
				   now and wait a bit.  */
				sprintf(response, "OKAY");
				fastboot_tx_status(response, strlen(response));
				udelay (1000000); /* 1 sec */

				if (ntohl(hdr->ih_magic) == IH_MAGIC) {
					/* Looks like a kernel.. */
					printf ("Booting kernel..\n");

					/*
					 * Check if the user sent a bootargs down.
					 * If not, do not override what is already there
					 */
					if (strlen ((char *) &fb_hdr->cmdline[0])) {
						printf("Image has cmdline:");
						printf("%s\n", &fb_hdr->cmdline[0]);
						set_env ("bootargs", (char *) &fb_hdr->cmdline[0]);
					}
					do_bootm (NULL, 0, 2, bootm);
				} else {
					/* Raw image, maybe another uboot */
					printf ("Booting raw image..\n");

					do_go (NULL, 0, 2, go);
				}
				printf ("ERROR : bootting failed\n");
				printf ("You should reset the board\n");
			}
			sprintf(response, "FAILinvalid boot image");
			ret = 0;
		}

		/* flash
		   Flash what was downloaded */

		if(memcmp(cmdbuf, "flash:", 6) == 0)
		{
			if (download_bytes)
			{
				struct fastboot_ptentry *ptn;

				ptn = fastboot_flash_find_ptn(cmdbuf + 6);
				if (ptn == 0)
				{
					printf("Partition:'%s' does not exist\n", ptn->name);
					sprintf(response, "FAILpartition does not exist");
				}
				else
				if ((download_bytes > ptn->length) &&
					!(ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_ENV))
				{
					printf("Image too large for the partition\n");
					sprintf(response, "FAILimage too large for partition");
					/* TODO : Improve check for yaffs write */
				}
				else
				{
					/* Check if this is not really a flash write
					but rather a saveenv */
					if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_ENV)
					{
						/* Since the response can only be 64 bytes,
						there is no point in having a large error message. */
						char err_string[32];
						if (saveenv_to_ptn(ptn, &err_string[0])) {
							printf("savenv '%s' failed : %s\n", ptn->name, err_string);
							sprintf(response, "FAIL%s", err_string);
						} else {
							printf("partition '%s' saveenv-ed\n", ptn->name);
							sprintf(response, "OKAY");
						}
					}
					else
					{
						/* Check if we have sparse compressed image */
						if ( ((sparse_header_t *)interface.transfer_buffer)->magic
								== SPARSE_HEADER_MAGIC) {
							printf("fastboot: %s is in sparse format\n", ptn->name);
							if (!do_unsparse(interface.transfer_buffer,
									ptn->start,
									ptn->length,
									NULL)) {
								printf("Writing sparsed: '%s' DONE!\n", ptn->name);
								sprintf(response, "OKAY");
							} else {
								printf("Writing sparsed '%s' FAILED!\n", ptn->name);
								sprintf(response, "FAIL: Sparsed Write");
							}
						}
						else
						{
							/* Normal image: no sparse */
							if (write_to_ptn(ptn))
							{
								printf("flashing '%s' failed\n", ptn->name);
								sprintf(response, "FAILfailed to flash partition");
							}
							else
							{
								printf("partition '%s' flashed\n", ptn->name);
								sprintf(response, "OKAY");
							}
						}
					}
				}
			}
			else
			{
				sprintf(response, "FAILno image downloaded");
			}
			ret = 0;
		}

		/* continue
		   Stop doing fastboot */
		if (memcmp(cmdbuf, "continue", 8) == 0) {
			sprintf(response, "OKAY");
#ifdef MY_DEBUG
			printf("continue\n");
#endif
			continue_booting = 1;
			ret = 0;
		}

		/* upload
		   Upload just the data in a partition */
		if ((memcmp(cmdbuf, "upload:", 7) == 0) ||
		    (memcmp(cmdbuf, "uploadraw:", 10) == 0)) {
/*#if defined(CONFIG_STORAGE_NAND)*/
			if(!storage_type){
			unsigned int adv, delim_index, len;
			struct fastboot_ptentry *ptn;
			unsigned int is_raw = 0;

			/* Is this a raw read ? */
			if (memcmp(cmdbuf, "uploadraw:", 10) == 0) {
				is_raw = 1;
				adv = 10;
			} else {
				adv = 7;
			}

			/* Scan to the next ':' to find when the size starts */
			len = strlen(cmdbuf);
			for (delim_index = adv;
			     delim_index < len; delim_index++) {
				if (cmdbuf[delim_index] == ':') {
					/* WARNING, cmdbuf is being modified. */
					*((char *) &cmdbuf[delim_index]) = 0;
					break;
				}
			}

			ptn = fastboot_flash_find_ptn(cmdbuf + adv);
			if (ptn == 0) {
				sprintf(response,
					"FAILpartition does not exist");
			} else {
				/* This is how much the user is expecting */
				unsigned int user_size;
				/*
				 * This is the maximum size needed for
				 * this partition
				 */
				unsigned int size;
				/* This is the length of the data */
				unsigned int length;
				/*
				 * Used to check previous write of
				 * the parition
				 */
				char env_ptn_length_var[128];
				char *env_ptn_length_val;

				user_size = 0;
				if (delim_index < len)
					user_size =
					  simple_strtoul(cmdbuf + delim_index +
							 1, NULL, 16);

				/* Make sure output is padded to block size */
				length = ptn->length;
				sprintf(env_ptn_length_var,
					"%s_nand_size", ptn->name);
				env_ptn_length_val = getenv(env_ptn_length_var);
				if (env_ptn_length_val) {
					length =
					  simple_strtoul(env_ptn_length_val,
							 NULL, 16);
					/* Catch possible problems */
					if (!length)
						length = ptn->length;
				}

				size = length / interface.nand_block_size;
				size *= interface.nand_block_size;
				if (length % interface.nand_block_size)
					size += interface.nand_block_size;

				if (is_raw)
					size += (size /
						 interface.nand_block_size) *
						interface.nand_oob_size;

				if (size > interface.transfer_buffer_size) {

					sprintf(response, "FAILdata too large");

				} else if (user_size == 0) {

					/* Send the data response */
					sprintf(response, "DATA%08x", size);

				} else if (user_size != size) {
					/* This is the wrong size */
					sprintf(response, "FAIL");
				} else {
					/*
					 * This is where the transfer
					 * buffer is populated
					 */
					unsigned char *buf =
					  interface.transfer_buffer;
					char start[32], length[32], type[32],
					  addr[32];
					char *read[6] = { "nand", NULL, NULL,
							  NULL, NULL, NULL, };

					/*
					 * Setting upload_size causes
					 * transfer to happen in main loop
					 */
					upload_size = size;
					upload_bytes = 0;
					upload_error = 0;

					/*
					 * Poison the transfer buffer, 0xff
					 * is erase value of nand
					 */
					memset(buf, 0xff, upload_size);

					/* Which flavor of read to use */
					if (is_raw)
						sprintf(type, "read.raw");
					else
						sprintf(type, "read.i");

					sprintf(addr, "0x%x",
						interface.transfer_buffer);
					sprintf(start, "0x%x", ptn->start);
					sprintf(length, "0x%x", upload_size);

					read[1] = type;
					read[2] = addr;
					read[3] = start;
					read[4] = length;

					set_ptn_ecc(ptn);

					do_nand(NULL, 0, 5, read);

					/* Send the data response */
					sprintf(response, "DATA%08x", size);
				}
			}
/*#endif*/}
			ret = 0;
		}

		fastboot_tx_status(response, strlen(response));

	} /* End of command */

	return ret;
}

static int check_against_static_partition(struct fastboot_ptentry *ptn)
{
	int ret = 0;
	struct fastboot_ptentry *c;
	int i;

	for (i = 0; i < static_pcount; i++) {
		c = fastboot_flash_get_ptn((unsigned int) i);

		if (0 == ptn->length)
			break;

		if ((ptn->start >= c->start) &&
		    (ptn->start < c->start + c->length))
			break;

		if ((ptn->start + ptn->length > c->start) &&
		    (ptn->start + ptn->length <= c->start + c->length))
			break;

		if ((0 == strcmp(ptn->name, c->name)) &&
		    (0 == strcmp(c->name, ptn->name)))
			break;
	}

	if (i >= static_pcount)
		ret = 1;
	return ret;
}

static unsigned long long memparse(char *ptr, char **retptr)
{
	char *endptr;	/* local pointer to end of parsed string */

	unsigned long ret = simple_strtoul(ptr, &endptr, 0);

	switch (*endptr) {
	case 'M':
	case 'm':
		ret <<= 10;
	case 'K':
	case 'k':
		ret <<= 10;
		endptr++;
	default:
		break;
	}

	if (retptr)
		*retptr = endptr;

	return ret;
}

static int add_partition_from_environment(char *s, char **retptr)
{
	unsigned long size;
	unsigned long offset = 0;
	char *name;
	int name_len;
	int delim;
	unsigned int flags;
	struct fastboot_ptentry part;

	size = memparse(s, &s);
	if (0 == size) {
		printf("Error:FASTBOOT size of parition is 0\n");
		return 1;
	}

	/* fetch partition name and flags */
	flags = 0; /* this is going to be a regular partition */
	delim = 0;
	/* check for offset */
	if (*s == '@') {
		s++;
		offset = memparse(s, &s);
	} else {
		printf("Error:FASTBOOT offset of parition is not given\n");
		return 1;
	}

	/* now look for name */
	if (*s == '(')
		delim = ')';

	if (delim) {
		char *p;

		name = ++s;
		p = strchr((const char *)name, delim);
		if (!p) {
			printf("Error:FASTBOOT no closing %c found in partition name\n", delim);
			return 1;
		}
		name_len = p - name;
		s = p + 1;
	} else {
		printf("Error:FASTBOOT no partition name for \'%s\'\n", s);
		return 1;
	}

	/* test for options */
	while (1) {
		if (strncmp(s, "i", 1) == 0) {
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_I;
			s += 1;
		} else if (strncmp(s, "yaffs", 5) == 0) {
			/* yaffs */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS;
			s += 5;
		} else if (strncmp(s, "swecc", 5) == 0) {
			/* swecc */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC;
			s += 5;
		} else if (strncmp(s, "hwecc", 5) == 0) {
			/* hwecc */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC;
			s += 5;
		} else {
			break;
		}
		if (strncmp(s, "|", 1) == 0)
			s += 1;
	}

	/* enter this partition (offset will be calculated later if it is zero at this point) */
	part.length = size;
	part.start = offset;
	part.flags = flags;

	if (name) {
		if (name_len >= sizeof(part.name)) {
			printf("Error:FASTBOOT partition name is too long\n");
			return 1;
		}
		strncpy(&part.name[0], name, name_len);
		/* name is not null terminated */
		part.name[name_len] = '\0';
	} else {
		printf("Error:FASTBOOT no name\n");
		return 1;
	}


	/* Check if this overlaps a static partition */
	if (check_against_static_partition(&part)) {
		printf("Adding: %s, offset 0x%8.8x, size 0x%8.8x, flags 0x%8.8x\n",
		       part.name, part.start, part.length, part.flags);
		fastboot_flash_add_ptn(&part);
	}

	/* return (updated) pointer command line string */
	*retptr = s;

	/* return partition table */
	return 0;
}



int do_fastboot (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 1;
	char fbparts[4096], *env;
	int check_timeout = 0;
	uint64_t timeout_endtime = 0;
	uint64_t timeout_ticks = 0;
	long timeout_seconds = -1;
	int continue_from_disconnect = 0;

	/*
	 * Place the runtime partitions at the end of the
	 * static paritions.  First save the start off so
	 * it can be saved from run to run.
	 */
	fastboot_flash_dump_ptn();

	if (static_pcount >= 0) {
		/* Reset */
		pcount = static_pcount;
	} else {
		/* Save */
		static_pcount = pcount;
	}
	env = getenv("fbparts");
	if (env) {
		unsigned int len;
		len = strlen(env);
		if (len && len < 4096) {
			char *s, *e;

			memcpy(&fbparts[0], env, len + 1);
			printf("Fastboot: Adding partitions from environment\n");
			s = &fbparts[0];
			e = s + len;
			while (s < e) {
				if (add_partition_from_environment(s, &s)) {
					printf("Error:Fastboot: Abort adding partitions\n");
					/* reset back to static */
					pcount = static_pcount;
					break;
				}
				/* Skip a bunch of delimiters */
				while (s < e) {
					if ((' ' == *s) ||
					    ('\t' == *s) ||
					    ('\n' == *s) ||
					    ('\r' == *s) ||
					    (',' == *s)) {
						s++;
					} else {
						break;
					}
				}
			}
		}
	}

	/* Time out */
	if (2 == argc) {
		long try_seconds;
		char *try_seconds_end;
		/* Check for timeout */
		try_seconds = simple_strtol(argv[1],
					    &try_seconds_end, 10);
		if ((try_seconds_end != argv[1]) &&
		    (try_seconds >= 0)) {
			check_timeout = 1;
			timeout_seconds = try_seconds;
			printf("Fastboot inactivity timeout %ld seconds\n", timeout_seconds);
		}
	}

	if (1 == check_timeout) {
		timeout_ticks = (uint64_t)
			(timeout_seconds * get_tbclk());
	}


	do {
		continue_from_disconnect = 0;

		/* Initialize the board specific support */
		if (0 == fastboot_init(&interface)) {

			int poll_status;

			/* If we got this far, we are a success */
			ret = 0;
			printf("Fastboot entered\n");

			timeout_endtime = get_ticks();
			timeout_endtime += timeout_ticks;

			while (1) {
				uint64_t current_time = 0;
				poll_status = fastboot_poll();

				if (1 == check_timeout)
					current_time = get_ticks();

				if (FASTBOOT_ERROR == poll_status) {
					/* Error */
					break;
				} else if (FASTBOOT_DISCONNECT == poll_status) {
					/* beak, cleanup and re-init */
					printf("Fastboot disconnect detected\n");
					continue_from_disconnect = 1;
					break;
				} else if ((1 == check_timeout) &&
					   (FASTBOOT_INACTIVE == poll_status)) {

					/* No activity */
					if (current_time >= timeout_endtime) {
						printf("Fastboot inactivity detected\n");
						break;
					}
				} else {
					/* Something happened */
					if (1 == check_timeout) {
						/* Update the timeout endtime */
						timeout_endtime = current_time;
						timeout_endtime += timeout_ticks;
					}
				}

				/* Check if the user wanted to terminate with ^C */
				if ((FASTBOOT_INACTIVE == poll_status) &&
				    (ctrlc())) {
					printf("Fastboot ended by user\n");
					break;
				}

				/*
				 * Check if the fastboot client wanted to
				 * continue booting
				 */
				if (continue_booting) {
					printf("Fastboot ended by client\n");
					break;
				}

				/* Check if there is something to upload */
				tx_handler();
			}
		}

		/* Reset the board specific support */
		fastboot_shutdown();

		/* restart the loop if a disconnect was detected */
	} while (continue_from_disconnect);

	return ret;
}

U_BOOT_CMD(
	fastboot,	2,	1,	do_fastboot,
	"fastboot- use USB Fastboot protocol\n",
	"[inactive timeout]\n"
	"    - Run as a fastboot usb device.\n"
	"    - The optional inactive timeout is the decimal seconds before\n"
	"    - the normal console resumes\n"
);


/*
 * Android style flash utilties */
void fastboot_flash_add_ptn(fastboot_ptentry *ptn)
{
    if(pcount < MAX_PTN){
        memcpy(ptable + pcount, ptn, sizeof(*ptn));
        pcount++;
    }
}

void fastboot_flash_dump_ptn(void)
{
    unsigned int n;
    for(n = 0; n < pcount; n++) {
        fastboot_ptentry *ptn = ptable + n;
        printf("ptn %d name='%s' start=%d len=%d\n",
                n, ptn->name, ptn->start, ptn->length);
    }
}


fastboot_ptentry *fastboot_flash_find_ptn(const char *name)
{
    unsigned int n;

	printf("find ptn: %s\n", name);

    for(n = 0; n < pcount; n++) {
	    /* Make sure a substring is not accepted */
	    if (strlen(name) == strlen(ptable[n].name))
	    {
		    if(0 == strcmp(ptable[n].name, name))
			    return ptable + n;
	    }
    }
    return 0;
}

fastboot_ptentry *fastboot_flash_get_ptn(unsigned int n)
{
    if(n < pcount) {
        return ptable + n;
    } else {
        return 0;
    }
}

unsigned int fastboot_flash_get_ptn_count(void)
{
    return pcount;
}



#endif	/* CONFIG_FASTBOOT */
