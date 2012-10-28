#include <common.h>

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void) {

       puts("Board: A13 MID\n");
       return 0;
}
#endif
