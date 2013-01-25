#
# (C) Copyright 2000-2007
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	:= $(obj)libvideo_sunxi.o

COBJS-y += dev_disp.o \
							OSAL/OSAL_Clock.o OSAL/OSAL_Int.o OSAL/OSAL_IrqLock.o OSAL/OSAL_Cache.o\
							OSAL/OSAL_Lib_C.o OSAL/OSAL_Pin.o OSAL/OSAL_Parser.o\
							de_bsp/de/ebios/de_be.o de_bsp/de/ebios/de_fe.o de_bsp/de/ebios/de_hwc.o\
							de_bsp/de/ebios/de_layer.o de_bsp/de/ebios/de_lcd.o de_bsp/de/ebios/de_tve.o de_bsp/de/ebios/de_dsi.o\
							de_bsp/de/disp_clk.o de_bsp/de/disp_combined.o de_bsp/de/disp_de.o de_bsp/de/disp_display.o\
							de_bsp/de/disp_event.o de_bsp/de/disp_hdmi.o de_bsp/de/disp_hwc.o de_bsp/de/disp_layer.o\
							de_bsp/de/disp_lcd.o de_bsp/de/disp_scaler.o de_bsp/de/disp_tv.o\
							de_bsp/de/disp_vga.o de_bsp/de/disp_video.o de_bsp/de/disp_lcd_iic.o\
							de_bsp/iep/cmu/iep_cmu.o de_bsp/de/disp_iep.o\
							de_bsp/iep/drc/iep_drc.o de_bsp/iep/drc/iep_drc_ebios.o\
							de_bsp/iep/deu/iep_deu.o de_bsp/iep/deu/iep_deu_ebios.o\
							de_bsp/lcd/lcd0_panel_cfg.o de_bsp/lcd/lcd1_panel_cfg.o\
							de_bsp/lcd/lcd_bak/lcd_edp_anx9804.o de_bsp/lcd/lcd_bak/lcd_edp_anx6345.o\

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################



