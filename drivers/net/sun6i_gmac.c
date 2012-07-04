/*
 * sun6i_gmac.c: Allwinnertech Gigabit Ethernet u-boot driver
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

#include <linux/types.h>
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <net.h>
#include <malloc.h>
#include <miiphy.h>
#include <linux/mii.h>
#include <netdev.h>
#include <errno.h>
#include "sun6i_gmac.h"

dma_desc_t dma_desc_tx[1];
dma_desc_t dma_desc_rx[1];
static char rx_packet[2048];

#if GET_SYSCLK_SELF
static u32 get_pll1_clk(void)
{
	u32 pll1_cfg;
	u32 n, k, m;

	pll1_cfg = readl(CCMU_BASE + PLL1_CFG);
	n = (0x1f & (pll1_cfg >> 8)) + 1;
	k = (0x03 & (pll1_cfg >> 4)) + 1;
	k = (0x03 & (pll1_cfg >> 0)) + 1;

	return (24000000 * n * k) / m;
}

static u32 get_pll6_clk(void)
{
	u32 pll6_cfg;
	u32 n, k;

	pll6_cfg = readl(CCMU_BASE + PLL6_CFG);
	n = (0x1f & (pll6_cfg >> 8)) + 1;
	k = (0x03 & (pll6_cfg >> 4)) + 1;

	return (24000000 * n * k) >> 1;
}

static u32 get_axi_clk(void)
{
	u32 axi_cfg, clk_hz, axi_div;

	axi_cfg = readl(CCMU_BASE + AXI_CFG);
	axi_div = axi_cfg & AXI_CLK_DIV;
	switch(axi_cfg & AXI_SRC_SEL){
		case AXI_SRC_LOSC:
			clk_hz = 32000;
			break;
		case AXI_SRC_OSC24M:
			clk_hz = 24000000;
			break;
		default:
			clk_hz = get_pll1_clk();
			break;
	}
	axi_div < 0x03 ? (clk_hz/(axi_div +1)) : (clk_hz >> 2);
	return clk_hz;
}

static u32 clk_ahb1_get(void)
{
	u32 ahb_cfg, clk_hz, ahb_div, ahb_pre;

	ahb_cfg = readl(CCMU_BASE + AHB1_CFG);
	ahb_pre = (ahb_cfg & AHB1_PER_DIV) >> 6;
	ahb_div = (ahb_cfg & AHB1_CLK_DIV) >> 4;
	switch(ahb_cfg & AHB1_SRC_SEL){
	case SRC_LOSC:
			clk_hz = 32000;
			break;
	case SRC_OSC24M:
			clk_hz = 24000000;
			break;
	case SRC_AXI:
			clk_hz = get_axi_clk();
			break;
	case SRC_PLL6:
			clk_hz = get_pll6_clk() / (ahb_pre + 1);
			break;
	default:
			break;
	}
	return (clk_hz >>= ahb_div);
}
#endif
static void random_ether_addr(u8 *addr)
{
	int i;
	unsigned long long rand;

	for(i=0; i<6; i++){
		rand = get_timer(0) * 0xfedf4fd;
		rand = rand * 0xd263f967 + 0xea6f22ad8235;
		addr[i] = (uchar)(rand % 0x100);
	}

	addr[0] &= 0xfe;
	addr[0] |= 0x02;
}

static u16 gmac_phy_read(struct eth_device *dev, int phy_adr, int reg)
{
	int reg_val;

	reg_val = readl(dev->iobase + GMAC_GMII_ADDR);
	reg_val &= ~(MII_PHY_MASK | MII_WRITE);
	reg_val |= (((phy_adr << 11) | (reg <<6)) | MII_BUSY);

	while(readl(dev->iobase + GMAC_GMII_ADDR) & MII_BUSY);

	writel(reg_val, dev->iobase + GMAC_GMII_ADDR);
	while(readl(dev->iobase + GMAC_GMII_ADDR) & MII_BUSY);

	return (u16)readl(dev->iobase + GMAC_GMII_DATA);
}

static void gmac_phy_write(struct eth_device *dev, u8 phy_adr, u8 reg, u16 data)
{
	int reg_val;

	reg_val = readl(dev->iobase + GMAC_GMII_ADDR);
	reg_val &= ~(MII_PHY_MASK);
	reg_val |= (((phy_adr << 11) | (reg <<6)) | MII_WRITE | MII_BUSY);

	/* Wait MII operation is complete */
	while(readl(dev->iobase + GMAC_GMII_ADDR) & MII_BUSY);

	writel(data, dev->iobase + GMAC_GMII_DATA);
	writel(reg_val, dev->iobase + GMAC_GMII_ADDR);

	/* Until operation is complete and exiting */
	while(readl(dev->iobase + GMAC_GMII_ADDR) & MII_BUSY);
}

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
int gmac_miiphy_read (const char *devname, u8 phy_adr, u8 reg, u16 *value)
{
	struct eth_device *dev;

	dev = eth_get_dev_by_name(devname);
	*value = gmac_phy_read(dev, phy_adr, reg);
	return 0;
}

int gmac_miiphy_write(const char *devname, u8 phy_adr, u8 reg, u16 data)
{
	struct eth_device *dev;

	dev = eth_get_dev_by_name(devname);
	gmac_phy_write(dev, phy_adr, reg, data);
	return 0;
}
#endif

static int gmac_xmit(struct eth_device *dev, volatile void *packet, int length)
{
	u32 reg_val, xmit_stat;
	dma_desc_t *tx_p = dma_desc_tx;
	int tmo;

	/* Wait the Prev packet compled and timeout flush it */
	tmo = get_timer(0) + 5 * CONFIG_SYS_HZ;
	while(tx_p->desc0.rx.own){
		if(get_timer(0) > tmo)
			break;
	}

	/* configure transmit dma descriptor */
	tx_p->desc0.all = TX_SINGLE_DESC0;
	tx_p->desc1.all = TX_SINGLE_DESC1;
	tx_p->desc1.tx.cic = cic_full;
	tx_p->desc1.tx.buf1_size = (MAX_BUF_LEN & length);
	tx_p->desc2 = (void *)packet;

	/* flush Transmit FIFO */
	reg_val = readl(GDMA_BASE + GDMA_OP_MODE);
	reg_val |= OP_MODE_FTF;
	writel(reg_val, GDMA_BASE + GDMA_OP_MODE);

	/* Start Transmit */
	xmit_stat = readl(GDMA_BASE + GDMA_STATUS) & STATUS_TPS;
	if(xmit_stat & STATUS_TBUS)
		writel(STATUS_TBUS, GDMA_BASE + GDMA_STATUS);

	xmit_stat &= STATUS_TPS;
	if(xmit_stat == TPS_SUSP){
		writel(0x01, GDMA_BASE + GDMA_XMT_POLL);
	} else if(xmit_stat == TPS_STOP) {
		reg_val = readl(GDMA_BASE + GDMA_OP_MODE);
		reg_val |= OP_MODE_ST;
		writel(reg_val, GDMA_BASE + GDMA_OP_MODE);
	}

	return 0;
}

static int check_rx_coe(int ipc_err, int type, int payload_err)
{
	int ret = good_frame;
	u32 status = (type << 2 | ipc_err << 1 | payload_err) & 0x7;

	/* bits 5 7 0 | Frame status
	 * ----------------------------------------------------------
	 *      0 0 0 | IEEE 802.3 Type frame (length < 1536 octects)
	 *      1 0 0 | IPv4/6 No CSUM errorS.
	 *      1 0 1 | IPv4/6 CSUM PAYLOAD error
	 *      1 1 0 | IPv4/6 CSUM IP HR error
	 *      1 1 1 | IPv4/6 IP PAYLOAD AND HEADER errorS
	 *      0 0 1 | IPv4/6 unsupported IP PAYLOAD
	 *      0 1 1 | COE bypassed.. no IPv4/6 frame
	 *      0 1 0 | Reserved.
	 */
	switch(status){
	case 0x00:
		ret = llc_snap;
		break;
	case 0x04:
		ret = good_frame;
		break;
	case 0x05:
		ret = csum_none;
		break;
	case 0x06:
		ret = csum_none;
		break;
	case 0x07:
		ret = csum_none;
		break;
	case 0x01:
		ret = discard_frame;
		break;
	case 0x03:
		ret = discard_frame;
		break;
	}

	return ret;
}

static int rx_status(dma_desc_t *p)
{
	int ret = good_frame;

	if (p->desc0.rx.err_sum)
		ret = discard_frame;

	/* After a payload csum error, the ES bit is set.
	 * It doesn't match with the information reported into the databook.
	 * At any rate, we need to understand if the CSUM hw computation is ok
	 * and report this info to the upper layers. */
	ret = check_rx_coe(p->desc0.rx.ipch_err,
		p->desc0.rx.frm_type, p->desc0.rx.chsum_err);

	return ret;
}

static int gmac_recv(struct eth_device *dev)
{
	u32 len, reg_val, recv_stat;
	dma_desc_t *rx_p = (void *)dma_desc_rx;

	if(rx_p->desc0.rx.own)
		return 0;

	recv_stat = rx_status(rx_p);
	if(recv_stat != discard_frame){
		if(recv_stat != llc_snap)
			len = (rx_p->desc0.rx.frm_len - 4);
		else
			len = rx_p->desc0.rx.frm_len;
		NetReceive((volatile uchar *)(rx_p->desc2), len);
	}

	rx_p->desc0.all = RX_SINGLE_DESC0;
	rx_p->desc1.all = RX_SINGLE_DESC1;
	rx_p->desc1.rx.buf1_size = (MAX_BUF_LEN & 0xFFF);
	rx_p->desc2 = (void *)rx_packet;

	recv_stat = readl(GDMA_BASE + GDMA_STATUS);
	if(recv_stat & STATUS_RBUS)
		writel(STATUS_RBUS, GDMA_BASE + GDMA_STATUS);

	recv_stat &= STATUS_RPS;
	if(recv_stat == RPS_STOP){
		reg_val = readl(GDMA_BASE + GDMA_OP_MODE);
		reg_val |= OP_MODE_SR;
		writel(reg_val, GDMA_BASE + GDMA_OP_MODE);
	}else if(recv_stat == RPS_SUSP){
		writel(0x01, GDMA_BASE + GDMA_RCV_POLL);
	}

	return 0;
}

static int gmac_sys_init(void)
{
	u32 reg_val;

	/* configure PIOA for gmac */
	writel(0x22222222, PA_CFG0);
	writel(0x22222222, PA_CFG1);
	writel(0x22222222, PA_CFG2);

	/* enalbe clk for gmac */
	reg_val = readl(CCMU_BASE + AHB1_GATING);
	reg_val |= GMAC_AHB_BIT;
	writel(reg_val, CCMU_BASE + AHB1_GATING);

	return 0;
}


static int mii_phy_init(struct eth_device *dev)
{
	int reg_val;

	/* set clk for mii */
	reg_val = readl(GMAC_BASE + GMAC_GMII_ADDR);
	reg_val |= MII_CLK;
	writel(reg_val, GMAC_BASE + GMAC_GMII_ADDR);

	/* Reset phy chip */
	if(gmac_phy_read(dev, PHY_ADDR, MII_BMCR) & BMCR_PDOWN){
		gmac_phy_write(dev, PHY_ADDR, MII_BMCR, ~BMCR_PDOWN);
		while(gmac_phy_read(dev, PHY_ADDR, MII_BMCR) & BMCR_PDOWN);
		printf("PHY Power on OK!!!\n");
	}

	reg_val = gmac_phy_read(dev, PHY_ADDR, MII_BMCR);
	gmac_phy_write(dev, PHY_ADDR, MII_BMCR, reg_val | BMCR_RESET);
	while(gmac_phy_read(dev, PHY_ADDR, MII_BMCR) & BMCR_RESET);
	while(!(gmac_phy_read(dev, PHY_ADDR, MII_BMSR) & BMSR_ANEGCOMPLETE));

	if(gmac_phy_read(dev, PHY_ADDR, MII_BMCR) & BMCR_FULLDPLX){
		reg_val = readl(GMAC_BASE + GMAC_CONTROL);
		reg_val |= GMAC_CTL_DM;
		writel(reg_val, GMAC_BASE + GMAC_CONTROL);
	}
	if(gmac_phy_read(dev, PHY_ADDR, MII_BMCR) & BMCR_SPEED100){
		reg_val = readl(GMAC_BASE + GMAC_CONTROL);
		reg_val |= GMAC_CTL_FES;
		writel(reg_val, GMAC_BASE + GMAC_CONTROL);
	}

	/* just for debug loopback */
	/*
	reg_val |= BMCR_LOOPBACK;
	gmac_phy_write(dev, PHY_ADDR, MII_BMCR, reg_val);
	reg_val = gmac_phy_read(dev, PHY_ADDR, MII_BMCR);
	printf("REG0: %08x\n", reg_val);
	*/
	
	reg_val = gmac_phy_read(dev, PHY_ADDR, MII_BMCR);
	printf("%s   Speed: %dMbps, Mode: %s, Loopback: %s\n", 
					dev->name,
					reg_val&BMCR_SPEED100 ? 100 : 10,
					reg_val&BMCR_FULLDPLX ? "Full duplex" : "Half duplex",
					reg_val&BMCR_LOOPBACK ? "LOOP" : "NO");

	return 0;
}

static int gmac_init(struct eth_device *dev, bd_t *bis)
{
	u32 reg_val;

	/* Reset gmac and gdma */
	writel(SOFT_RESET, GDMA_BASE + GDMA_BUS_MODE);
	while(readl(GDMA_BASE + GDMA_BUS_MODE) & SOFT_RESET);

	/* init phy */
	mii_phy_init(dev);

	/* GMAC core init */
	reg_val = readl(GMAC_BASE + GMAC_CONTROL);
	reg_val |= GMAC_DEF_CONF;
	reg_val &= (~GMAC_CTL_LM);
	writel(reg_val, GMAC_BASE + GMAC_CONTROL);

	/* GMAC frame filter */
	reg_val = readl(GMAC_BASE + GMAC_FRAME_FILTER);
	reg_val |= 0x80000000;
	writel(reg_val, GMAC_BASE + GMAC_FRAME_FILTER);

	/* GDMA init */
	reg_val = readl(GDMA_BASE + GDMA_BUS_MODE);
	reg_val &= (~BUS_ADDR_ALIGN);
	reg_val &= (~BUS_MODE_4PBL);
	reg_val &= (~BUS_MODE_USP);
	/* set rx_user_separate_pbl */
	reg_val |= (1<<17) & BUS_MODE_RXPBL;
	/* set Programmbale Burst Length */
	reg_val |= ((1<<8) & BUS_MODE_PBL);
	reg_val &= (~BUS_MODE_FIXBUST);
	reg_val |= ((0<<2) & BUS_MODE_DSL);
	reg_val |= ((0<<14) & BUS_MODE_RTPR);
	reg_val |= BUS_MODE_DA;
	writel(reg_val, GDMA_BASE + GDMA_BUS_MODE);

	reg_val = readl(GDMA_BASE + GDMA_OP_MODE);
	reg_val |= GDMA_OP_CONF;
	reg_val &= (~OP_MODE_OSF);
	writel(reg_val, GDMA_BASE + GDMA_OP_MODE);

	/* Disable all interrupt of dma */
	writel(0x00000000, GDMA_BASE + GDMA_INTR_ENA);
	memset((void *)dma_desc_tx, 0, sizeof(dma_desc_t));
	memset((void *)dma_desc_rx, 0, sizeof(dma_desc_t));
	writel((u32)dma_desc_tx, GDMA_BASE + GDMA_XMT_LIST);
	writel((u32)dma_desc_rx, GDMA_BASE + GDMA_RCV_LIST);

	return 0;
}

static void gmac_halt(struct eth_device *dev)
{
	int reg_val;

	reg_val = readl(dev->iobase + GMAC_CONTROL);
	reg_val &= (~GMAC_CTL_TE & ~GMAC_CTL_RE);
	writel(reg_val, dev->iobase + GMAC_CONTROL);

}

int gmac_write_hwaddr(struct eth_device *dev)
{
	u32 mac_hi;
	u32 mac_lo;

	mac_hi = (dev->enetaddr[3] << 24)
			| (dev->enetaddr[2] << 16)
			| (dev->enetaddr[1] << 8)
			| (dev->enetaddr[0]);
	writel(mac_hi, dev->iobase + GMAC_ADDR_HI(0));

	mac_lo = (dev->enetaddr[5] << 8)
			| (dev->enetaddr[4]);
	writel(mac_lo, dev->iobase + GMAC_ADDR_LO(0));

	return 0;
}

int gmac_initialize(bd_t *bis)
{
	struct eth_device *dev;

	dev = (struct eth_device *)malloc(sizeof *dev);
	if(!dev)
		return -ENOMEM;
	memset(dev, 0, (size_t)sizeof(*dev));
	strcpy(dev->name, "eth0");

	/*set random hwaddr for mac */
	random_ether_addr(dev->enetaddr);
	
	dev->iobase = GMAC_BASE;
	dev->init = gmac_init;
	dev->halt = gmac_halt;
	dev->send = gmac_xmit;
	dev->recv = gmac_recv;
	dev->write_hwaddr = gmac_write_hwaddr;

	gmac_sys_init();
	//gmac_init(dev, bis);
	//mii_phy_init(dev);
	//gmac_write_hwaddr(dev);
	
	eth_register(dev);

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
	miiphy_register(dev->name, gmac_miiphy_read, gmac_miiphy_write);
#endif

	return 0;
}

