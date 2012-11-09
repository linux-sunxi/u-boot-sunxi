/*
 * (C) Copyright 2007
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * based on a the Linux rtc-x1207.c driver which is:
 *	Copyright 2004 Karen Spearel
 *	Copyright 2005 Alessandro Zummo
 *
 * Information and datasheet:
 * http://www.intersil.com/cda/deviceinfo/0,1477,X1205,00.html
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Date & Time support for Xicor/Intersil X1205 RTC
 */

/* #define	DEBUG	*/

#include <common.h>
#include <command.h>
#include <rtc.h>
#include <asm/arch/rtc.h>

/*
 * In the routines that deal directly with the x1205 hardware, we use
 * rtc_time -- month 0-11, hour 0-23, yr = calendar year-epoch
 * Epoch is initialized as 2000. Time is set to UTC.
 */
int rtc_get(struct rtc_time *tm)
{
	return 0;
}

int rtc_set(struct rtc_time *tm)
{
	return 0;
}

void rtc_reset(void)
{
	/*
	 * Nothing to do
	 */
}

void rtc_disable(void)
{
    struct sunxi_rtc_regs *rtc = (struct sunxi_rtc_regs *)SUNXI_RTC_BASE;

	rtc->alarm0_irq_enable = 0;
	rtc->alarm0_irq_status = 1;
	
	rtc->alarm1_irq_enable = 0;
	rtc->alarm1_irq_status = 1;

	return ;
}
