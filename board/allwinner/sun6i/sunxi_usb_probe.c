/*
 * (C) Copyright 2012
 *     wangflord@allwinnertech.com
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 */
#include <common.h>
#include <pmu.h>
#include <asm/arch/timer.h>
#include <asm/arch/cpu.h>
#include <asm/arch/intc.h>
#include <asm/io.h>
#include "sunxi_usb_register.h"

struct timer_list  timer_for_usb;
static int         usb_working = 0;
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static unsigned int usb_get_bus_interrupt_status(void)
{
	return readl(SUNXI_USBOTG_BASE + USB_bINTRUSB_OFF) & 0xff;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void usb_clear_bus_interrupt_status(unsigned int bm)
{
	writel(bm&0xff, SUNXI_USBOTG_BASE + USB_bINTRUSB_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void usb_drive_vbus(int vbus, int index)
{
	unsigned int temp;
	//Set PIOH14/15/16 to Output
	if(index == 0)
	{
		temp = readl(0x01c20800+0x100);
		temp &= ~(0x7<<24);
		temp |= (0x1<<24);
		writel(temp, 0x01c20800+0x100);
		if(vbus)
		{
			temp = readl(0x01c20800+0x10c);
			temp |= (0x1<<14);
			writel(temp, 0x01c20800+0x10c);
		}
		else
		{
			temp = readl(0x01c20800+0x10c);
			temp &= ~(0x1<<14);
			writel(temp, 0x01c20800+0x10c);
		}
	}
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void usb_force_id(int id)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_wISCR_OFF);
	temp |= (1 << 15);
	if(id)
	{
		temp |= (1 << 14);
	}
	else
	{
		temp &= ~(1 << 14);
	}
	writel(temp, SUNXI_USBOTG_BASE + USB_wISCR_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_high_speed_set(int on_off)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_bPOWER_OFF);
	if(on_off)
	{
		temp |= (0x1<<5);
	}
	else
	{
		temp &= ~(0x1<<5);
	}
	writel(temp, SUNXI_USBOTG_BASE + USB_bPOWER_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_suspendm_enable(void)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_bPOWER_OFF);
	temp |= (0x1<<0);
	writel(temp, SUNXI_USBOTG_BASE + USB_bPOWER_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_vbus_src(int src)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_wISCR_OFF);
	temp &= ~(0x3<<10);
	temp |=  ((src&0x03)<<10);
	writel(temp, SUNXI_USBOTG_BASE + USB_wISCR_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_release_vbus(void)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_wISCR_OFF);
	temp &= ~(0x3<<12);
	writel(temp, SUNXI_USBOTG_BASE + USB_wISCR_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_force_vbus(int vbus)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_wISCR_OFF);
	temp |= (1 << 13);
	if(vbus)
	{
		temp |= (1 << 12);
	}
	else
	{
		temp &= ~(1 << 12);
	}
	writel(temp, SUNXI_USBOTG_BASE + USB_wISCR_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_select_ep(int ep_no)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_GCS_OFFSET);
	temp &= ~(0xff << 16);
	writel(ep_no | temp, SUNXI_USBOTG_BASE + USB_GCS_OFFSET);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_ep0_flush_fifo(void)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_hTXMAXP_OFF);
	temp |= (0x1<<24);
	writel(temp, SUNXI_USBOTG_BASE + USB_hTXMAXP_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_clear_bus_interrupt_enable(int bm)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_bINTRUSBE_OFF);
	temp &= ~(bm&0xff);
	writel(temp, SUNXI_USBOTG_BASE + USB_bINTRUSBE_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_set_bus_interrupt_enable(int bm)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_bINTRUSBE_OFF);
	temp |= (bm&0xff);
	writel(temp, SUNXI_USBOTG_BASE + USB_bINTRUSBE_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_soft_connect(void)
{
	uint temp;

	temp = readl(SUNXI_USBOTG_BASE + USB_bPOWER_OFF);
	temp |= (0x1<<6);
	writel(temp, SUNXI_USBOTG_BASE + USB_bPOWER_OFF);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void usb_clock_init(void)
{
    u32 ccmu_base = 0x1c20000;
    u32 reg_value = 0;
    u32 offset_ahb = 0x60;
    u32 offset_ahb_rst = 0x2c0;
    u32 offet_phy = 0xcc;

    /* AHB gating */
    reg_value = readl(ccmu_base + offset_ahb);
    reg_value |= (1 << 24);
    writel(reg_value, (ccmu_base + offset_ahb));

    /* AHB reset */
    reg_value = readl(ccmu_base + offset_ahb_rst);
    reg_value |= (1 << 24);
    writel(reg_value, (ccmu_base + offset_ahb_rst));
    __msdelay(1);

    /* phy gating */
    reg_value = readl(ccmu_base + offet_phy);
    reg_value |= (1 << 8);
    writel(reg_value, (ccmu_base + offet_phy));

    /* phy reset */
    reg_value = readl(ccmu_base + offet_phy);
    reg_value |= (1 << 0);
    writel(reg_value, (ccmu_base + offet_phy));
    __msdelay(1);

    return;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void usb_clock_exit(void)
{
    u32 ccmu_base = 0x1c20000;
    u32 reg_value = 0;
    u32 offset_ahb = 0x60;
    u32 offset_ahb_rst = 0x2c0;
    u32 offet_phy = 0xcc;

    /* AHB gating */
    reg_value = readl(ccmu_base + offset_ahb);
    reg_value &= ~(1 << 24);
    writel(reg_value, (ccmu_base + offset_ahb));

    /* AHB reset */
    reg_value = readl(ccmu_base + offset_ahb_rst);
    reg_value &= ~(1 << 24);
    writel(reg_value, (ccmu_base + offset_ahb_rst));
    __msdelay(1);

    /* phy gating */
    reg_value = readl(ccmu_base + offet_phy);
    reg_value &= ~(1 << 8);
    writel(reg_value, (ccmu_base + offet_phy));

    /* phy reset */
    reg_value = readl(ccmu_base + offet_phy);
    reg_value &= ~(1 << 0);
    writel(reg_value, (ccmu_base + offet_phy));
    __msdelay(1);

    return;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void usb_init(void)
{
	usb_drive_vbus(0, 0);
	usb_force_id(1);
	usb_high_speed_set(1);

	usb_suspendm_enable();

	usb_vbus_src(0x1);
	usb_release_vbus();
	usb_force_vbus(1);

	usb_select_ep(0);
	usb_ep0_flush_fifo();

	debug("USB Device!!\n");
	usb_clear_bus_interrupt_enable(0xff);
	usb_set_bus_interrupt_enable(0xf7);
	//usb_set_eptx_interrupt_enable(0x3f);
	//usb_set_eprx_interrupt_enable(0x3e);
	return;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void timer_test_usbdc(unsigned long p_arg)
{
	usb_clock_exit();
	irq_disable(AW_IRQ_USB_OTG);
	usb_working = 0;

	//打开火牛限流
	tick_printf("timer irq ,set dc\n");
	axp_set_vbus_limit_pc();

	return ;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void usb_detect_irq_handler(void *p_arg)
{
	uint temp;

	temp = usb_get_bus_interrupt_status();
	usb_clear_bus_interrupt_status(temp);

	debug("usb irq %x\n", temp);

	if(temp & 0x04)
	{
		usb_clock_exit();
		irq_disable(AW_IRQ_USB_OTG);
		del_timer(&timer_for_usb);
		usb_working = 0;

		//打开usb限流
		tick_printf("usb irq ,set pc\n");
		axp_set_vbus_limit_pc();
	}

	return;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int usb_detect_enter(void)
{
	debug("usb start detect\n");
	if(!usb_working)
	{
		tick_printf("usb enter detect\n");
		usb_working = 1;
		usb_clock_init();

		timer_for_usb.expires  = BOOT_USB_DETECT_DELAY_TIME;
		timer_for_usb.function = timer_test_usbdc;
		timer_for_usb.data     = 0;
		add_timer(&timer_for_usb);

		irq_install_handler(AW_IRQ_USB_OTG, usb_detect_irq_handler, 0);
		irq_enable(AW_IRQ_USB_OTG);
		usb_init();
		usb_soft_connect();
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int usb_detect_exit(void)
{
	tick_printf("usb exit detect\n");
	usb_working = 0;
	usb_clock_exit();
	irq_disable(AW_IRQ_USB_OTG);
	del_timer(&timer_for_usb);

	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void usb_detect_for_charge(int detect_time)
{
	usb_detect_enter();
	__msdelay(detect_time);
	usb_detect_exit();
}















