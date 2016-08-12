#ifndef N3CRYPTO_DRV_H
#define N3CRYPTO_DRV_H



typedef enum {
	N3CRYPTO_DRV_SEND_COMMAND			= 1,
	N3CRYPTO_DRV_GET_VERSION			= 2,
	N3CRYPTO_DRV_GET_SLOT_LIST			= 3,
	N3CRYPTO_DRV_GET_TOKEN_STATE		= 4,
	N3CRYPTO_DRV_SET_SRAM				= 11,
	N3CRYPTO_DRV_GET_SRAM				= 12,
	N3CRYPTO_DRV_SET_DEBUG_LEVEL		= 91,
	N3CRYPTO_DRV_GET_DEBUG_LEVEL		= 92,
} N3CRYPTO_DRV_CMD;


typedef struct {
	/* 150721, NKHA - fix a bug for 64-bit operation: size_t -> unsigned int */
	unsigned int *length;
	char 	*buffer;
	int		*rv;
} N3CRYPTO_DRV_ARG1;






#endif	/* N3CRYPTO_DRV_H */
