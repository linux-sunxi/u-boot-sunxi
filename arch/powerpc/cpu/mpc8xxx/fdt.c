/*
 * Copyright 2009-2012 Freescale Semiconductor, Inc.
 *
 * This file is derived from arch/powerpc/cpu/mpc85xx/cpu.c and
 * arch/powerpc/cpu/mpc86xx/cpu.c. Basically this file contains
 * cpu specific common code for 85xx/86xx processors.
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

#include <common.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/mp.h>
#include <asm/fsl_serdes.h>
#include <phy.h>
#include <hwconfig.h>

#define FSL_MAX_NUM_USB_CTRLS	2

#if defined(CONFIG_MP) && (defined(CONFIG_MPC85xx) || defined(CONFIG_MPC86xx))
static int ft_del_cpuhandle(void *blob, int cpuhandle)
{
	int off, ret = -FDT_ERR_NOTFOUND;

	/* if we find a match, we'll delete at it which point the offsets are
	 * invalid so we start over from the beginning
	 */
	off = fdt_node_offset_by_prop_value(blob, -1, "cpu-handle",
						&cpuhandle, 4);
	while (off != -FDT_ERR_NOTFOUND) {
		fdt_delprop(blob, off, "cpu-handle");
		ret = 1;
		off = fdt_node_offset_by_prop_value(blob, -1, "cpu-handle",
				&cpuhandle, 4);
	}

	return ret;
}

void ft_fixup_num_cores(void *blob) {
	int off, num_cores, del_cores;

	del_cores = 0;
	num_cores = cpu_numcores();

	off = fdt_node_offset_by_prop_value(blob, -1, "device_type", "cpu", 4);
	while (off != -FDT_ERR_NOTFOUND) {
		u32 *reg = (u32 *)fdt_getprop(blob, off, "reg", 0);
		u32 phys_cpu_id = thread_to_core(*reg);

		if (!is_core_valid(phys_cpu_id) || is_core_disabled(phys_cpu_id)) {
			int ph = fdt_get_phandle(blob, off);

			/* Delete the cpu node once there are no cpu handles */
			if (-FDT_ERR_NOTFOUND == ft_del_cpuhandle(blob, ph)) {
				fdt_del_node(blob, off);
				del_cores++;
			}
			/* either we deleted some cpu handles or the cpu node
			 * so we reset the offset back to the start since we
			 * can't trust the offsets anymore
			 */
			off = -1;
		}
		off = fdt_node_offset_by_prop_value(blob, off,
				"device_type", "cpu", 4);
	}
	debug ("%x core system found\n", num_cores);
	debug ("deleted %d extra core entry entries from device tree\n",
								del_cores);
}
#endif /* defined(CONFIG_MPC85xx) || defined(CONFIG_MPC86xx) */

#if defined(CONFIG_HAS_FSL_DR_USB) || defined(CONFIG_HAS_FSL_MPH_USB)
static int fdt_fixup_usb_mode_phy_type(void *blob, const char *mode,
				const char *phy_type, int start_offset)
{
	const char *compat_dr = "fsl-usb2-dr";
	const char *compat_mph = "fsl-usb2-mph";
	const char *prop_mode = "dr_mode";
	const char *prop_type = "phy_type";
	const char *node_type = NULL;
	int node_offset;
	int err;

	node_offset = fdt_node_offset_by_compatible(blob,
			start_offset, compat_mph);
	if (node_offset < 0) {
		node_offset = fdt_node_offset_by_compatible(blob,
			start_offset, compat_dr);
		if (node_offset < 0) {
			printf("WARNING: could not find compatible"
				" node %s or %s: %s.\n", compat_mph,
				compat_dr, fdt_strerror(node_offset));
			return -1;
		} else
			node_type = compat_dr;
	} else
		node_type = compat_mph;

	if (mode) {
		err = fdt_setprop(blob, node_offset, prop_mode, mode,
				  strlen(mode) + 1);
		if (err < 0)
			printf("WARNING: could not set %s for %s: %s.\n",
			       prop_mode, node_type, fdt_strerror(err));
	}

	if (phy_type) {
		err = fdt_setprop(blob, node_offset, prop_type, phy_type,
				  strlen(phy_type) + 1);
		if (err < 0)
			printf("WARNING: could not set %s for %s: %s.\n",
			       prop_type, node_type, fdt_strerror(err));
	}

	return node_offset;
}

void fdt_fixup_dr_usb(void *blob, bd_t *bd)
{
	const char *modes[] = { "host", "peripheral", "otg" };
	const char *phys[] = { "ulpi", "utmi" };
	const char *mode = NULL;
	const char *phy_type = NULL;
	const char *dr_mode_type = NULL;
	const char *dr_phy_type = NULL;
	char usb1_defined = 0;
	int usb_mode_off = -1;
	int usb_phy_off = -1;
	char str[5];
	int i, j;

	for (i = 1; i <= FSL_MAX_NUM_USB_CTRLS; i++) {
		int mode_idx = -1, phy_idx = -1;
		snprintf(str, 5, "%s%d", "usb", i);
		if (hwconfig(str)) {
			for (j = 0; j < ARRAY_SIZE(modes); j++) {
				if (hwconfig_subarg_cmp(str, "dr_mode",
						modes[j])) {
					mode_idx = j;
					break;
				}
			}

			for (j = 0; j < ARRAY_SIZE(phys); j++) {
				if (hwconfig_subarg_cmp(str, "phy_type",
						phys[j])) {
					phy_idx = j;
					break;
				}
			}

			if (mode_idx < 0 || phy_idx < 0) {
				puts("ERROR: wrong usb mode/phy defined!!\n");
				return;
			}

			dr_mode_type = modes[mode_idx];
			dr_phy_type = phys[phy_idx];

			/* use usb_dr_mode and usb_phy_type if
			   usb1_defined = 0; these variables are to
			   be deprecated */
			if (!strcmp(str, "usb1"))
				usb1_defined = 1;

			if (mode_idx < 0 && phy_idx < 0) {
				printf("WARNING: invalid phy or mode\n");
				return;
			}
		}

		usb_mode_off = fdt_fixup_usb_mode_phy_type(blob,
			dr_mode_type, NULL, usb_mode_off);

		if (usb_mode_off < 0)
			return;

		usb_phy_off = fdt_fixup_usb_mode_phy_type(blob,
			NULL, dr_phy_type, usb_phy_off);

		if (usb_phy_off < 0)
			return;
	}

	if (!usb1_defined) {
		int usb_off = -1;
		mode = getenv("usb_dr_mode");
		phy_type = getenv("usb_phy_type");
		if (mode || phy_type) {
			printf("WARNING: usb_dr_mode and usb_phy_type "
				"are to be deprecated soon. Use "
				"hwconfig to set these values instead!!\n");
			fdt_fixup_usb_mode_phy_type(blob, mode,
				phy_type, usb_off);
		}
	}
}
#endif /* defined(CONFIG_HAS_FSL_DR_USB) || defined(CONFIG_HAS_FSL_MPH_USB) */

/*
 * update crypto node properties to a specified revision of the SEC
 * called with sec_rev == 0 if not on an E processor
 */
#if CONFIG_SYS_FSL_SEC_COMPAT == 2 /* SEC 2.x/3.x */
void fdt_fixup_crypto_node(void *blob, int sec_rev)
{
	static const struct sec_rev_prop {
		u32 sec_rev;
		u32 num_channels;
		u32 channel_fifo_len;
		u32 exec_units_mask;
		u32 descriptor_types_mask;
	} sec_rev_prop_list [] = {
		{ 0x0200, 4, 24, 0x07e, 0x01010ebf }, /* SEC 2.0 */
		{ 0x0201, 4, 24, 0x0fe, 0x012b0ebf }, /* SEC 2.1 */
		{ 0x0202, 1, 24, 0x04c, 0x0122003f }, /* SEC 2.2 */
		{ 0x0204, 4, 24, 0x07e, 0x012b0ebf }, /* SEC 2.4 */
		{ 0x0300, 4, 24, 0x9fe, 0x03ab0ebf }, /* SEC 3.0 */
		{ 0x0301, 4, 24, 0xbfe, 0x03ab0ebf }, /* SEC 3.1 */
		{ 0x0303, 4, 24, 0x97c, 0x03a30abf }, /* SEC 3.3 */
	};
	static char compat_strlist[ARRAY_SIZE(sec_rev_prop_list) *
				   sizeof("fsl,secX.Y")];
	int crypto_node, sec_idx, err;
	char *p;
	u32 val;

	/* locate crypto node based on lowest common compatible */
	crypto_node = fdt_node_offset_by_compatible(blob, -1, "fsl,sec2.0");
	if (crypto_node == -FDT_ERR_NOTFOUND)
		return;

	/* delete it if not on an E-processor */
	if (crypto_node > 0 && !sec_rev) {
		fdt_del_node(blob, crypto_node);
		return;
	}

	/* else we got called for possible uprev */
	for (sec_idx = 0; sec_idx < ARRAY_SIZE(sec_rev_prop_list); sec_idx++)
		if (sec_rev_prop_list[sec_idx].sec_rev == sec_rev)
			break;

	if (sec_idx == ARRAY_SIZE(sec_rev_prop_list)) {
		puts("warning: unknown SEC revision number\n");
		return;
	}

	val = cpu_to_fdt32(sec_rev_prop_list[sec_idx].num_channels);
	err = fdt_setprop(blob, crypto_node, "fsl,num-channels", &val, 4);
	if (err < 0)
		printf("WARNING: could not set crypto property: %s\n",
		       fdt_strerror(err));

	val = cpu_to_fdt32(sec_rev_prop_list[sec_idx].descriptor_types_mask);
	err = fdt_setprop(blob, crypto_node, "fsl,descriptor-types-mask", &val, 4);
	if (err < 0)
		printf("WARNING: could not set crypto property: %s\n",
		       fdt_strerror(err));

	val = cpu_to_fdt32(sec_rev_prop_list[sec_idx].exec_units_mask);
	err = fdt_setprop(blob, crypto_node, "fsl,exec-units-mask", &val, 4);
	if (err < 0)
		printf("WARNING: could not set crypto property: %s\n",
		       fdt_strerror(err));

	val = cpu_to_fdt32(sec_rev_prop_list[sec_idx].channel_fifo_len);
	err = fdt_setprop(blob, crypto_node, "fsl,channel-fifo-len", &val, 4);
	if (err < 0)
		printf("WARNING: could not set crypto property: %s\n",
		       fdt_strerror(err));

	val = 0;
	while (sec_idx >= 0) {
		p = compat_strlist + val;
		val += sprintf(p, "fsl,sec%d.%d",
			(sec_rev_prop_list[sec_idx].sec_rev & 0xff00) >> 8,
			sec_rev_prop_list[sec_idx].sec_rev & 0x00ff) + 1;
		sec_idx--;
	}
	err = fdt_setprop(blob, crypto_node, "compatible", &compat_strlist, val);
	if (err < 0)
		printf("WARNING: could not set crypto property: %s\n",
		       fdt_strerror(err));
}
#elif CONFIG_SYS_FSL_SEC_COMPAT >= 4  /* SEC4 */
static u8 caam_get_era(void)
{
	static const struct {
		u16 ip_id;
		u8 maj_rev;
		u8 era;
	} caam_eras[] = {
		{0x0A10, 1, 1},
		{0x0A10, 2, 2},
		{0x0A12, 1, 3},
		{0x0A14, 1, 3},
		{0x0A14, 2, 4},
		{0x0A16, 1, 4},
		{0x0A10, 3, 4},
		{0x0A11, 1, 4},
		{0x0A18, 1, 4},
		{0x0A11, 2, 5},
		{0x0A12, 2, 5},
		{0x0A13, 1, 5},
		{0x0A1C, 1, 5}
	};

	ccsr_sec_t __iomem *sec = (void __iomem *)CONFIG_SYS_FSL_SEC_ADDR;
	u32 secvid_ms = in_be32(&sec->secvid_ms);
	u32 ccbvid = in_be32(&sec->ccbvid);
	u16 ip_id = (secvid_ms & SEC_SECVID_MS_IPID_MASK) >>
				SEC_SECVID_MS_IPID_SHIFT;
	u8 maj_rev = (secvid_ms & SEC_SECVID_MS_MAJ_REV_MASK) >>
				SEC_SECVID_MS_MAJ_REV_SHIFT;
	u8 era = (ccbvid & SEC_CCBVID_ERA_MASK) >> SEC_CCBVID_ERA_SHIFT;

	int i;

	if (era)	/* This is '0' prior to CAAM ERA-6 */
		return era;

	for (i = 0; i < ARRAY_SIZE(caam_eras); i++)
		if (caam_eras[i].ip_id == ip_id &&
		    caam_eras[i].maj_rev == maj_rev)
			return caam_eras[i].era;

	return 0;
}

static void fdt_fixup_crypto_era(void *blob, u32 era)
{
	int err;
	int crypto_node;

	crypto_node = fdt_path_offset(blob, "crypto");
	if (crypto_node < 0) {
		printf("WARNING: Missing crypto node\n");
		return;
	}

	err = fdt_setprop(blob, crypto_node, "fsl,sec-era", &era,
			  sizeof(era));
	if (err < 0) {
		printf("ERROR: could not set fsl,sec-era property: %s\n",
		       fdt_strerror(err));
	}
}

void fdt_fixup_crypto_node(void *blob, int sec_rev)
{
	u8 era;

	if (!sec_rev) {
		fdt_del_node_and_alias(blob, "crypto");
		return;
	}

	/* Add SEC ERA information in compatible */
	era = caam_get_era();
	if (era) {
		fdt_fixup_crypto_era(blob, era);
	} else {
		printf("WARNING: Unable to get ERA for CAAM rev: %d\n",
			sec_rev);
	}
}
#endif

int fdt_fixup_phy_connection(void *blob, int offset, phy_interface_t phyc)
{
	return fdt_setprop_string(blob, offset, "phy-connection-type",
					 phy_string_for_interface(phyc));
}

#ifdef CONFIG_SYS_SRIO
static inline void ft_disable_srio_port(void *blob, int srio_off, int port)
{
	int off = fdt_node_offset_by_prop_value(blob, srio_off,
			"cell-index", &port, 4);
	if (off >= 0) {
		off = fdt_setprop_string(blob, off, "status", "disabled");
		if (off > 0)
			printf("WARNING unable to set status for fsl,srio "
				"port %d: %s\n", port, fdt_strerror(off));
	}
}

static inline void ft_disable_rman(void *blob)
{
	int off = fdt_node_offset_by_compatible(blob, -1, "fsl,rman");
	if (off >= 0) {
		off = fdt_setprop_string(blob, off, "status", "disabled");
		if (off > 0)
			printf("WARNING unable to set status for fsl,rman %s\n",
				fdt_strerror(off));
	}
}

static inline void ft_disable_rmu(void *blob)
{
	int off = fdt_node_offset_by_compatible(blob, -1, "fsl,srio-rmu");
	if (off >= 0) {
		off = fdt_setprop_string(blob, off, "status", "disabled");
		if (off > 0)
			printf("WARNING unable to set status for "
				"fsl,srio-rmu %s\n", fdt_strerror(off));
	}
}

void ft_srio_setup(void *blob)
{
	int srio1_used = 0, srio2_used = 0;
	int srio_off;

	/* search for srio node, if doesn't exist just return - nothing todo */
	srio_off = fdt_node_offset_by_compatible(blob, -1, "fsl,srio");
	if (srio_off < 0)
		return ;

#ifdef CONFIG_SRIO1
	if (is_serdes_configured(SRIO1))
		srio1_used = 1;
#endif
#ifdef CONFIG_SRIO2
	if (is_serdes_configured(SRIO2))
		srio2_used = 1;
#endif

	/* mark port1 disabled */
	if (!srio1_used)
		ft_disable_srio_port(blob, srio_off, 1);

	/* mark port2 disabled */
	if (!srio2_used)
		ft_disable_srio_port(blob, srio_off, 2);

	/* if both ports not used, disable controller, rmu and rman */
	if (!srio1_used && !srio2_used) {
		fdt_setprop_string(blob, srio_off, "status", "disabled");

		ft_disable_rman(blob);
		ft_disable_rmu(blob);
	}
}
#endif
