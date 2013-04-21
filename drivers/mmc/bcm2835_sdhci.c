/*
 * This code was extracted from:
 * git://github.com/gonzoua/u-boot-pi.git master
 * and hence presumably (C) 2012 Oleksandr Tymoshenko
 *
 * Tweaks for U-Boot upstreaming
 * (C) 2012 Stephen Warren
 *
 * Portions (e.g. read/write macros, concepts for back-to-back register write
 * timing workarounds) obviously extracted from the Linux kernel at:
 * https://github.com/raspberrypi/linux.git rpi-3.6.y
 *
 * The Linux kernel code has the following (c) and license, which is hence
 * propagated to Oleksandr's tree and here:
 *
 * Support for SDHCI device on 2835
 * Based on sdhci-bcm2708.c (c) 2010 Broadcom
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Supports:
 * SDHCI platform device - Arasan SD controller in BCM2708
 *
 * Inspired by sdhci-pci.c, by Pierre Ossman
 */

#include <common.h>
#include <malloc.h>
#include <sdhci.h>
#include <asm/arch/timer.h>

/* 400KHz is max freq for card ID etc. Use that as min */
#define MIN_FREQ 400000

struct bcm2835_sdhci_host {
	struct sdhci_host host;
	uint twoticks_delay;
	ulong last_write;
};

static inline struct bcm2835_sdhci_host *to_bcm(struct sdhci_host *host)
{
	return (struct bcm2835_sdhci_host *)host;
}

static inline void bcm2835_sdhci_raw_writel(struct sdhci_host *host, u32 val,
						int reg)
{
	struct bcm2835_sdhci_host *bcm_host = to_bcm(host);

	/*
	 * The Arasan has a bugette whereby it may lose the content of
	 * successive writes to registers that are within two SD-card clock
	 * cycles of each other (a clock domain crossing problem).
	 * It seems, however, that the data register does not have this problem.
	 * (Which is just as well - otherwise we'd have to nobble the DMA engine
	 * too)
	 */
	while (get_timer_us(bcm_host->last_write) < bcm_host->twoticks_delay)
		;

	writel(val, host->ioaddr + reg);
	bcm_host->last_write = get_timer_us(0);
}

static inline u32 bcm2835_sdhci_raw_readl(struct sdhci_host *host, int reg)
{
	return readl(host->ioaddr + reg);
}

static void bcm2835_sdhci_writel(struct sdhci_host *host, u32 val, int reg)
{
	bcm2835_sdhci_raw_writel(host, val, reg);
}

static void bcm2835_sdhci_writew(struct sdhci_host *host, u16 val, int reg)
{
	static u32 shadow;
	u32 oldval = (reg == SDHCI_COMMAND) ? shadow :
		bcm2835_sdhci_raw_readl(host, reg & ~3);
	u32 word_num = (reg >> 1) & 1;
	u32 word_shift = word_num * 16;
	u32 mask = 0xffff << word_shift;
	u32 newval = (oldval & ~mask) | (val << word_shift);

	if (reg == SDHCI_TRANSFER_MODE)
		shadow = newval;
	else
		bcm2835_sdhci_raw_writel(host, newval, reg & ~3);
}

static void bcm2835_sdhci_writeb(struct sdhci_host *host, u8 val, int reg)
{
	u32 oldval = bcm2835_sdhci_raw_readl(host, reg & ~3);
	u32 byte_num = reg & 3;
	u32 byte_shift = byte_num * 8;
	u32 mask = 0xff << byte_shift;
	u32 newval = (oldval & ~mask) | (val << byte_shift);

	bcm2835_sdhci_raw_writel(host, newval, reg & ~3);
}

static u32 bcm2835_sdhci_readl(struct sdhci_host *host, int reg)
{
	u32 val = bcm2835_sdhci_raw_readl(host, reg);

	return val;
}

static u16 bcm2835_sdhci_readw(struct sdhci_host *host, int reg)
{
	u32 val = bcm2835_sdhci_raw_readl(host, (reg & ~3));
	u32 word_num = (reg >> 1) & 1;
	u32 word_shift = word_num * 16;
	u32 word = (val >> word_shift) & 0xffff;

	return word;
}

static u8 bcm2835_sdhci_readb(struct sdhci_host *host, int reg)
{
	u32 val = bcm2835_sdhci_raw_readl(host, (reg & ~3));
	u32 byte_num = reg & 3;
	u32 byte_shift = byte_num * 8;
	u32 byte = (val >> byte_shift) & 0xff;

	return byte;
}

static const struct sdhci_ops bcm2835_ops = {
	.write_l = bcm2835_sdhci_writel,
	.write_w = bcm2835_sdhci_writew,
	.write_b = bcm2835_sdhci_writeb,
	.read_l = bcm2835_sdhci_readl,
	.read_w = bcm2835_sdhci_readw,
	.read_b = bcm2835_sdhci_readb,
};

int bcm2835_sdhci_init(u32 regbase, u32 emmc_freq)
{
	struct bcm2835_sdhci_host *bcm_host;
	struct sdhci_host *host;

	bcm_host = malloc(sizeof(*bcm_host));
	if (!bcm_host) {
		printf("sdhci_host malloc fail!\n");
		return 1;
	}

	/*
	 * See the comments in bcm2835_sdhci_raw_writel().
	 *
	 * This should probably be dynamically calculated based on the actual
	 * frequency. However, this is the longest we'll have to wait, and
	 * doesn't seem to slow access down too much, so the added complexity
	 * doesn't seem worth it for now.
	 *
	 * 1/MIN_FREQ is (max) time per tick of eMMC clock.
	 * 2/MIN_FREQ is time for two ticks.
	 * Multiply by 1000000 to get uS per two ticks.
	 * +1 for hack rounding.
	 */
	bcm_host->twoticks_delay = ((2 * 1000000) / MIN_FREQ) + 1;
	bcm_host->last_write = 0;

	host = &bcm_host->host;
	host->name = "bcm2835_sdhci";
	host->ioaddr = (void *)regbase;
	host->quirks = SDHCI_QUIRK_BROKEN_VOLTAGE | SDHCI_QUIRK_BROKEN_R1B |
		SDHCI_QUIRK_WAIT_SEND_CMD;
	host->voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;
	host->ops = &bcm2835_ops;

	host->version = sdhci_readw(host, SDHCI_HOST_VERSION);
	add_sdhci(host, emmc_freq, MIN_FREQ);

	return 0;
}
