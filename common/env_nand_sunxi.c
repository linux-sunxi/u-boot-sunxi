/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
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
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <mmc.h>
#include <search.h>
#include <errno.h>
#include <nand.h>
#ifdef CONFIG_ALLWINNER
#include <asm/arch/boot_type.h>
#endif
extern uchar default_environment[];

char * env_name_spec = "SUNXI";

#ifdef ENV_IS_EMBEDDED
extern uchar environment[];
env_t *env_ptr = (env_t *)(&environment[0]);
#else /* ! ENV_IS_EMBEDDED */
env_t *env_ptr = NULL;
#endif /* ENV_IS_EMBEDDED */

/* local functions */
#if !defined(ENV_IS_EMBEDDED)
static void use_default(void);
#endif

DECLARE_GLOBAL_DATA_PTR;


#if !defined(CONFIG_ENV_OFFSET)
#define CONFIG_ENV_OFFSET 0
#endif

loff_t env_offset = (loff_t)CONFIG_ENV_ADDR;
size_t env_size = (size_t)CONFIG_ENV_SIZE;

extern int sunxi_nand_read_opts(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags);

extern int sunxi_nand_write_opts(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags);

static int __mmc_get_env_addr(struct mmc *mmc, u32 *env_addr)
{
	*env_addr = CONFIG_ENV_OFFSET;
	return 0;
}
__attribute__((weak, alias("__mmc_get_env_addr")))
int mmc_get_env_addr(struct mmc *mmc, u32 *env_addr);


uchar env_get_char_spec(int index)
{
	return ( *((uchar *)(gd->env_addr + index)) );
}

int env_init(void)
{
#ifdef DEBUG
	printf("env init\n");
#endif
	/* use default */
	gd->env_addr = (ulong)&default_environment[0];
	gd->env_valid = 1;

	return 0;
}

#ifdef CONFIG_ENV_OFFSET_REDUND
#error No support for redundant environment on sunxi nand yet!
#endif
int init_mmc_for_env(struct mmc *mmc)
{
	if (!mmc) {
		puts("No MMC card found\n");
		return -1;
	}

	if (mmc_init(mmc)) {
		puts("MMC init failed\n");
		return  -1;
	}

	return 0;
}



inline int write_env(struct mmc *mmc, unsigned long size,
			unsigned long offset, const void *buffer)
{
	uint blk_start, blk_cnt, n;

	blk_start = ALIGN(offset, mmc->write_bl_len) / mmc->write_bl_len;
	blk_cnt   = ALIGN(size, mmc->write_bl_len) / mmc->write_bl_len;

	n = mmc->block_dev.block_write(CONFIG_SYS_MMC_ENV_DEV, blk_start,
					blk_cnt, (u_char *)buffer);

	return (n == blk_cnt) ? 0 : -1;
}

int mmc_saveenv(void)
{
	env_t	env_new;
	ssize_t	len;
	char	*res;
	struct mmc *mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
	u32 offset;

	if (init_mmc_for_env(mmc))
		return 1;

	if(mmc_get_env_addr(mmc, &offset))
		return 1;

	res = (char *)&env_new.data;
	len = hexport_r(&env_htab, '\0', &res, ENV_SIZE);
	if (len < 0) {
		error("Cannot export environment: errno = %d\n", errno);
		return 1;
	}
	env_new.crc   = crc32(0, env_new.data, ENV_SIZE);
	printf("Writing to MMC(%d)... ", CONFIG_SYS_MMC_ENV_DEV);
	if (write_env(mmc, CONFIG_ENV_SIZE, offset, (u_char *)&env_new)) {
		puts("failed\n");
		return 1;
	}

	puts("done\n");
	return 0;
}

int nand_saveenv(void)
{
	env_t	env_new;
	ssize_t	len;
	char	*res;

	res = (char *)&env_new.data;
	len = hexport_r(&env_htab, '\0', &res, ENV_SIZE);
	if (len < 0) {
		error("Cannot export environment: errno = %d\n", errno);
		return 1;
	}
	env_new.crc   = crc32(0, env_new.data, ENV_SIZE);

	return sunxi_nand_write_opts(&nand_info[0], (loff_t)env_offset, &env_size,
			(u_char *)&env_new, 0);
}

int saveenv(void)
{
	int ret;
#ifdef DEBUG
if(script_parser_fetch("target", "storage_type", &ret, sizeof(int)))
	mmc_card = 0;
#endif
	if(mmc_card)
		ret = mmc_saveenv();
	else
		ret = nand_saveenv();
	return ret;
}

void nand_env_relocate_spec(void)
{
	char buf[CONFIG_ENV_SIZE];
	unsigned int tmp;

	tmp = sunxi_nand_getpart_offset_byname(CONFIG_SUNXI_ENV_PARTITION);
	if( tmp != -1) {
		env_offset = (loff_t) (tmp * 512);
	}

	tmp = sunxi_nand_getpart_size_byname(CONFIG_SUNXI_ENV_PARTITION);
	if( tmp != -1) {
		env_size = (size_t) (tmp * 512);
		if(env_size > CONFIG_ENV_SIZE)
			env_size = CONFIG_ENV_SIZE;
	}

	sunxi_nand_read_opts(&nand_info[0], env_offset, &env_size, buf, 0);
	env_import(buf, 1);
}

inline int read_env(struct mmc *mmc, unsigned long size,
			unsigned long offset, const void *buffer)
{
	uint blk_start, blk_cnt, n;
#ifdef DEBUG
	printf("\n\n\n\n\noffset is %d\n\n\n\n\n",offset);
#endif/*DEBUG*/
	offset+= 128 * 1024;
	blk_start = ALIGN(offset, mmc->read_bl_len) / mmc->read_bl_len;
	blk_cnt   = ALIGN(size, mmc->read_bl_len) / mmc->read_bl_len;
#ifdef DEBUG
	printf("new offset: %d\n",offset);
	printf("blk_start: %d blk_cnt %d\n",blk_start,blk_cnt);
	printf("mmc->read_bl_len: %d\n",mmc->read_bl_len);
#endif/*DEBUG*/
	n = mmc->block_dev.block_read(CONFIG_SYS_MMC_ENV_DEV, blk_start,
					blk_cnt, (uchar *)buffer);

	return (n == blk_cnt) ? 0 : -1;
}

void mmc_env_relocate_spec(void)
{
#if !defined(ENV_IS_EMBEDDED)
	char buf[CONFIG_ENV_SIZE];

	struct mmc *mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
	u32 offset;

	if (init_mmc_for_env(mmc)) {
		use_default();
		return;
	}

	if(mmc_get_env_addr(mmc, &offset)) {
		use_default();
		return ;
	}

	if (read_env(mmc, CONFIG_ENV_SIZE, offset, buf)) {
		use_default();
		return;
	}

	env_import(buf, 1);
#endif
}

void env_relocate_spec(void)
{
#ifdef DEBUG
	if(script_parser_fetch("target", "storage_type", &mmc_card, sizeof(int)))
		mmc_card=0;
#endif
	if(mmc_card)
		mmc_env_relocate_spec();
	else
		nand_env_relocate_spec();
}

#if !defined(ENV_IS_EMBEDDED)
static void use_default()
{
	set_default_env(NULL);
}
#endif

/*
 *  This is called before nand_init() so we can't read NAND to
 *  validate env data.
 */

