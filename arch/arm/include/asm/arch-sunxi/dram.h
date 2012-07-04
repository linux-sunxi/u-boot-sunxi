#ifndef _SUNXI_DRAM_H
#define _SUNXI_DRAM_H
#define PLL5_DDR_CLK	(24000000)
#define	uint32 unsigned
#define DRAM_MEM_BASE		(0x40000000)
#define HEAP_MEM_START		(0x48000000)	//heap start offset: 128MB
#define HEAP_MEM_SIZE		(0x18000000)	//384MB
#define DRAM_FREE_MEM_BASE	(0x40100000)	//free memory for test, size 7M

#ifdef CONFIG_SUN6I_FPGA
	//#define DDR2_32B
	//#define DDR3_32B
	//#define DDR2_FPGA_BJ530 //DDR2_32B 
	#define DDR2_FPGA_S2C
	//#define LPDDR2_FPGA_S2C_2CS_2CH
	#define MCTL_CLK			PLL5_DDR_CLK
	#define MCTL_ACCESS_MODE	1				//0: sequence mode, 1: interleave mode
	#define MCTL_CHANNEL_NUM	1
	#ifdef DDR2_FPGA_S2C
		#define MCTL_CS_NUM		2
	#else
		#define MCTL_CS_NUM		2
	#endif
#else
//#define DDR2_32B
#define DDR3_32B
//#define LPDDR2_32B
#define MCTL_CLK			PLL5_DDR_CLK
#define MCTL_ACCESS_MODE	1				//0: sequence mode, 1: interleave mode
#define MCTL_CHANNEL_NUM	2
#define MCTL_CS_NUM			1
#endif
extern uint32 mctl_init(void);
extern uint32 mctl_set_emrs(uint32 emrs_id, uint32 emrs_val);
extern uint32 mctl_scan_readpipe(uint32 clk);

extern void mctl_self_refresh_entry(uint32 channel_num);
extern void mctl_self_refresh_exit(uint32 channel_num);
extern void mctl_power_down_entry(void);
extern void mctl_power_down_exit(void);
extern void mctl_precharge_all(void);

extern void mctl_setup_ar_interval(uint32 clk);
extern void mctl_DLL_reset(void);
extern void mctl_DLL_enable(void);
extern void mctl_DLL_disable(void);
extern void mctl_hostport_control(uint32 enable);
extern uint32 mctl_init_dram(void);


extern void mctl_host_port_cfg(uint32 port_no, uint32 cfg);
extern void mctl_power_save_process(void);
extern uint32 mctl_power_up_process(void);
extern uint32 mctl_ahb_reset(void);



//DDR2_32B				DDR2_32B_128Mx8x4
//DDR3_32B				DDR3_32B_256Mx8x4



//*****************************************************************************
//DDR2 SDRAM(x32)
//*****************************************************************************
#ifdef DDR2_FPGA_S2C
//DDR2 128Mx8 (128M Byte), total 1GB
#define MCTL_DDR_TYPE			2				//1: DDR, 2: DDR2, 3: DDR3
#define MCTL_BANK_SIZE			8
#define MCTL_ROW_WIDTH			14
#define MCTL_BUS_WIDTH			32
#define MCTL_PAGE_SIZE			4				//unit in KByte for one rank
#endif

#ifdef DDR2_FPGA_S2C_2C
//DDR2 128Mx8 (128M Byte), total 512MB
#define MCTL_DDR_TYPE			2				//1: DDR, 2: DDR2, 3: DDR3
#define MCTL_BANK_SIZE			8
#define MCTL_ROW_WIDTH			14
#define MCTL_BUS_WIDTH			16
#define MCTL_PAGE_SIZE			2				//unit in KByte for one rank
#endif

#ifdef DDR2_32B
//DDR2 128Mx8 (128M Byte)
#define MCTL_DDR_TYPE			2				//1: DDR, 2: DDR2, 3: DDR3
#define MCTL_BANK_SIZE			8
#define MCTL_ROW_WIDTH			14
#define MCTL_BUS_WIDTH			32
#define MCTL_PAGE_SIZE			4				//unit in KByte for one rank
#endif

//*****************************************************************************
//DDR3 SDRAM(x32)
//*****************************************************************************
#ifdef DDR3_32B
//DDR3 256Mx8 (256M Byte)
#define MCTL_DDR_TYPE			3				//1: DDR, 2: DDR2, 3: DDR3
#define MCTL_BANK_SIZE			8
#define MCTL_ROW_WIDTH			15
#define MCTL_BUS_WIDTH			32
#define MCTL_PAGE_SIZE			4				//unit in KByte for one rank
#endif


//*****************************************************************************
//LPDDR2 SDRAM(x32)
//*****************************************************************************
#ifdef LPDDR2_FPGA_S2C_2CS_2CH
//LPDDR2 128Mx32 (512M Byte), total 2GB
#define MCTL_DDR_TYPE			6				//1: DDR, 2: DDR2, 3: DDR3
#define MCTL_BANK_SIZE			8
#define MCTL_ROW_WIDTH			14
#define MCTL_BUS_WIDTH			32
#define MCTL_PAGE_SIZE			4				//unit in KByte for one rank
#endif

#ifdef LPDDR2_32B
//LPDDR2 128Mx32
#define MCTL_DDR_TYPE			6				//1: DDR, 2: DDR2, 3: DDR3
#define MCTL_BANK_SIZE			8
#define MCTL_ROW_WIDTH			14
#define MCTL_BUS_WIDTH			32
#define MCTL_PAGE_SIZE			4				//unit in KByte for one rank
#endif




#if (MCTL_DDR_TYPE==2)		//DDR2-800

#define MCTL_TREFI			78
#define MCTL_TMRD			2
#define MCTL_TRFC			52
#define MCTL_TRP			6
#define MCTL_TPREA			1
#define MCTL_TRTW			2
#define MCTL_TAL			0
#define MCTL_TCL			6
#define MCTL_TCWL			5
#define MCTL_TRAS			18
#define MCTL_TRC			24
#define MCTL_TRCD			6
#define MCTL_TRRD			4
#define MCTL_TRTP			3
#define MCTL_TWR			6
#define MCTL_TWTR			4
#define MCTL_TEXSR			200
#define MCTL_TXP			2
#define MCTL_TXPDLL			6
#define MCTL_TZQCS			0
#define MCTL_TZQCSI			0
#define MCTL_TDQS			1
#define MCTL_TCKSRE			0
#define MCTL_TCKSRX			0
#define MCTL_TCKE			3
#define MCTL_TMOD			0
#define MCTL_TRSTL			0
#define MCTL_TZQCL			0
#define MCTL_TMRR			2
#define MCTL_TCKESR			2
#define MCTL_TDPD			0

#define MCTL_MR0			0xa62
#define MCTL_MR1			0x0
#define MCTL_MR2			0x0
#define MCTL_MR3			0x0


#elif(MCTL_DDR_TYPE==3)		//DDR3-1333
#define MCTL_TREFI			78
#define MCTL_TMRD			4
#define MCTL_TRFC			107
#define MCTL_TRP			9
#define MCTL_TPREA			0
#define MCTL_TRTW			2
#define MCTL_TAL			0
#define MCTL_TCL			9
#define MCTL_TCWL			8
#define MCTL_TRAS			24
#define MCTL_TRC			33
#define MCTL_TRCD			9
#define MCTL_TRRD			4
#define MCTL_TRTP			5
#define MCTL_TWR			10
#define MCTL_TWTR			5
#define MCTL_TEXSR			512
#define MCTL_TXP			5
#define MCTL_TXPDLL			16
#define MCTL_TZQCS			64
#define MCTL_TZQCSI			0
#define MCTL_TDQS			1
#define MCTL_TCKSRE			7
#define MCTL_TCKSRX			7
#define MCTL_TCKE			4
#define MCTL_TMOD			12
#define MCTL_TRSTL			80
#define MCTL_TZQCL			512
#define MCTL_TMRR			2
#define MCTL_TCKESR			5
#define MCTL_TDPD			0

#define MCTL_MR0			0x1a50
#define MCTL_MR1			0x0
#define MCTL_MR2			0x18
#define MCTL_MR3			0x0

#elif(MCTL_DDR_TYPE==5)		//LPDDR
#else						//LPDDR2-800
#define MCTL_TREFI			78
#define MCTL_TMRD			2
#define MCTL_TRFC			52
#define MCTL_TRP			8
#define MCTL_TPREA			0
#define MCTL_TRTW			2
#define MCTL_TAL			0
#define MCTL_TCL			6
#define MCTL_TCWL			4
#define MCTL_TRAS			18
#define MCTL_TRC			27
#define MCTL_TRCD			8
#define MCTL_TRRD			4
#define MCTL_TRTP			3
#define MCTL_TWR			6
#define MCTL_TWTR			3
#define MCTL_TEXSR			200
#define MCTL_TXP			3
#define MCTL_TXPDLL			6
#define MCTL_TZQCS			0
#define MCTL_TZQCSI			0
#define MCTL_TDQS			1
#define MCTL_TCKSRE			5
#define MCTL_TCKSRX			5
#define MCTL_TCKE			3
#define MCTL_TMOD			0
#define MCTL_TRSTL			0
#define MCTL_TZQCL			0
#define MCTL_TMRR			2
#define MCTL_TCKESR			6
#define MCTL_TDPD			0

#define MCTL_MR0			0x0
#define MCTL_MR1			0x92
#define MCTL_MR2			0x4
#define MCTL_MR3			0x2


#endif



#define MCTL_COM_BASE		0x01c62000
#define MCTL_CTL0			0x01c63000
#define MCTL_CTL1			0x01c64000
#define MCTL_PHY0			0x01c65000
#define MCTL_PHY1			0x01c66000

#define MCTL_CTL_BASE		0x01c63000
#define MCTL_PHY_BASE		0x01c65000
#define MCTL_CCU_BASE		0x01c20000


#define SDR_COM_CR				(MCTL_COM_BASE + 0x00)
#define SDR_COM_CCR				(MCTL_COM_BASE + 0x04)
#define SDR_COM_MFACR			(MCTL_COM_BASE + 0x10)
#define SDR_COM_MSACR			(MCTL_COM_BASE + 0x30)
#define SDR_COM_MBACR			(MCTL_COM_BASE + 0x50)

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


#define CCU_PLL5CFG				(MCTL_CCU_BASE + 0x20)
#define CCU_AXIGATE				(MCTL_CCU_BASE + 0x5c)
#define CCU_AHBGATE0			(MCTL_CCU_BASE + 0x60)
#define CCU_AHBGATE1			(MCTL_CCU_BASE + 0x64)
#define CCU_MDFS_CLK			(MCTL_CCU_BASE + 0xf0)
#define CCU_MDFS_CFG			(MCTL_CCU_BASE + 0xf4)
#define CCU_AHB1_RST			(MCTL_CCU_BASE + 0x2c0)

#define mctl_read_w(n)   		(*((volatile uint32 *)(n)))          
#define mctl_write_w(n,c) 		(*((volatile uint32 *)(n)) = (c))

extern uint32 mctl_sys_init(void);
extern uint32 mctl_reset_release(void);

#endif /*SUNXI_DRAM_H*/



