/*
 * MarsBoard <marsboard@gmail.com>
 *
 * Some board init for the Allwinner A10-evb board.
 *
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void) {

	puts("Board: MarsBoard A10\n");
	/* set LED */
	gpio_direction_output(SUNXI_GPB(5),0);
	gpio_direction_output(SUNXI_GPB(6),0);
	gpio_direction_output(SUNXI_GPB(7),0);
	gpio_direction_output(SUNXI_GPB(8),0);
        /* Set pull-up resistor PB23 */
	writel(0x40004000,(SUNXI_PIO_BASE + 0x44));
	return 0;
}
#endif
