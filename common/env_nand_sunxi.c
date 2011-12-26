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
#include <search.h>
#include <errno.h>
#include <nand.h>

DECLARE_GLOBAL_DATA_PTR;

env_t *env_ptr = NULL;

char * env_name_spec = "SUNXI NAND";

loff_t env_offset = (loff_t)CONFIG_ENV_ADDR;
size_t env_size = (size_t)CONFIG_ENV_SIZE;

extern uchar default_environment[];

extern int sunxi_nand_read_opts(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags);

extern int sunxi_nand_write_opts(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int flags);

uchar env_get_char_spec(int index)
{
	return ( *((uchar *)(gd->env_addr + index)) );
}

void env_relocate_spec(void)
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

#ifdef CONFIG_ENV_OFFSET_REDUND
#error No support for redundant environment on sunxi nand yet!
#endif

int saveenv(void)
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

/*
 *  This is called before nand_init() so we can't read NAND to
 *  validate env data.
 */
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
