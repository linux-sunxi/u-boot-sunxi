#include <common.h>

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void) {

	puts("Board: A13-OLinuXino\n");
	return 0;
}
#endif
