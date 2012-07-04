/**
 * gpio.c - definition for registers of gpio controller
 * date:    2012/2/12 22:34:41
 * author:  Aaron<leafy.myeh@allwinnertech.com>
 * history: V0.1
 */
#include <asm/arch/sys_proto.h>
#include <asm/arch/gpio.h>
#include <asm/arch/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/gic.h>
/**
 *	static __inline void gpio_set_single_pio_cfg(u32 pio_no, u32 mode) ;
 *  Description:
 *
 *	Arguments:		NO. of start pio. eg: GPIO_A(13), GPIO_B(21)
 *                  mode - config mode
 *	Return Value:
 */
static __inline void gpio_set_single_pio_cfg(u32 pio_no, u32 mode)
{
    u32 start = pio_no&0x1f;
	u32 cfg_addr = GPIO_CFG_REG(pio_no) + 4*(start>>3);
	u32 offs = (start&0x7)<<2;

	sr32_aw(cfg_addr, offs, 4, mode);
}

/**
 *	__inline void gpio_set_single_pio_pull(u32 pio_no, u32 pull);
 *  Description:
 *
 *	Arguments:		pio_no - NO. of start pio. eg: GPIO_A(13), GPIO_B(21)
 *                  pull - pull mode
 *	Return Value:
 */
static __inline void gpio_set_single_pio_pull(u32 pio_no, u32 pull)
{
    u32 start = pio_no&0x1f;
	u32 pull_addr = GPIO_PULL_REG(pio_no) + 4*(start>>4);
	u32 offs = (start&0xf)<<1;

	sr32_aw(pull_addr, offs, 2, pull);
}

/**
 *	static __inline void gpio_set_single_pio_drv(u32 pio_no, u32 level);
 *  Description:
 *
 *	Arguments:		pio_no - NO. of start pio. eg: GPIO_A(13), GPIO_B(21)
 *                  level - driver level
 *	Return Value:
 */
static __inline void gpio_set_single_pio_drv(u32 pio_no, u32 level)
{
    u32 start = pio_no&0x1f;
	u32 drv_addr = GPIO_DRV_REG(pio_no) + 4*(start>>4);
	u32 offs = (start&0xf)<<1;

	sr32_aw(drv_addr, offs, 2, level);
}

/**
 *	void gpio_set_cfg(u32 pio_no, u32 num, u32 mode);
 *  Description:
 *
 *	Arguments:		pio_no - NO. of start pio. eg: GPIO_A(13), GPIO_B(21)
 *                  num - number of continus pins to config
 *                  mode - config mode
 *	Return Value:
 */
void gpio_set_cfg(u32 pio_no, u32 num, u32 mode)
{
    u32 i;

    for (i=0; i<num; i++)
        gpio_set_single_pio_cfg(pio_no+i, mode);
}

/**
 *	void gpio_set_pull(u32 pio_no, u32 num, u32 pull);
 *  Description:
 *
 *	Arguments:		pio_no - NO. of start pio. eg: GPIO_A(13), GPIO_B(21)
 *                  num - number of continus pins to set
 *                  pull - pull mode
 *	Return Value:
 */
void gpio_set_pull(u32 pio_no, u32 num, u32 pull)
{
    u32 i;

    for (i=0; i<num; i++)
        gpio_set_single_pio_pull(pio_no+i, pull);
}

/**
 *	void gpio_set_drv(u32 pio_no, u32 num, u32 level);
 *  Description:
 *
 *	Arguments:		pio_no - NO. of start pio. eg: GPIO_A(13), GPIO_B(21)
 *                  num - number of continus pins to set
 *                  level - driver level
 *	Return Value:
 */
void gpio_set_drv(u32 pio_no, u32 num, u32 level)
{
    u32 i;

    for (i=0; i<num; i++)
        gpio_set_single_pio_drv(pio_no+i, level);
}

/**
 *	void gpio_set_val(u32 pio_no, u32 val);
 *  Description:
 *
 *	Arguments:		pio_no - NO. of start pio. eg: GPIO_A(13), GPIO_B(21)
 *                  val - value to set 0 or 1
 *	Return Value:
 */
void gpio_set_val(u32 pio_no, u32 val)
{
	u32 offs = pio_no&0x1f;
	u32 data_addr = GPIO_DATA_REG(pio_no);

	sr32_aw(data_addr, offs, 1, val);
}

/**
 *	u32 gpio_get_val(u32 pio_no);
 *  Description:
 *
 *	Arguments:		pio_no - NO. of start pio. eg: GPIO_A(13), GPIO_B(21)
 *	Return Value:
 */
u32 gpio_get_val(u32 pio_no)
{
	u32 offs = pio_no&0x1f;

	return 0x1 & (readl(GPIO_DATA_REG(pio_no))>>offs);
}

#ifndef FPGA_PLATFORM
static Eint_Map eint_map[] = {
    {PIO_INT_A(0 ), GPIO_A(0 ), 6},
    {PIO_INT_A(1 ), GPIO_A(1 ), 6},
    {PIO_INT_A(2 ), GPIO_A(2 ), 6},
    {PIO_INT_A(3 ), GPIO_A(3 ), 6},
    {PIO_INT_A(4 ), GPIO_A(4 ), 6},
    {PIO_INT_A(5 ), GPIO_A(5 ), 6},
    {PIO_INT_A(6 ), GPIO_A(6 ), 6},
    {PIO_INT_A(7 ), GPIO_A(7 ), 6},
    {PIO_INT_A(8 ), GPIO_A(8 ), 6},
    {PIO_INT_A(9 ), GPIO_A(9 ), 6},
    {PIO_INT_A(10), GPIO_A(10), 6},
    {PIO_INT_A(11), GPIO_A(11), 6},
    {PIO_INT_A(12), GPIO_A(12), 6},
    {PIO_INT_A(13), GPIO_A(13), 6},
    {PIO_INT_A(14), GPIO_A(0),  6},
    {PIO_INT_A(15), GPIO_A(1),  6},
    {PIO_INT_A(16), GPIO_A(2),  6},
    {PIO_INT_A(17), GPIO_A(3),  6},
    {PIO_INT_A(18), GPIO_A(4),  6},
    {PIO_INT_A(19), GPIO_A(5),  6},
    {PIO_INT_A(20), GPIO_A(6),  6},
    {PIO_INT_A(21), GPIO_A(7),  6},
    {PIO_INT_A(22), GPIO_A(8),  6},
    {PIO_INT_A(23), GPIO_A(9),  6},
    {PIO_INT_A(24), GPIO_A(10), 6},
    {PIO_INT_A(25), GPIO_A(11), 6},
    {PIO_INT_A(26), GPIO_A(12), 6},
    {PIO_INT_A(27), GPIO_A(13), 6},
    
    {PIO_INT_B(0 ), GPIO_B(0 ), 6},
    {PIO_INT_B(1 ), GPIO_B(1 ), 6},
    {PIO_INT_B(2 ), GPIO_B(2 ), 6},
    {PIO_INT_B(3 ), GPIO_B(3 ), 6},
    {PIO_INT_B(4 ), GPIO_B(4 ), 6},
    {PIO_INT_B(5 ), GPIO_B(5 ), 6},
    {PIO_INT_B(6 ), GPIO_B(6 ), 6},
    {PIO_INT_B(7 ), GPIO_B(7 ), 6},
    
    {PIO_INT_E(0 ), GPIO_E(0 ), 6},
    {PIO_INT_E(1 ), GPIO_E(1 ), 6},
    {PIO_INT_E(2 ), GPIO_E(2 ), 6},
    {PIO_INT_E(3 ), GPIO_E(3 ), 6},
    {PIO_INT_E(4 ), GPIO_E(4 ), 6},
    {PIO_INT_E(5 ), GPIO_E(5 ), 6},
    {PIO_INT_E(6 ), GPIO_E(6 ), 6},
    {PIO_INT_E(7 ), GPIO_E(7 ), 6},
    {PIO_INT_E(8 ), GPIO_E(8 ), 6},
    {PIO_INT_E(9 ), GPIO_E(9 ), 6},
    {PIO_INT_E(10), GPIO_E(10), 6},
    {PIO_INT_E(11), GPIO_E(11), 6},
    {PIO_INT_E(12), GPIO_E(12), 6},
    {PIO_INT_E(13), GPIO_E(13), 6},
    {PIO_INT_E(14), GPIO_E(14), 6},
    {PIO_INT_E(15), GPIO_E(15), 6},
    {PIO_INT_E(16), GPIO_E(16), 6},

    {PIO_INT_G(0 ), GPIO_G(0 ), 6},
    {PIO_INT_G(1 ), GPIO_G(1 ), 6},
    {PIO_INT_G(2 ), GPIO_G(2 ), 6},
    {PIO_INT_G(3 ), GPIO_G(3 ), 6},
    {PIO_INT_G(4 ), GPIO_G(4 ), 6},
    {PIO_INT_G(5 ), GPIO_G(5 ), 6},
    {PIO_INT_G(6 ), GPIO_G(6 ), 6},
    {PIO_INT_G(7 ), GPIO_G(7 ), 6},
    {PIO_INT_G(8 ), GPIO_G(8 ), 6},
    {PIO_INT_G(9 ), GPIO_G(9 ), 6},
    {PIO_INT_G(10), GPIO_G(10), 6},
    {PIO_INT_G(11), GPIO_G(11), 6},
    {PIO_INT_G(12), GPIO_G(12), 6},
    {PIO_INT_G(13), GPIO_G(13), 6},
    {PIO_INT_G(14), GPIO_G(14), 6},
    {PIO_INT_G(15), GPIO_G(15), 6},
    {PIO_INT_G(16), GPIO_G(16), 6},
    {PIO_INT_G(17), GPIO_G(17), 6},
    {PIO_INT_G(18), GPIO_G(18), 6},
};
#else
static Eint_Map eint_map[] = {
    {0 , GPIO_I(0 ), 3},
};
#endif

struct gpio_irq_vector {
	void (*callback)(u32 data);
	u32 data;
};
static struct gpio_irq_vector gpio_irq_a[PIO_EINTA_NUM];
static struct gpio_irq_vector gpio_irq_b[PIO_EINTB_NUM];
static struct gpio_irq_vector gpio_irq_e[PIO_EINTE_NUM];
static struct gpio_irq_vector gpio_irq_g[PIO_EINTG_NUM];
static struct gpio_irq_vector* vector[] = {
	NULL, 
	gpio_irq_a, 
	gpio_irq_b, 
	gpio_irq_e, 
	gpio_irq_g
};

static void gpio_irq_hdle(u32 group)
{
	u32 base = PIO_EINT_GRP_BASE(group);
	u32 en = readl(base + 0x10);
	u32 stat = readl(base + 0x14);
	u32 mskd_sta = en & stat;
	u32 i;
	
	for (i = 0; i < PIO_EINTx_NUM(group); i++) {
		if (mskd_sta & (1 << i)) {
			if (vector[group][i].callback)
				vector[group][i].callback(vector[group][i].data);
		}
	}
	writel(mskd_sta, base + 0x14);
}

static void gpio_a_irq_hdle(void)
{
	gpio_irq_hdle(PIO_INT_GRP_A);
}

static void gpio_b_irq_hdle(void)
{
	gpio_irq_hdle(PIO_INT_GRP_B);
}

static void gpio_e_irq_hdle(void)
{
	gpio_irq_hdle(PIO_INT_GRP_E);
}

static void gpio_g_irq_hdle(void)
{
	gpio_irq_hdle(PIO_INT_GRP_G);
}

s32 gpio_request_eintio(u32 eint_no, u32 mode, gpio_callback hdle, u32 data)
{
	u32 i = 0;
	u32 group = (eint_no >> 8) & 0xff;
	u32 number = eint_no & 0xff;
	u32 cfg_base = PIO_EINT_CFG_REG(eint_no) + 4 * ((number) >> 3);
	u32 en_base = PIO_EINT_CTRL_REG(eint_no);
	u32 offs = (number & 7) << 2;
	
	/* find this EINT IO */
	if (group == PIO_INT_GRP_B)
		i = PIO_EINTA_NUM;
	else if (group == PIO_INT_GRP_E)
		i = PIO_EINTA_NUM + PIO_EINTB_NUM;
	else if (group == PIO_INT_GRP_G)
		i = PIO_EINTA_NUM + PIO_EINTB_NUM + PIO_EINTE_NUM;
	i += number;
	if (eint_no != eint_map[i].eint) {
		//printk("No GPIO eint for group %d, number %d\n", group, number);
		return -1;
	}
	
	/* config gpio mode for interrupt */
	gpio_set_cfg(eint_map[i].gpio, 1, eint_map[i].cfg);
	gpio_set_pull(eint_map[i].gpio, 1, 1);	// pull up
	
	/* config gpio irq trigger mode */
	sr32_aw(cfg_base, offs, 4, mode);
	/* enable irq */
	sr32_aw(en_base, number, 1, 1);
	vector[group][number].callback = hdle;
	vector[group][number].data = data;
	return 0;
}

s32 gpio_free_eintio(u32 eint_no)
{
	u32 i = 0;
	u32 group = (eint_no >> 8) & 0xff;
	u32 number = eint_no & 0xff;
	u32 cfg_base = PIO_EINT_CFG_REG(eint_no) + 4 * (number >> 3);
	u32 en_base = PIO_EINT_CTRL_REG(eint_no);
	u32 offs = (number & 7) << 2;
	
	/* find this EINT IO */
	if (group == PIO_INT_GRP_B)
		i = PIO_EINTA_NUM;
	else if (group == PIO_INT_GRP_E)
		i = PIO_EINTA_NUM + PIO_EINTB_NUM;
	else if (group == PIO_INT_GRP_G)
		i = PIO_EINTA_NUM + PIO_EINTB_NUM + PIO_EINTE_NUM;
	for (i=0; i<PIO_EINT_TOTAL_NUM; i++) {
		if (eint_no == eint_map[i].eint)
			break;
	}
	if (i == PIO_EINT_TOTAL_NUM) {
		//printk("No GPIO eint for group %d, number %d\n", group, number);
		return -1;
	}
	
	/* config gpio mode for interrupt */
	gpio_set_cfg(eint_map[i].gpio, 1, 0);	// input
	gpio_set_pull(eint_map[i].gpio, 1, 0);	// no pull
	
	/* config gpio irq trigger mode */
	sr32_aw(cfg_base, offs, 4, 0);
	/* disable irq */
	sr32_aw(en_base, number, 1, 0);
	vector[group][number].callback = NULL;
	vector[group][number].data = 0;
	
	return 0;
}

s32 init_gpio(void)
{
	ccm_module_reset(PIO_CKID);
	ccm_clock_enable(PIO_CKID);
	return 0;
}

s32 init_gpio_irq(void)
{
	u32 i = 0;
	s32 irq;

	for (i=0; i<PIO_EINTA_NUM; i++) {
		gpio_irq_a[i].callback = NULL;
		gpio_irq_a[i].data = 0;
	}
	for (i=0; i<PIO_EINTB_NUM; i++) {
		gpio_irq_b[i].callback = NULL;
		gpio_irq_b[i].data = 0;
	}
	for (i=0; i<PIO_EINTE_NUM; i++) {
		gpio_irq_e[i].callback = NULL;
		gpio_irq_e[i].data = 0;
	}
	for (i=0; i<PIO_EINTG_NUM; i++) {
		gpio_irq_g[i].callback = NULL;
		gpio_irq_g[i].data = 0;
	}
	
	irq = irq_request(GIC_SRC_PAEINT, gpio_a_irq_hdle);
	if (irq < 0) {
		//printk("Request gpio eint a hdle failed\n");
	}
	irq_enable(irq);
	
	irq = irq_request(GIC_SRC_PBEINT, gpio_b_irq_hdle);
	if (irq < 0) {
		//printk("Request gpio eint b hdle failed\n");
	}
	irq_enable(irq);
	
	irq = irq_request(GIC_SRC_PEEINT, gpio_e_irq_hdle);
	if (irq < 0) {
		//printk("Request gpio eint e hdle failed\n");
	}
	irq_enable(irq);
	
	irq = irq_request(GIC_SRC_PGEINT, gpio_g_irq_hdle);
	if (irq < 0) {
		//printk("Request gpio eint g hdle failed\n");
	}
	irq_enable(irq);
	return 0;
}
