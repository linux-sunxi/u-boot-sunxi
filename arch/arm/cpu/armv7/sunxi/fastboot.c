#include <common.h>
#include <asm/io.h>
#include <asm/types.h>
#include <environment.h>
#include <command.h>
#include <fastboot.h>

int fastboot_init(struct cmd_fastboot_interface *interface)
{
	printf("$s\n", __FUNCTION__);
	return 0;
}

int fastboot_tx(unsigned char *buffer, unsigned int buffer_size)
{
	printf("$s\n", __FUNCTION__);
	return 0;
}
int fastboot_tx_status(const char *buffer, unsigned int buffer_size)
{
	printf("$s\n", __FUNCTION__);
	return 0;
}



int fastboot_getvar(const char *rx_buffer, char *tx_buffer)
{
	printf("$s\n", __FUNCTION__);
	return 0;
}


int fastboot_flash_init(void)
{
	printf("$s\n", __FUNCTION__);
	return 0;
}

int fastboot_poll(void)
{
	printf("$s\n", __FUNCTION__);
	return 0;
}



void fastboot_shutdown(void)
{
	printf("$s\n", __FUNCTION__);
}
