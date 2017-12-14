/*****************************************
 Copyright © 2004-2005
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/

#include "setup.h"
#include "xenv.h"

#if 0
# define DPRINTK(fmt, args...)	printk(KERN_DEBUG "xenv: " fmt, ## args)
#else
# define DPRINTK(fmt, args...)
#endif

void sha1_full(u8 *digest, const u8 *src, u32 len);

#define REC_SIZE(x)	((((u16)x[0] & 0xff) << 8) | ((u16)x[1] & 0xff))
#define REC_ATTR(x)	((x[0] & 0xff) >> 4)

/*
 * check for valid XENV at given address
 */
int __init xenv_isvalid(u32 *base, u32 maxsize)
{
	u32 env_size = base[0];

	if ((24 <= env_size) && (env_size <= maxsize)) {
		u8 hash[20];

		memset(hash, 0, sizeof (hash));
		sha1_full(hash, (const u8 *)(base + 6), env_size - 24);
		if (memcmp(base + 1, hash, 5) != 0) {
			DPRINTK("corrupted\n");
			return -1;
		}

		/* valid xenv ! */
		return env_size;
	}

	DPRINTK("runaway %d\n", env_size);
	return -1;
}

int __init xenv_foreach(u32 *base, u32 size,
			void (*cb)(char *recordname, void *data, u32 datasize))
{
	int i;

	/* jump over first header */
	i = 24;

	/* loop on each record name */
	while (i < size) {
		u16 rec_size;
		char *p, *recordname;
		void *data;
		u32 key_len, data_len;

		p = (char *)base + i;
		rec_size = REC_SIZE(p);
		recordname = p + 2;
		key_len = strlen(recordname);
		data = recordname + key_len + 1;
		data_len = rec_size - 2 - key_len - 1;

		cb(recordname, data, data_len);
		i += rec_size;
	}

	return -1;
}


#ifdef CONFIG_TANGOX_XENV_DUMP
void __init xenv_dump(u32 *base, u32 size)
{
	int i;
	u32 records = 0;

	printk("@%p\n", base);

	/* jump over first header */
	i = 24;

	while (i < size){
		u8 rec_attr;
		u16 rec_size;
		char *p, *recordname, *x;
		u32 key_len;

		p = (char *)base + i;
		rec_attr = REC_ATTR(p);
		rec_size = REC_SIZE(p);
		recordname = p + 2;
		key_len = strlen(recordname);

		printk("(0x%02x) [%s] =", rec_attr, recordname);
		for (x = recordname + key_len + 1;
		     x < recordname + rec_size - 2; x++)
			printk(" %02x", (u8)*x);
		printk(" .\n");

		records++;
		i += rec_size;
	}

	printk("%d records, %d bytes\n\n", records, size);
}
#endif
