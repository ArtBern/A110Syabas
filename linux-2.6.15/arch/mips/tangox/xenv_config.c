/*
 * Check and  read full xenv config  at boot if valid,  else will stop
 * boot process or use failsafe values.
 */

#include "setup.h"
#include "xenv.h"
#include "xenvkeys.h"

#include <linux/module.h>
#include <asm/bootinfo.h>

/*
 * use CPU_remap4 to access XENV content
 */
#define REMAPPED_REG	CPU_remap4
#define REMAPPED_BASE	KSEG1ADDR(CPU_remap4_address)

/*
 * cached values of xenv content
 */
#define XENV_MAX_FLASH_PARTITIONS   16


/*
 * default is  to have one  partition on each  flash at offset  0 that
 * span all the flash. If CONFIG_TANGOX_XENV_DEF_CSx_SIZE is set to 0,
 * cs will be ignored.
 */
#ifdef CONFIG_TANGOX_XENV_READ_SAFE
/* The data will be filled from XENV later */
static u32 cs_flash_size[4] = { 0, 0, 0, 0 };
static u32 flash_parts_size[4][XENV_MAX_FLASH_PARTITIONS] = { { 0 }, { 0 }, { 0 }, { 0 }, };
static u32 enabled_devices = 0;
static u32 uart_baudrate = 0;
static u32 uart_baudrates[2] = { 0, 0 };
static u32 uart_used_ports = 0;
static u32 pcidev_irq_route[4] = { 0, 0, 0, 0 };
static u32 uart_console_port = 0;
#else
static u32 cs_flash_size[4] = {
	CONFIG_TANGOX_XENV_DEF_CS0_SIZE,
	CONFIG_TANGOX_XENV_DEF_CS1_SIZE,
	CONFIG_TANGOX_XENV_DEF_CS2_SIZE,
	CONFIG_TANGOX_XENV_DEF_CS3_SIZE
};

static u32 flash_parts_size[4][XENV_MAX_FLASH_PARTITIONS] = {
	{ CONFIG_TANGOX_XENV_DEF_CS0_SIZE },
	{ CONFIG_TANGOX_XENV_DEF_CS1_SIZE },
	{ CONFIG_TANGOX_XENV_DEF_CS2_SIZE },
	{ CONFIG_TANGOX_XENV_DEF_CS3_SIZE },
};

static u32 enabled_devices =
#ifdef CONFIG_TANGOX_XENV_DEF_PCI_ID1 
	(CONFIG_TANGOX_XENV_DEF_PCI_ID1 << PCI1_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_PCI_ID2 
	(CONFIG_TANGOX_XENV_DEF_PCI_ID2 << PCI2_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_PCI_ID3 
	(CONFIG_TANGOX_XENV_DEF_PCI_ID3 << PCI3_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_PCI_ID4 
	(CONFIG_TANGOX_XENV_DEF_PCI_ID4 << PCI4_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_ENET 
	(CONFIG_TANGOX_XENV_DEF_ENET << ETHERNET_SHIFT) |
#else
	0 | 
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_FIP 
	(CONFIG_TANGOX_XENV_DEF_FIP << FIP_SHIFT) |
#else 
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_I2CM 
	(CONFIG_TANGOX_XENV_DEF_I2CM << I2CM_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_I2CS 
	(CONFIG_TANGOX_XENV_DEF_I2CS << I2CS_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_BMIDE 
	(CONFIG_TANGOX_XENV_DEF_BMIDE << BMIDE_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_ISAIDE 
	(CONFIG_TANGOX_XENV_DEF_ISAIDE << ISAIDE_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_IR 
	(CONFIG_TANGOX_XENV_DEF_IR << IR_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_PCIHOST
	(CONFIG_TANGOX_XENV_DEF_PCIHOST << PCIHOST_SHIFT) |
#else
	0 |
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_USB 
	(CONFIG_TANGOX_XENV_DEF_USB << USB_SHIFT)
#else
	0
#endif
	;

static u32 uart_baudrate = CONFIG_TANGOX_XENV_DEF_BAUDRATE;
static u32 uart_baudrates[2] = { CONFIG_TANGOX_XENV_DEF_BAUDRATE, CONFIG_TANGOX_XENV_DEF_BAUDRATE };

static u32 uart_used_ports = 
#ifdef CONFIG_TANGOX_XENV_DEF_UART0
		1 +
#else
		0 +
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_UART1
		1;
#else
		0;
#endif

static u32 pcidev_irq_route[4] = { 
#ifdef CONFIG_TANGOX_XENV_DEF_PCI_ID1_IRQ
	CONFIG_TANGOX_XENV_DEF_PCI_ID1_IRQ,
#else
	0,
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_PCI_ID2_IRQ
	CONFIG_TANGOX_XENV_DEF_PCI_ID2_IRQ,
#else
	0,
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_PCI_ID3_IRQ
	CONFIG_TANGOX_XENV_DEF_PCI_ID3_IRQ,
#else
	0,
#endif
#ifdef CONFIG_TANGOX_XENV_DEF_PCI_ID4_IRQ 
	CONFIG_TANGOX_XENV_DEF_PCI_ID4_IRQ 
#else
	0
#endif
};

static u32 uart_console_port = CONFIG_TANGOX_XENV_DEF_CONSOLE_UART_PORT;
#endif

static u32 cs_flash_parts[4] = { 1, 1, 1, 1 };
static u32 flash_parts_offset[4][XENV_MAX_FLASH_PARTITIONS] = { { 0 }, { 0 }, { 0 }, { 0 }, };
static u32 scard_off_pin = 0;
static u32 scard_cmd_pin = 0;
static u32 scard_5v_pin = 0;
static u32 isaide_timing_slot = 0;
static u32 isaide_irq = 0;

static u32 xenv_gbus_addr = 0;

/* mac address to use if xenv is not readable  */
static const u8 def_mac_address[6] = { 0x48, 0x4a, 0xe5, 0x00, 0x00, 0x01 };
static u8 mac_address[6];

/* for power saving (e.g. low frequency */
static u32 ps_pll3=0, ps_pll2=0, ps_pll1=0, ps_pll0=0, ps_mux=0, ps_d0cfg=0, ps_d1cfg=0, ps_d0delay=0, ps_d1delay=0;

static char xenv_cmdline[CL_SIZE] = { 0 };

#ifdef CONFIG_TANGOX_XENV_READ
/*
 * called for each entry found in xenv
 */
void __init xenv_val_cb(char *recordname, void *data, u32 datasize)
{
	char buf[64];
	int i;

#define CHECK_AND_STORE(_key, _reqlen, _var)				\
	if (!strcmp(_key, recordname) && datasize <= _reqlen)	{	\
		memcpy(&_var, data, _reqlen);				\
		return;							\
	}

	CHECK_AND_STORE(XENV_KEY_ENABLED_DEVICES, 4, enabled_devices);
	CHECK_AND_STORE(XENV_KEY_DEF_BAUDRATE, 4, uart_baudrate);
	CHECK_AND_STORE(XENV_KEY_UART_USED_PORTS, 4, uart_used_ports);
	CHECK_AND_STORE(XENV_KEY_CONSOLE_UART_PORT, 4, uart_console_port);

	if (uart_console_port == 0) /* for backward compatibility */
		uart_used_ports |= 1;

	for (i = 0; i < 2; i++) {
		sprintf(buf, XENV_KEYS_UART_BAUDRATE, i);
		CHECK_AND_STORE(buf, 4, uart_baudrates[i]);
	}

	if (uart_baudrate == 0)
		uart_baudrate = 115200; /* default 115200 */
	if (uart_baudrates[0] == 0)
		uart_baudrates[0] = uart_baudrate;
	if (uart_baudrates[1] == 0)
		uart_baudrates[1] = uart_baudrate;

	for (i = 1; i < 5; i++) {
		sprintf(buf, XENV_KEYS_PCI_IRQ_ROUTE, i);
		CHECK_AND_STORE(buf, 4, pcidev_irq_route[i - 1]);
	}

	CHECK_AND_STORE(XENV_KEY_SCARD_OFF, 4, scard_off_pin);
	CHECK_AND_STORE(XENV_KEY_SCARD_5V, 4, scard_5v_pin);
	CHECK_AND_STORE(XENV_KEY_SCARD_CMD, 4, scard_cmd_pin);

	for (i = 0; i < 4; i++) {
		int j;

		sprintf(buf, XENV_KEYS_CS_SIZE, i);
		CHECK_AND_STORE(buf, 4, cs_flash_size[i]);

		sprintf(buf, XENV_KEYS_CS_PARTS, i);
		CHECK_AND_STORE(buf, 4, cs_flash_parts[i]);

		for (j = 1; j < XENV_MAX_FLASH_PARTITIONS; j++) {

			sprintf(buf, XENV_KEYS_CS_PART_SIZE, i, j);
			CHECK_AND_STORE(buf, 4, flash_parts_size[i][j - 1]);

			sprintf(buf, XENV_KEYS_CS_PART_OFFSET, i, j);
			CHECK_AND_STORE(buf, 4, flash_parts_offset[i][j - 1]);
		}
	}

	CHECK_AND_STORE(XENV_KEY_ISAIDE_IRQ_ROUTE, 4, isaide_irq);
	CHECK_AND_STORE(XENV_KEY_ISAIDE_TIMING_SLOT, 4, isaide_timing_slot);

	if (!strcmp(recordname, XENV_KEY_LINUX_CMD) &&
	    datasize <= sizeof (xenv_cmdline) - 1) {
		memcpy(xenv_cmdline, data, datasize);
		xenv_cmdline[datasize] = 0;
	}

	CHECK_AND_STORE(XENV_KEY_PS_PLL3, 4, ps_pll3);
	CHECK_AND_STORE(XENV_KEY_PS_PLL2, 4, ps_pll2);
	CHECK_AND_STORE(XENV_KEY_PS_PLL1, 4, ps_pll1);
	CHECK_AND_STORE(XENV_KEY_PS_PLL0, 4, ps_pll0);
	CHECK_AND_STORE(XENV_KEY_PS_MUX, 4, ps_mux);
	CHECK_AND_STORE(XENV_KEY_PS_D0CFG, 4, ps_d0cfg);
	CHECK_AND_STORE(XENV_KEY_PS_D1CFG, 4, ps_d1cfg);
	CHECK_AND_STORE(XENV_KEY_PS_D0DELAY, 4, ps_d0delay);
	CHECK_AND_STORE(XENV_KEY_PS_D1DELAY, 4, ps_d1delay);
}

/*
 * try to read config from XENV
 */
static int __init xenv_read_content(void)
{
	unsigned long xenv_addr;
	int xenv_size;
	uint32_t mac_lo, mac_hi;

	/*
	 * fetch XENV address
	 */
	xenv_gbus_addr = xenv_addr = gbus_readl(REG_BASE_cpu_block + LR_XENV_LOCATION);
	if (!xenv_addr)
		return 1;

	/*
	 * got the xenv address in  gbus form, now convert it in remap
	 * form so we can access it
	 */
	gbus_writel(REG_BASE_cpu_block + REMAPPED_REG, xenv_addr & 0xfc000000);
	iob();
	xenv_addr = REMAPPED_BASE + (xenv_addr & 0x03ffffff);

	/*
	 * check xenv sanity
	 */
	xenv_size = xenv_isvalid((u32 *)xenv_addr, MAX_XENV_SIZE);
	if (xenv_size < 0)
		return 1;

#ifdef CONFIG_TANGOX_XENV_DUMP
	xenv_dump((u32 *)xenv_addr, xenv_size);
#endif

	/*
	 * ok, we can start to load each wanted value
	 */
	xenv_foreach((u32 *)xenv_addr, xenv_size, xenv_val_cb);

	/*
	 * load remaining values
	 */
	mac_hi = gbus_readl(REG_BASE_cpu_block + LR_ETH_MAC_HI);
	mac_lo = gbus_readl(REG_BASE_cpu_block + LR_ETH_MAC_LO);
	mac_hi = cpu_to_be32(mac_hi);
	mac_lo = cpu_to_be32(mac_lo);
	memcpy(mac_address, (u8 *)&mac_hi + 2, 2);
	memcpy(mac_address + 2, &mac_lo, 4);

	return 0;
}
#endif

/*
 * load default values and try to fetch xenv content
 */
int __init xenv_config(void)
{
#ifndef CONFIG_TANGOX_XENV_READ
	/* will use default values */
	return 0;
#else
	/*
	 * try to load XENV content
	 */
	if (xenv_read_content() == 0) {
		/* ok */
		return 0;
	}

#ifndef CONFIG_TANGOX_XENV_READ_SAFE
	/* fallback to failsafe values */
	return 1;
#else
	/* stop boot process */
	while (1)
		cpu_relax();
	/* not reached */
	return 1;
#endif

#endif /* !CONFIG_TANGOX_XENV_READ */
}


/*
 * helpers to access xenv configuration cached data
 */

/*
 * enabled device query function
 */
#define BUILD_ENABLED(name, shift)					\
int tangox_##name##_enabled(void)					\
{									\
	return (((enabled_devices >> shift) & 1) != 0) ? 1 : 0;	\
} \
EXPORT_SYMBOL(tangox_##name##_enabled);

BUILD_ENABLED(isaide, ISAIDE_SHIFT)
BUILD_ENABLED(bmide, BMIDE_SHIFT)
BUILD_ENABLED(ir, IR_SHIFT)
BUILD_ENABLED(fip, FIP_SHIFT)
BUILD_ENABLED(ethernet, ETHERNET_SHIFT)
BUILD_ENABLED(usb, USB_SHIFT)
BUILD_ENABLED(sdio, SDIO_SHIFT)
BUILD_ENABLED(i2cm, I2CM_SHIFT)
BUILD_ENABLED(i2cs, I2CS_SHIFT)
BUILD_ENABLED(pci_host, PCIHOST_SHIFT)
BUILD_ENABLED(sata, SATA_SHIFT)
BUILD_ENABLED(scard, SCARD_SHIFT)
BUILD_ENABLED(gnet, GNET_SHIFT)

int tangox_pcidev_enabled(int idsel)
{
	if (!tangox_pci_host_enabled())
		return 0;

	idsel--;
	return (((enabled_devices >> (idsel + PCI1_SHIFT)) & 1) != 0) ? 1 : 0;
}

int tangox_pcidev_irq_map(int pci_idsel, int int_num)
{
	int route;
	int irq;

	route = pcidev_irq_route[pci_idsel - 1];

	/* int_num: 0-3 = INTA-D */
	irq = (int)((route >> (int_num * 8)) & 0x3);
	if (irq >= 0)
		irq += (IRQ_CONTROLLER_IRQ_BASE + LOG2_CPU_PCI_INTA);
	return irq;
}

int tangox_isaide_irq_map(void)
{
	int irq = 0;

	if (tangox_isaide_enabled() == 0)
		return(-1);
	irq = isaide_irq;
	if (irq >= 0)
		irq += (IRQ_CONTROLLER_IRQ_BASE + LOG2_CPU_PCI_INTA);
	return irq;
}

int tangox_isaide_cs_select(void)
{
        int i;
        unsigned long cs_config = (gbus_readl(REG_BASE_host_interface + PB_CS_config) >> 12) & 0xf;

        if (tangox_isaide_enabled() == 0)
                return(-1);

        for (i = 0; i < 4; i++) {
                if ((cs_config & 0x1) != 0)
                        return(i);
                else
                        cs_config >>= 1;
        }
        return(-1);
}

int tangox_isaide_timing_slot(void)
{
        return(isaide_timing_slot & 0x7);
}

EXPORT_SYMBOL(tangox_isaide_irq_map);
EXPORT_SYMBOL(tangox_isaide_cs_select);
EXPORT_SYMBOL(tangox_isaide_timing_slot);

int tangox_ethernet_getmac(unsigned char *mac)
{
	/* filter broadcast & multicast addresses */
	if (mac_address[0] == 0x01 || mac_address[0] == 0xff)
		memcpy(mac, def_mac_address, 6);
	else
		memcpy(mac, mac_address, 6);
	return 0;
}

int tangox_uart_baudrate(int uart)
{
	return uart_baudrates[uart];
}

int tangox_uart_console_port(void)
{
	return uart_console_port;
}

int tangox_uart_enabled(int uart)
{
	return (((uart_used_ports >= 2) || (uart_console_port == uart)) ? 1 : 0);
}

int tangox_flash_get_info(int cs, unsigned int *size, unsigned int *part_count)
{
	if (cs > 3)
		return 1;

	*size = cs_flash_size[cs];
	*part_count = 0;
	if (cs_flash_size[cs] > 0)
		*part_count = cs_flash_parts[cs];

	return 0;
}

int tangox_flash_get_parts(int cs, unsigned int offset[], unsigned int size[])
{
	int i;

	if (!cs_flash_size[cs])
		return 1;

	for (i = 0; i < cs_flash_parts[cs]; i++) {
		offset[i] = flash_parts_offset[cs][i];
		size[i] = flash_parts_size[cs][i];
	}

	return 0;
}

const char *tangox_xenv_cmdline(void)
{
	/* remove "" from command line */
	if (xenv_cmdline[0] == '"') {
		int len;

		len = strlen(xenv_cmdline);
		if (xenv_cmdline[len - 1] == '"')
			xenv_cmdline[len - 1] = 0;
		return xenv_cmdline + 1;
	}
	return xenv_cmdline;
}

int tangox_get_scard_info(int *pin_5v, int *pin_cmd, int *pin_off)
{
	*pin_5v = scard_5v_pin;
	*pin_off = scard_off_pin;
	*pin_cmd = scard_cmd_pin;
	return 0;
}

/*
 * show enabled devices according to xenv content
 */
void __init tangox_device_info(void)
{
	int i;

	printk(KERN_INFO "SMP863x/SMP865x Enabled Devices under Linux/"
	       "XENV 0x%08x = 0x%08x\n", xenv_gbus_addr, enabled_devices);

	printk(KERN_INFO);
	if (tangox_isaide_enabled())
		printk(" ISA/IDE");
	if (tangox_bmide_enabled())
		printk(" BM/IDE");
	if (tangox_pci_host_enabled())
		printk(" PCIHost");
	if (tangox_ethernet_enabled())
		printk(" Ethernet");
	if (tangox_ir_enabled())
		printk(" IR");
	if (tangox_fip_enabled())
		printk(" FIP");
	if (tangox_i2cm_enabled())
		printk(" I2CM");
	if (tangox_i2cs_enabled())
		printk(" I2CS");
	if (tangox_sdio_enabled())
		printk(" SDIO");
	if (tangox_usb_enabled())
		printk(" USB");
	for (i = 1; i <= 6; i++) {
		if (tangox_pcidev_enabled(i))
			printk(" PCIDev%d", i);
	}
	if (tangox_sata_enabled())
		printk(" SATA");
	if (tangox_scard_enabled())
		printk(" SCARD");
	if (tangox_gnet_enabled())
		printk(" GNET");
	printk("\n");
}

void tangox_get_ps_config(u32 *pll3, u32 *pll2, u32 *pll1, u32 *pll0, u32 *mux, u32 *d0cfg, u32 *d0delay, u32 *d1cfg, u32 *d1delay)
{
	*pll3 = ps_pll3;
	*pll2 = ps_pll2;
	*pll1 = ps_pll1;
	*pll0 = ps_pll0;
	*mux = ps_mux;
	*d0cfg = ps_d0cfg;
	*d1cfg = ps_d1cfg;
	*d0delay = ps_d0delay;
	*d1delay = ps_d1delay;
}

EXPORT_SYMBOL(tangox_ethernet_getmac);
EXPORT_SYMBOL(tangox_get_scard_info);
EXPORT_SYMBOL(tangox_get_ps_config);

