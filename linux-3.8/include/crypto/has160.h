/*
 * Hash: Hash algorithms under the crypto API
 * 
 */

#ifndef _CRYPTO_HAS160_H
#define _CRYPTO_HAS160_H

#include <linux/crypto.h>
#include <crypto/sha.h>

#define	HAS160_DIGEST_SIZE	SHA1_DIGEST_SIZE
#define	HAS160_BLOCK_SIZE	SHA1_BLOCK_SIZE

struct has160_state {
	u64 state[HAS160_DIGEST_SIZE / 4];
	u32 length[2];
	u8 data[HAS160_BLOCK_SIZE];
};

#define	HAS160_H0	0x67452301
#define HAS160_H1	0xefcdab89
#define HAS160_H2	0x98badcfe
#define HAS160_H3	0x10325476
#define HAS160_H4	0xc3d2e1f0

#define GATE_1(x, y, z)		( (z)^( (x) & ((y)^(z)) ) )
#define GATE_2(x, y, z)		( (x) ^ (y) ^ (z)         )
#define GATE_3(x, y, z)		( (y)^( (x) | ~(z)      ) )

#define ROTL(x, n)     (((x) << (n)) | ((x) >> (32-(n))))		// left rotation

#define HAS160_F(a, b, c, d, e, x, s)        {         \
	(e) += ROTL(a, s) + GATE_1(b, c, d) + (x);               \
	(b) = ROTL(b, 10);                                  \
   }

#define HAS160_G(a, b, c, d, e, x, s)        {         \
	(e) += ROTL(a, s) + GATE_2(b, c, d) + (x) + 0x5A827999L; \
	(b) = ROTL(b, 17);                                  \
   }

#define HAS160_H(a, b, c, d, e, x, s)        {         \
	(e) += ROTL(a, s) + GATE_3(b, c, d) + (x) + 0x6ED9EBA1L; \
	(b) = ROTL(b, 25);                                  \
   }

#define HAS160_G_(a, b, c, d, e, x, s)        {         \
	(e) += ROTL(a, s) + GATE_2(b, c, d) + (x) + 0x8F1BBCDCL; \
	(b) = ROTL(b, 30);                                  \
   }

#endif	/* _CRYPTO_HAS160_H */
