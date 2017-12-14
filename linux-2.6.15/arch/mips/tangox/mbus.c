
#include <linux/config.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>

#include "setup.h"

/*
 * computed in prom.c
 */
extern unsigned long em8xxx_kmem_start;
extern unsigned long em8xxx_kmem_size;

/*
 * switchbox stuffs
 *
 * We keep  track of  current mapping using  this globals  rather than
 * reading hardware registers each time.
 */
static unsigned int g_sbox_map[SBOX_MAX + 1];

static inline void sbox_update_route(void)
{
	int i;
	unsigned int data;

	for (i = SBOX_MAX, data = 0; i >= 0; --i)
		data = (data << 4) | g_sbox_map[i];

	gbus_writel(REG_BASE_host_interface + SBOX_ROUTE, data);
}

static void sbox_reset(void)
{
	/* Leave W1/R1 alone. */
	gbus_writel(REG_BASE_host_interface + SBOX_FIFO_RESET, 0x7d7dfdfd);
	gbus_writel(REG_BASE_host_interface + SBOX_FIFO_RESET, 0x7d00fd00);
}


static void sbox_setup(void)
{
	/* W0 initially disconnected */
	g_sbox_map[SBOX_MBUS_W0] = 0xf;

	/* Leave W1 alone */
	g_sbox_map[SBOX_MBUS_W1] = 0;

	g_sbox_map[SBOX_PCIMASTER] = 0xf;
	g_sbox_map[SBOX_PCISLAVE] = SBOX_PCISLAVE + 1; /* Loopback */
	g_sbox_map[SBOX_UNUSED1] = 0xf;
	g_sbox_map[SBOX_IDEDVD] = 0xf;
	g_sbox_map[SBOX_IDEFLASH] = 0xf;
	g_sbox_map[SBOX_UNUSED2] = 0xf;

	sbox_update_route();
}

/*
 * Connect given interface to R0/W0 channel
 */
static int sbox_connect(int iface)
{
	unsigned long flags;

	/* Already connected? */
	if (g_sbox_map[SBOX_MBUS_W0] == (iface + 1) && g_sbox_map[iface] == 1)
		return 0;

	/* In use ? */
	if (g_sbox_map[SBOX_MBUS_W0] != 0xf || g_sbox_map[iface] != 0xf)
		return 1;

	local_irq_save(flags);

	g_sbox_map[SBOX_MBUS_W0] = iface + 1;
	g_sbox_map[iface] = 1;
	sbox_update_route();
	wmb();

	local_irq_restore(flags);

	return 0;
}

static void sbox_disconnect(int iface)
{
	unsigned long flags;

	if (iface >= 0) {
		local_irq_save(flags);

		g_sbox_map[SBOX_MBUS_W0] = 0xf;
		g_sbox_map[iface] = 0xf;
		sbox_update_route();
		wmb();

		local_irq_restore(flags);
	}
}

static void sbox_init(void)
{
	sbox_setup();
	sbox_reset();
}


/*
 * mbus stuffs
 *
 * to  avoid   requesting/freeing  irq   each  time,  we   keep  given
 * handler/args  for each  dma  request and  call  it in  our own  irq
 * handler.
 */
#define MBUS_LINEAR_MAX		(0x2000 - 1)

static mbus_irq_handler_t g_mbus_intr_handler[4];
static void *g_mbus_intr_handler_arg[4];

/*
 * alloc_dma, need to be called before setup, will try to connect
 * needed sbox.
 */
int em86xx_mbus_alloc_dma(int sbox, int fromdev, unsigned long *pregbase,
			  int *pirq)
{
	int x;

	if (sbox_connect(sbox) < 0)
		return -1;

	x = (fromdev ? 0 : 2);

	if (pirq)
		*pirq = LOG2_CPU_HOST_MBUS_W0_INT +
			IRQ_CONTROLLER_IRQ_BASE + x;

	if (pregbase)
		*pregbase = REG_BASE_host_interface + MIF_W0_ADD + x * 0x40;

	return 0;
}

/*
 * free_dma,  need to  be called  after  transfer is  done to  release
 * switchbox.
 */
void em86xx_mbus_free_dma(unsigned long regbase, int sbox)
{
	unsigned long flags;
	int idx;

	idx = (regbase - REG_BASE_host_interface - MIF_W0_ADD) / 0x40;

	local_irq_save(flags);
	g_mbus_intr_handler[idx] = NULL;
	wmb();
	local_irq_restore(flags);

	sbox_disconnect(sbox);
}

/*
 * irq handler for mbus interrupt
 */
static irqreturn_t mbus_intr(int irq, void *devinfo, struct pt_regs *regs)
{
	int idx;

	idx = irq - (LOG2_CPU_HOST_MBUS_W0_INT + IRQ_CONTROLLER_IRQ_BASE);

	if (g_mbus_intr_handler[idx]) {
		mbus_irq_handler_t f;

		f = g_mbus_intr_handler[idx];
		g_mbus_intr_handler[idx] = NULL;
		wmb();
		f(irq, g_mbus_intr_handler_arg[idx]);
	}

	return IRQ_HANDLED;
}

/*
 * check if mbus is in use for given regbase
 */
static inline int mbus_inuse(unsigned int regbase)
{
	return (gbus_readl(regbase + MIF_cmd_offset) & 0x7) != 0;
}

/*
 * setup mbus  register to start  a linear transfer (count  bytes from
 * addr, where count < MBUS_LINEAR_MAX)
 */
void mbus_setup_dma_linear(unsigned int regbase,
					 unsigned int addr,
					 unsigned int count)
{
#ifndef CONFIG_SD_DIRECT_DMA
	if ((addr < CPHYSADDR(em8xxx_kmem_start)) || (addr >= (CPHYSADDR(em8xxx_kmem_start) + em8xxx_kmem_size)))
		printk("MBUS Warning (linear): bad transfer address 0x%08x\n", addr);
#endif

	gbus_writel(regbase + MIF_add_offset, tangox_dma_address(addr));
	gbus_writel(regbase + MIF_cnt_offset, count);
	iob();
	gbus_writel(regbase + MIF_cmd_offset, 0x5);
}

/*
 * setup mbus  register to start  a double transfer (count  bytes from
 * addr and count2 bytes from addr2, where count < MBUS_LINEAR_MAX and
 * count2 < MBUS_LINEAR_MAX)
 */
void mbus_setup_dma_double(unsigned int regbase,
					 unsigned int addr,
					 unsigned int count,
					 unsigned int addr2,
					 unsigned int count2)
{
#ifndef CONFIG_SD_DIRECT_DMA
	if ((addr < CPHYSADDR(em8xxx_kmem_start)) || (addr >= (CPHYSADDR(em8xxx_kmem_start) + em8xxx_kmem_size)))
		printk("MBUS Warning (double): bad transfer address 0x%08x\n", addr);
	if ((addr2 < CPHYSADDR(em8xxx_kmem_start)) || (addr2 >= (CPHYSADDR(em8xxx_kmem_start) + em8xxx_kmem_size)))
		printk("MBUS Warning (double): bad transfer address2 0x%08x\n", addr2);
#endif

	gbus_writel(regbase + MIF_add_offset, tangox_dma_address(addr));
	gbus_writel(regbase + MIF_cnt_offset, (count2 << 16) | count);
	gbus_writel(regbase + MIF_add2_skip_offset, tangox_dma_address(addr2));
	iob();
	gbus_writel(regbase + MIF_cmd_offset, 0x6);
}

/*
 * setup mbus  register to start  a rectangle transfer (horiz  * lines
 * bytes  from  addr,  where  horiz  <  MBUS_LINEAR_MAX  and  lines  <
 * MBUS_LINEAR_MAX)
 */
void mbus_setup_dma_rectangle(unsigned int regbase,
					    unsigned int addr,
					    unsigned int horiz,
					    unsigned int lines)
{
#ifndef CONFIG_SD_DIRECT_DMA
	if ((addr < CPHYSADDR(em8xxx_kmem_start)) || (addr >= (CPHYSADDR(em8xxx_kmem_start) + em8xxx_kmem_size)))
		printk("MBUS Warning (rectangle): bad transfer address 0x%08x\n", addr);
#endif

	gbus_writel(regbase + MIF_add_offset, tangox_dma_address(addr));
	gbus_writel(regbase + MIF_cnt_offset, (lines << 16) | horiz);
	gbus_writel(regbase + MIF_add2_skip_offset, horiz);
	iob();
	gbus_writel(regbase + MIF_cmd_offset, 0x7);
}

/*
 * register mbus interrupt if not done
 */
static inline void mbus_register_intr(void)
{
	static int done = 0;

	if (done)
		return;
	done = 1;
	/*
	 * register irq handler for R0/W0 only (R1/W1 are not used for
	 * the moment)
	 */
	request_irq(LOG2_CPU_HOST_MBUS_R0_INT + IRQ_CONTROLLER_IRQ_BASE,
		    mbus_intr, SA_INTERRUPT, "tangox_mbus_r0", NULL);

	request_irq(LOG2_CPU_HOST_MBUS_W0_INT + IRQ_CONTROLLER_IRQ_BASE,
		    mbus_intr, SA_INTERRUPT, "tangox_mbus_w0", NULL);
}

/*
 * start  a   mbus  dma,   use  this  after   a  sucessfull   call  to
 * em86xx_mbus_alloc_dma
 */
int em86xx_mbus_setup_dma(unsigned int regbase, unsigned int addr,
			  unsigned int count, mbus_irq_handler_t handler,
			  void *arg)
{
	unsigned int idx, horiz, lines, sz;
	unsigned long flags;
	idx = (regbase - REG_BASE_host_interface - MIF_W0_ADD) / 0x40;

	/*
	 * make sure no one uses the mbus before
	 */
	if (unlikely(mbus_inuse(regbase))) {
		printk(KERN_ERR "MBUS: error previous command is pending\n");
		return 1;
	}

	/*
	 * "register" given handler if any
	 */
	if (handler) {
		mbus_register_intr();
		local_irq_save(flags);
		g_mbus_intr_handler[idx] = handler;
		g_mbus_intr_handler_arg[idx] = arg;
		wmb();
		local_irq_restore(flags);
	}

	/*
	 * decide which dma function to use depending on count
	 */
	if (count <= MBUS_LINEAR_MAX) {
		mbus_setup_dma_linear(regbase, addr, count);
		return 0;
	}

	if (count <= (MBUS_LINEAR_MAX * 2)) {
		mbus_setup_dma_double(regbase, addr, MBUS_LINEAR_MAX,
				      addr + MBUS_LINEAR_MAX,
				      count - MBUS_LINEAR_MAX);
		return 0;
	}

	/*
	 * we need to use rectangle, compute  horiz & lines
	 * values to use
	 */
	for (idx = 0, horiz = 1, sz = count; (idx < 10) && ((sz & 0x01) == 0); ++idx, horiz <<= 1, sz >>= 1)
		;
	lines = count >> idx;
	if ((horiz > MBUS_LINEAR_MAX) || (lines > MBUS_LINEAR_MAX)) {
		printk(KERN_ERR "MBUS: can't handle rectangle transfer "
		       "of %d bytes (h: %d, v: %d)\n", count, horiz, lines);
		BUG();
	}
	mbus_setup_dma_rectangle(regbase, addr, horiz, lines);

	return 0;
}

/*
 * Note: 06/24/2004 there're errors  in the spec.  The bit assignments
 * should be (* indicates error)
 *
 * Bit 0/8: MBUS_R0_SBOX
 * Bit 1/9: MBUS_R1_SBOX
 * Bit 2/10: PCI_MASTER_SBOX
 * Bit 3/11: PCI_SLAVE_SBOX
 * Bit 4/12: CIPHER_SBOX
 * Bit 5/13: IDE_ISA_SBOX*
 * Bit 6/14: IDE_DVD_SBOX*
 * Bit 7/15: SFLA_SBOX
 * Bit 16/24: SBOX_MBUS_W0*
 * Bit 17/25: SBOX_MBUS_W1*
 * Bit 18/26: SBOX_PCI_MASTER*
 * Bit 19/27: SBOX_PCI_SLAVE*
 * Bit 20/28: SBOX_CIPHER*
 * Bit 21/29: SBOX_ISA*
 * Bit 22/30: SBOX_DVD*
 */
static const unsigned int sbox_reset_vals[2][4] = {
	{ 0x01012020, 0x02022020, 0x20200101, 0x20200202 },
	{ 0x01014040, 0x02024040, 0x40400101, 0x40400202 }
};

static const unsigned int sbox_unreset_vals[2][4] = {
	{ 0x01002000, 0x02002000, 0x20000100, 0x20000200 },
	{ 0x01004000, 0x02004000, 0x40000100, 0x40000200 }
};

/*
 * clear MBUS transaction for given regbase/sbox
 */
static void mbus_reset(unsigned int regbase, int sbox)
{
	int midx;
	int sidx;

	midx = (regbase - REG_BASE_host_interface - MIF_W0_ADD) / 0x40;
	sidx = sbox - SBOX_IDEFLASH;

	if (((midx < 0) || (midx > 3)) || ((sidx < 0) || (sidx > 2))) {
		printk("MBUS reset: out of range, midx %d, sidx %d\n",
		       midx, sidx);
		return;
	}

	gbus_writel(REG_BASE_host_interface + SBOX_FIFO_RESET,
		    sbox_reset_vals[sidx][midx]);
	iob();
	gbus_writel(REG_BASE_host_interface + SBOX_FIFO_RESET,
		    sbox_unreset_vals[sidx][midx]);
	iob();
}

/*
 * busy wait  for current mbus transfer  to finish, will  not wait for
 * more than 200 ms. 0 is ok, 1 timeout, 2 for timeout + reset error.
 */
#define MBUS_TIMEOUT	200000

int em86xx_mbus_wait(unsigned int regbase, int sbox)
{

	int timeout;

	/* wait for mbus to be released */
	timeout = 0;
	do {
		if (!mbus_inuse(regbase))
			break;
		udelay(1);
		timeout++;
	} while (timeout < MBUS_TIMEOUT);

	if (timeout < MBUS_TIMEOUT ) {
		/* ok */
		if(sbox == SBOX_IDEFLASH){
                        int i;
			unsigned int pb_count = 0;

			pb_count = gbus_readl( REG_BASE_host_interface + 
					       PB_automode_control) & 0xffff;

                        for (i = 0; pb_count && (i < MBUS_TIMEOUT); i++){
                                udelay(1);
				pb_count = gbus_readl( REG_BASE_host_interface +
					       PB_automode_control) & 0xffff;
			}

                        if (i < MBUS_TIMEOUT) 
				return 0;

		} else
			return 0;
	}

	/* timeout, let's dump some registers ! */
        if (sbox == SBOX_IDEFLASH) {
  		printk("MBUS timeout : MBUS CMD = %ld, PB Automode = %08x\n",
                	(unsigned long)gbus_readl(regbase + MIF_cmd_offset) & 0x7,
                	(unsigned int)gbus_readl(REG_BASE_host_interface + PB_automode_control));
        } else {
		printk("MBUS timeout : MBUS CMD = %08lx\n",
			gbus_readl(regbase + MIF_cmd_offset) & 0x7);
	}

	printk("MBUS registers : %08lx %08lx %08lx %08lx\n",
	       gbus_readl(regbase + MIF_add_offset),
	       gbus_readl(regbase + MIF_cnt_offset),
	       gbus_readl(regbase + MIF_add2_skip_offset),
	       gbus_readl(regbase + MIF_cmd_offset));

	printk(KERN_ERR "MBUS fails, resetting %d ..\n", sbox);
	mbus_reset(regbase, sbox);

	/* If not able to reset, return  1, so the DMA can be disabled
	   accordingly  */
	return mbus_inuse(regbase) ? 0 : 1;
}


int em86xx_mbus_init(void)
{
	static int done = 0;

	if (done)
		return 0;
	done = 1;

	/* reset sbox to default values */
	sbox_init();

#if defined(CONFIG_BLK_DEV_BMIDE_TANGOX_DMA) || defined(CONFIG_BLK_DEV_PBIDE_TANGOX_DMA)
	/* give maximum MBUS bandwidth for IDE */
	gbus_writel(REG_BASE_system_block + MARB_mid02_cfg, 0x11f1f);
	gbus_writel(REG_BASE_system_block + MARB_mid22_cfg, 0x11f1f);
#endif

	return 0;
}

EXPORT_SYMBOL(em86xx_mbus_alloc_dma);
EXPORT_SYMBOL(em86xx_mbus_free_dma);
EXPORT_SYMBOL(em86xx_mbus_setup_dma);
EXPORT_SYMBOL(em86xx_mbus_wait);
EXPORT_SYMBOL(em86xx_mbus_init);
EXPORT_SYMBOL(mbus_setup_dma_linear);
EXPORT_SYMBOL(mbus_setup_dma_double);
EXPORT_SYMBOL(mbus_setup_dma_rectangle);
