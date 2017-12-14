/*
 * SHA1 Secure Hash Algorithm.
 *
 * dup of  crypto/sha1.c, because we  can't use crypto api  cleanly at
 * init time...
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/crypto.h>
#include <linux/cryptohash.h>
#include <asm/scatterlist.h>
#include <asm/byteorder.h>

#define SHA1_DIGEST_SIZE	20
#define SHA1_HMAC_BLOCK_SIZE	64

struct sha1_ctx {
	u64 count;
	u32 state[5];
	u8 buffer[64];
};

static void __init sha1_init(void *ctx)
{
	struct sha1_ctx *sctx = ctx;
	static const struct sha1_ctx initstate = {
	  0,
	  { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 },
	  { 0, }
	};

	*sctx = initstate;
}

static void __init sha1_update(void *ctx, const u8 *data, unsigned int len)
{
	struct sha1_ctx *sctx = ctx;
	unsigned int i, j;
	u32 temp[SHA_WORKSPACE_WORDS];

	j = (sctx->count >> 3) & 0x3f;
	sctx->count += len << 3;

	if ((j + len) > 63) {
		memcpy(&sctx->buffer[j], data, (i = 64-j));
		sha_transform(sctx->state, sctx->buffer, temp);
		for ( ; i + 63 < len; i += 64) {
			sha_transform(sctx->state, &data[i], temp);
		}
		j = 0;
	}
	else i = 0;
	memset(temp, 0, sizeof(temp));
	memcpy(&sctx->buffer[j], &data[i], len - i);
}

/* Add padding and return the message digest. */
static void __init sha1_final(void* ctx, u8 *out)
{
	struct sha1_ctx *sctx = ctx;
	u32 i, j, index, padlen;
	u64 t;
	u8 bits[8] = { 0, };
	static const u8 padding[64] = { 0x80, };

	t = sctx->count;
	bits[7] = 0xff & t; t>>=8;
	bits[6] = 0xff & t; t>>=8;
	bits[5] = 0xff & t; t>>=8;
	bits[4] = 0xff & t; t>>=8;
	bits[3] = 0xff & t; t>>=8;
	bits[2] = 0xff & t; t>>=8;
	bits[1] = 0xff & t; t>>=8;
	bits[0] = 0xff & t;

	/* Pad out to 56 mod 64 */
	index = (sctx->count >> 3) & 0x3f;
	padlen = (index < 56) ? (56 - index) : ((64+56) - index);
	sha1_update(sctx, padding, padlen);

	/* Append length */
	sha1_update(sctx, bits, sizeof bits);

	/* Store state in digest */
	for (i = j = 0; i < 5; i++, j += 4) {
		u32 t2 = sctx->state[i];
		out[j+3] = t2 & 0xff; t2>>=8;
		out[j+2] = t2 & 0xff; t2>>=8;
		out[j+1] = t2 & 0xff; t2>>=8;
		out[j  ] = t2 & 0xff;
	}

	/* Wipe context */
	memset(sctx, 0, sizeof *sctx);
}

void __init sha1_full(u8 *digest, const u8 *src, u32 len)
{
	struct sha1_ctx ctx;
	int i;

	sha1_init(&ctx);
	sha1_update(&ctx, src, len);
	sha1_final(&ctx, digest);

	for (i = 0; i < SHA1_DIGEST_SIZE / 2; i++)
		digest[i] = digest[SHA1_DIGEST_SIZE - i - 1];
}
