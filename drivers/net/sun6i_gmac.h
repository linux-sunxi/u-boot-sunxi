/*
 * sun6i_gmac.h: Allwinnertech Gigabit Ethernet u-boot driver
 * Copyright © 2006-2012, AllWinner Technology. Co., Ltd.
 *				All Rights Resvered                   
 *		Author: shuge <shuge@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, mail to service@allwinnertech.com.
 */

#ifndef __SUN6I_GMAC_H__
#define __SUN6I_GMAC_H__

#define DMA_DESC_TX			0x40000000
#define DMA_DESC_RX			(DMA_DESC_TX + sizeof(dma_desc_t))
#define TX_BUF1_ADDR		0x44A00000
#define TX_BUF2_ADDR		0x44A01000
#define RX_BUF1_ADDR		0x44A02000
#define RX_BUF2_ADDR		0x44A03000
#define MAX_BUF_LEN			0x000007FF

#define GMAC_BASE			0x01c30000	/* aw1633: 0x01c30000  aw1630: 0x01c50000 */
#define GMAC_CONTROL		(0x00) /* Configuration */
#define GMAC_FRAME_FILTER	(0x04) /* Frame Filter */
#define GMAC_HASH_HIGH		(0x08) /* Multicast Hash Table High */
#define GMAC_HASH_LOW		(0x0c) /* Multicast Hash Table Low */
#define GMAC_GMII_ADDR		(0x10) /* MII Address */
#define GMAC_GMII_DATA		(0x14) /* MII Data */
#define GMAC_FLOW_CTRL		(0x18) /* Flow Control */
#define GMAC_INT_STATUS		(0x38) /* Interrupt status register */
#define GMAC_INT_MASK		(0x3c) /* interrupt mask register */
#define GMAC_ADDR_HI(reg)	(0x40 + (reg<<3)) /* upper 16bits of MAC address */
#define GMAC_ADDR_LO(reg)	(0x44 + (reg<<3)) /* lower 32bits of MAC address */
#define GMAC_RGMII_STATUS	(0xD8) /* S/R-GMII status */

/* GMAC_CONTROL value */
#define GMAC_CTL_TC			0x01000000 /* Transmit Configuration in RGMII */
#define GMAC_CTL_WD			0x00800000 /* Watchdog Disable */
#define GMAC_CTL_JD			0x00400000 /* Jabber Disable */
#define GMAC_CTL_BE			0x00200000 /* Frame Burst Enable (only Half) */
#define GMAC_CTL_JE			0x00100000 /* Jumbo Frame Enable */
#define GMAC_CTL_IFG		0x000E0000 /* Inter-Frame Gap */
#define GMAC_CTL_DCRS		0x00010000 /* Disable Carrier Sense During Transmission (only Half) */
#define GMAC_CTL_PS			0x00008000 /* Port Select 0:GMII, 1:MII */
#define GMAC_CTL_FES		0x00004000 /* Indicates the speed in Fast Ethernet(MII) mode */
#define GMAC_CTL_ROD		0x00002000 /* Receive own disable (only half-duplex) */
#define GMAC_CTL_LM			0x00001000 /* Loopback mode */
#define GMAC_CTL_DM			0x00000800 /* Duplex mode */
#define GMAC_CTL_DR			0x00000200 /* Retry disable (only half-duplex) */
#define GMAC_CTL_LUD		0x00000100 /* Link Up/Down (only RGMII/SGMII) */
#define GMAC_CTL_ACS		0x00000080 /* Automatic Pad/CRC Stripping */
#define GMAC_CTL_BL			0x00000060 /* Back-off limit.(only half-duplex) */
#define GMAC_CTL_DC			0x00000010 /* Deferral Check.(only half-duplex) */
#define GMAC_CTL_TE			0x00000008 /* Transmit Enable */
#define GMAC_CTL_RE			0x00000004 /* Receiver Enalbe */


/* GMAC_GMII_ADDR value */
#define MII_BUSY			0x00000001
#define MII_WRITE			0x00000002
#define MII_PHY_MASK		0x0000FFC0
#define MII_CR_MASK			0x0000001C


#define GDMA_BASE			(GMAC_BASE + 0x1000) /* GDMA Base */
#define GDMA_BUS_MODE		(0x00) /* Bus Mode Register */
#define GDMA_XMT_POLL		(0x04) /* Transmit Poll Demand */
#define GDMA_RCV_POLL		(0x08) /* Received Poll Demand */
#define GDMA_RCV_LIST		(0x0C) /* Receive List Base */
#define GDMA_XMT_LIST		(0x10) /* Transmit List Base */
#define GDMA_STATUS			(0x14) /* Status Register */
#define GDMA_OP_MODE		(0x18) /* DMA Operational Mode */
#define GDMA_INTR_ENA		(0x1c) /* Interrupt Enable */
#define GDMA_MISSED_FRAME	(0x20) /* Missed Frame and Buffer Overflow Counter */
#define GDMA_CUR_TX_DESC	(0x48) /* Current Host Transmit Descriptor */
#define GDMA_CUR_RX_DESC	(0x4C) /* Current Host Received Descriptor */
#define GDMA_CUR_TX_BUF		(0x50) /* Current Host Transmit Buffer Address */
#define GDMA_CUR_RX_BUF		(0x54) /* Current Host Received Buffer Address */

/*	GDMA_BUS_MODE value */
#define SOFT_RESET			0x00000001 /* Software reset gdma */
#define BUS_MODE_DA			0x00000002 /* DMA Arbitration */
#define BUS_ADDR_ALIGN		0x02000000 /* Address-Aligned Beats */
#define BUS_MODE_4PBL		0x01000000 /* 4xPBL Mode */
#define BUS_MODE_USP		0x00800000
#define BUS_MODE_RXPBL		0x007E0000
#define BUS_MODE_PBL		0x00003F00
#define BUS_MODE_FIXBUST	0x00010000
#define BUS_MODE_DSL		0x0000007C /* Descriptor skip length */
#define BUS_MODE_RTPR		0x00000C00 /* Rx TX priority ratio */

/* GDMA_STATUS value */
#define STATUS_TBUS			0x00000004
#define STATUS_RBUS			0x00000080
#define STATUS_TPS			0x00700000
#define TPS_STOP			0x00000000
#define TPS_FETCH_DESC		0x00100000
#define TPS_WAIT_STAT		0x00200000
#define TPS_READ_DATA		0x00300000
#define TPS_SUSP			0x00600000
#define TPS_CLOSE_DESC		0x00700000

#define STATUS_RPS			0x000E0000
#define RPS_STOP			0x00000000
#define RPS_FETCH_DESC		0x00020000
#define RPS_WAIT_STAT		0x00060000
#define RPS_SUSP			0x00080000
#define RPS_CLOSE_DESC		0x000A0000
#define RPS_WRITE_HOST		0x000E0000

/* GDMA_OP_MODE value */
#define OP_MODE_RSF			0x02000000 /* Receive Store and Forward */
#define OP_MODE_DFF			0x01000000 /* Disable Flushing of Received Frames */
#define OP_MODE_RFA2		0x00800000 /* MSB of Threshold for Activating Flow Control */
#define OP_MODE_RFD2		0x00400000 /* MSB of Threshold for Deactivating Flow Control */
#define OP_MODE_TSF			0x00200000 /* Transmit Store and Forward */
#define OP_MODE_FTF			0x00100000 /* Flush Transmit FIFO */
#define OP_MODE_TTC			0x0001C000 /* Transmit Threshold Control */
#define OP_MODE_ST			0x00002000 /* Start/Stop Transmission Command */
#define OP_MODE_RFD			0x00001800 /* Threshold for deactivating flow control */
#define OP_MODE_RFA			0x00000600 /* Threshold for activating flow control */
#define OP_MODE_EFC			0x00000100 /* Enable HW flow control */
#define OP_MODE_FEF			0x00000080 /* Forward Error Frames */
#define OP_MODE_FUF			0x00000040 /* Forward Undersized Frames */
#define OP_MODE_RTC			0x00000018 /* Receive Threshold Control */
#define OP_MODE_OSF			0x00000004 /* Operate on Second Frame */
#define OP_MODE_SR			0x00000002 /* Start/Stop Receive */

/* pin configure for GMAC */
#define GPIO_BASE			0x01C20800
#define PA_CFG0				(GPIO_BASE + 0x00)
#define PA_CFG1				(GPIO_BASE + 0x04)
#define PA_CFG2				(GPIO_BASE + 0x08)
#define PA_CFG3				(GPIO_BASE + 0x0C)

/* Clk control */
#define CCMU_BASE			0x01c20000

#if GET_SYSCLK_SELF

#define PLL1_CFG			0x00
#define PLL6_CFG			0x28

#define AXI_CFG				(0x50)
#define AXI_SRC_SEL			0x00030000
#define AXI_SRC_LOSC		0x00000000
#define AXI_SRC_OSC24M		0x00010000
#define AXI_SRC_PLL1		0x00020000
#define AXI_CLK_DIV			0x00000007

#define AHB1_CFG			(0x54)
#define AHB1_SRC_SEL		0x00003000
#define AHB1_SRC_LOSC		0x00000000
#define AHB1_SRC_OSC24M		0x00001000
#define AHB1_SRC_AXI		0x00002000
#define AHB1_SRC_PLL6		0x00003000
#define AHB1_PER_DIV		0x000000C0
#define AHB1_CLK_DIV		0x00000030

#endif

#define AHB1_GATING			(0x60)
#define GMAC_AHB_BIT		0x00000200

/* PHY address */
#define PHY_ADDR			0x01
#define PHY_DM				0x0010
#define PHY_AUTO_NEG		0x0020
#define PHY_POWERDOWN		0x0080
#define PHY_NEG_EN			0x1000
#define PHY_

#define MII_CLK				0x00000008
/* bits 4 3 2 | AHB1 Clock     | MDC Clock
 * -------------------------------------------------------
 *      0 0 0 |	60 ~ 100 MHz   | div-42
 *      0 0 1 | 100 ~ 150 MHz  | div-62
 *      0 1 0 | 20 ~ 35 MHz	   | div-16
 *      0 1 1 | 35 ~ 60 MHz    | div-26
 *      1 0 0 | 150 ~ 250 MHz  | div-102
 *      1 0 1 | 250 ~ 300 MHz  | div-124
 *      1 1 x | Reserved       |
 */

/* Default tx descriptor */
#define TX_SINGLE_DESC0		0x80000000
#define TX_SINGLE_DESC1		0x63000000

/* Default rx descriptor */
#define RX_SINGLE_DESC0		0x80000000
#define RX_SINGLE_DESC1		0x83000000

#define GMAC_DEF_CONF		( \
								GMAC_CTL_WD | \
								GMAC_CTL_JD | \
								GMAC_CTL_JE | \
								GMAC_CTL_PS | \
								GMAC_CTL_DM | \
								GMAC_CTL_ACS | \
								GMAC_CTL_TE | \
								GMAC_CTL_RE | \
								((0<<17) & GMAC_CTL_IFG) \
							)

#define GDMA_OP_CONF		( \
								OP_MODE_RSF | \
								OP_MODE_DFF | \
								OP_MODE_TSF | \
								((0<<17) & OP_MODE_TTC) | \
								((0<<11) & OP_MODE_RFD) | \
								((0<<9) & OP_MODE_RFA) | \
								OP_MODE_EFC | \
								OP_MODE_FEF | \
								OP_MODE_FUF | \
								((0<<3) & OP_MODE_RSF) \
							)

enum rx_frame_status { /* IPC status */
	good_frame = 0,
	discard_frame = 1,
	csum_none = 2,
	llc_snap = 4,
};

typedef union {
	struct {
		/* TDES0 */
		u32 deferred:1;		/* Deferred bit (only half-duplex) */
		u32 under_err:1;	/* Underflow error */
		u32 ex_deferral:1;	/* Excessive deferral */
		u32 coll_cnt:4;		/* Collision count */
		u32 vlan_tag:1;		/* VLAN Frame */
		u32 ex_coll:1;		/* Excessive collision */
		u32 late_coll:1;	/* Late collision */
		u32 no_carr:1;		/* No carrier */
		u32 loss_carr:1;	/* Loss of collision */
		u32 ipdat_err:1;	/* IP payload error */
		u32 frm_flu:1;		/* Frame flushed */
		u32 jab_timeout:1;	/* Jabber timeout */
		u32 err_sum:1;		/* Error summary */
		u32 iphead_err:1;	/* IP header error */
		u32 ttss:1;			/* Transmit time stamp status */
		u32 reserved0:13;
		u32 own:1;			/* Own bit. CPU:0, DMA:1 */
	} tx;
	struct {
		/* RDES0 */
		u32 chsum_err:1;	/* Payload checksum error */
		u32 crc_err:1;		/* CRC error */
		u32 dribbling:1;	/* Dribble bit error */
		u32 mii_err:1;		/* Received error (bit3) */
		u32 recv_wt:1;		/* Received watchdog timeout */
		u32 frm_type:1;		/* Frame type */
		u32 late_coll:1;	/* Late Collision */
		u32	ipch_err:1;		/* IPv header checksum error (bit7) */
		u32 last_desc:1;	/* Laset descriptor */
		u32 first_desc:1;	/* First descriptor */
		u32 vlan_tag:1;		/* VLAN Tag */
		u32 over_err:1;		/* Overflow error (bit11) */
		u32 len_err:1;		/* Length error */
		u32 sou_filter:1;	/* Source address filter fail */
		u32 desc_err:1;		/* Descriptor error */
		u32 err_sum:1;		/* Error summary (bit15) */
		u32 frm_len:14;		/* Frame length */
		u32 des_filter:1;	/* Destination address filter fail */
		u32 own:1;			/* Own bit. CPU:0, DMA:1 */
	#define RX_PKT_OK		0x7FFFB77C
	#define RX_LEN			0x3FFF0000
	} rx;
	u32 all;
} desc0_u;

typedef union {
	struct {
		/* TDES1 */
		u32 buf1_size:11;	/* Transmit buffer1 size */
		u32 buf2_size:11;	/* Transmit buffer2 size */
		u32 ttse:1;			/* Transmit time stamp enable */
		u32 dis_pad:1;		/* Disable pad (bit23) */
		u32 adr_chain:1;	/* Second address chained */
		u32 end_ring:1;		/* Transmit end of ring */
		u32 crc_dis:1;		/* Disable CRC */
		u32 cic:2;			/* Checksum insertion control (bit27:28) */
		u32 first_sg:1;		/* First Segment */
		u32 last_seg:1;		/* Last Segment */
		u32 interrupt:1;	/* Interrupt on completion */
	} tx;
	struct {
		/* RDES1 */
		u32 buf1_size:11;	/* Received buffer1 size */
		u32 buf2_size:11;	/* Received buffer2 size */
		u32 reserved1:2;
		u32 adr_chain:1;	/* Second address chained */
		u32 end_ring:1;		/* Received end of ring */
		u32 reserved2:5;
		u32 dis_ic:1;		/* Disable interrupt on completion */
	} rx;
	u32 all;
} desc1_u;

enum csum_insertion{
	cic_dis		= 0, 
	cic_ip		= 1,
	cic_no_pse	= 2,
	cic_full	= 3,
};

typedef struct dma_desc {
	desc0_u desc0;
	desc1_u desc1;
	u32 *desc2;
	u32	*desc3;
} __attribute__((packed)) dma_desc_t;

int gmac_initialize(bd_t * bis);

#endif /*_GMAC_REG_H_*/
