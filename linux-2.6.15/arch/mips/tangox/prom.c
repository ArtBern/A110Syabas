
#include <linux/init.h>
#include <asm/bootinfo.h>
#include <asm/page.h>
#include <linux/module.h>
#include <linux/mm.h>

#include "setup.h"

/*
 * em8xxx_sys_frequency may be used later in the serial  code, DON'T mark
 * it as initdata
 */
unsigned long em8xxx_sys_frequency;
unsigned long em8xxx_cpu_frequency;
unsigned long em8xxx_kmem_start;
unsigned long em8xxx_kmem_size;

unsigned long tangox_chip_id(void);
int is_tango2_chip(void);
int is_tango3_chip(void);

/*
 * we will restore remap registers before rebooting
 */
unsigned long em8xxx_remap_registers[4];

/*
 * helper to access base registers
 */
#define RD_BASE_REG32(r)	\
		gbus_readl(REG_BASE_system_block + (r))

/*
 * return system type (/proc/cpuinfo)
 */
const char *get_system_type(void)
{
	return "Sigma Designs TangoX";
}

/*
 * return system frequency
 */
#ifdef CONFIG_TANGOX_SYS_FREQUENCY
unsigned long tangox_get_sysclock(void)
{
	return(CONFIG_TANGOX_SYS_FREQUENCY);
}
#else
unsigned long tangox_get_sysclock(void)
{
#if (defined(CONFIG_TANGO2) && (EM86XX_REVISION > 3)) || defined(CONFIG_TANGO3)
	unsigned long sys_clkgen_pll, sysclk_mux, sysclk_premux;
	unsigned long n, m, freq, div, k, mux;

	k = m = sys_clkgen_pll = 0;
	sysclk_mux = RD_BASE_REG32(SYS_sysclk_mux);
	sysclk_premux = RD_BASE_REG32(SYS_sysclk_premux);

	switch (sysclk_premux & 0x3) {
		case 0:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen0_pll);
			m = (sys_clkgen_pll >> 16) & 0x1f;
			k = (sys_clkgen_pll >> 14) & 0x3;
			break;
		case 1:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen1_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
		case 2:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen2_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
		case 3:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen3_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
	}
	n = sys_clkgen_pll & 0x000003ff;

	/* Not using XTAL_IN, cannot calculate */
	if ((sys_clkgen_pll & 0x07000000) != 0x01000000)
		return(0);

	/* Calculate the divider */
	mux = (sysclk_mux >> 8) & 0xf;
	if (mux == 0) /* Get system clock frequency */
		div = 2;
	else if ((mux == 1) || ((mux >= 8) && (mux < 0xc)))
		div = 4;
	else if ((mux >= 2) && (mux < 8))
		div = 3;
	else
		return(0); /* Wrong divider setting */

	if (sysclk_mux & 0x1) 	/* PLL is used */
		freq = ((TANGOX_BASE_FREQUENCY / (m + 2)) * (n + 2)) /
			(div * (1 << k));
	else
		freq = TANGOX_BASE_FREQUENCY / div;
#else
	unsigned long sys_clkgen_pll, sysclk_mux, n, m, freq, div;

	sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen_pll);
	sysclk_mux = RD_BASE_REG32(SYS_sysclk_mux);
	n = sys_clkgen_pll & 0x000003ff;
	m = (sys_clkgen_pll & 0x003f0000) >> 16;

	/* Calculate the divider */
	if ((sysclk_mux & 0x300) == 0x000) /* Get system clock frequency */
		div = 2;
	else if ((sysclk_mux & 0x300) == 0x100)
		div = 4;
	else
		div = 3;

	if (sysclk_mux & 0x1) 	/* PLL is used */
		freq = ((TANGOX_BASE_FREQUENCY / (m + 2)) * (n + 2)) / div;
	else
		freq = TANGOX_BASE_FREQUENCY / div;
#endif
	return(freq);
}
#endif

/*
 * return cpu frequency
 */
#ifdef CONFIG_TANGOX_CPU_FREQUENCY
unsigned long tangox_get_cpuclock(void)
{
	return(CONFIG_TANGOX_CPU_FREQUENCY);
}
#else
unsigned long tangox_get_cpuclock(void)
{
#if (defined(CONFIG_TANGO2) && (EM86XX_REVISION > 3)) || defined(CONFIG_TANGO3)
	unsigned long sys_clkgen_pll, sysclk_mux, sysclk_premux;
	unsigned long n, m, freq, div, k, mux;

	k = m = sys_clkgen_pll = 0;
	sysclk_mux = RD_BASE_REG32(SYS_sysclk_mux);
	sysclk_premux = RD_BASE_REG32(SYS_sysclk_premux);

	switch(sysclk_premux & 0x3) {
		case 0:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen0_pll);
			m = (sys_clkgen_pll >> 16) & 0x1f;
			k = (sys_clkgen_pll >> 14) & 0x3;
			break;
		case 1:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen1_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
		case 2:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen2_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
		case 3:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen3_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
	}
	n = sys_clkgen_pll & 0x000003ff;

	/* Not using XTAL_IN, cannot calculate */
	if ((sys_clkgen_pll & 0x07000000) != 0x01000000)
		return(0);

	/* Calculate the divider */
	mux = (sysclk_mux >> 8) & 0xf;
	if ((mux == 3) || (mux == 4) || (mux == 6)) /* Get CPU frequency */
		div = 3;
	else if ((mux == 8) || (mux == 0xa))
		div = 4;
	else if ((mux == 0) || (mux == 1) || (mux == 2) ||
		 (mux == 5) || (mux == 7) ||
		 (mux == 9) || (mux == 0xb))
		div = 2;
	else
		return(0); /* Wrong divider setting */

	if (sysclk_mux & 0x1) 	/* PLL is used */
		freq = ((TANGOX_BASE_FREQUENCY / (m + 2)) * (n + 2)) /
			(div * (1 << k));
	else
		freq = TANGOX_BASE_FREQUENCY / div;
#else
	unsigned long sys_clkgen_pll, sysclk_mux, n, m, freq, div;

	sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen_pll);
	sysclk_mux = RD_BASE_REG32(SYS_sysclk_mux);
	n = sys_clkgen_pll & 0x000003ff;
	m = (sys_clkgen_pll & 0x003f0000) >> 16;

	if ((sysclk_mux & 0x300) == 0x300) /* Get CPU frequency */
		div = 3;
	else
		div = 2;

	/* Calculate the divider */
	if (sysclk_mux & 0x1) 	/* PLL is used */
		freq = ((TANGOX_BASE_FREQUENCY / (m + 2)) * (n + 2)) / div;
	else
		freq = TANGOX_BASE_FREQUENCY / div;
#endif
	return(freq);
}
#endif

#ifdef CONFIG_TANGOX_FIXED_FREQUENCIES
unsigned long tangox_get_dspclock(void)
{
	return(0);
}
#else
unsigned long tangox_get_dspclock(void)
{
#if (defined(CONFIG_TANGO2) && (EM86XX_REVISION > 3)) || defined(CONFIG_TANGO3)
	unsigned long sys_clkgen_pll, sysclk_mux, sysclk_premux;
	unsigned long n, m, freq, div, k, mux;

	k = m = sys_clkgen_pll = 0;
	sysclk_mux = RD_BASE_REG32(SYS_sysclk_mux);
	sysclk_premux = RD_BASE_REG32(SYS_sysclk_premux);

	switch(sysclk_premux & 0x3) {
		case 0:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen0_pll);
			m = (sys_clkgen_pll >> 16) & 0x1f;
			k = (sys_clkgen_pll >> 14) & 0x3;
			break;
		case 1:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen1_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
		case 2:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen2_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
		case 3:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen3_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
	}
	n = sys_clkgen_pll & 0x000003ff;

	/* Not using XTAL_IN, cannot calculate */
	if ((sys_clkgen_pll & 0x07000000) != 0x01000000)
		return(0);

	/* Calculate the divider */
	mux = (sysclk_mux >> 8) & 0xf;
	if ((mux >= 2) && (mux <= 5)) /* Get DSP frequency */
		div = 3;
	else if ((mux == 1) || (mux == 8) || (mux == 9))
		div = 4;
	else if ((mux == 0) || (mux == 6) || (mux == 7) ||
		 (mux == 0xa) || (mux == 0xb))
		div = 2;
	else
		return(0); /* Wrong divider setting */

	if (sysclk_mux & 0x1) 	/* PLL is used */
		freq = ((TANGOX_BASE_FREQUENCY / (m + 2)) * (n + 2)) /
			(div * (1 << k));
	else
		freq = TANGOX_BASE_FREQUENCY / div;
#else
	unsigned long sys_clkgen_pll, sysclk_mux, n, m, freq, div;

	sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen_pll);
	sysclk_mux = RD_BASE_REG32(SYS_sysclk_mux);
	n = sys_clkgen_pll & 0x000003ff;
	m = (sys_clkgen_pll & 0x003f0000) >> 16;

	/* Calculate the divider */
	if ((sysclk_mux & 0x300) == 0x000) /* Get system clock frequency */
		div = 2;
	else if ((sysclk_mux & 0x300) == 0x100)
		div = 4;
	else
		div = 3;

	if (sysclk_mux & 0x1) 	/* PLL is used */
		freq = ((TANGOX_BASE_FREQUENCY / (m + 2)) * (n + 2)) / div;
	else
		freq = TANGOX_BASE_FREQUENCY / div;
#endif
	return(freq);
}
#endif

#ifdef CONFIG_TANGOX_FIXED_FREQUENCIES
unsigned long tangox_get_pllclock(int pll)
{
	return(0);
}
#else
unsigned long tangox_get_pllclock(int pll)
{
#if (defined(CONFIG_TANGO2) && (EM86XX_REVISION > 3)) || defined(CONFIG_TANGO3)
	unsigned long sys_clkgen_pll, sysclk_mux;
	unsigned long n, m, freq, k;

	k = m = sys_clkgen_pll = 0;
	sysclk_mux = RD_BASE_REG32(SYS_sysclk_mux);

	switch(pll) {
		case 0:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen0_pll);
			m = (sys_clkgen_pll >> 16) & 0x1f;
			k = (sys_clkgen_pll >> 14) & 0x3;
			break;
		case 1:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen1_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
		case 2:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen2_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
		case 3:
			sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen3_pll);
			m = (sys_clkgen_pll >> 16) & 0x7f;
			break;
	}
	n = sys_clkgen_pll & 0x000003ff;

	/* Not using XTAL_IN, cannot calculate */
	if ((sys_clkgen_pll & 0x07000000) != 0x01000000)
		return(0);

	if (sysclk_mux & 0x1) 	/* PLL is used */
		freq = ((TANGOX_BASE_FREQUENCY / (m + 2)) * (n + 2)) / (1 << k);
	else
		freq = TANGOX_BASE_FREQUENCY;
#else
	unsigned long sys_clkgen_pll, sysclk_mux, n, m, freq;

	sys_clkgen_pll = RD_BASE_REG32(SYS_clkgen_pll);
	sysclk_mux = RD_BASE_REG32(SYS_sysclk_mux);
	n = sys_clkgen_pll & 0x000003ff;
	m = (sys_clkgen_pll & 0x003f0000) >> 16;

	if (sysclk_mux & 0x1) 	/* PLL is used */
		freq = (TANGOX_BASE_FREQUENCY / (m + 2)) * (n + 2);
	else
		freq = TANGOX_BASE_FREQUENCY;
#endif
	return(freq);
}
#endif

extern int do_syslog(int type, char * buf, int len);
extern int __init xenv_config(void);
extern void __init tangox_device_info(void);
extern const char *tangox_xenv_cmdline(void);

void __init prom_init(void)
{
	extern char _text;
	unsigned long offset, em8xxx_kmem_end;
	memcfg_t *m;
	int clksel, xenv_res, i;
	char *revStr = NULL;

	/*
	 * save remap registers for reboot time
	 */
	for (i = 0; i < 4; i++) {
		em8xxx_remap_registers[i] =
			gbus_readl(REG_BASE_cpu_block + CPU_remap + i * 4);
	}

	/* 
	 * Set remap so that 0x1fc00000 and 0x0 back to they should be...
	 */
	gbus_writel(REG_BASE_cpu_block + CPU_remap, 0x1fc00000);
	gbus_writel(REG_BASE_cpu_block + CPU_remap1, 0x0);
	iob();
	
#if defined(CONFIG_TANGO2_SMP863X)
	printk("Configured for SMP863%c (revision %s), ",
#if defined(CONFIG_TANGO2_ES1)
			'0', "ES1"
#elif defined(CONFIG_TANGO2_ES2)
			'0', "ES2"
#elif defined(CONFIG_TANGO2_ES3)
			'0', "ES3"
#elif defined(CONFIG_TANGO2_ES4)
			'4', "ES4"
#elif defined(CONFIG_TANGO2_ES5)
			'4', "ES5"
#elif defined(CONFIG_TANGO2_ES6)
			'4', "ES6/RevA"
#elif defined(CONFIG_TANGO2_ES7)
			'4', "ES7/RevB"
#elif defined(CONFIG_TANGO2_ES8)
			'4', "ES8"
#elif defined(CONFIG_TANGO2_ES9)
			'4', "ES9/RevC"
#endif
	      );
#elif defined(CONFIG_TANGO3_SMP865X)
#if defined(CONFIG_TANGO3_ES1)
	printk("Configured for SMP865x (revision ES1), ");
#else
#error Unsupported platform.
#endif
#else
#error Unsupported platform.
#endif
	printk("detected SMP%lx (revision ", (tangox_chip_id()>>16)&0xffff);
	if (is_tango2_chip()) {
		unsigned long revision = tangox_chip_id() & 0xff;
		switch(revision) {
			case 0x81: /* ES1-3 */
				revStr = "ES1-3";
				break;
			case 0x82: /* ES4-5 */
				revStr = "ES4-5";
				break;
			case 0x83: /* ES6/RevA */
				revStr = "ES6/RevA";
				break;
			case 0x84: /* ES7/RevB */
				revStr = "ES7/RevB";
				break;
			case 0x85: /* ES8 */
				revStr = "ES8";
				break;
			case 0x86: /* ES9/RevC */
				revStr = "ES9/RevC";
				break;
		}
	} else if (is_tango3_chip()) {
		revStr = "ES1";
	} else
		revStr = "unknown";
	
	printk("%s).\n", revStr);

	/*
	 * read xenv  configuration, we  need it quickly  to configure
	 * console accordingly.
	 *
	 * NOTE: We  may stay STUCK in  this if safe  mode is required
	 * and XENV is not valid !
	 */
	xenv_res = xenv_config();

	/*
	 * calculate cpu & sys frequency (may be needed for uart init)
	 */
	em8xxx_cpu_frequency = tangox_get_cpuclock();
	em8xxx_sys_frequency = tangox_get_sysclock();

	/*
	 * program the right clock divider in both uart
	 */
#ifdef CONFIG_TANGOX_UART_USE_SYSCLK
	clksel = 0;
#else
	clksel = 1;
#endif
	gbus_writel(REG_BASE_cpu_block + CPU_UART0_base + CPU_UART_CLKSEL,
		    clksel);
	gbus_writel(REG_BASE_cpu_block + CPU_UART1_base + CPU_UART_CLKSEL,
		    clksel);

#ifdef CONFIG_TANGO3
	/* Temporary HACK */
	gbus_writel(REG_BASE_cpu_block + CPU_uart0_gpio_mode, 0xff00);
	gbus_writel(REG_BASE_cpu_block + CPU_uart1_gpio_mode, 0xff00);

	gbus_writel(REG_BASE_cpu_block + CPU_irq_enableclr, 0xffffffff);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_config_rise_clr, 0xffffffff);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_config_fall_clr, 0xffffffff);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_config_rise_set, 0);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_config_fall_set, 0);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_rawstat, 0xffffffff);

	gbus_writel(REG_BASE_cpu_block + CPU_irq_enableclr_hi, 0xffffffff);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_config_rise_clr_hi, 0xffffffff);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_config_fall_clr_hi, 0xffffffff);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_config_rise_set_hi, 0);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_config_fall_set_hi, 0);
	gbus_writel(REG_BASE_cpu_block + CPU_edge_rawstat_hi, 0xffffffff);
#endif

	/*
	 * show KERN_DEBUG message on console
	 */
	do_syslog(8, NULL, 8);

#ifdef CONFIG_TANGOX_PROM_CONSOLE
	/* initialize uart and register early console */
	prom_console_register();
#endif

	/* warn user if we use failsafe values for xenv */
	if (xenv_res)
		printk("Invalid XENV content, using failsafe values\n");
	tangox_device_info();

	/*
	 * compute kernel memory start address/size
	 * _text section gives kernel address start
	 */
	em8xxx_kmem_start = ((unsigned long)(&_text)) & PAGE_MASK;
	em8xxx_kmem_size = (((CONFIG_TANGOX_SYSTEMRAM_ACTUALSIZE << 20) +
	     em8xxx_kmem_start) & 0xfff00000) - em8xxx_kmem_start;

	/*
	 * check/fill the memcfg
	 */
	m = (memcfg_t *)KSEG1ADDR(MEM_BASE_dram_controller_0 + FM_MEMCFG);
	em8xxx_kmem_end = KSEG1ADDR(em8xxx_kmem_start + em8xxx_kmem_size) -
		KSEG1ADDR(MEM_BASE_dram_controller_0);

	if (is_valid_memcfg(m) == 0) {
		printk("Invalid MEMCFG, creating new one at 0x%08x.\n", MEM_BASE_dram_controller_0 + FM_MEMCFG);
		memset(m, 0, sizeof (memcfg_t));
		m->signature = MEMCFG_SIGNATURE;
		m->dram0_size = TANGOX_SYSTEMRAM_ACTUALSIZE;
		m->kernel_end = em8xxx_kmem_end;
		gen_memcfg_checksum(m);
	} else {
		printk("Valid MEMCFG found at 0x%08x.\n", MEM_BASE_dram_controller_0 + FM_MEMCFG);
		m->kernel_end = em8xxx_kmem_end;
		gen_memcfg_checksum(m);
	}

	/*
	 * tell kernel about available memory size/offset
	 */
	offset = KSEG1ADDR(em8xxx_kmem_start) -
		KSEG1ADDR(MEM_BASE_dram_controller_0);

	add_memory_region(MEM_BASE_dram_controller_0 + offset,
			  em8xxx_kmem_size, BOOT_MEM_RAM);

#ifndef CONFIG_TANGOX_IGNORE_CMDLINE
	/*
	 * set up correct linux command line according to XENV, memcfg
	 * and YAMON args, if not told to ignore them
	 */

	/* If specified by xenv, override the command line */
	if (tangox_xenv_cmdline())
		strcpy(arcs_cmdline, tangox_xenv_cmdline());

	/* If specified by memcfg, override the command line */
//	if (m->linux_cmd != 0 && strlen((char *)KSEG1ADDR(m->linux_cmd)) > 0)
//disabled. e.m. 2006feb3rd		strcpy(arcs_cmdline, (char *)KSEG1ADDR(m->linux_cmd));

	/* take regular args given by bootloader */
	if ((fw_arg0 > 1) && (fw_arg0 < 65)) { /* Up to 64 arguments */
		int argc, i, pos;
		char **argv;

		argc = fw_arg0;
		arcs_cmdline[0] = '\0';
		argv = (char **) fw_arg1;
		pos = 0;
		for (i = 1; i < argc; i++) {
			int len;

			len = strlen(argv[i]);
			if (pos + 1 + len + 1 > sizeof (arcs_cmdline))
				break;
			if (pos)
				arcs_cmdline[pos++] = ' ';
			strcpy(arcs_cmdline + pos, argv[i]);
			pos += len;
		}
	}
#endif /* CONFIG_TANGOX_IGNORE_CMDLINE */

	mips_machgroup = MACH_GROUP_SIGMADESIGNS;
	mips_machtype = MACH_TANGOX;
}

unsigned long __init prom_free_prom_memory(void)
{
	return 0;
}

EXPORT_SYMBOL(tangox_get_sysclock);
EXPORT_SYMBOL(tangox_get_cpuclock);
EXPORT_SYMBOL(tangox_get_dspclock);
EXPORT_SYMBOL(tangox_get_pllclock);

unsigned long tangox_chip_id(void)
{
	return (((gbus_readl(REG_BASE_host_interface + PCI_REG0) & 0xffff) << 16) |
                        (gbus_readl(REG_BASE_host_interface + PCI_REG1) & 0xff));
}

int is_tango2_chip(void)
{
	unsigned long chip = (tangox_chip_id()>>16) & 0xfff0;
	return (chip == 0x8630) ? 1 : 0;
}

static inline int is_tango2_revision(unsigned char revid)
{
	unsigned char rev = tangox_chip_id() & 0xff;
	return (is_tango2_chip() && rev == revid) ? 1 : 0;
}

int is_tango2_es123(void)
{
	return(is_tango2_revision(0x81));
}

int is_tango2_es45(void)
{
	return(is_tango2_revision(0x82));
}

int is_tango2_es6(void)
{
	return(is_tango2_revision(0x83));
}

int is_tango2_es7(void)
{
	return(is_tango2_revision(0x84));
}

int is_tango2_es89(void)
{
	return(is_tango2_revision(0x85) || is_tango2_revision(0x86));
}

int is_tango3_chip(void)
{
	unsigned long chip = (tangox_chip_id()>>16) & 0xfff0;
	return (chip == 0x8640) ? 1 : 0;
}

int is_tango3_es1(void)
{
	return(is_tango3_chip());
}

EXPORT_SYMBOL(tangox_chip_id);
EXPORT_SYMBOL(is_tango2_chip);
EXPORT_SYMBOL(is_tango3_chip);
EXPORT_SYMBOL(is_tango2_es123);
EXPORT_SYMBOL(is_tango2_es45);
EXPORT_SYMBOL(is_tango2_es6);
EXPORT_SYMBOL(is_tango2_es7);
EXPORT_SYMBOL(is_tango2_es89);
EXPORT_SYMBOL(is_tango3_es1);

#ifdef CONFIG_SD_DIRECT_DMA

int is_contiguous_memory(void __user *userbuf, unsigned int len, unsigned long *physaddr)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	unsigned long start = (unsigned long)userbuf;
	unsigned long pg, offset, paddr, ppaddr, end;
	struct mm_struct *mm = current->mm;
	int ret = 0;

//printk("%s:%d: start=0x%08lx, len=0x%x\n", __FILE__, __LINE__, start, len);

	*physaddr = 0;
	down_read(&mm->mmap_sem);
	for (ppaddr = 0, end = start + len; start < end; start += PAGE_SIZE) {
		pg = start & PAGE_MASK;
		offset = start & (PAGE_SIZE - 1);
		if (pg > TASK_SIZE)
			pgd = pgd_offset_k(pg);
		else
			pgd = pgd_offset_gate(mm, pg);
		BUG_ON(pgd_none(*pgd));
		pud = pud_offset(pgd, pg);
		BUG_ON(pud_none(*pud));
		pmd = pmd_offset(pud, pg);
		if (pmd_none(*pmd)) 
			goto error;
		pte = pte_offset_map(pmd, pg);
		if (pte_none(*pte)) {
			pte_unmap(pte);
			goto error;
		}
		paddr = pte_val(*pte) & PAGE_MASK;
//printk("TRANSLATED 0x%08lx, pte=0x%p, paddr=0x%lx\n", pg, pte, paddr);
		pte_unmap(pte);

		if (ppaddr == 0) { /* first page */
			ppaddr = paddr;
			*physaddr = ppaddr | offset;
		} else if ((ppaddr + PAGE_SIZE) != paddr) /* not contiguous */
			goto not_contiguous;
		else
			ppaddr = paddr;
	}
	ret = 1;

not_contiguous:
error:
	up_read(&mm->mmap_sem);
//printk("%s:%d: return %d\n", __FILE__, __LINE__, ret);
	return ret;
}

EXPORT_SYMBOL(is_contiguous_memory);

#endif /* CONFIG_SD_DIRECT_DMA */

