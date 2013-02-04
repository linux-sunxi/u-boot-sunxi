/*
**********************************************************************************************************************
*											        eGon
*						                     the Embedded System
*									       script parser sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : script.c
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#ifndef  __STANDBY_I_H__
#define  __STANDBY_I_H__


#include  <common.h>
#include  <asm/io.h>
#include  "dram/dram_i.h"

#define   BOOT_STANDBY_DRAM_PARA_ADDR    (0x200)

extern void boot_store_sp(void);
extern void boot_restore_sp(void);
extern void boot_set_sp(void);
extern void boot_halt(void);

extern int standby_axp_output_control(int onoff);
extern int standby_axp_store_int_status(void);
extern int standby_axp_restore_int_status(void);
extern int standby_axp_int_query(__u8 *int_status);
extern int standby_axp_probe_power_exist(void);
extern int standby_axp_probe_key(void);
extern int standby_axp_probe_usb(void);

extern int  standby_int_disable(void);
extern int  standby_int_enable(void);
extern void standby_gic_store(void);
extern void standby_gic_restore(void);
extern void standby_gic_clear_pengding(void);


extern int  standby_axp_i2c_read(unsigned char chip, unsigned char addr, unsigned char *buffer);
extern int  standby_axp_i2c_write(unsigned char chip, unsigned char addr, unsigned char data);

extern void standby_timer_delay(unsigned int ms);

extern int  standby_clock_to_24M(void);
extern int  standby_clock_to_pll1(void);
extern void standby_clock_plldisable(void);
extern void standby_clock_pllenable(void);

extern void standby_serial_putc(char c);

extern int dram_power_save_process(void);
extern int dram_power_up_process(void);


#endif  // __STANDBY_I_H__


