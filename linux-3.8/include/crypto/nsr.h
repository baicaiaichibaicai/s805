/*
 * Common values for NSR algorithms
 */

#ifndef _CRYPTO_NSR_H
#define _CRYPTO_NSR_H

#include <future/ci/commandblock.h>
#include <future/ci/conf.h>
#include <future/ci/n3type.h>
#include <future/ci/n3crypto_drv.h>
#include <future/ci/n3pkcsCI.h>

#define CELO_BLOCK_SIZE	32
#define DICE_BLOCK_SIZE	32

typedef struct raka_handle_t raka_handle_t;
struct raka_handle_t {
	CK_OBJECT_HANDLE alg_handle;
	CK_OBJECT_HANDLE mac_handle;
	CK_OBJECT_HANDLE iv_handle;
	u16 hash;
	u16 inbound;
};

typedef struct crypto_nsr_ctx crypto_dice_ctx;
typedef struct crypto_nsr_ctx crypto_celo_ctx;
typedef struct crypto_nsr_ctx crypto_nsr_ctx;
struct crypto_nsr_ctx {
	raka_handle_t raka;
	CK_SESSION_HANDLE session;
	u32 nbytes;
	u8 *ihash;
};

#endif
