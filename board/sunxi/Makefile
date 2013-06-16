#
# (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
#
# Based on some other board Makefile
#
# (C) Copyright 2000-2003
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	:= $(obj)lib$(BOARD).o

COBJS-y	:= board.o
COBJS-$(CONFIG_A10_MID_1GB)	+= dram_sun4i_360_1024_iow16.o
COBJS-$(CONFIG_A10S_OLINUXINO_M)	+= dram_a10s_olinuxino_m.o
COBJS-$(CONFIG_A13_OLINUXINO)	+= dram_a13_olinuxino.o
COBJS-$(CONFIG_A13_OLINUXINOM)	+= dram_a13_oli_micro.o
COBJS-$(CONFIG_A13_MID)		+= dram_a13_mid.o
COBJS-$(CONFIG_A20_OLINUXINO_M)	+= dram_a20_olinuxino_m.o
COBJS-$(CONFIG_AUXTEK_T003)	+= dram_auxtek_t003.o
# This is not a typo, uses the same mem settings as the a10s-olinuxino-m
COBJS-$(CONFIG_AUXTEK_T004)	+= dram_a10s_olinuxino_m.o
COBJS-$(CONFIG_BA10_TV_BOX)	+= dram_sun4i_384_1024_iow8.o
COBJS-$(CONFIG_COBY_MID7042)	+= dram_sun4i_408_1024_iow16.o
COBJS-$(CONFIG_COBY_MID8042)	+= dram_sun4i_360_1024_iow16.o
COBJS-$(CONFIG_COBY_MID9742)	+= dram_sun4i_408_1024_iow16.o
COBJS-$(CONFIG_CUBIEBOARD)	+= dram_cubieboard.o
COBJS-$(CONFIG_CUBIEBOARD_512)	+= dram_cubieboard_512.o
COBJS-$(CONFIG_CUBIEBOARD2)	+= dram_cubieboard2.o
COBJS-$(CONFIG_DNS_M82)		+= dram_sun4i_360_1024_iow16.o
COBJS-$(CONFIG_EOMA68_A10)	+= dram_sun4i_360_1024_iow8.o
COBJS-$(CONFIG_EOMA68_A20)	+= dram_eoma68_a20.o
COBJS-$(CONFIG_GOOSEBERRY_A721)	+= dram_gooseberry_a721.o
COBJS-$(CONFIG_H6)		+= dram_h6.o
COBJS-$(CONFIG_HACKBERRY)	+= dram_hackberry.o
COBJS-$(CONFIG_A7HD)		+= dram_sun4i_360_1024_iow8.o
COBJS-$(CONFIG_INET97F_II)	+= dram_sun4i_408_512.o
COBJS-$(CONFIG_MELE_A1000)	+= dram_sun4i_360_512.o
COBJS-$(CONFIG_MELE_A1000G)	+= dram_sun4i_360_1024_iow8.o
COBJS-$(CONFIG_MELE_A3700)	+= dram_sun4i_360_1024_iow8.o
COBJS-$(CONFIG_MINI_X)		+= dram_sun4i_360_512.o
COBJS-$(CONFIG_MINI_X_1GB)	+= dram_sun4i_360_1024_iow16.o
COBJS-$(CONFIG_MK802)		+= dram_sun4i_360_512.o
COBJS-$(CONFIG_MK802_1GB)	+= dram_sun4i_360_1024_iow16.o
COBJS-$(CONFIG_MK802_A10S)	+= dram_mk802_a10s.o
COBJS-$(CONFIG_MK802II)		+= dram_sun4i_408_1024_iow8.o
COBJS-$(CONFIG_PCDUINO)		+= dram_sun4i_408_1024_iow8.o
COBJS-$(CONFIG_PENGPOD700)	+= dram_sun4i_384_1024_iow8.o
COBJS-$(CONFIG_PENGPOD1000)	+= dram_sun4i_408_1024_iow16.o
COBJS-$(CONFIG_POV_PROTAB2)	+= dram_pov_protab2.o
COBJS-$(CONFIG_POV_PROTAB2_XXL)	+= dram_pov_protab2_xxl.o
COBJS-$(CONFIG_R7DONGLE)	+= dram_r7dongle.o
COBJS-$(CONFIG_SANEI_N90)	+= dram_sanei_n90.o
COBJS-$(CONFIG_UHOST_U1A)	+= dram_sun4i_360_1024_iow8.o
COBJS-$(CONFIG_WOBO_I5)		+= dram_wobo_i5.o
COBJS-$(CONFIG_XZPAD700)	+= dram_xzpad700.o

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

########################################################################
