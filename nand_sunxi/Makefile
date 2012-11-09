#
# (C) Copyright 2000, 2001, 2002
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

LIB	= $(obj)libsunxi-nand.o

$(shell mkdir -p $(obj)nand_drv $(obj)src/format $(obj)src/logic \
	$(obj)src/ndfc $(obj)src/physic $(obj)src/scan)

COBJS	:= ./nand_drv/nand_for_uboot.o  		\
		   ./nand_drv/nand_osal_uboot.o 		\
		   ./nand_drv/nand_fspart.o 			\
		   ./src/format/nand_format.o			\
		   ./src/logic/bad_manage.o				\
		   ./src/logic/logic_cache.o			\
		   ./src/logic/logic_ctl.o				\
		   ./src/logic/mapping.o				\
		   ./src/logic/mapping_base.o			\
		   ./src/logic/merge.o					\
		   ./src/logic/read_reclaim.o			\
		   ./src/logic/wear_levelling.o			\
		   ./src/ndfc/nfc_r.o					\
		   ./src/ndfc/nfc_w.o					\
		   ./src/physic/nand_phy.o				\
		   ./src/physic/nand_simple_r.o			\
		   ./src/physic/nand_simple_w.o			\
		   ./src/scan/nand_id.o					\
		   ./src/scan/nand_scan.o

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
