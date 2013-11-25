#
# (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
#
# Based on some other Makefile
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

obj-y	+= timer.o
obj-y	+= dram.o
obj-y	+= board.o
obj-y	+= clock.o
obj-y	+= pinmux.o
obj-y	+= watchdog.o
ifdef DEBUG
obj-y	+= early_print.o
endif
obj-$(CONFIG_BOARD_POSTCLK_INIT)	+= postclk_init.o
obj-$(CONFIG_SYS_SECONDARY_ON)	+= secondary_init.o
obj-$(CONFIG_SYS_SECONDARY_ON)	+= smp.o

ifndef CONFIG_SPL_BUILD
obj-y	+= key.o
obj-y	+= cpu_info.o
ifdef CONFIG_CMD_WATCHDOG
obj-$(CONFIG_CMD_WATCHDOG)	+= cmd_watchdog.o
endif
endif

ifdef CONFIG_SPL_BUILD
ifdef CONFIG_SPL_FEL
obj-y	+= start.o
endif
endif

