/*
 * (C) Copyright 2012
 * Henrik Nordstrom <henrik@henriknordstrom.net>
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
 * Boot support
 */
#include <common.h>
#include <command.h>

DECLARE_GLOBAL_DATA_PTR;

int do_setarch(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	if (argc != 2)
		return cmd_usage(cmdtp);

	bd->bi_arch_number = simple_strtoul(argv[1], NULL, 0);

	return 0;
}


/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	setarch,	2,	0,	do_setarch,
	"Set board arch number",
	""
);
