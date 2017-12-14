#ifndef __HARDWARE_H
#define __HARDWARE_H

#include <linux/version.h>
#include <asm/tango3/emhwlib_registers_tango3.h>
#include <asm/tango3/tango3_gbus.h>

// UART0
#define CPU_uart0_gpio_dir	    (CPU_UART0_base + CPU_UART_GPIODIR)
#define CPU_uart0_gpio_data	    (CPU_UART0_base + CPU_UART_GPIODATA)
#define CPU_uart0_gpio_mode	    (CPU_UART0_base + CPU_UART_GPIOMODE)

// UART0
#define CPU_uart1_gpio_dir	    (CPU_UART1_base + CPU_UART_GPIODIR)
#define CPU_uart1_gpio_data	    (CPU_UART1_base + CPU_UART_GPIODATA)
#define CPU_uart1_gpio_mode	    (CPU_UART1_base + CPU_UART_GPIOMODE)

#define MIPS_CPU_IRQ_BASE 0

#if defined(CONFIG_TANGO3_SMP865X)
#define IRQ_CONTROLLER_IRQ_BASE 8
#define FIQ_CONTROLLER_IRQ_BASE 72
#define IIQ_CONTROLLER_IRQ_BASE 136 // bit31 of iiq is linux irq 199
#define IRQ_COUNT               64  // 64 interrupt sources
#else
#error "Unsupported Tango3 chip."
#endif

#define IRQ_SOFTINT                     (IRQ_CONTROLLER_IRQ_BASE+0)   // gnet compatibility
// IDE interrupts
#define IRQ_IDECTRL_IDEDMA	(LOG2_CPU_DMAIDE_INT + IRQ_CONTROLLER_IRQ_BASE)
#define IRQ_IDECTRL_IDE		(LOG2_CPU_IDE_INT + IRQ_CONTROLLER_IRQ_BASE)

// MBUS interface
#define MIF_add_offset		    0x0
#define MIF_cnt_offset              (MIF_W0_CNT - MIF_W0_ADD) //0x04
#define MIF_add2_skip_offset        (MIF_W0_SKIP - MIF_W0_ADD) //0x08
#define MIF_cmd_offset              (MIF_W0_CMD - MIF_W0_ADD) //0x0c

// GPIO
#define GPIO_DIR_INPUT(gpio)        ((1 << (16 + (gpio))))
#define GPIO_DIR_OUTPUT(gpio)       ((1 << (16 + (gpio))) | (1 << (gpio)))
#define GPIO_DATA_SET(gpio)         ((1 << (16 + (gpio))) | (1 << (gpio)))
#define GPIO_DATA_CLEAR(gpio)       ((1 << (16 + (gpio))))

// UART GPIO
#define UART_GPIO_DIR_INPUT(gpio)        ((1 << (8 + (gpio))))
#define UART_GPIO_DIR_OUTPUT(gpio)       ((1 << (8 + (gpio))) | (1 << (gpio)))
#define UART_GPIO_DATA_SET(gpio)         ((1 << (8 + (gpio))) | (1 << (gpio)))
#define UART_GPIO_DATA_CLEAR(gpio)       ((1 << (8 + (gpio))))

/* PCI Memories */
#define MEMORY_BASE_PCI_CONFIG      0x50000000UL  /* PCI configuration */
#define MEMORY_BASE_PCI_IO          0x58000000UL  /* PCi I/O space */
#define MEMORY_BASE_PCI_MEMORY      0x60000000UL  /* PCI Memory Base */

#define PCIBIOS_MIN_MEM_EM86XX  (MEMORY_BASE_PCI_MEMORY + 0x10000000UL)   /* base
address of EM86xx PCI slave */

// Peripheral bus Registers
#define HOST_pb0_base               0x0000
#define HOST_pb_base_cs(n)          (HOST_pb0_base + (0x0200 * (n)))

#define PB_timing_slot(n)	    (PB_timing0 + (0x04 * (n)))

// Bus Master IDE
#define REG_BASE_host_interface_BMIDE         (REG_BASE_host_interface + IDECTRL_pri_cmdblock)

#define REG_BASE_host_interface_ISAIDE(x)  (REG_BASE_host_interface + HOST_pb_base_cs(x))

#ifndef __ASSEMBLY__
// Physical address mapping
static inline unsigned long tangox_dma_address(unsigned long physaddr)
{
	/* for Tango3, another remap takes place */
	if ((physaddr >= 0x10000000) && (physaddr < 0x20000000))
		return((physaddr - 0x10000000) + 0x80000000); /* DRAM0 */
	else if ((physaddr >= 0x20000000) && (physaddr < 0x30000000))
		return((physaddr - 0x20000000) + 0xc0000000); /* DRAM1 */
#if 1
	else if ((physaddr >= 0x04000000) && (physaddr < 0x08000000)) 
		return(gbus_readl(REG_BASE_cpu_block + CPU_remap2) + (physaddr - 0x04000000));
	else if ((physaddr >= 0x08000000) && (physaddr < 0x0c000000)) 
		return(gbus_readl(REG_BASE_cpu_block + CPU_remap3) + (physaddr - 0x04000000));
	else if ((physaddr >= 0x0c000000) && (physaddr < 0x10000000)) 
		return(gbus_readl(REG_BASE_cpu_block + CPU_remap4) + (physaddr - 0x04000000));
#endif
	return(physaddr); /* use whatever is specified */
}

// Inverted physical address mapping
static inline unsigned long tangox_inv_dma_address(unsigned long mapaddr)
{
	if ((mapaddr >= 0x80000000) && (mapaddr < 0x90000000))
		return((mapaddr - 0x80000000) + 0x10000000);
	else if ((mapaddr >= 0xc0000000) && (mapaddr < 0xd0000000))
		return((mapaddr - 0xc0000000) + 0x20000000);
#if 1
	{
		unsigned long remap;
		remap = gbus_readl(REG_BASE_cpu_block + CPU_remap2);
		if ((mapaddr >= remap) && (mapaddr < (remap + 0x04000000)))
			return(CPU_remap2_address + (mapaddr - remap));
		remap = gbus_readl(REG_BASE_cpu_block + CPU_remap3);
		if ((mapaddr >= remap) && (mapaddr < (remap + 0x04000000)))
			return(CPU_remap3_address + (mapaddr - remap));
		remap = gbus_readl(REG_BASE_cpu_block + CPU_remap4);
		if ((mapaddr >= remap) && (mapaddr < (remap + 0x04000000)))
			return(CPU_remap4_address + (mapaddr - remap));
	}
#endif
	return(mapaddr); /* use whatever is specified */
}
#endif

#endif //__HARDWARE_H

