/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Early uart print for debugging.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/early_print.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sys_proto.h>

static int uart_initialized = 0;

#if CONFIG_CONS_INDEX < 5
#define UART	CONFIG_CONS_INDEX-1
#else
/* SUNXI_R_UART_BASE */
#define UART	2922
#endif

void uart_init(void) {

	/* select dll dlh */
	writel(UART_LCR_DLAB, UART_LCR(UART));
	/* set baudrate */
	writel(0, UART_DLH(UART));
	writel(BAUD_115200, UART_DLL(UART));
	/* set line control */
	writel(LC_8_N_1, UART_LCR(UART));

	uart_initialized = 1;
}

#define TX_READY (readl(UART_LSR(UART)) & UART_LSR_TEMT)

void uart_putc(char c) {

	while (!TX_READY)
		;
	writel(c, UART_THR(UART));
}

void uart_puts(const char *s) {

	while (*s)
		uart_putc(*s++);
}


