/*
 * Command for accessing DataFlash.
 *
 * Copyright (C) 2008 Atmel Corporation
 */
#include <common.h>

static int do_shutdown(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int off_vol;
	int ret;

	ret = script_parser_fetch("pmu_para", "pmu_pwroff_vol", &off_vol, 1);
    if(ret)
    {
    	off_vol = 3300;
    }
    debug("off vol = %d\n", off_vol);
    /* close RTC */
    debug("rtc disable\n");
    rtc_disable();
	/* write boot status to axp */
	debug("set next status\n");
	axp_set_next_poweron_status(0);
	/* close flash */
	debug("flash exit\n");
	sunxi_flash_exit();
	/* set power off vol for next time */
	debug("set power off vol\n");
	axp_set_hardware_poweroff_vol(off_vol);
	/* power off */
	debug("begin power off\n");
	axp_set_power_off();
	debug("power off already\n");

	return 0;
usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	shutdown,	2,	1,	do_shutdown,
	"shutdown the system",
	"power off the power supply"
);
