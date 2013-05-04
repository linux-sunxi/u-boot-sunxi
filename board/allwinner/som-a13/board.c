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

	puts("Board: MarsBoard SOM-A13\n");
	/* set LED */
	gpio_direction_output(SUNXI_GPB(10),0);
	return 0;
}
#endif
