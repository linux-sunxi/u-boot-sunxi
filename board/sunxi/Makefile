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
COBJS-$(CONFIG_A13_OLINUXINO)	+= dram_a13_olinuxino.o
COBJS-$(CONFIG_A13_MID)		+= dram_a13_mid.o
COBJS-$(CONFIG_CUBIEBOARD)	+= dram_cubieboard.o
COBJS-$(CONFIG_CUBIEBOARD_512)	+= dram_cubieboard_512.o
COBJS-$(CONFIG_HACKBERRY)	+= dram_hackberry.o
COBJS-$(CONFIG_MELE_A1000)	+= dram_mele_a1000.o
COBJS-$(CONFIG_MINI_X)		+= dram_mini_x.o

COBJS	:= $(COBJS-y)
SRCS 	:= $(COBJS:.o=.c)
OBJS 	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

########################################################################
