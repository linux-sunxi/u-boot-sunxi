/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Early uart print for debugging.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_EARLY_PRINT_H
#define _SUNXI_EARLY_PRINT_H

#include <asm/arch/cpu.h>

#define SUNXI_UART_BASE SUNXI_UART0_BASE

#define UART_OFFSET 0x400

/* receive buffer register */
#define UART_RBR(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0x0)
/* transmit holding register */
#define UART_THR(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0x0)
/* divisor latch low register */
#define UART_DLL(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0x0)

/* divisor latch high register */
#define UART_DLH(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0x4)
/* interrupt enable reigster */
#define UART_IER(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0x4)

/* interrupt identity register */
#define UART_IIR(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0x8)
/* fifo control register */
#define UART_FCR(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0x8)

/* line control register */
#define UART_LCR(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0xc)
#define UART_LCR_DLAB (0x1 << 7)

/* line status register */
#define UART_LSR(n) (SUNXI_UART_BASE + (n) * UART_OFFSET + 0x14)
#define UART_LSR_TEMT (0x1 << 6)


#define BAUD_115200    (0xd) /* 24 * 1000 * 1000 / 16 / 115200 = 13 */
#define NO_PARITY      (0)
#define ONE_STOP_BIT   (0)
#define DAT_LEN_8_BITS (3)
#define LC_8_N_1          (NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

#ifndef __ASSEMBLY__
void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
#endif /* __ASSEMBLY__ */

#endif /* _SUNXI_EARLY_PRINT_H */
