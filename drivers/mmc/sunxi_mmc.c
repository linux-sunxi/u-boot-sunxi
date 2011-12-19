/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC driver for allwinner sunxi platform.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>
#include <mmc.h>

#undef SUNXI_MMCDBG

#ifdef SUNXI_MMCDBG
#define MMCDBG(fmt...)	printf("[mmc]: "fmt)

static void dumphex32(char* name, char* base, int len)
{
	__u32 i;
	
	printf("dump %s registers:", name);
	for (i=0; i<len; i+=4)
	{
		if (!(i&0xf))
			printf("\n0x%p : ", base + i);
		printf("0x%08x ", readl(base + i));
	}
	printf("\n");
}

static void dumpmmcreg(struct sunxi_mmc *reg)
{
	printf("gctrl     0x%08x\n", reg->gctrl     );
	printf("clkcr     0x%08x\n", reg->clkcr     );
	printf("timeout   0x%08x\n", reg->timeout   );
	printf("width     0x%08x\n", reg->width     );
	printf("blksz     0x%08x\n", reg->blksz     );
	printf("bytecnt   0x%08x\n", reg->bytecnt   );
	printf("cmd       0x%08x\n", reg->cmd       );
	printf("arg       0x%08x\n", reg->arg       );
	printf("resp0     0x%08x\n", reg->resp0     );
	printf("resp1     0x%08x\n", reg->resp1     );
	printf("resp2     0x%08x\n", reg->resp2     );
	printf("resp3     0x%08x\n", reg->resp3     );
	printf("imask     0x%08x\n", reg->imask     );
	printf("mint      0x%08x\n", reg->mint      );
	printf("rint      0x%08x\n", reg->rint      );
	printf("status    0x%08x\n", reg->status    );
	printf("ftrglevel 0x%08x\n", reg->ftrglevel );
	printf("funcsel   0x%08x\n", reg->funcsel   );
}
#else
#define MMCDBG(fmt...)
#define dumphex32(fmt...)
#define dumpmmcreg(fmt...)
#endif /* SUNXI_MMCDBG */

struct sunxi_mmc_host {
	struct sunxi_mmc *reg;
	unsigned mmc_no;
	unsigned hclkbase;
	unsigned mclkbase;
	unsigned database;
	unsigned fatal_err;
};

/* support 4 mmc hosts */
struct mmc mmc_dev[4];
struct sunxi_mmc_host mmc_host[4];

static int sunxi_mmc_res_init(int sdc_no)
{
	struct sunxi_mmc_host* mmchost = &mmc_host[sdc_no];
	MMCDBG("init mmc %d resource\n", sdc_no);
	switch (sdc_no) {
		case 0:
			mmchost->reg = (struct sunxi_mmc *)SUNXI_MMC0_BASE;
			mmchost->mclkbase = SUNXI_CCM_MMC0_SCLK_CFG;
			break;
		case 1:
			mmchost->reg = (struct sunxi_mmc *)SUNXI_MMC1_BASE;
			mmchost->mclkbase = SUNXI_CCM_MMC1_SCLK_CFG;
			break;
		case 2:
			mmchost->reg = (struct sunxi_mmc *)SUNXI_MMC2_BASE;
			mmchost->mclkbase = SUNXI_CCM_MMC2_SCLK_CFG;
			break;
		case 3:
			mmchost->reg = (struct sunxi_mmc *)SUNXI_MMC3_BASE;
			mmchost->mclkbase = SUNXI_CCM_MMC3_SCLK_CFG;
			break;
		default:
			printf("Wrong mmc number %d\n", sdc_no);
			break;
	}
	mmchost->hclkbase = SUNXI_CCM_AHB_GATING0;
	mmchost->database = (unsigned int)mmchost->reg + 0x100;
	mmchost->mmc_no = sdc_no;
	
	return 0;
}

static int sunxi_mmc_clk_io_on(int sdc_no)
{
	unsigned int rval;
	struct sunxi_mmc_host* mmchost = &mmc_host[sdc_no];
	static struct sunxi_gpio *gpio_c =
			&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[SUNXI_GPIO_C];
	static struct sunxi_gpio *gpio_f =
			&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[SUNXI_GPIO_F];
	#if 0
	static struct sunxi_gpio *gpio_g =
			&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[SUNXI_GPIO_G];
	#endif
	static struct sunxi_gpio *gpio_h =
			&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[SUNXI_GPIO_H];
	static struct sunxi_gpio *gpio_i =
			&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[SUNXI_GPIO_I];
	MMCDBG("init mmc %d clock and io\n", sdc_no);
	/* config gpio */
	switch (sdc_no) {
        case 0:
            /* D1-PF0, D0-PF1, CLK-PF2, CMD-PF3, D3-PF4, D4-PF5 */
            writel(0x222222, &gpio_f->cfg[0]);
            writel(0x555, &gpio_f->pull[0]);
            writel(0x555, &gpio_f->drv[0]);
            break;

        case 1:
            #if 0
            /* PG0-CMD, PG1-CLK, PG2~5-D0~3 : 4 */
            writel(0x444444, &gpio_g->cfg[0]);
            writel(0x555, &gpio_g->pull[0]);
            writel(0x555, &gpio_g->drv[0]);
            #else
            /* PH22-CMD, PH23-CLK, PH24~27-D0~D3 : 5 */
            writel(0x55<<24, &gpio_h->cfg[2]);
            writel(0x5555, &gpio_h->cfg[3]);
            writel(0x555<<12, &gpio_h->pull[1]);
            writel(0x555<<12, &gpio_h->drv[1]);
            #endif
            break;

        case 2:
            /* CMD-PC6, CLK-PC7, D0-PC8, D1-PC9, D2-PC10, D3-PC11 */
            writel(0x33<<24, &gpio_c->cfg[0]);
            writel(0x3333, &gpio_c->cfg[1]);
            writel(0x555<<12, &gpio_c->pull[0]);
            writel(0x555<<12, &gpio_c->drv[0]);
            break;

        case 3:
            /* PI4-CMD, PI5-CLK, PI6~9-D0~D3 : 2 */
            writel(0x2222<<16, &gpio_i->cfg[0]);
            writel(0x22, &gpio_i->cfg[1]);
            writel(0x555<<8, &gpio_i->pull[0]);
            writel(0x555<<8, &gpio_i->drv[0]);
            break;

        default:
            return -1;
	}

	/* config clock */
	rval = readl(mmchost->hclkbase);
	rval |= (1 << (8 + sdc_no));
	writel(rval, mmchost->hclkbase);
	writel(1U << 31, mmchost->mclkbase);
	
	dumphex32("ccmu", SUNXI_CCM_BASE, 0x100);
	dumphex32("gpio", SUNXI_PIO_BASE, 0x100);
	dumphex32("mmc", mmchost->reg, 0x100);
    return 0;
}

static int mmc_update_clk(struct mmc *mmc)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned int cmd;
	unsigned timeout = 0xfffff;

	cmd = (1U << 31) | (1 << 21) | (1 << 13);
  	writel(cmd, &mmchost->reg->cmd);
	while((readl(&mmchost->reg->cmd)&0x80000000) && timeout--);
	if (!timeout)
		return -1;

	writel(readl(&mmchost->reg->rint), &mmchost->reg->rint);
	return 0;
}

static int mmc_config_clock(struct mmc *mmc, unsigned div)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned rval = readl(&mmchost->reg->clkcr);

	/*
	 * CLKCREG[7:0]: divider
	 * CLKCREG[16]:  on/off
	 * CLKCREG[17]:  power save
	 */
	/* Disable Clock */
	rval &= ~(1 << 16);
	writel(rval, &mmchost->reg->clkcr);
	if(mmc_update_clk(mmc))
		return -1;
	/* Change Divider Factor */
	rval &= ~(0xFF);
	rval |= div;
	writel(rval, &mmchost->reg->clkcr);
	if(mmc_update_clk(mmc))
		return -1;
	/* Re-enable Clock */
	rval |= (1 << 16);
	writel(rval, &mmchost->reg->clkcr);
	if(mmc_update_clk(mmc))
		return -1;
	return 0;
}

static void mmc_set_ios(struct mmc *mmc)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned int clkdiv = 0;

	MMCDBG("set ios: bus_width: %x, clock: %d\n", mmc->bus_width, mmc->clock);

	/* Change clock first */
	if (mmc->clock <= 400000)
		clkdiv = (24000000 + (mmc->clock>>1))/mmc->clock/2;
	if (mmc->clock)
		if (mmc_config_clock(mmc, clkdiv)) {
			mmchost->fatal_err = 1;
			return;
		}
	/* Change bus width */
	if (mmc->bus_width == 8)
		writel(2, &mmchost->reg->width);
	else if (mmc->bus_width == 4)
		writel(1, &mmchost->reg->width);
	else
		writel(0, &mmchost->reg->width);
}

static int mmc_core_init(struct mmc *mmc)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	/* Reset controller */
	writel(0x7, &mmchost->reg->gctrl);
	return 0;
}

static int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data)
{
	struct sunxi_mmc_host* mmchost = (struct sunxi_mmc_host *)mmc->priv;
	unsigned int cmdval = 0x80000000;
	signed int timeout = 0;
	unsigned int error = 0;
	unsigned int status = 0;

	if (mmchost->fatal_err)
		return -1;
	if (cmd->resp_type & MMC_RSP_BUSY)
		MMCDBG("mmc cmd %d check rsp busy\n", cmd->cmdidx);
	if (cmd->cmdidx == 12)
		return 0;
	/*
	 * CMDREG
	 * CMD[5:0]	: Command index
	 * CMD[6]	: Has response
	 * CMD[7]	: Long response
	 * CMD[8]	: Check response CRC
	 * CMD[9]	: Has data
	 * CMD[10]	: Write
	 * CMD[11]	: Steam mode
	 * CMD[12]	: Auto stop
	 * CMD[13]	: Wait previous over
	 * CMD[14]	: About cmd
	 * CMD[15]	: Send initialization
	 * CMD[21]	: Update clock
	 * CMD[31]	: Load cmd
	 */
	if (!cmd->cmdidx)
		cmdval |= (1 << 15);
	if (cmd->resp_type & MMC_RSP_PRESENT)
		cmdval |= (1 << 6);
	if (cmd->resp_type & MMC_RSP_136)
		cmdval |= (1 << 7);
	if (cmd->resp_type & MMC_RSP_CRC)
		cmdval |= (1 << 8);
	if (data) {
		if ((u32)data->dest & 0x3) {
			error = -1;
			goto out;
		}
		
		cmdval |= (1 << 9) | (1 << 13);
		if (data->flags & MMC_DATA_WRITE)
			cmdval |= (1 << 10);
		if (data->blocks > 1)
			cmdval |= (1 << 12);
		writel(data->blocksize, &mmchost->reg->blksz);
		writel(data->blocks * data->blocksize, &mmchost->reg->bytecnt);
		writel(1U << 31, &mmchost->reg->gctrl);
	}

	MMCDBG("mmc %d, cmd %d(0x%08x), arg 0x%08x\n", mmchost->mmc_no, cmd->cmdidx, cmdval|cmd->cmdidx, cmd->cmdarg);
	writel(cmd->cmdarg, &mmchost->reg->arg);
	writel(cmdval|cmd->cmdidx, &mmchost->reg->cmd);
	
	/*
	 * transfer data and check status
	 * STATREG[2] : FIFO empty
	 * STATREG[3] : FIFO full
	 */
	if (data) {
		unsigned int i;
		unsigned int byte_cnt = data->blocksize * data->blocks;
		unsigned int *buff;
		timeout = 0xfffff;
		
		MMCDBG("with data %d bytes\n", byte_cnt);
		if (data->flags & MMC_DATA_READ) {
			buff = (unsigned int *)data->dest;
		    for (i=0; i<(byte_cnt>>2); i++) {
				while(--timeout && (readl(&mmchost->reg->status)&(1 << 2)))
					if (timeout <= 0) goto dataout;
				buff[i] = readl(mmchost->database);
				timeout = 0xfffff;
			}
		} else {
			buff = (unsigned int *)data->src;
		    for (i=0; i<(byte_cnt>>2); i++) {
				while(--timeout && (readl(&mmchost->reg->status)&(1 << 3)));
					if (timeout <= 0) goto dataout;
				writel(buff[i], mmchost->database);
				timeout = 0xfffff;
			}
		}
dataout:
		if (timeout <= 0) {
			error = readl(&mmchost->reg->rint) & 0xbfc2;
			goto out;
		}
	}
	
	timeout = 0xfffff;
	do {
		status = readl(&mmchost->reg->rint);
		if (!timeout-- || (status & 0xbfc2)) {
			error = status & 0xbfc2;
			MMCDBG("cmd timeout %x\n", error);
			goto out;
		}
	} while (!(status&0x4));
	
	if (data) {
		timeout = 0xffff;
		unsigned done = 0;
		do {
			status = readl(&mmchost->reg->rint);
			if (!timeout-- || (status & 0xbfc2)) {
				error = status & 0xbfc2;
				MMCDBG("data timeout %x\n", error);
				goto out;
			}
			if (data->blocks > 1)
				done = status & (1 << 14);
			else
				done = status & (1 << 3);
		} while (!done);
	}

	if (cmd->resp_type & MMC_RSP_BUSY) {
		timeout = 0xfffff;
		do {
			status = readl(&mmchost->reg->status);
			if (!timeout--) {
				error = -1;
				MMCDBG("busy timeout\n");
				goto out;
			}
		} while (status & (1 << 9));
	}
	if (cmd->resp_type & MMC_RSP_136) {
		cmd->response[0] = readl(&mmchost->reg->resp3);
		cmd->response[1] = readl(&mmchost->reg->resp2);
		cmd->response[2] = readl(&mmchost->reg->resp1);
		cmd->response[3] = readl(&mmchost->reg->resp0);
		MMCDBG("mmc resp 0x%08x 0x%08x 0x%08x 0x%08x\n",
			cmd->response[3], cmd->response[2],
			cmd->response[1], cmd->response[0]);
	} else {
		cmd->response[0] = readl(&mmchost->reg->resp0);
		MMCDBG("mmc resp 0x%08x\n", cmd->response[0]);
	}
out:
	if (error) {
		writel(0x7, &mmchost->reg->gctrl);
		mmc_update_clk(mmc);
		MMCDBG("mmc cmd %d err 0x%08x\n", cmd->cmdidx, error);
	}
	writel(0xffffffff, &mmchost->reg->rint);

	if (error)
		return -1;
	else
		return 0;
}

int sunxi_mmc_init(int sdc_no)
{
	struct mmc *mmc;

	//printf(" mmc initialize %d\n", sdc_no);

	memset(&mmc_dev[sdc_no], 0, sizeof(struct mmc));
	memset(&mmc_host[sdc_no], 0, sizeof(struct sunxi_mmc_host));
	mmc = &mmc_dev[sdc_no];

	sprintf(mmc->name, "Allwinner SD/MMC");
	mmc->priv = &mmc_host[sdc_no];
	mmc->send_cmd = mmc_send_cmd;
	mmc->set_ios = mmc_set_ios;
	mmc->init = mmc_core_init;

	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->host_caps = MMC_MODE_4BIT;
	mmc->host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;

	mmc->f_min = 400000;
	mmc->f_max = 25000000;

	sunxi_mmc_res_init(sdc_no);
	sunxi_mmc_clk_io_on(sdc_no);

	mmc_register(mmc);

	return 0;
}
