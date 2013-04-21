/*
 * (C) Copyright 2012 Stephen Warren
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <lcd.h>
#include <asm/arch/mbox.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

/* Global variables that lcd.c expects to exist */
vidinfo_t panel_info;

struct msg_query {
	struct bcm2835_mbox_hdr hdr;
	struct bcm2835_mbox_tag_physical_w_h physical_w_h;
	u32 end_tag;
};

struct msg_setup {
	struct bcm2835_mbox_hdr hdr;
	struct bcm2835_mbox_tag_physical_w_h physical_w_h;
	struct bcm2835_mbox_tag_virtual_w_h virtual_w_h;
	struct bcm2835_mbox_tag_depth depth;
	struct bcm2835_mbox_tag_pixel_order pixel_order;
	struct bcm2835_mbox_tag_alpha_mode alpha_mode;
	struct bcm2835_mbox_tag_virtual_offset virtual_offset;
	struct bcm2835_mbox_tag_overscan overscan;
	struct bcm2835_mbox_tag_allocate_buffer allocate_buffer;
	u32 end_tag;
};

void lcd_ctrl_init(void *lcdbase)
{
	ALLOC_ALIGN_BUFFER(struct msg_query, msg_query, 1, 16);
	ALLOC_ALIGN_BUFFER(struct msg_setup, msg_setup, 1, 16);
	int ret;
	u32 w, h;

	debug("bcm2835: Query resolution...\n");

	BCM2835_MBOX_INIT_HDR(msg_query);
	BCM2835_MBOX_INIT_TAG_NO_REQ(&msg_query->physical_w_h,
					GET_PHYSICAL_W_H);
	ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN, &msg_query->hdr);
	if (ret) {
		printf("bcm2835: Could not query display resolution\n");
		/* FIXME: How to disable the LCD to prevent errors? hang()? */
		return;
	}

	w = msg_query->physical_w_h.body.resp.width;
	h = msg_query->physical_w_h.body.resp.height;

	debug("bcm2835: Setting up display for %d x %d\n", w, h);

	BCM2835_MBOX_INIT_HDR(msg_setup);
	BCM2835_MBOX_INIT_TAG(&msg_setup->physical_w_h, SET_PHYSICAL_W_H);
	msg_setup->physical_w_h.body.req.width = w;
	msg_setup->physical_w_h.body.req.height = h;
	BCM2835_MBOX_INIT_TAG(&msg_setup->virtual_w_h, SET_VIRTUAL_W_H);
	msg_setup->virtual_w_h.body.req.width = w;
	msg_setup->virtual_w_h.body.req.height = h;
	BCM2835_MBOX_INIT_TAG(&msg_setup->depth, SET_DEPTH);
	msg_setup->depth.body.req.bpp = 16;
	BCM2835_MBOX_INIT_TAG(&msg_setup->pixel_order, SET_PIXEL_ORDER);
	msg_setup->pixel_order.body.req.order = BCM2835_MBOX_PIXEL_ORDER_BGR;
	BCM2835_MBOX_INIT_TAG(&msg_setup->alpha_mode, SET_ALPHA_MODE);
	msg_setup->alpha_mode.body.req.alpha = BCM2835_MBOX_ALPHA_MODE_IGNORED;
	BCM2835_MBOX_INIT_TAG(&msg_setup->virtual_offset, SET_VIRTUAL_OFFSET);
	msg_setup->virtual_offset.body.req.x = 0;
	msg_setup->virtual_offset.body.req.y = 0;
	BCM2835_MBOX_INIT_TAG(&msg_setup->overscan, SET_OVERSCAN);
	msg_setup->overscan.body.req.top = 0;
	msg_setup->overscan.body.req.bottom = 0;
	msg_setup->overscan.body.req.left = 0;
	msg_setup->overscan.body.req.right = 0;
	BCM2835_MBOX_INIT_TAG(&msg_setup->allocate_buffer, ALLOCATE_BUFFER);
	msg_setup->allocate_buffer.body.req.alignment = 0x100;

	ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN, &msg_setup->hdr);
	if (ret) {
		printf("bcm2835: Could not configure display\n");
		/* FIXME: How to disable the LCD to prevent errors? hang()? */
		return;
	}

	w = msg_setup->physical_w_h.body.resp.width;
	h = msg_setup->physical_w_h.body.resp.height;

	debug("bcm2835: Final resolution is %d x %d\n", w, h);

	panel_info.vl_col = w;
	panel_info.vl_row = h;
	panel_info.vl_bpix = LCD_COLOR16;

	gd->fb_base = msg_setup->allocate_buffer.body.resp.fb_address;
}

void lcd_enable(void)
{
}
