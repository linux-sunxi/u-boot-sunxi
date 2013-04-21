/*
 * Copyright (C) 2012 Samsung Electronics
 *
 * Author: InKi Dae <inki.dae@samsung.com>
 * Author: Donghwa Lee <dh09.lee@samsung.com>
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

#include <config.h>
#include <common.h>
#include <lcd.h>
#include <fdtdec.h>
#include <libfdt.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/clk.h>
#include <asm/arch/mipi_dsim.h>
#include <asm/arch/dp_info.h>
#include <asm/arch/system.h>
#include <asm-generic/errno.h>

#include "exynos_fb.h"

DECLARE_GLOBAL_DATA_PTR;

static unsigned int panel_width, panel_height;

/*
 * board_init_f(arch/arm/lib/board.c) calls lcd_setmem() which needs
 * panel_info.vl_col, panel_info.vl_row and panel_info.vl_bpix to reserve
 * FB memory at a very early stage, i.e even before exynos_fimd_parse_dt()
 * is called. So, we are forced to statically assign it.
 */
#ifdef CONFIG_OF_CONTROL
vidinfo_t panel_info  = {
	.vl_col = LCD_XRES,
	.vl_row = LCD_YRES,
	.vl_bpix = LCD_COLOR16,
};
#endif

static void exynos_lcd_init_mem(void *lcdbase, vidinfo_t *vid)
{
	unsigned long palette_size;
	unsigned int fb_size;

	fb_size = vid->vl_row * vid->vl_col * (NBITS(vid->vl_bpix) >> 3);

	palette_size = NBITS(vid->vl_bpix) == 8 ? 256 : 16;

	exynos_fimd_lcd_init_mem((unsigned long)lcdbase,
			(unsigned long)fb_size, palette_size);
}

static void exynos_lcd_init(vidinfo_t *vid)
{
	exynos_fimd_lcd_init(vid);

	/* Enable flushing after LCD writes if requested */
	lcd_set_flush_dcache(1);
}

#ifdef CONFIG_CMD_BMP
static void draw_logo(void)
{
	int x, y;
	ulong addr;

	if (panel_width >= panel_info.logo_width) {
		x = ((panel_width - panel_info.logo_width) >> 1);
	} else {
		x = 0;
		printf("Warning: image width is bigger than display width\n");
	}

	if (panel_height >= panel_info.logo_height) {
		y = ((panel_height - panel_info.logo_height) >> 1) - 4;
	} else {
		y = 0;
		printf("Warning: image height is bigger than display height\n");
	}

	addr = panel_info.logo_addr;
	bmp_display(addr, x, y);
}
#endif

void __exynos_cfg_lcd_gpio(void)
{
}
void exynos_cfg_lcd_gpio(void)
	__attribute__((weak, alias("__exynos_cfg_lcd_gpio")));

void __exynos_backlight_on(unsigned int onoff)
{
}
void exynos_backlight_on(unsigned int onoff)
	__attribute__((weak, alias("__exynos_cfg_lcd_gpio")));

void __exynos_reset_lcd(void)
{
}
void exynos_reset_lcd(void)
	__attribute__((weak, alias("__exynos_reset_lcd")));

void __exynos_lcd_power_on(void)
{
}
void exynos_lcd_power_on(void)
	__attribute__((weak, alias("__exynos_lcd_power_on")));

void __exynos_cfg_ldo(void)
{
}
void exynos_cfg_ldo(void)
	__attribute__((weak, alias("__exynos_cfg_ldo")));

void __exynos_enable_ldo(unsigned int onoff)
{
}
void exynos_enable_ldo(unsigned int onoff)
	__attribute__((weak, alias("__exynos_enable_ldo")));

void __exynos_backlight_reset(void)
{
}
void exynos_backlight_reset(void)
	__attribute__((weak, alias("__exynos_backlight_reset")));

static void lcd_panel_on(vidinfo_t *vid)
{
	udelay(vid->init_delay);

	exynos_backlight_reset();

	exynos_cfg_lcd_gpio();

	exynos_lcd_power_on();

	udelay(vid->power_on_delay);

	if (vid->dp_enabled)
		exynos_init_dp();

	exynos_reset_lcd();

	udelay(vid->reset_delay);

	exynos_backlight_on(1);

	exynos_cfg_ldo();

	exynos_enable_ldo(1);

	if (vid->mipi_enabled)
		exynos_mipi_dsi_init();
}

#ifdef CONFIG_OF_CONTROL
int exynos_fimd_parse_dt(const void *blob)
{
	unsigned int node;
	node = fdtdec_next_compatible(blob, 0, COMPAT_SAMSUNG_EXYNOS_FIMD);
	if (node <= 0) {
		debug("exynos_fb: Can't get device node for fimd\n");
		return -ENODEV;
	}

	panel_info.vl_col = fdtdec_get_int(blob, node, "samsung,vl-col", 0);
	if (panel_info.vl_col == 0) {
		debug("Can't get XRES\n");
		return -ENXIO;
	}

	panel_info.vl_row = fdtdec_get_int(blob, node, "samsung,vl-row", 0);
	if (panel_info.vl_row == 0) {
		debug("Can't get YRES\n");
		return -ENXIO;
	}

	panel_info.vl_width = fdtdec_get_int(blob, node,
						"samsung,vl-width", 0);

	panel_info.vl_height = fdtdec_get_int(blob, node,
						"samsung,vl-height", 0);

	panel_info.vl_freq = fdtdec_get_int(blob, node, "samsung,vl-freq", 0);
	if (panel_info.vl_freq == 0) {
		debug("Can't get refresh rate\n");
		return -ENXIO;
	}

	if (fdtdec_get_bool(blob, node, "samsung,vl-clkp"))
		panel_info.vl_clkp = CONFIG_SYS_LOW;

	if (fdtdec_get_bool(blob, node, "samsung,vl-oep"))
		panel_info.vl_oep = CONFIG_SYS_LOW;

	if (fdtdec_get_bool(blob, node, "samsung,vl-hsp"))
		panel_info.vl_hsp = CONFIG_SYS_LOW;

	if (fdtdec_get_bool(blob, node, "samsung,vl-vsp"))
		panel_info.vl_vsp = CONFIG_SYS_LOW;

	if (fdtdec_get_bool(blob, node, "samsung,vl-dp"))
		panel_info.vl_dp = CONFIG_SYS_LOW;

	panel_info.vl_bpix = fdtdec_get_int(blob, node, "samsung,vl-bpix", 0);
	if (panel_info.vl_bpix == 0) {
		debug("Can't get bits per pixel\n");
		return -ENXIO;
	}

	panel_info.vl_hspw = fdtdec_get_int(blob, node, "samsung,vl-hspw", 0);
	if (panel_info.vl_hspw == 0) {
		debug("Can't get hsync width\n");
		return -ENXIO;
	}

	panel_info.vl_hfpd = fdtdec_get_int(blob, node, "samsung,vl-hfpd", 0);
	if (panel_info.vl_hfpd == 0) {
		debug("Can't get right margin\n");
		return -ENXIO;
	}

	panel_info.vl_hbpd = (u_char)fdtdec_get_int(blob, node,
							"samsung,vl-hbpd", 0);
	if (panel_info.vl_hbpd == 0) {
		debug("Can't get left margin\n");
		return -ENXIO;
	}

	panel_info.vl_vspw = (u_char)fdtdec_get_int(blob, node,
							"samsung,vl-vspw", 0);
	if (panel_info.vl_vspw == 0) {
		debug("Can't get vsync width\n");
		return -ENXIO;
	}

	panel_info.vl_vfpd = fdtdec_get_int(blob, node,
							"samsung,vl-vfpd", 0);
	if (panel_info.vl_vfpd == 0) {
		debug("Can't get lower margin\n");
		return -ENXIO;
	}

	panel_info.vl_vbpd = fdtdec_get_int(blob, node, "samsung,vl-vbpd", 0);
	if (panel_info.vl_vbpd == 0) {
		debug("Can't get upper margin\n");
		return -ENXIO;
	}

	panel_info.vl_cmd_allow_len = fdtdec_get_int(blob, node,
						"samsung,vl-cmd-allow-len", 0);

	panel_info.win_id = fdtdec_get_int(blob, node, "samsung,winid", 0);
	panel_info.init_delay = fdtdec_get_int(blob, node,
						"samsung,init-delay", 0);
	panel_info.power_on_delay = fdtdec_get_int(blob, node,
						"samsung,power-on-delay", 0);
	panel_info.reset_delay = fdtdec_get_int(blob, node,
						"samsung,reset-delay", 0);
	panel_info.interface_mode = fdtdec_get_int(blob, node,
						"samsung,interface-mode", 0);
	panel_info.mipi_enabled = fdtdec_get_int(blob, node,
						"samsung,mipi-enabled", 0);
	panel_info.dp_enabled = fdtdec_get_int(blob, node,
						"samsung,dp-enabled", 0);
	panel_info.cs_setup = fdtdec_get_int(blob, node,
						"samsung,cs-setup", 0);
	panel_info.wr_setup = fdtdec_get_int(blob, node,
						"samsung,wr-setup", 0);
	panel_info.wr_act = fdtdec_get_int(blob, node, "samsung,wr-act", 0);
	panel_info.wr_hold = fdtdec_get_int(blob, node, "samsung,wr-hold", 0);

	panel_info.logo_on = fdtdec_get_int(blob, node, "samsung,logo-on", 0);
	if (panel_info.logo_on) {
		panel_info.logo_width = fdtdec_get_int(blob, node,
						"samsung,logo-width", 0);
		panel_info.logo_height = fdtdec_get_int(blob, node,
						"samsung,logo-height", 0);
		panel_info.logo_addr = fdtdec_get_int(blob, node,
						"samsung,logo-addr", 0);
	}

	panel_info.rgb_mode = fdtdec_get_int(blob, node,
						"samsung,rgb-mode", 0);
	panel_info.pclk_name = fdtdec_get_int(blob, node,
						"samsung,pclk-name", 0);
	panel_info.sclk_div = fdtdec_get_int(blob, node,
						"samsung,sclk-div", 0);
	panel_info.dual_lcd_enabled = fdtdec_get_int(blob, node,
						"samsung,dual-lcd-enabled", 0);

	return 0;
}
#endif

void lcd_ctrl_init(void *lcdbase)
{
	set_system_display_ctrl();
	set_lcd_clk();

#ifdef CONFIG_OF_CONTROL
	if (exynos_fimd_parse_dt(gd->fdt_blob))
		debug("Can't get proper panel info\n");
#endif
	/* initialize parameters which is specific to panel. */
	init_panel_info(&panel_info);

	panel_width = panel_info.vl_width;
	panel_height = panel_info.vl_height;

	exynos_lcd_init_mem(lcdbase, &panel_info);

	exynos_lcd_init(&panel_info);
}

void lcd_enable(void)
{
	if (panel_info.logo_on) {
		memset((void *) gd->fb_base, 0, panel_width * panel_height *
				(NBITS(panel_info.vl_bpix) >> 3));
#ifdef CONFIG_CMD_BMP
		draw_logo();
#endif
	}

	lcd_panel_on(&panel_info);
}

/* dummy function */
void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue)
{
	return;
}
