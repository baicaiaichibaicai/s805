/* pkcs11f.h include file for PKCS #11.  2000 January 13 */

/* This function contains pretty much everything about all the */
/* Cryptoki function prototypes.  Because this information is */
/* used for more than just declaring function prototypes, the */
/* order of the functions appearing herein is important, and */
/* should not be altered. */

CK_PKCS11_FUNCTION_INFO(C_OpenToken)
#ifdef CK_NEED_ARG_LIST
(
	CK_SLOT_ID slotID
 );
#endif

CK_PKCS11_FUNCTION_INFO(C_CloseToken)
#ifdef CK_NEED_ARG_LIST
(
	CK_SLOT_ID slotID
);
#endif

CK_PKCS11_FUNCTION_INFO(C_PacketEncryptInit)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,    /* the session's handle */
	CK_MECHANISM_PTR  pMechanism,  /* the encryption mechanism */
	CK_OBJECT_HANDLE  hKey         /* handle of encryption key */	
);
#endif

CK_PKCS11_FUNCTION_INFO(C_EncryptUpdateGCM)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,           	/* session's handle */
	CK_BYTE_PTR       pPart,              	/* the plaintext data */
	CK_ULONG          ulPartLen,          	/* plaintext data len */
	CK_BYTE_PTR       pEncryptedPart,     	/* gets ciphertext */
	CK_ULONG_PTR      pulEncryptedPartLen, 	/* gets c-text size */
	CK_VOID_PTR		  pGCM,				  	/* parameter of GCM */
	CK_ULONG		  ulGCMLen			/* length of GCM parameter in bytes */
);
#endif

CK_PKCS11_FUNCTION_INFO(C_DecryptUpdateGCM)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,            /* session's handle */
	CK_BYTE_PTR			pEncryptedPart,      /* encrypted data */
	CK_ULONG			ulEncryptedPartLen,  /* input length */
	CK_BYTE_PTR			pPart,               /* gets plaintext */ 
	CK_ULONG_PTR		pulPartLen,           /* p-text size */
	CK_VOID_PTR		  	pGCM,				 /* parameter of GCM */
	CK_ULONG		  	ulGCMLen		 /* length of GCM parameter in bytes */
);
#endif




CK_PKCS11_FUNCTION_INFO(C_PacketEncryptUpdate)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,           /* session's handle */
	CK_BYTE_PTR       pPart,              /* the plaintext data */
	CK_ULONG          ulPartLen,          /* plaintext data len */
	CK_BYTE_PTR       pPacketEncryptedPart,     /* gets ciphertext */
	CK_ULONG_PTR      pulPacketEncryptedPartLen /* gets c-text size */
);
#endif


CK_PKCS11_FUNCTION_INFO(C_PacketEncryptUpdateGCM)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,           	/* session's handle */
	CK_BYTE_PTR       pPart,              	/* the plaintext data */
	CK_ULONG          ulPartLen,          	/* plaintext data len */
	CK_BYTE_PTR       pEncryptedPart,     	/* gets ciphertext */
	CK_ULONG_PTR      pulEncryptedPartLen, 	/* gets c-text size */
	CK_VOID_PTR		  pGCM,				  	/* parameter of GCM */
	CK_ULONG		  ulGCMLen			  	/* length of GCM parameter in bytes */
);
#endif

CK_PKCS11_FUNCTION_INFO(C_PacketEncryptFinal)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,                  /* session handle */
	CK_BYTE_PTR       pPacketEncryptedPart,      /* gets ciphertext */
	CK_ULONG_PTR      pulPacketEncryptedPartLen, /* gets c-text size */
	CK_BYTE_PTR		  pIV
);
#endif

CK_PKCS11_FUNCTION_INFO(C_PacketDecryptInit)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,    /* the session's handle */
	CK_MECHANISM_PTR  pMechanism,  /* the encryption mechanism */
	CK_OBJECT_HANDLE  hKey         /* handle of encryption key */
);
#endif

CK_PKCS11_FUNCTION_INFO(C_PacketDecryptUpdate)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,            /* session's handle */
	CK_BYTE_PTR			pEncryptedPart,      /* encrypted data */
	CK_ULONG			ulEncryptedPartLen,  /* input length */
	CK_BYTE_PTR			pPart,               /* gets plaintext */ 
	CK_ULONG_PTR		pulPartLen           /* p-text size */
);
#endif

CK_PKCS11_FUNCTION_INFO(C_PacketDecryptUpdateGCM)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,            /* session's handle */
	CK_BYTE_PTR			pEncryptedPart,      /* encrypted data */
	CK_ULONG			ulEncryptedPartLen,  /* input length */
	CK_BYTE_PTR			pPart,               /* gets plaintext */ 
	CK_ULONG_PTR		pulPartLen,           /* p-text size */
	CK_VOID_PTR		  	pGCM,				 /* parameter of GCM */
	CK_ULONG		  	ulGCMLen			 /* length of GCM parameter in bytes */
);
#endif

CK_PKCS11_FUNCTION_INFO(C_PacketDecryptFinal)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,              /* session handle */
	CK_BYTE_PTR       pLastPacketPart,       /* gets ciphertext */
	CK_ULONG_PTR      pulLastPacketPartLen,  /* gets c-text size */
	CK_BYTE_PTR		  pIV
);
#endif


CK_PKCS11_FUNCTION_INFO(C_PacketSignEncryptInit)
#ifdef CK_NEED_ARG_LIST
(	CK_SESSION_HANDLE 	hSession,
	CK_MECHANISM_PTR 	pMechanism,
	CK_OBJECT_HANDLE 	hKey,
	CK_MECHANISM_PTR 	pMAC_mech,
	CK_OBJECT_HANDLE 	hMAC_key

);
#endif



CK_PKCS11_FUNCTION_INFO(C_PacketSignEncryptUpdate)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE 	hSession,
	CK_BYTE_PTR 		pPart,
	CK_ULONG 			ulPartLen,
	CK_BYTE_PTR 		pEncryptedPart,
	CK_ULONG_PTR 		pulEncryptedPartLen,
	CK_BYTE_PTR 		pIV,
	CK_ULONG	 		ulIVLen,
	CK_BYTE_PTR 		pSignature,
	CK_ULONG_PTR	 	pulSignatureLen,
	CK_BYTE_PTR			pAAD,
	CK_ULONG			ulAADLen
);
#endif



CK_PKCS11_FUNCTION_INFO(C_PacketSignEncryptFinal)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession

);
#endif



CK_PKCS11_FUNCTION_INFO(C_PacketDecryptVerifyInit)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE 	hSession,
	CK_MECHANISM_PTR 	pMechanism,
	CK_OBJECT_HANDLE 	hKey,
	CK_MECHANISM_PTR 	pMAC_mech,
	CK_OBJECT_HANDLE 	hMAC_key 

);
#endif



CK_PKCS11_FUNCTION_INFO(C_PacketDecryptVerifyUpdate)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE 	hSession,
	CK_BYTE_PTR 		pEncryptedPart,
	CK_ULONG 			ulEncryptedPartLen,
	CK_BYTE_PTR 		pPart,
	CK_ULONG_PTR 		pulPartLen,
	CK_BYTE_PTR 		pIV,
	CK_ULONG 			ulIVLen,
	CK_BYTE_PTR 		pSignature,
	CK_ULONG 			ulSignatureLen,
	CK_BYTE_PTR			pAAD,
	CK_ULONG			ulAADLen
);
#endif



CK_PKCS11_FUNCTION_INFO(C_PacketDecryptVerifyFinal)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession
);
#endif


//krkim : High Packet
CK_PKCS11_FUNCTION_INFO(C_HighPacketSignEncryptUpdate)
#ifdef CK_NEED_ARG_LIST
(
 	CK_SESSION_HANDLE 	hSession,
	CK_BYTE_PTR       	pSKB,
	CK_ULONG			bUdpEncap,			
	CK_ULONG			ulSignatureLen
);
#endif

CK_PKCS11_FUNCTION_INFO(C_HighPacketDecryptVerifyUpdate)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE 	hSession,
	CK_BYTE_PTR       	pSKB,
	CK_ULONG			bUdpEncap,
	CK_ULONG			ulSignatureLen
);
#endif

/* General-purpose */

/* C_Initialize initializes the Cryptoki library. */
CK_PKCS11_FUNCTION_INFO(C_DiagModule)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,
	CK_ULONG			ulMechanism,
	CK_BYTE_PTR			pucDiagResult,
	CK_ULONG_PTR		pulResultLength
);
#endif



#if 1	// cloud add 2011.2.28 - x_ 함수 추가
CK_PKCS11_FUNCTION_INFO(x_PacketEncryptInit)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,    /* the session's handle */
	CK_ULONG		  ulMechanism,  /* the encryption mechanism */
	CK_OBJECT_HANDLE  hKey,         /* handle of encryption key */	
	CK_BYTE_PTR		  pIV,
	CK_BYTE_PTR		  pSK
);
#endif

CK_PKCS11_FUNCTION_INFO(x_PacketEncryptUpdate)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,           /* session's handle */
	CK_BYTE_PTR       pPart,              /* the plaintext data */
	CK_ULONG          ulPartLen,          /* plaintext data len */
	CK_BYTE_PTR       pPacketEncryptedPart,     /* gets ciphertext */
	CK_ULONG_PTR      pulPacketEncryptedPartLen, /* gets c-text size */
	CK_BYTE_PTR		  pIV
);
#endif

CK_PKCS11_FUNCTION_INFO(x_PacketEncryptFinal)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession                /* session handle */
);
#endif

CK_PKCS11_FUNCTION_INFO(x_PacketDecryptInit)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession,    /* the session's handle */
	CK_ULONG		  ulMechanism,  /* the encryption mechanism */
	CK_OBJECT_HANDLE  hKey,         /* handle of encryption key */
	CK_BYTE_PTR		  pIV,
	CK_BYTE_PTR		  pSK
);
#endif

CK_PKCS11_FUNCTION_INFO(x_PacketDecryptUpdate)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,            /* session's handle */
	CK_BYTE_PTR			pEncryptedPart,      /* encrypted data */
	CK_ULONG			ulEncryptedPartLen,  /* input length */
	CK_BYTE_PTR			pPart,               /* gets plaintext */ 
	CK_ULONG_PTR		pulPartLen,          /* p-text size */
	CK_BYTE_PTR			pIV
);
#endif

CK_PKCS11_FUNCTION_INFO(x_PacketDecryptFinal)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE hSession                /* session handle */
);
#endif


/* General-purpose */

CK_PKCS11_FUNCTION_INFO(x_DiagModule)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,
	CK_ULONG			ulMechanism,
	CK_BYTE_PTR			pucDiagResult,
	CK_ULONG_PTR		pulResultLength
);
#endif


#ifdef N3CRYPTO_DEBUG
CK_PKCS11_FUNCTION_INFO(C_SetDebugString)
#ifdef CK_NEED_ARG_LIST
(
	CK_BYTE_PTR			pDebugString
);
#endif

CK_PKCS11_FUNCTION_INFO(C_GetDebugString)
#ifdef CK_NEED_ARG_LIST
(
	CK_BYTE_PTR			pDebugString,
	CK_ULONG_PTR		pulDebugLen
);
#endif
#endif	// 141006, NKHA - N3CRYPTO_DEBUG

CK_PKCS11_FUNCTION_INFO(C_SetSerialID)
#ifdef CK_NEED_ARG_LIST
(
#ifndef SOLACIA_PKCS11
	CK_SESSION_HANDLE	hSession,
#endif
	CK_BYTE_PTR			pSerial
);
#endif



CK_PKCS11_FUNCTION_INFO(C_Loopback)
#ifdef CK_NEED_ARG_LIST
(
	CK_BYTE_PTR			pData,
	CK_ULONG			ulDataLen
);
#endif



/* 150416, NKHA - add APIs to inject PI */
CK_PKCS11_FUNCTION_INFO(C_EraseSRAM)
#ifdef CK_NEED_ARG_LIST
(
	CK_SLOT_ID			slotID
);
#endif



CK_PKCS11_FUNCTION_INFO(C_InjectPI)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,
	CK_BYTE_PTR			pData,
	CK_ULONG			ulDataLen,
	CK_ULONG			ulBlockCounter,
	CK_ULONG_PTR		pulSentBytes,
	CK_VERSION			PIversion
);
#endif


CK_PKCS11_FUNCTION_INFO(C_SetRTC)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,
	CK_VOID_PTR			pTime,
	CK_ULONG			ulTimeLen
);
#endif



CK_PKCS11_FUNCTION_INFO(C_GetRTC)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,
	CK_VOID_PTR			pTime,
	CK_ULONG_PTR		pulTimeLen
);
#endif

// 150822, NKHA - add new api to handle debug commands
CK_PKCS11_FUNCTION_INFO(C_DebugInfo)
#ifdef CK_NEED_ARG_LIST
(
	CK_SESSION_HANDLE	hSession,
	CK_BYTE_PTR			pDebugCmd,
	CK_ULONG			ulDebugCmdLen,
	CK_ULONG_PTR		pulDebugRspLen
);
#endif

CK_PKCS11_FUNCTION_INFO(C_SetDebugString)
#ifdef CK_NEED_ARG_LIST
(
	CK_BYTE_PTR			pDebugString
);
#endif

CK_PKCS11_FUNCTION_INFO(C_GetDebugString)
#ifdef CK_NEED_ARG_LIST
(
	CK_BYTE_PTR			pDebugString,
	CK_ULONG_PTR		pulDebugLen
);
#endif

#endif	// cloud add 2011.2.28 - x_ 함수 추가

