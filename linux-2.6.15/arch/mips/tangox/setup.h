/*
 * misc vars/func shared by platform setup code
 */

#ifndef __SETUP_H
#define __SETUP_H

#ifdef CONFIG_TANGO2
#define EM86XX_CHIP EM86XX_CHIPID_TANGO2
#include <linux/interrupt.h>
#include <asm/tango2/rmem86xxid.h>
#include <asm/tango2/rmdefs.h>
#include <asm/tango2/emhwlib_dram.h>
#include <asm/tango2/tango2_gbus.h>
#include <asm/tango2/tango2.h>
#include <asm/tango2/tango2api.h>
#include <asm/tango2/memcfg.h>
#elif defined(CONFIG_TANGO3)
#define EM86XX_CHIP EM86XX_CHIPID_TANGO3
#include <linux/interrupt.h>
#include <asm/tango3/rmem86xxid.h>
#include <asm/tango3/rmdefs.h>
#include <asm/tango3/emhwlib_dram.h>
#include <asm/tango3/tango3_gbus.h>
#include <asm/tango3/tango3.h>
#include <asm/tango3/tango3api.h>
#include <asm/tango3/memcfg.h>
#else
#error "Unknown architecture"
#endif

/*
 * in console.c
 */
void prom_console_register(void);

/*
 * in prom.c
 */
extern unsigned long em8xxx_cpu_frequency;
extern unsigned long em8xxx_sys_frequency;
extern unsigned long em8xxx_remap_registers[4];
unsigned long tangox_get_cpuclock(void);
unsigned long tangox_get_sysclock(void);

/*
 * in irq.c
 */
void tangox_dispatch(int ipline, struct pt_regs *regs);

/*
 * in tangoxIRQ.S
 */
extern asmlinkage void tangoxIRQ(void);

#endif
