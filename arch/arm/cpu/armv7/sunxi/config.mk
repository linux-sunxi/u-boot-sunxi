# Build a combined spl + u-boot image
ifdef CONFIG_SPL
ifndef CONFIG_SPL_BUILD
ALL-y = $(obj)u-boot-sunxi-with-spl.bin
endif
endif
