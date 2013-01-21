//*****************************************************************************
//	Allwinner Technology, All Right Reserved. 2006-2010 Copyright (c)
//
//	File: 				mctl_reg.h
//
//	Description:  This file implements basic functions for AW1633 DRAM controller
//
//	History:
//              2012/02/06      Berg Xing       0.10    Initial version
//              2012/02/24      Berg Xing       0.20    Support 2 channel
//              2012/02/27      Berg Xing       0.30    modify mode register access
//				2012/03/01		Berg Xing       0.40    add LPDDR2
//				2012/03/10		Berg Xing       0.50    add mctl_dll_init() function
//				2012/04/26		Berg Xing       0.60    add deep sleep
//				2012/06/19		Berg Xing       0.70    add 2T mode
//				2012/11/07		CPL				0.80	FPGA version based on berg's code
//				2012/11/14		CPL				0.90	add SID and regulate the parameters order
//				2012/11/21		CPL				0.91	modify parameters error
//				2012/11/25		CPL				0.92	modify for IC test
//				2012/11/27		CPL				0.93	add master configuration
//				2012/11/28		CPL				0.94	modify for boot and burn interface compatible
//				2012/11/29		CPL				0.95	modify lock parameters configuration
//				2012/12/3		CPL				0.96	add dll&pll delay and simple test
//*****************************************************************************

#ifndef   _MCTL_REG_H   
#define   _MCTL_REG_H

//DRAMC base address definition
#define MCTL_COM_BASE			0x01c62000
#define MCTL_CTL_BASE			0x01c63000
#define MCTL_PHY_BASE			0x01c65000
#define MCTL_RAM_BASE			0x01c64000

//#define MCTL_CTL0				0x01c63000
//#define MCTL_CTL1				0x01c64000
//#define MCTL_PHY0				0x01c65000
//#define MCTL_PHY1				0x01c66000

#define SDR_COM_CR				(MCTL_COM_BASE + 0x00)
#define SDR_COM_CCR				(MCTL_COM_BASE + 0x04)
#define SDR_COM_DBGCR			(MCTL_COM_BASE + 0x08)
#define SDR_COM_DBGCR1			(MCTL_COM_BASE + 0x0c)
#define SDR_COM_RMCR			(MCTL_COM_BASE + 0x10)
#define SDR_COM_MMCR			(MCTL_COM_BASE + 0x30)
#define SDR_COM_MBAGCR			(MCTL_COM_BASE + 0x70)
#define SDR_COM_MBACR			(MCTL_COM_BASE + 0x74)
#define SDR_COM_MAER			(MCTL_COM_BASE + 0x88)
#define SDR_COM_MDFSCR			(MCTL_COM_BASE + 0x100)
#define SDR_COM_MDFSMER			(MCTL_COM_BASE + 0x104)
#define SDR_COM_MDFSMRMR		(MCTL_COM_BASE + 0x108)
#define SDR_COM_MDFSTR0			(MCTL_COM_BASE + 0x10c)
#define SDR_COM_MDFSTR1			(MCTL_COM_BASE + 0x110)
#define SDR_COM_MDFSTR2			(MCTL_COM_BASE + 0x114)
#define SDR_COM_MDFSTR3			(MCTL_COM_BASE + 0x118)
#define SDR_COM_MDFSGCR			(MCTL_COM_BASE + 0x11c)
#define SDR_COM_MDFSIVR			(MCTL_COM_BASE + 0x13c)
#define SDR_COM_MDFSTCR			(MCTL_COM_BASE + 0x14c)

#define SDR_SCTL				(MCTL_CTL_BASE + 0x04)  
#define SDR_SSTAT				(MCTL_CTL_BASE + 0x08)
#define SDR_MCMD				(MCTL_CTL_BASE + 0x40)  
#define SDR_CMDSTAT				(MCTL_CTL_BASE + 0x4c)  
#define SDR_CMDSTATEN			(MCTL_CTL_BASE + 0x50)
#define SDR_MRRCFG0				(MCTL_CTL_BASE + 0x60)
#define SDR_MRRSTAT0			(MCTL_CTL_BASE + 0x64)
#define SDR_MRRSTAT1			(MCTL_CTL_BASE + 0x68)
#define SDR_MCFG1				(MCTL_CTL_BASE + 0x7c)
#define SDR_MCFG				(MCTL_CTL_BASE + 0x80)
#define SDR_PPCFG				(MCTL_CTL_BASE + 0x84)
#define SDR_MSTAT				(MCTL_CTL_BASE + 0x88)
#define SDR_LP2ZQCFG			(MCTL_CTL_BASE + 0x8c)
#define SDR_DTUSTAT				(MCTL_CTL_BASE + 0x94)
#define SDR_DTUNA				(MCTL_CTL_BASE + 0x98)
#define SDR_DTUNE				(MCTL_CTL_BASE + 0x9c)
#define SDR_DTUPRD0				(MCTL_CTL_BASE + 0xa0)
#define SDR_DTUPRD1				(MCTL_CTL_BASE + 0xa4)
#define SDR_DTUPRD2				(MCTL_CTL_BASE + 0xa8)
#define SDR_DTUPRD3				(MCTL_CTL_BASE + 0xac)
#define SDR_DTUAWDT				(MCTL_CTL_BASE + 0xb0)
#define SDR_TOGCNT1U			(MCTL_CTL_BASE + 0xc0)
#define SDR_TOGCNT100N			(MCTL_CTL_BASE + 0xcc)
#define SDR_TREFI				(MCTL_CTL_BASE + 0xd0)
#define SDR_TMRD				(MCTL_CTL_BASE + 0xd4)
#define SDR_TRFC				(MCTL_CTL_BASE + 0xd8)
#define SDR_TRP					(MCTL_CTL_BASE + 0xdc)
#define SDR_TRTW				(MCTL_CTL_BASE + 0xe0)
#define SDR_TAL					(MCTL_CTL_BASE + 0xe4)
#define SDR_TCL					(MCTL_CTL_BASE + 0xe8)
#define SDR_TCWL				(MCTL_CTL_BASE + 0xec)
#define SDR_TRAS				(MCTL_CTL_BASE + 0xf0)
#define SDR_TRC					(MCTL_CTL_BASE + 0xf4)
#define SDR_TRCD				(MCTL_CTL_BASE + 0xf8)
#define SDR_TRRD				(MCTL_CTL_BASE + 0xfc)
#define SDR_TRTP				(MCTL_CTL_BASE + 0x100)
#define SDR_TWR					(MCTL_CTL_BASE + 0x104)
#define SDR_TWTR				(MCTL_CTL_BASE + 0x108)
#define SDR_TEXSR				(MCTL_CTL_BASE + 0x10c)
#define SDR_TXP					(MCTL_CTL_BASE + 0x110)
#define SDR_TXPDLL				(MCTL_CTL_BASE + 0x114)
#define SDR_TZQCS				(MCTL_CTL_BASE + 0x118)
#define SDR_TZQCSI				(MCTL_CTL_BASE + 0x11c)
#define SDR_TDQS  				(MCTL_CTL_BASE + 0x120)
#define SDR_TCKSRE 				(MCTL_CTL_BASE + 0x124)
#define SDR_TCKSRX 				(MCTL_CTL_BASE + 0x128)
#define SDR_TCKE 				(MCTL_CTL_BASE + 0x12c)
#define SDR_TMOD 				(MCTL_CTL_BASE + 0x130)
#define SDR_TRSTL 				(MCTL_CTL_BASE + 0x134)
#define SDR_TZQCL 				(MCTL_CTL_BASE + 0x138)
#define SDR_TMRR 				(MCTL_CTL_BASE + 0x13c)
#define SDR_TCKESR 				(MCTL_CTL_BASE + 0x140)
#define SDR_TDPD 				(MCTL_CTL_BASE + 0x144)
#define SDR_DTUWACTL			(MCTL_CTL_BASE + 0x200)
#define SDR_DTURACTL			(MCTL_CTL_BASE + 0x204)
#define SDR_DTUCFG				(MCTL_CTL_BASE + 0x208)
#define SDR_DTUECTL				(MCTL_CTL_BASE + 0x20c)
#define SDR_DTUWD0				(MCTL_CTL_BASE + 0x210)
#define SDR_DTUWD1				(MCTL_CTL_BASE + 0x214)
#define SDR_DTUWD2				(MCTL_CTL_BASE + 0x218)
#define SDR_DTUWD3				(MCTL_CTL_BASE + 0x21c)
#define SDR_DTUWDM				(MCTL_CTL_BASE + 0x220)
#define SDR_DTURD0				(MCTL_CTL_BASE + 0x224)
#define SDR_DTURD1				(MCTL_CTL_BASE + 0x224)
#define SDR_DTURD2				(MCTL_CTL_BASE + 0x22c)
#define SDR_DTURD3				(MCTL_CTL_BASE + 0x230)
#define SDR_DTULFSRWD 		    (MCTL_CTL_BASE + 0x234)
#define SDR_DTULFSRRD 		    (MCTL_CTL_BASE + 0x238)
#define SDR_DTUEAF	 		    (MCTL_CTL_BASE + 0x23c)
#define SDR_DFITCTLDLY 		    (MCTL_CTL_BASE + 0x240)
#define SDR_DFIODTCFG 		    (MCTL_CTL_BASE + 0x244)
#define SDR_DFIODTCFG1 		    (MCTL_CTL_BASE + 0x248)
#define SDR_DFIODTRMAP 		    (MCTL_CTL_BASE + 0x24c)
#define SDR_DFITPHYWRD 		    (MCTL_CTL_BASE + 0x250)
#define SDR_DFITPHYWRL 		    (MCTL_CTL_BASE + 0x254)
#define SDR_DFITRDDEN 		    (MCTL_CTL_BASE + 0x260)
#define SDR_DFITPHYRDL 		    (MCTL_CTL_BASE + 0x264)
#define SDR_DFITPHYUPDTYPE0	    (MCTL_CTL_BASE + 0x270)
#define SDR_DFITPHYUPDTYPE1	    (MCTL_CTL_BASE + 0x274)
#define SDR_DFITPHYUPDTYPE2	    (MCTL_CTL_BASE + 0x278)
#define SDR_DFITPHYUPDTYPE3	    (MCTL_CTL_BASE + 0x27c)

#define SDR_DFITCTRLUPDMIN	    (MCTL_CTL_BASE + 0x280)
#define SDR_DFITCTRLUPDMAX	    (MCTL_CTL_BASE + 0x284)
#define SDR_DFITCTRLUPDDLY	    (MCTL_CTL_BASE + 0x288)
#define SDR_DFIUPDCFG		    (MCTL_CTL_BASE + 0x290)
#define SDR_DFITREFMSKI		    (MCTL_CTL_BASE + 0x294)
#define SDR_DFITCRLUPDI		    (MCTL_CTL_BASE + 0x298)
#define SDR_DFITRCFG0		    (MCTL_CTL_BASE + 0x2ac)
#define SDR_DFITRSTAT0		    (MCTL_CTL_BASE + 0x2b0)
#define SDR_DFITRWRLVLEN	    (MCTL_CTL_BASE + 0x2b4)
#define SDR_DFITRRDLVLEN	    (MCTL_CTL_BASE + 0x2b8)
#define SDR_DFITRRDLVLGATEEN    (MCTL_CTL_BASE + 0x2bc)

#define SDR_DFISTCFG0		    (MCTL_CTL_BASE + 0x2c4)
#define SDR_DFISTCFG1		    (MCTL_CTL_BASE + 0x2c8)
#define SDR_DFITDRAMCLKEN	    (MCTL_CTL_BASE + 0x2d0)
#define SDR_DFITDRAMCLKDIS	    (MCTL_CTL_BASE + 0x2d4)
#define SDR_DFILPCFG0		    (MCTL_CTL_BASE + 0x2f0)

#define SDR_PIR					(MCTL_PHY_BASE + 0x04)  
#define SDR_PGCR				(MCTL_PHY_BASE + 0x08)
#define SDR_PGSR				(MCTL_PHY_BASE + 0x0c)
#define SDR_DLLGCR				(MCTL_PHY_BASE + 0x10)
#define SDR_ACDLLCR				(MCTL_PHY_BASE + 0x14)
#define SDR_PTR0				(MCTL_PHY_BASE + 0x18)
#define SDR_PTR1				(MCTL_PHY_BASE + 0x1c)
#define SDR_PTR2				(MCTL_PHY_BASE + 0x20)
#define SDR_ACIOCR				(MCTL_PHY_BASE + 0x24)
#define SDR_DXCCR				(MCTL_PHY_BASE + 0x28)
#define SDR_DSGCR				(MCTL_PHY_BASE + 0x2c)
#define SDR_DCR					(MCTL_PHY_BASE + 0x30)
#define SDR_DTPR0				(MCTL_PHY_BASE + 0x34)
#define SDR_DTPR1				(MCTL_PHY_BASE + 0x38)
#define SDR_DTPR2				(MCTL_PHY_BASE + 0x3c)
#define SDR_MR0					(MCTL_PHY_BASE + 0x40)
#define SDR_MR1					(MCTL_PHY_BASE + 0x44)
#define SDR_MR2					(MCTL_PHY_BASE + 0x48)
#define SDR_MR3					(MCTL_PHY_BASE + 0x4c)
#define SDR_ODTCR				(MCTL_PHY_BASE + 0x50)
#define SDR_DTAR				(MCTL_PHY_BASE + 0x54)
#define SDR_DTDT0				(MCTL_PHY_BASE + 0x58)
#define SDR_DTDT1				(MCTL_PHY_BASE + 0x5c)
#define SDR_DCUAR				(MCTL_PHY_BASE + 0xc0)
#define SDR_DCUDR				(MCTL_PHY_BASE + 0xc4)
#define SDR_DCURR				(MCTL_PHY_BASE + 0xc8)
#define SDR_DCULR				(MCTL_PHY_BASE + 0xcc)
#define SDR_DCUGCR				(MCTL_PHY_BASE + 0xd0)
#define SDR_DCUTPR				(MCTL_PHY_BASE + 0xd4)
#define SDR_DCUSR0				(MCTL_PHY_BASE + 0xd8)
#define SDR_DCUSR1				(MCTL_PHY_BASE + 0xdc)
#define SDR_BISTRR				(MCTL_PHY_BASE + 0x100)
#define SDR_BISTMSKR0			(MCTL_PHY_BASE + 0x104)
#define SDR_BISTMSKR1			(MCTL_PHY_BASE + 0x108)
#define SDR_BISTWCR 			(MCTL_PHY_BASE + 0x10c)
#define SDR_BISTLSR 			(MCTL_PHY_BASE + 0x110)
#define SDR_BISTAR0 			(MCTL_PHY_BASE + 0x114)
#define SDR_BISTAR1 			(MCTL_PHY_BASE + 0x118)
#define SDR_BISTAR2 			(MCTL_PHY_BASE + 0x11c)
#define SDR_BISTUDPR 			(MCTL_PHY_BASE + 0x120)
#define SDR_BISTGSR 			(MCTL_PHY_BASE + 0x124)
#define SDR_BISTWER 			(MCTL_PHY_BASE + 0x128)
#define SDR_BISTBER0 			(MCTL_PHY_BASE + 0x12c)
#define SDR_BISTBER1 			(MCTL_PHY_BASE + 0x130)
#define SDR_BISTBER2 			(MCTL_PHY_BASE + 0x134)
#define SDR_BISTWCSR 			(MCTL_PHY_BASE + 0x138)
#define SDR_BISTFWR0 			(MCTL_PHY_BASE + 0x13c)
#define SDR_BISTFWR1 			(MCTL_PHY_BASE + 0x140)
#define SDR_ZQ0CR0	 			(MCTL_PHY_BASE + 0x180)
#define SDR_ZQ0CR1	 			(MCTL_PHY_BASE + 0x184)
#define SDR_ZQ0SR0	 			(MCTL_PHY_BASE + 0x188)
#define SDR_ZQ0SR1	 			(MCTL_PHY_BASE + 0x18c)
#define SDR_DX0GCR	 			(MCTL_PHY_BASE + 0x1c0)
#define SDR_DX0GSR0	 			(MCTL_PHY_BASE + 0x1c4)
#define SDR_DX0GSR1	 			(MCTL_PHY_BASE + 0x1c8)
#define SDR_DX0DLLCR 			(MCTL_PHY_BASE + 0x1cc)
#define SDR_DX0DQTR	 			(MCTL_PHY_BASE + 0x1d0)
#define SDR_DX0DQSTR 			(MCTL_PHY_BASE + 0x1d4)
#define SDR_DX1GCR	 			(MCTL_PHY_BASE + 0x200)
#define SDR_DX1GSR0	 			(MCTL_PHY_BASE + 0x204)
#define SDR_DX1GSR1	 			(MCTL_PHY_BASE + 0x208)
#define SDR_DX1DLLCR 			(MCTL_PHY_BASE + 0x20c)
#define SDR_DX1DQTR	 			(MCTL_PHY_BASE + 0x210)
#define SDR_DX1DQSTR 			(MCTL_PHY_BASE + 0x214)
#define SDR_DX2GCR	 			(MCTL_PHY_BASE + 0x240)
#define SDR_DX2GSR0	 			(MCTL_PHY_BASE + 0x244)
#define SDR_DX2GSR1	 			(MCTL_PHY_BASE + 0x248)
#define SDR_DX2DLLCR 			(MCTL_PHY_BASE + 0x24c)
#define SDR_DX2DQTR	 			(MCTL_PHY_BASE + 0x250)
#define SDR_DX2DQSTR 			(MCTL_PHY_BASE + 0x254)
#define SDR_DX3GCR	 			(MCTL_PHY_BASE + 0x280)
#define SDR_DX3GSR0	 			(MCTL_PHY_BASE + 0x284)
#define SDR_DX3GSR1	 			(MCTL_PHY_BASE + 0x288)
#define SDR_DX3DLLCR 			(MCTL_PHY_BASE + 0x28c)
#define SDR_DX3DQTR	 			(MCTL_PHY_BASE + 0x290)
#define SDR_DX3DQSTR 			(MCTL_PHY_BASE + 0x294)

#ifndef CCM_BASE
	#define CCM_BASE				(0x01c20000)
#endif

#ifndef CCM_PLL5_DDR_CTRL
	#define CCM_PLL5_DDR_CTRL  		(CCM_BASE+0x020)
#endif

#ifndef CCM_MDFS_CLK_CTRL
	#define CCM_MDFS_CLK_CTRL		(CCM_BASE+0x0f0)
#endif

#ifndef CCM_DRAMCLK_CFG_CTRL
	#define CCM_DRAMCLK_CFG_CTRL	(CCM_BASE+0x0f4)
#endif

#ifndef CCM_AHB1_RST_REG0
	#define CCM_AHB1_RST_REG0		(CCM_BASE+0x02C0)
#endif

#ifndef CCM_AHB1_GATE0_CTRL
	#define CCM_AHB1_GATE0_CTRL		(CCM_BASE+0x060)
#endif

#ifndef R_PRCM_BASE
	#define R_PRCM_BASE			(0x01f01400)
#endif

#ifndef R_VDD_SYS_PWROFF_GATE
	#define R_VDD_SYS_PWROFF_GATE	(R_PRCM_BASE + 0x110)
#endif

#ifndef CCM_AXI_GATE_CTRL
	#define CCM_AXI_GATE_CTRL		(CCM_BASE+0x05c)
#endif

#ifndef CCM_DRAM_GATING
	#define CCM_DRAM_GATING			(CCM_BASE+0x100)
#endif

#define mctl_read_w(n)   		(*((volatile unsigned int *)(n)))
#define mctl_write_w(n,c) 		(*((volatile unsigned int *)(n)) = (c))

#endif  //_MCTL_REG_H
