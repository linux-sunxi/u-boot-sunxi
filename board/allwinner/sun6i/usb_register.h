/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm boot sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : usb_registers.h
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#ifndef   __USB_REGISTERS_H__
#define   __USB_REGISTERS_H__

//Common Register
#define USB_GCS_OFFSET				0x40
#define USB_EPINTF_OFFSET			0x44
#define USB_EPINTE_OFFSET			0x48
#define USB_BUSINTF_OFFSET			0x4C
#define USB_BUSINTE_OFFSET			0x50
#define USB_FNUM_OFFSET				0x54
#define USB_TESTC_OFFSET			0x7C

//Endpoint Indexed Register
#define USB_CSR0_OFFSET				0x80
#define USB_COUNT0_OFFSET			0x88
#define USB_ATTR0_OFFSET			0x8C
#define USB_FADDR_OFFSET			0x98

#define USB_TXCSR_OFFSET			0x80
#define USB_RXCSR_OFFSET			0x84
#define USB_RXCOUNT_OFFSET			0x88
#define USB_EPATTR_OFFSET			0x8C
#define USB_TXFIFO_OFFSET			0x90
#define USB_RXFIFO_OFFSET			0x94
#define USB_TXFADDR_OFFSET			0x98
#define USB_RXFADDR_OFFSET			0x9C

//Configuration Register
#define USB_CONFIGINFO_OFFSET		0xC0
#define USB_LINKTIM_OFFSET			0xC4
#define USB_OTGTIM_OFFSET			0xC8

#define USB_DPBDIS_OFFSET			0xD0
#define USB_CHIRP_RESMUE_OFFSET		0xD4
#define USB_HSPKT_TIMEOUT_OFFSET	0xD8

#define USB_LPMCTRL_OFFSET			0xE0
#define USB_LPMINT_OFFSET			0xE4

//PHY&Interface Control and Status Registers
#define USB_ISCR_OFFSET				0x400
#define USB_PHYCTL_OFFSET			0x404
#define USB_PHYBIST_OFFSET			0x408




#define  USB_bFADDR_OFF       		(0x98)
#define  USB_bPOWER_OFF       		(0x40)
#define  USB_hINTRTX_OFF      		(0x44)
#define  USB_hINTRRX_OFF      		(0x46)
#define  USB_hINTRTXE_OFF     		(0x48)
#define  USB_hINTRRXE_OFF     		(0x4A)
#define  USB_bINTRUSB_OFF     		(0x4C)
#define  USB_bINTRUSBE_OFF    		(0x50)
#define  USB_hFRAME_OFF       		(0x54)
#define  USB_bINDEX_OFF       		(0x42)
#define  USB_bTESTMODE_OFF    		(0x7C)

#define  USB_hTXMAXP_OFF      		(0x80)
#define  USB_hCSR0_OFF        		(0x82)
#define  USB_hTXCSR_OFF       		(0x82)
#define  USB_hRXMAXP_OFF      		(0x84)
#define  USB_hRXCSR_OFF       		(0x86)
#define  USB_hCOUNT0_OFF      		(0x88)
#define  USB_hRXCOUNT_OFF     		(0x88)
#define  USB_hRXPKTCNT_OFF  	 	(0x8A)

#define  USB_bTXTYPE_OFF      		(0x8C)
#define  USB_bTXINTERVAL_OFF  		(0x8D)
#define  USB_bRXTYPE_OFF      		(0x8E)
#define  USB_bRXINTERVAL_OFF  		(0x8F)

#define  USB_bFIFO0_OFF       		(0x00)
#define  USB_bFIFO1_OFF       		(0x04)
#define  USB_bFIFO2_OFF       		(0x08)
#define  USB_bFIFO3_OFF       		(0x0C)
#define  USB_bFIFO4_OFF       		(0x10)
#define  USB_bFIFO5_OFF       		(0x14)
#define  USB_bFIFO_OFF(n)     		(((n)<<2))

#define  USB_bDEVCTL_OFF      		(0x41)

#define  USB_bTxFIFOsz_OFF    		(0x90)
#define  USB_bRxFIFOsz_OFF    		(0x94)
#define  USB_hTxFIFOad_OFF    		(0x92)
#define  USB_hRxFIFOad_OFF    		(0x96)

#define  ULPI_bVBUSCTL_OFF    		(0x70)
#define  ULPI_bREGDATA_OFF    		(0x74)
#define  ULPI_bREGADDR_OFF    		(0x75)
#define  ULPI_bREGCTL_OFF     		(0x76)

#define USB_bDMACTL_OFF       		(0x43)
#define USB_bVEND1_OFF        		(0x7D)
//#define USB_bVEND2_OFF       		(0x7E)
#define USB_FSM_OFF        			(0x7E)

#define USB_bHSEOF_OFF        		(0xC4)
#define USB_bFSEOF_OFF        		(0xC5)
#define USB_bLSEOF_OFF        		(0xC6)

#define USB_bTYPE0_OFF        		(0x8C)
#define USB_bTXFADDR_OFF   	 		(0x98)
#define USB_bTXHADDR_OFF   	 		(0x9A)
#define USB_bTXHPORT_OFF   	 		(0x9B)
#define USB_bRXFADDR_OFF   	 		(0x9C)
#define USB_bRXHADDR_OFF   	 		(0x9E)
#define USB_bRXHPORT_OFF   	 		(0x9F)

#define USB_hRXDBDIS_OFF      		(0xD0)
#define USB_hTXDBDIS_OFF      		(0xD2)
#define USB_hCTUCH_OFF        		(0xD4)
#define USB_hCTHSRTN_OFF      		(0xD6)

#define USB_hLPMATTR_OFF      		(0xE0)
#define USB_bLPMCTRL_OFF      		(0xE2)
#define USB_bLPMINTRE_OFF     		(0xE5)
#define USB_bLPMINTR_OFF      		(0xE4)
#define USB_bLPMFADDR_OFF     		(0xE3)

#define  USB_bCORECONFIG_OFF		(0xC0)

#define  USB_wISCR_OFF    			(0x400)
#define  USB_wPHYCSR_OFF    		(0x404)
#define  USB_wPHYBIST_OFF    		(0x408)

#define USB_CSRX_REQPKT            	(0x1<<21)
#define USB_TXCSR_AUTOSET			(0x1<<15)
#define USB_TXCSR_ISO				(0x1<<14)
#define USB_TXCSR_TXFIFO			(0x1<<13)
#define USB_TXCSR_DMAREQEN			(0x1<<12)
#define USB_TXCSR_DMAREQMODE		(0x1<<10)
#define USB_TXCSR_RXSTALL			(0x1<<5)
#define USB_TXCSR_FLUSHFIFO			(0X1<<3)
#define USB_TXCSR_ERROR				(0x1<<2)
#define USB_TXCSR_FIFONOTEMP		(0x1<<1)
#define USB_TXCSR_TXPKTRDY			(0X1<<0)
#define USB_BUSINT_VBUSERROR		(0x1<<7)
#define USB_BUSINT_SESSREQ			(0x1<<6)
#define USB_BUSINT_SESSEND			(0x1<<5)
#define USB_BUSINT_DISCONN			(0x1<<5)
#define USB_BUSINT_CONNECT			(0x1<<4)
#define USB_BUSINT_SOF				(0x1<<3)
#define USB_BUSINT_RESET			(0x1<<2)
#define USB_BUSINT_BABBLE			(0x1<<2)
#define USB_BUSINT_RESUME			(0x1<<1)
#define USB_BUSINT_SUSPEND			(0x1<<0)

#define USB_RXCSR_AUTOCLR			(0x1<<15)
#define USB_RXCSR_AUTOREQ			(0x1<<14)  //for Host only
#define USB_RXCSR_ISO				(0x1<<14)  //for device only
#define USB_RXCSR_DMAREQEN			(0x1<<13)
#define USB_RXCSR_PIDERROR			(0x1<<12)  //for Host only
#define USB_RXCSR_DMAREQMODE		(0x1<<11)
#define USB_RXCSR_RXSTALL			(0x1<<6)   //for Host only
#define USB_RXCSR_REQPKT			(0x1<<5)   //for Host only
#define USB_RXCSR_FLUSHFIFO			(0X1<<4)
#define USB_RXCSR_ERROR				(0X1<<2)
#define USB_RXCSR_FIFOFULL			(0X1<<1)
#define USB_RXCSR_RXPKTRDY			(0X1<<0)


#endif  //__USB_REGISTERS_H__

