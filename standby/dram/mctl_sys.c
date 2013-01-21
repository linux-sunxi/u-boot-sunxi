//*****************************************************************************
//	Allwinner Technology, All Right Reserved. 2006-2010 Copyright (c)
//
//	File: 				mctl_sys.c
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
//				2012/12/6		CPL				0.97	add write odt enable function
//				2012/12/8		CPL				0.98	add read odt enable & adjust dll phase
//				2012/12/10		CPL				0.99	extend DLL & PLL delay
//				2012/12/22		CPL				0.991	disable master access when enter standby and enable when exit standby
//*****************************************************************************
#include "dram_i.h"
#include "mctl_reg.h"
#include "mctl_hal.h"

static __u32 ccm_dram_gating_reserved;

void mctl_self_refresh_entry(unsigned int ch_index)
{
	unsigned int reg_val;
	unsigned int ch_id;

  	//gating off the host access interface
  	ccm_dram_gating_reserved = mctl_read_w(CCM_DRAM_GATING);
  	mctl_write_w(CCM_DRAM_GATING, 0);

  	//master access disable
  	mctl_write_w(SDR_COM_MAER, 0);

	if(ch_index == 1)
		ch_id = 0x1000;
	else
		ch_id = 0x0;
	//set SLEEP command
	reg_val = 0x3;
	mctl_write_w(ch_id + SDR_SCTL, reg_val);
	//check whether in Low Power State
	while(  (mctl_read_w(ch_id + SDR_SSTAT)&0x7) != 0x5 ) {};

	//put PAD into power down state
	reg_val = mctl_read_w(ch_id + SDR_ACIOCR);
	reg_val |= (0x1<<3)|(0x1<<8)|(0x3<<18);
	mctl_write_w(ch_id + SDR_ACIOCR, reg_val);

	reg_val = mctl_read_w(ch_id + SDR_DXCCR);
	reg_val |= (0x1<<2)|(0x1<<3);
	mctl_write_w(ch_id + SDR_DXCCR, reg_val);

	reg_val = mctl_read_w(ch_id + SDR_DSGCR);
	reg_val &= ~(0x1<<28);
	mctl_write_w(ch_id + SDR_DSGCR, reg_val);

}

void mctl_self_refresh_exit(unsigned int ch_index)
{
	unsigned int reg_val;
	unsigned int ch_id;

	if(ch_index == 1)
		ch_id = 0x1000;
	else
		ch_id = 0x0;

	reg_val = mctl_read_w(ch_id + SDR_DSGCR);
	reg_val |= (0x1<<28);
	mctl_write_w(ch_id + SDR_DSGCR, reg_val);

	reg_val = mctl_read_w(ch_id + SDR_DXCCR);
	reg_val &= ~((0x1<<2)|(0x1<<3));
	mctl_write_w(ch_id + SDR_DXCCR, reg_val);

	//put PAD into power down state
	reg_val = mctl_read_w(ch_id + SDR_ACIOCR);
	reg_val &= ~((0x1<<3)|(0x1<<8)|(0x3<<18));
	mctl_write_w(ch_id + SDR_ACIOCR, reg_val);

	//set WAKEUP command
	reg_val = 0x4;
	mctl_write_w(ch_id + SDR_SCTL, reg_val);

	//check whether in Active State
	while(  (mctl_read_w(ch_id + SDR_SSTAT)&0x7) != 0x3 ) {};

	//Enable the DRAM master access
	mctl_write_w(SDR_COM_MAER, 0xFFFFFFFF);

	//ccmu dram gating bit return
  	mctl_write_w(CCM_DRAM_GATING, ccm_dram_gating_reserved);
}

void mctl_deep_sleep_entry(void)
{
	unsigned int reg_val;

	//put external DRAM into sleep state
	mctl_self_refresh_entry(0);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_self_refresh_entry(1);
	}


	//hold PAD
	reg_val = mctl_read_w(R_VDD_SYS_PWROFF_GATE);
	reg_val |= (0x1<<1);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
		reg_val |=0x1;
	mctl_write_w(R_VDD_SYS_PWROFF_GATE, reg_val);

	standby_serial_putc('q');
	standby_serial_putc('\n');
//
//	mctl_write_w(SDR_COM_MAER, 0);
//	mctl_write_w(CCM_AHB1_RST_REG0, (0x1<<14));
//	mctl_write_w(CCM_AHB1_RST_REG0+0x4, 0);
//	mctl_write_w(CCM_AHB1_RST_REG0+0x8, 0);


//  	reg_val = mctl_read_w(SDR_PIR);
// 	reg_val |= (0x1U<<4);
// 	mctl_write_w(SDR_PIR, reg_val);
//
//  	reg_val = mctl_read_w(SDR_PIR+0x1000);
// 	reg_val |= (0x1U<<4);
// 	mctl_write_w(SDR_PIR+0x1000, reg_val);

	//turn off SCLK
 	reg_val = mctl_read_w(SDR_COM_CCR);
 	reg_val &= ~(0x7<<0);
 	mctl_write_w(SDR_COM_CCR, reg_val);

// 	reg_val = mctl_read_w(SDR_COM_CCR);
// 	reg_val |= (0x18<<0);
// 	mctl_write_w(SDR_COM_CCR, reg_val);

 	//gate off DRAMC AHB clk
 	reg_val = mctl_read_w(CCM_AHB1_GATE0_CTRL);
 	reg_val &=~(0x1<<14);
 	mctl_write_w(CCM_AHB1_GATE0_CTRL, reg_val);

 	//gate off DRAMC MDFS clk
 	reg_val = mctl_read_w(CCM_MDFS_CLK_CTRL);
 	reg_val &= ~(0x1U<<31);
 	mctl_write_w(CCM_MDFS_CLK_CTRL, reg_val);

//	standby_serial_putc('2');
//
// 	//put DRAMC AHB register circuit on reset state
// 	reg_val = mctl_read_w(CCM_AHB1_RST_REG0);
// 	standby_serial_putc('5');
// 	reg_val &= ~(0x1<<14);
// 	mctl_write_w(CCM_AHB1_RST_REG0, reg_val);
//
//	standby_serial_putc('3');
//
// 	//put DRAMC other circuit on reset state
// 	reg_val = mctl_read_w(CCM_DRAMCLK_CFG_CTRL);
// 	reg_val &= ~(0x1U<<31);
// 	mctl_write_w(CCM_DRAMCLK_CFG_CTRL, reg_val);
//
//	standby_serial_putc('4');
//
//
//	standby_serial_putc('5');
//
//
	standby_serial_putc('6');

//
// 	//DRAMC PLL off
// 	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
// 	reg_val &=~(0x1U<<31);
// 	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);
//
// 	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
// 	reg_val |= 0x1<<20;
// 	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);
//
// 	while(mctl_read_w(CCM_PLL5_DDR_CTRL) & (0x1<<20)){}
}

void mctl_deep_sleep_exit(boot_dram_para_t *para)
{
	unsigned int reg_val;
//
//	standby_serial_putc('b');
//	standby_serial_putc('\n');
//
//	//config PLL5 DRAM CLOCK: PLL5 = (24*N*K)/M
//	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
//	reg_val &= ~((0x3<<0) | (0x3<<4) | (0x1F<<8));
//	reg_val |= ((0x1<<0) | (0x1<<4));	//K = M = 2;
//	reg_val |= ((para->dram_clk/24-1)<<0x8);//N
//	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);
//
//  	//PLL5 enable
//	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
//  	reg_val |= 0x1U<<31;
//  	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);
//
//  	//PLL5 configuration update(validate PLL5)
//  	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
//  	reg_val |= 0x1U<<20;
//  	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);
//
//  	while(mctl_read_w(CCM_PLL5_DDR_CTRL) & (0x1<<20)){
//  	standby_serial_putc('d');
//  	}
//
//  	while(!(mctl_read_w(CCM_PLL5_DDR_CTRL) & (0x1<<28))){
//  	standby_serial_putc('e');
//  	}
//  	standby_serial_putc('\n');
//	aw_delay(0x1000000);
//
  	standby_serial_putc('c');
	standby_serial_putc('\n');
//

//	reg_val = mctl_read_w(SDR_COM_CCR);
// 	reg_val &= ~(0x18<<0);
// 	mctl_write_w(SDR_COM_CCR, reg_val);
//
//	standby_timer_delay(1);
//
// 	reg_val = mctl_read_w(SDR_PIR);
// 	reg_val &= ~(0x1U<<4);
// 	reg_val |= (0x1U<<0);
// 	mctl_write_w(SDR_PIR, reg_val);
//
//  	reg_val = mctl_read_w(SDR_PIR+0x1000);
// 	reg_val &= ~(0x1U<<4);
// 	reg_val |= (0x1U<<0);
// 	mctl_write_w(SDR_PIR+0x1000, reg_val);

 	//turn on DRAMC MDFS clk
 	reg_val = mctl_read_w(CCM_MDFS_CLK_CTRL);
 	reg_val |= (0x1U<<31);
 	mctl_write_w(CCM_MDFS_CLK_CTRL, reg_val);

	//turn on DRAMC AHB clk
 	reg_val = mctl_read_w(CCM_AHB1_GATE0_CTRL);
 	reg_val |= (0x1<<14);
 	mctl_write_w(CCM_AHB1_GATE0_CTRL, reg_val);

	//turn on SCLK
 	reg_val = mctl_read_w(SDR_COM_CCR);
 	reg_val |= (0x7<<0);
 	mctl_write_w(SDR_COM_CCR, reg_val);

	//close pad hold function
	reg_val = mctl_read_w(R_VDD_SYS_PWROFF_GATE);
	reg_val &= ~(0x1<<1);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
		reg_val &= ~(0x1);
	mctl_write_w(R_VDD_SYS_PWROFF_GATE, reg_val);

	standby_serial_putc('1');

	mctl_self_refresh_exit(0);

	standby_serial_putc('2');

	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_self_refresh_exit(1);
	}

  	standby_serial_putc('z');
  	standby_serial_putc('f');
	standby_serial_putc('\n');

//	init_DRAM(1, (void *)para);
//	standby_serial_putc('k');
//	standby_serial_putc('\n');
}

