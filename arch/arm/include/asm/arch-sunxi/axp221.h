/*
 * (C) Copyright 2012
 *     wangflord@allwinnerstech.com
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
#ifndef   __AXP221_REGS_H__
#define   __AXP221_REGS_H__

#define   AXP22_ADDR              (0x68)

//define AXP22 REGISTER
#define   BOOT_POWER22_STATUS              			(0x00)
#define   BOOT_POWER22_MODE_CHGSTATUS      			(0x01)
#define   BOOT_POWER22_OTG_STATUS          			(0x02)
#define   BOOT_POWER22_VERSION         	   			(0x03)
#define   BOOT_POWER22_DATA_BUFFER0        			(0x04)
#define   BOOT_POWER22_DATA_BUFFER1        			(0x05)
#define   BOOT_POWER22_DATA_BUFFER2        			(0x06)
#define   BOOT_POWER22_DATA_BUFFER3        			(0x07)
#define   BOOT_POWER22_DATA_BUFFER4        			(0x08)
#define   BOOT_POWER22_DATA_BUFFER5        			(0x09)
#define   BOOT_POWER22_DATA_BUFFER6        			(0x0a)
#define   BOOT_POWER22_DATA_BUFFER7        			(0x0b)
#define   BOOT_POWER22_DATA_BUFFER8        			(0x0c)
#define   BOOT_POWER22_DATA_BUFFER9        			(0x0d)
#define   BOOT_POWER22_DATA_BUFFER10       			(0x0e)
#define   BOOT_POWER22_DATA_BUFFER11       			(0x0f)
#define   BOOT_POWER22_OUTPUT_CTL1     	   			(0x10)
#define   BOOT_POWER22_OUTPUT_CTL2     	   			(0x12)
#define   BOOT_POWER22_OUTPUT_CTL3     	   			(0x13)
#define   BOOT_POWER22_DLDO1_VOL                    (0x15)
#define   BOOT_POWER22_DLDO2_VOL                    (0x16)
#define   BOOT_POWER22_DLDO3_VOL                    (0x17)
#define   BOOT_POWER22_DLDO4_VOL                    (0x18)
#define   BOOT_POWER22_ELDO1_VOL                    (0x19)
#define   BOOT_POWER22_ELDO2_VOL                    (0x1A)
#define   BOOT_POWER22_ELDO3_VOL                    (0x1B)
#define   BOOT_POWER22_DC5LDO3_VOL                  (0x1C)
#define   BOOT_POWER22_DC1OUT_VOL               	(0x21)
#define   BOOT_POWER22_DC2OUT_VOL          			(0x22)
#define   BOOT_POWER22_DC3OUT_VOL          			(0x23)
#define   BOOT_POWER22_DC4OUT_VOL          			(0x24)
#define   BOOT_POWER22_DC5OUT_VOL          			(0x25)
#define   BOOT_POWER22_ALDO1OUT_VOL                 (0x28)
#define   BOOT_POWER22_ALDO2OUT_VOL                 (0x29)
#define   BOOT_POWER22_ALDO3OUT_VOL                 (0x2A)

#define   BOOT_POWER22_IPS_SET             			(0x30)
#define   BOOT_POWER22_VOFF_SET            			(0x31)
#define   BOOT_POWER22_OFF_CTL             			(0x32)
#define   BOOT_POWER22_CHARGE1             			(0x33)
#define   BOOT_POWER22_CHARGE2             			(0x34)
#define   BOOT_POWER22_BACKUP_CHG          			(0x35)
#define   BOOT_POWER22_POK_SET             			(0x36)
#define   BOOT_POWER22_DCDC_FREQSET        			(0x37)
#define   BOOT_POWER22_VLTF_CHGSET         			(0x38)
#define   BOOT_POWER22_VHTF_CHGSET         			(0x39)
#define   BOOT_POWER22_APS_WARNING1        			(0x3A)
#define   BOOT_POWER22_APS_WARNING2        			(0x3B)
#define   BOOT_POWER22_VLTF_DISCHGSET      			(0x3C)
#define   BOOT_POWER22_VHTF_DISCHGSET      			(0x3D)
#define   BOOT_POWER22_DCDC_MODESET        			(0x80)
#define   BOOT_POWER22_VOUT_MONITOR        			(0x81)
#define   BOOT_POWER22_ADC_EN             			(0x82)
#define   BOOT_POWER22_ADC_SPEED           			(0x84)
#define   BOOT_POWER22_ADC_INPUTRANGE      			(0x85)
#define   BOOT_BOOT_POWER22_GPIO1ADC_VTH   			(0x86)
#define   BOOT_POWER22_GPIO1ADC_VTL        			(0x87)
#define   BOOT_POWER22_TIMER_CTL           			(0x8A)
#define   BOOT_POWER22_VBUS_DET_SRP        			(0x8B)
#define   BOOT_POWER22_HOTOVER_CTL         			(0x8F)
#define   BOOT_POWER22_GPIO0_CTL           			(0x90)
#define   BOOT_POWER22_GPIO0_VOL           			(0x91)
#define   BOOT_POWER22_GPIO1_CTL           			(0x92)
#define   BOOT_POWER22_GPIO1_VOL           			(0x93)
#define   BOOT_POWER22_GPIO012_SIGNAL      			(0x94)
#define   BOOT_POWER22_GPIO3_CTL           			(0x95)
#define   BOOT_POWER22_GPIO012_PDCTL       			(0x97)
#define   BOOT_POWER22_INTEN1              			(0x40)
#define   BOOT_POWER22_INTEN2              			(0x41)
#define   BOOT_POWER22_INTEN3              			(0x42)
#define   BOOT_POWER22_INTEN4              			(0x43)
#define   BOOT_POWER22_INTEN5              			(0x44)
#define   BOOT_POWER22_INTSTS1             			(0x48)
#define   BOOT_POWER22_INTSTS2             			(0x49)
#define   BOOT_POWER22_INTSTS3             			(0x4a)
#define   BOOT_POWER22_INTSTS4             			(0x4b)
#define   BOOT_POWER22_INTSTS5             			(0x4c)
#define   BOOT_POWER22_COULOMB_CTL         			(0xB8)
#define   BOOT_POWER22_COULOMB_CAL					(0xB9)

//adc data register
#define   BOOT_POWER22_BAT_AVERVOL_H8          		(0x78)
#define   BOOT_POWER22_BAT_AVERVOL_L4          		(0x79)
#define   BOOT_POWER22_BAT_AVERCHGCUR_H8       		(0x7A)
#define   BOOT_POWER22_BAT_AVERCHGCUR_L4       		(0x7B)
#define   BOOT_POWER22_ACIN_VOL_H8             		(0x56)
#define   BOOT_POWER22_ACIN_VOL_L4             		(0x57)
#define   BOOT_POWER22_ACIN_CUR_H8             		(0x58)
#define   BOOT_POWER22_ACIN_CUR_L4             		(0x59)
#define   BOOT_POWER22_VBUS_VOL_H8             		(0x5A)
#define   BOOT_POWER22_VBUS_VOL_L4             		(0x5B)
#define   BOOT_POWER22_VBUS_CUR_H8             		(0x5C)
#define   BOOT_POWER22_VBUS_CUR_L4             		(0x5D)
#define   BOOT_POWER22_BAT_AVERDISCHGCUR_H8    		(0x7C)
#define   BOOT_POWER22_BAT_AVERDISCHGCUR_L5    		(0x7D)
#define   BOOT_POWER22_APS_AVERVOL_H8          		(0x7E)
#define   BOOT_POWER22_APS_AVERVOL_L4          		(0x7F)
#define   BOOT_POWER22_BAT_CHGCOULOMB3         		(0xB0)
#define   BOOT_POWER22_BAT_CHGCOULOMB2         		(0xB1)
#define   BOOT_POWER22_BAT_CHGCOULOMB1         		(0xB2)
#define   BOOT_POWER22_BAT_CHGCOULOMB0         		(0xB3)
#define   BOOT_POWER22_BAT_DISCHGCOULOMB3      		(0xB4)
#define   BOOT_POWER22_BAT_DISCHGCOULOMB2      		(0xB5)
#define   BOOT_POWER22_BAT_DISCHGCOULOMB1      		(0xB6)
#define   BOOT_POWER22_BAT_DISCHGCOULOMB0      		(0xB7)
#define   BOOT_POWER22_BAT_POWERH8             		(0x70)
#define   BOOT_POWER22_BAT_POWERM8             		(0x71)
#define   BOOT_POWER22_BAT_POWERL8             		(0x72)


#endif /* __AXP221_REGS_H__ */
