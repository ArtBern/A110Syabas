
/*
 * zBoot configuration 
 */

#ifndef __ZBOOT_H__
#define __ZBOOT_H__

#ifndef __ASSEMBLY__

#define ZBOOT_ATTR_LOAD		0x00000001	/* Load */
#define ZBOOT_ATTR_EXEC		0x00000002	/* Executable */
#define ZBOOT_ATTR_GZIP		0x00000010	/* Compressed */
#define ZBOOT_ATTR_ENCRYPT	0x00000100	/* Encrypted */
#define ZBOOT_ATTR_FINAL	0x10000000	/* Last to process */

#define ZBOOT_VERSION		0x02030000

struct zboot_fhdr
{
	unsigned long signature;
	unsigned long attributes;
	unsigned long loadaddr;
	unsigned long startaddr;
	unsigned long checksum;
	unsigned long version;
	unsigned long size;
	unsigned long reserved[1];	/* make it 32bytes total */
};

int process_zboot_record(void *flashaddr, void *env, int idx);

#endif /* !__ASSEMBLY__ */

#if defined(CONFIG_SIGBLOCK_SUPPORT)

#include "sigblock.h"

#ifndef __ASSEMBLY__

/* Defineition of zboot extension in signature block */
#define ZBOOT_B0OFFSET_IDX	0
#define ZBOOT_B1OFFSET_IDX	1
#define ZBOOT_YAMON_SIG_LO	2
#define ZBOOT_YAMON_SIG_HI	3
#define ZBOOT_YAMON_EV_IDX	4
#define ZBOOT_DEF_BAUD		5
#define ZBOOT_UNUSED_1		6
#define ZBOOT_UNUSED_2		7
#define ZBOOT_PFLASH_ADDR	8
#define ZBOOT_PFLASH_SIZE	9
#define ZBOOT_PFLASH_PART1_SZ	10
#define ZBOOT_PFLASH_PART1_OFT	11
#define ZBOOT_PFLASH_PART2_SZ	12
#define ZBOOT_PFLASH_PART2_OFT	13
#define ZBOOT_PFLASH_PART3_SZ	14
#define ZBOOT_PFLASH_PART3_OFT	15
#define ZBOOT_PFLASH_PART4_SZ	16
#define ZBOOT_PFLASH_PART4_OFT	17
#define ZBOOT_PFLASH_PART5_SZ	18
#define ZBOOT_PFLASH_PART5_OFT	19

/* Accessor functions */
static inline unsigned long bootimage_offset(struct signature_block *sigptr, int idx)
{
	switch(idx) {
		case 0: return(sigptr->extension[ZBOOT_B0OFFSET_IDX]);
		case 1: return(sigptr->extension[ZBOOT_B1OFFSET_IDX]);
		default: break;
	}
	return(0);
}

static inline unsigned long zboot_default_baudrate(struct signature_block *sigptr)
{
	return(sigptr->extension[ZBOOT_DEF_BAUD]);
}

static inline int pflash_info(struct signature_block *sigptr, unsigned long *addr, unsigned long *size)
{
	*addr = *size = 0;
	if ((sigptr->extension[ZBOOT_PFLASH_ADDR] == 0) || (sigptr->extension[ZBOOT_PFLASH_SIZE] == 0))
		return(-1);
	*addr = sigptr->extension[ZBOOT_PFLASH_ADDR];
	*size = sigptr->extension[ZBOOT_PFLASH_SIZE];
	return(0);
}

static inline int pflash_num_partitions(void)
{
	return(5);
}

/* To follow Linux MTD convention, idx starts from 1 instead of 0 */
static inline int pflash_partition_info(struct signature_block *sigptr, int idx, unsigned long *offset, unsigned long *size)
{
	*offset = *size = 0;
	if ((idx <= 0) || (idx > pflash_num_partitions()))
		return(-1);
	else {
		*size = sigptr->extension[ZBOOT_PFLASH_PART1_SZ + (idx - 1) * 2];
		*offset = sigptr->extension[ZBOOT_PFLASH_PART1_OFT + (idx - 1) * 2];
	}
	return(0);
}
#endif /* !__ASSEMBLY__ */
#endif /* CONFIG_SIGBLOCK_SUPPORT */
#endif /* __ZBOOT_H__ */

