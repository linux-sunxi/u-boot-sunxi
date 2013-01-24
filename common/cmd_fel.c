/*
 * Command for accessing DataFlash.
 *
 * Copyright (C) 2008 Atmel Corporation
 */
#include <common.h>

int do_fel(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long temp;
    uint addr;
	uint control;

	if(argc >= 2)
	{
		char *tmp;

		tmp = argv[1];
		if(strcmp(tmp, "help"))
		{
			goto usage;
		}
	}
	addr = FEL_BASE;
	axp_set_next_poweron_status(0);
	timer_exit();
	sunxi_key_exit();
	p2wi_exit();
	sunxi_dma_exit();
	sunxi_flash_exit();
	disable_interrupts();
	interrupt_exit();
	asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r" (control) : : "cc");
    control &= ~0x0802;        /* disable program flow prediction, disable strict alignment fault checking */
	control |=  0x2000;        /* set high exception victors */
	asm volatile ("mcr p15, 0, %0, c1, c0, 0" : : "r" (control) : "memory");
	disable_caches();
	asm volatile("mov pc, %0" : : "r" (addr) : "memory");

	return 0;
usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	jump_fel,	2,	1,	do_fel,
	"jump to fel",
	"try to burn new img"
);

int early_fel(void)
{
	unsigned long temp;
    uint addr;
	uint control;

	addr = FEL_BASE;
	timer_exit();
	sunxi_key_exit();
	p2wi_exit();
	sunxi_dma_exit();
	disable_interrupts();
	interrupt_exit();
	asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r" (control) : : "cc");
    control &= ~0x0802;        /* disable program flow prediction, disable strict alignment fault checking */
	control |=  0x2000;        /* set high exception victors */
	asm volatile ("mcr p15, 0, %0, c1, c0, 0" : : "r" (control) : "memory");
	disable_caches();
	asm volatile("mov pc, %0" : : "r" (addr) : "memory");

	return 0;
}

