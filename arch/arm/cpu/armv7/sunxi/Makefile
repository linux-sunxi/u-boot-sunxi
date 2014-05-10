#
# (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
#
# Based on some other Makefile
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#
obj-y	+= timer.o
obj-y	+= board.o
obj-y	+= clock.o
obj-y	+= pinmux.o
obj-y	+= watchdog.o
obj-$(CONFIG_SUN6I)	+= prcm.o
obj-$(CONFIG_SUN8I)	+= prcm.o
obj-$(CONFIG_SUN6I)	+= p2wi.o
obj-$(CONFIG_SUN4I)	+= clock_sun4i.o
obj-$(CONFIG_SUN5I)	+= clock_sun4i.o
obj-$(CONFIG_SUN6I)	+= clock_sun6i.o
obj-$(CONFIG_SUN7I)	+= clock_sun4i.o
obj-$(CONFIG_SUN8I)	+= clock_sun6i.o
ifdef DEBUG
obj-y	+= early_print.o
endif
obj-$(CONFIG_BOARD_POSTCLK_INIT)	+= postclk_init.o
obj-$(CONFIG_SYS_SECONDARY_ON)	+= secondary_init.o
obj-$(CONFIG_SYS_SECONDARY_ON)	+= smp.o

ifndef CONFIG_SPL_BUILD
obj-y	+= cpu_info.o
ifdef CONFIG_CMD_WATCHDOG
obj-$(CONFIG_CMD_WATCHDOG)	+= cmd_watchdog.o
endif
endif

ifdef CONFIG_SPL_BUILD
obj-$(CONFIG_SUN4I)	+= dram.o
obj-$(CONFIG_SUN5I)	+= dram.o
obj-$(CONFIG_SUN7I)	+= dram.o
ifdef CONFIG_SPL_FEL
obj-y	+= start.o
endif
endif
