#ifndef	N3CRYPTO_LIB_PKCS11_H
#define	N3CRYPTO_LIB_PKCS11_H

#define RETURN_IF_CRYPTOKI_NOT_INIT()									\
		if (g_bLibInitialized == CK_FALSE)								\
			return CKR_CRYPTOKI_NOT_INITIALIZED

#define RETURN_IF_SESSION_INVALID(hSession);							\
		if (hSession == 0 || hSession >= MAX_SESSION_NUM) 				\
			return CKR_SESSION_HANDLE_INVALID;

#define RETURN_IF_OBJECT_INVALID(hObject);								\
		if (( MASK_OBJECT_HANDLE_INDEX(hObject) >= MAX_OBJECT_NUM ) || 	\
			( MASK_OBJECT_HANDLE_INDEX(hObject) == 0))					\
			return CKR_OBJECT_HANDLE_INVALID;

#define RETURN_IF_ARGUMENT_BAD(false_cond);								\
		if ( (false_cond) )												\
			return CKR_ARGUMENTS_BAD;

#define RETURN_IF_NOT_OK(rv)											\
		if (rv != CKR_OK) 												\
			return rv;

#define pkcs11_return(x)												\
		do{ 															\
			LOGW("rv=0x%x (%s)", x, #x); 								\
			return x; 													\
		} while(0)

#define GET_DEVICE_OBJECT_HANDLE(x)			(x)


extern CK_SLOT_ID	g_ulSlotID;
extern CK_BOOL		g_bLibInitialized;

// extra functions decluded in "pkcs11f.h"

int C_LIB_InitBlockMsg
(
	void *			pblk,
	unsigned int	ulBlockLen,
	unsigned char 	ucCmd,
	unsigned int	ulAppSession
);

int C_LIB_TransBlockMsg
(
	unsigned int		slotID, 
	void *				pBlock, 
	unsigned int *		pulDataLen
);

int C_LIB_ControlDev
(
	int		command,
	void 	*pData,
	void	*pArg1
);

#endif 	/* N3CRYPTO_LIB_PKCS11_H */
