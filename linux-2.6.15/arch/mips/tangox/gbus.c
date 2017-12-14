/*
 * export gbus symbol to modules
 */

#include <linux/module.h>
#include <asm/system.h>

#include "setup.h"

#define TMP_REMAPPED_REG   CPU_remap4
#define TMP_REMAPPED_BASE  CPU_remap4_address

#ifdef CONFIG_TANGOX_USE_TLB_REMAP_DRAM1
unsigned long em86xx_tlb_dram1_map_base;
unsigned long em86xx_tlb_dram1_map_size;
#endif

static RMuint32 set_remap(RMuint32 remap_reg, RMuint32 value)
{
	RMuint32 orig = *((volatile RMuint32 *)KSEG1ADDR(REG_BASE_cpu_block + remap_reg));
	if (orig != value) {
		*((volatile RMuint32 *)KSEG1ADDR(REG_BASE_cpu_block + remap_reg)) = value;
		iob();
	}
	return(orig);
}

RMuint32 gbus_read_uint32(struct gbus *pgbus, RMuint32 byte_address)
{
	RMuint32 remap;
	RMuint32 tmp;

	if (byte_address < (MEM_BASE_dram_controller_0 + 0x10000000))
		return gbus_read_dram_uint32(pgbus, byte_address);
	else if (byte_address < (MEM_BASE_dram_controller_1 + 0x10000000))
#ifdef CONFIG_TANGOX_USE_TLB_REMAP_DRAM1
		if (byte_address < (MEM_BASE_dram_controller_1 + em86xx_tlb_dram1_map_size))
			return gbus_read_dram_uint32(pgbus, byte_address);
		else {
			printk("accessing non-existed DRAM1 area 0x%08lx\n", byte_address);
			return(0);
		}
#else
		return gbus_read_dram_uint32(pgbus, byte_address);
#endif
	else {
		unsigned long flags;
		local_irq_save(flags); // Ensure remap won't be changed 
		// Use CPU_remapx to temporarily map the address
		remap = set_remap(TMP_REMAPPED_REG, byte_address & 0xfc000000);
		tmp = *((volatile RMuint32 *)KSEG1ADDR(TMP_REMAPPED_BASE + (byte_address & 0x03ffffff)));
		set_remap(TMP_REMAPPED_REG, remap);
		local_irq_restore(flags);
		return(tmp);
	}
}

RMuint16 gbus_read_uint16(struct gbus *pgbus, RMuint32 byte_address)
{
	RMuint32 remap;
	RMuint16 tmp;

	if (byte_address < (MEM_BASE_dram_controller_0 + 0x10000000))
		return gbus_read_dram_uint16(pgbus, byte_address);
	else if (byte_address < (MEM_BASE_dram_controller_1 + 0x10000000))
#ifdef CONFIG_TANGOX_USE_TLB_REMAP_DRAM1
		if (byte_address < (MEM_BASE_dram_controller_1 + em86xx_tlb_dram1_map_size))
			return gbus_read_dram_uint16(pgbus, byte_address);
		else {
			printk("accessing non-existed DRAM1 area 0x%08lx\n", byte_address);
			return(0);
		}
#else
		return gbus_read_dram_uint16(pgbus, byte_address);
#endif
	else {
		unsigned long flags;
		local_irq_save(flags); // Ensure remap won't be changed 
		// Use CPU_remapx to temporarily map the address
		remap = set_remap(TMP_REMAPPED_REG, byte_address & 0xfc000000);
		tmp = *((volatile RMuint16 *)KSEG1ADDR(TMP_REMAPPED_BASE + (byte_address & 0x03ffffff)));
		set_remap(TMP_REMAPPED_REG, remap);
		local_irq_restore(flags);
		return(tmp & 0xffff);
	}
}

RMuint8 gbus_read_uint8(struct gbus *pgbus, RMuint32 byte_address)
{
	RMuint32 remap;
	RMuint8 tmp;

	if (byte_address < (MEM_BASE_dram_controller_0 + 0x10000000))
		return gbus_read_dram_uint8(pgbus, byte_address);
	else if (byte_address < (MEM_BASE_dram_controller_1 + 0x10000000))
#ifdef CONFIG_TANGOX_USE_TLB_REMAP_DRAM1
		if (byte_address < (MEM_BASE_dram_controller_1 + em86xx_tlb_dram1_map_size))
			return gbus_read_dram_uint8(pgbus, byte_address);
		else {
			printk("accessing non-existed DRAM1 area 0x%08lx\n", byte_address);
			return(0);
		}
#else
		return gbus_read_dram_uint8(pgbus, byte_address);
#endif
	else {
		unsigned long flags;
		local_irq_save(flags); // Ensure remap won't be changed 
		// Use CPU_remapx to temporarily map the address
		remap = set_remap(TMP_REMAPPED_REG, byte_address & 0xfc000000);
		tmp = *((volatile RMuint8 *)KSEG1ADDR(TMP_REMAPPED_BASE + (byte_address & 0x03ffffff)));
		set_remap(TMP_REMAPPED_REG, remap);
		local_irq_restore(flags);
		return(tmp & 0xff);
	}
}

void gbus_write_uint32(struct gbus *pgbus, RMuint32 byte_address, RMuint32 data)
{
	RMuint32 remap;

	if (byte_address < (MEM_BASE_dram_controller_0 + 0x10000000))
		gbus_write_dram_uint32(pgbus, byte_address, data);
	else if (byte_address < (MEM_BASE_dram_controller_1 + 0x10000000))
#ifdef CONFIG_TANGOX_USE_TLB_REMAP_DRAM1
		if (byte_address < (MEM_BASE_dram_controller_1 + em86xx_tlb_dram1_map_size))
			gbus_write_dram_uint32(pgbus, byte_address, data);
		else 
			printk("accessing non-existed DRAM1 area 0x%08lx.\n", byte_address);
#else
		gbus_write_dram_uint32(pgbus, byte_address, data);
#endif
	else {
		unsigned long flags;
		local_irq_save(flags); // Ensure remap won't be changed 
		// Use CPU_remapx to temporarily map the address
		remap = set_remap(TMP_REMAPPED_REG, byte_address & 0xfc000000);
		*((volatile RMuint32 *)KSEG1ADDR(TMP_REMAPPED_BASE + (byte_address & 0x03ffffff))) = data;
		set_remap(TMP_REMAPPED_REG, remap);
		local_irq_restore(flags);
	}
	__sync();
}

void gbus_write_uint16(struct gbus *pgbus, RMuint32 byte_address, RMuint16 data)
{
	RMuint32 remap;

	if (byte_address < (MEM_BASE_dram_controller_0 + 0x10000000))
		gbus_write_dram_uint16(pgbus, byte_address, data);
	else if (byte_address < (MEM_BASE_dram_controller_1 + 0x10000000))
#ifdef CONFIG_TANGOX_USE_TLB_REMAP_DRAM1
		if (byte_address < (MEM_BASE_dram_controller_1 + em86xx_tlb_dram1_map_size))
			gbus_write_dram_uint16(pgbus, byte_address, data);
		else 
			printk("accessing non-existed DRAM1 area 0x%08lx.\n", byte_address);
#else
		gbus_write_dram_uint16(pgbus, byte_address, data);
#endif
	else {
		unsigned long flags;
		local_irq_save(flags); // Ensure remap won't be changed 
		// Use CPU_remapx to temporarily map the address
		remap = set_remap(TMP_REMAPPED_REG, byte_address & 0xfc000000);
		*((volatile RMuint16 *)KSEG1ADDR(TMP_REMAPPED_BASE + (byte_address & 0x03ffffff))) = (data & 0xffff);
		set_remap(TMP_REMAPPED_REG, remap);
		local_irq_restore(flags);
	}
	__sync();
}

void gbus_write_uint8(struct gbus *pgbus, RMuint32 byte_address, RMuint8 data)
{
	RMuint32 remap;

	if (byte_address < (MEM_BASE_dram_controller_0 + 0x10000000))
		gbus_write_dram_uint8(pgbus, byte_address, data);
	else if (byte_address < (MEM_BASE_dram_controller_1 + 0x10000000))
#ifdef CONFIG_TANGOX_USE_TLB_REMAP_DRAM1
		if (byte_address < (MEM_BASE_dram_controller_1 + em86xx_tlb_dram1_map_size))
			gbus_write_dram_uint8(pgbus, byte_address, data);
		else 
			printk("accessing non-existed DRAM1 area 0x%08lx.\n", byte_address);
#else
		gbus_write_dram_uint8(pgbus, byte_address, data);
#endif
	else {
		unsigned long flags;
		local_irq_save(flags); // Ensure remap won't be changed 
		// Use CPU_remapx to temporarily map the address
		remap = set_remap(TMP_REMAPPED_REG, byte_address & 0xfc000000);
		*((volatile RMuint8 *)KSEG1ADDR(TMP_REMAPPED_BASE + (byte_address & 0x03ffffff))) = (data & 0xff);
		set_remap(TMP_REMAPPED_REG, remap);
		local_irq_restore(flags);
	}
	__sync();
}

EXPORT_SYMBOL(gbus_read_uint32);
EXPORT_SYMBOL(gbus_write_uint32);
EXPORT_SYMBOL(gbus_read_uint16);
EXPORT_SYMBOL(gbus_write_uint16);
EXPORT_SYMBOL(gbus_read_uint8);
EXPORT_SYMBOL(gbus_write_uint8);

#ifdef CONFIG_TANGOX_USE_TLB_REMAP_DRAM1
EXPORT_SYMBOL(em86xx_tlb_dram1_map_base);
EXPORT_SYMBOL(em86xx_tlb_dram1_map_size);
#endif
