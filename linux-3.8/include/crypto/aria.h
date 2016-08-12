/*
 * Common values for ARIA algorithms
 */

#ifndef _CRYPTO_ARIA_H
#define _CRYPTO_ARIA_H

#include <linux/types.h>
#include <linux/crypto.h>

#define ARIA_MIN_KEY_SIZE    16
#define ARIA_MAX_KEY_SIZE    32
#define ARIA_KEYSIZE_128     16
#define ARIA_KEYSIZE_192     24
#define ARIA_KEYSIZE_256     32
#define ARIA_BLOCK_SIZE      16
#define ARIA_MAX_KEYLENGTH   (15 * 16)
#define ARIA_MAX_KEYLENGTH_U32   (ARIA_MAX_KEYLENGTH / sizeof(u32))

/*
 * Please ensure that the first two fields are 16-byte aligned
 * relative to the start of the structure, i.e., don't move them!
 */
struct crypto_aria_ctx {
	u32 key_enc[ARIA_MAX_KEYLENGTH_U32];
	u32 key_dec[ARIA_MAX_KEYLENGTH_U32];
	u32 key_length;
};

#endif // _CRYTO_ARIA_H
