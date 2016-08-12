/* vi: set sw=4 ts=4: */

#ifndef _COMMAND_BLOCK_H_
#define _COMMAND_BLOCK_H_

#ifdef __cplusplus
extern	"C" {
#endif

#include "conf.h"
#include "n3type.h"
#include "cryptoki.h"



#pragma pack(push, 1)

/* sizeof(CI_BLOCK_COMMON) = 8 */
typedef struct  
{
	UCHAR		ucCmd;
	// ?????? Á¾???? ?Ö¾? ??????.
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	// ?????Ì½? ?????Ì¹??? ????À» ?????? ?? Card Type?? Slot ID?? Á¶???Ø¼? ??????.
	USHORT		usReturnCode;
} CI_BLOCK_COMMON, CK_PTR CI_BLOCK_COMMON_PTR, CK_PTR PCI_BLOCK_COMMON;



/* 
 *	The following is structure of CMD/RSP message 
 *
 *		|<-------------- MAX_BLOCK_MSG_LEN ---------------->|
 *		+====================+==============================+
 *		| MAX_BLOCK_HEAD_LEN |      MAX_BLOCK_DATA_LEN      |
 *		+====================+==============================+
 *
 *	MAX_BLOCK_DATA_LEN = 2048 - 128 = 1920 (15 blocks for NaSH)
 *	MAX_BLOCK_HEAD_LEN = 8
 *	MAX_BLOCK_MSG_LEN = MAX_BLOCK_DATA_LEN + MAX_BLOCK_HEAD_LEN = 1928
 *
 *	128 @ MAX_BLOCK_DATA_LEN means block size of NaSH
 *
 *	Reason to avoid 1024n size for CMD/RSP message
 *		Zero-length packet(ZLP) should be sent for every (message%1025=0) case
 *
 */
#define MAX_BLOCK_DATA_LEN			(2048 - 128)
#define MAX_BLOCK_HEAD_LEN			sizeof(CI_BLOCK_COMMON)
#define MAX_BLOCK_MSG_LEN			(MAX_BLOCK_DATA_LEN + MAX_BLOCK_HEAD_LEN)

#define	MAX_BLOCK_IV_LEN			32
#define MAX_BLOCK_SIGN_LEN			64
#define MAX_BLOCK_AAD_LEN			32
#define MAX_BLOCK_KEY_LEN			32
#define MAX_BLOCK_PACKET_IV_LEN		8
#define MAX_BLOCK_SLOT_ID_NUM		32
#define MAX_BLOCK_OBJECT_ID_NUM		128


//#define M_AX_FIND_OBJECT_HANDLE_NUM	256
//#define M_AX_TLV_SIZE				MAX_BLOCK_DATA_LEN
//#define MAX_RANDOM_BUF_SIZE			MAX_BLOCK_DATA_LEN
//#define MAX_CRYPT_BLOCK_SIZE		32
//#define MAX_DIGEST_BLOCK_SIZE		64
//#define MAX_SIGN_BLOCK_SIZE			256
//#define UTC_TIME_SIZE				16




typedef CI_BLOCK_COMMON		CK_BLOCK_INITIALIZE;
typedef CI_BLOCK_COMMON		CK_BLOCK_FINALIZE;


typedef	struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	CK_TOKEN_INFO		tokenInfo;
} CK_BLOCK_GET_TOKEN_INFO, CK_PTR CK_BLOCK_GET_TOKEN_INFO_PTR, CK_PTR PCK_BLOCK_GET_TOKEN_INFO;



typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	CK_BYTE		pucLabel[MAX_LABEL_LEN];
	ULONG		ulPinLength;
	CK_BYTE		pucDefaultPin[MAX_PIN_LEN];
} CK_BLOCK_INIT_TOKEN, CK_PTR CK_BLOCK_INIT_TOKEN_PTR, CK_PTR PCK_BLOCK_INIT_TOKEN;


typedef struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	CK_INFO		info;
} CK_BLOCK_GET_INFO, CK_PTR CK_BLOCK_GET_INFO_PTR, CK_PTR PCK_BLOCK_GET_INFO;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulPinLength;
	CK_BYTE		pucPin[MAX_PIN_LEN];
} CK_BLOCK_INIT_PIN, CK_PTR CK_BLOCK_INIT_PIN_PTR, CK_PTR PCK_BLOCK_INIT_PIN;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulOldPinLength;
	ULONG		ulNewPinLength;
	CK_BYTE		pucOldPin[MAX_PIN_LEN];
	CK_BYTE		pucNewPin[MAX_PIN_LEN];
} CK_BLOCK_SET_PIN, CK_PTR CK_BLOCK_SET_PIN_PTR, CK_PTR PCK_BLOCK_SET_PIN;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	CK_SESSION_INFO info;
} CK_BLOCK_GET_SESSION_INFO, CK_PTR CK_BLOCK_GET_SESSION_INFO_PTR, CK_PTR PCK_BLOCK_GET_SESSION_INFO;





typedef	struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	ULONG		bTokenPresent;
	ULONG		ulCount;
	CK_SLOT_ID	slotList[MAX_BLOCK_SLOT_ID_NUM];
} CK_BLOCK_GET_SLOT_LIST, CK_PTR CK_BLOCK_GET_SLOT_LIST_PTR, CK_PTR PCK_BLOCK_GET_SLOT_LIST;

typedef	struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	ULONG			ulSlotID;
	CK_SLOT_INFO	slotInfo;
} CK_BLOCK_GET_SLOT_INFO, CK_PTR CK_BLOCK_GET_SLOT_INFO_PTR, CK_PTR PCK_BLOCK_GET_SLOT_INFO;

typedef	struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	ULONG		ulFlag;
	ULONG		ulApplication;
	ULONG		ulNotify;
	ULONG		ulReserved;
} CK_BLOCK_OPEN_SESSION, CK_PTR CK_BLOCK_OPEN_SESSION_PTR, CK_PTR PCK_BLOCK_OPEN_SESSION;

typedef	struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
} CK_BLOCK_CLOSE_SESSION, CK_PTR CK_BLOCK_CLOSE_SESSION_PTR, CK_PTR PCK_BLOCK_CLOSE_SESSION;

typedef	struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
} CK_BLOCK_CLOSE_ALL_SESSIONS, CK_PTR CK_BLOCK_CLOSE_ALL_SESSIONS_PTR, 
	CK_PTR PCK_BLOCK_CLOSE_ALL_SESSIONS;


typedef	struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	ULONG		userType;
	ULONG		ulPinLen;
	CK_BYTE		sucPin[MAX_PIN_LEN];
} CK_BLOCK_LOGIN, CK_PTR CK_BLOCK_LOGIN_PTR, CK_PTR PCK_BLOCK_LOGIN;




typedef struct {
	CK_ATTRIBUTE_TYPE	type;
	ULONG				ulValueLen;
	CK_BYTE				pValue[1];
} CK_OBJECT_TLV;


#define GET_ADDR_ALIGN(offset,align)	\
		(offset + (align - ((unsigned long int)offset % align)) % align)



typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	ULONG		ulCount;
	ULONG		ulObjectID;

	UCHAR		pucTemplateTLV[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*2];
} CK_BLOCK_CREATE_OBJECT, CK_PTR CK_BLOCK_CREATE_OBJECT_PTR, CK_PTR PCK_BLOCK_CREATE_OBJECT;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	// 150910, eckim - CK_BLOCK_COPY_OBJECT is modified
	ULONG		ulObjectID;
	ULONG		ulNewObjectID;
	ULONG		ulCount;
	ULONG		ulReserved;

	UCHAR		pucTemplateTLV[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*4];
} CK_BLOCK_COPY_OBJECT, CK_PTR CK_BLOCK_COPY_OBJECT_PTR, CK_PTR PCK_BLOCK_COPY_OBJECT;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulObjectID;
} CK_BLOCK_DESTROY_OBJECT, CK_PTR CK_BLOCK_DESTROY_OBJECT_PTR, CK_PTR PCK_BLOCK_DESTROY_OBJECT;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	union {
		ULONG		ulObjectID;
		ULONG		ulSize;
	} uCommon;
} CK_BLOCK_GET_OBJECT_SIZE, CK_PTR CK_BLOCK_GET_OBJECT_SIZE_PTR, CK_PTR PCK_BLOCK_GET_OBJECT_SIZE;


typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	ULONG		ulObjectID;
	ULONG		ulAttributeCount;

	UCHAR		pucTemplateTLV[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*2];
} CK_BLOCK_GET_ATTRIBUTE_VALUE, CK_PTR CK_BLOCK_GET_ATTRIBUTE_VALUE_PTR, CK_PTR PCK_BLOCK_GET_ATTRIBUTE_VALUE;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulAttributeCount;

	UCHAR		pucTemplateTLV[MAX_BLOCK_DATA_LEN - sizeof(ULONG)];
} CK_BLOCK_RES_GET_ATTRIBUTE_VALUE, CK_PTR CK_BLOCK_RES_GET_ATTRIBUTE_VALUE_PTR, CK_PTR PCK_BLOCK_RES_GET_ATTRIBUTE_VALUE;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulObjectID;
	ULONG		ulAttributeCount;

	UCHAR		pucTemplateTLV[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*2];
} CK_BLOCK_SET_ATTRIBUTE_VALUE, CK_PTR CK_BLOCK_SET_ATTRIBUTE_VALUE_PTR, CK_PTR PCK_BLOCK_SET_ATTRIBUTE_VALUE;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulAttributeCount;

	UCHAR		pucTemplateTLV[MAX_BLOCK_DATA_LEN - sizeof(ULONG)];
} CK_BLOCK_FIND_OBJECTS_INIT, CK_PTR CK_BLOCK_FIND_OBJECTS_INIT_PTR, CK_PTR PCK_BLOCK_FIND_OBJECTS_INIT;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG 		ulMaxObjectCount;
	ULONG 		ulObjectCount;

	ULONG		pulObjectIdTable[MAX_BLOCK_OBJECT_ID_NUM];
} CK_BLOCK_FIND_OBJECTS, CK_PTR CK_BLOCK_FIND_OBJECTS_PTR, CK_PTR PCK_BLOCK_FIND_OBJECTS;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
} CK_BLOCK_FIND_OBJECTS_FINAL, CK_PTR CK_BLOCK_FIND_OBJECTS_FINAL_PTR, CK_PTR PCK_BLOCK_FIND_OBJECTS_FINAL;
















typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	CK_BYTE		pucSeed[MAX_BLOCK_DATA_LEN];	
} CK_BLOCK_SEED_RANDOM, CK_PTR CK_BLOCK_SEED_RANDOM_PTR, CK_PTR PCK_BLOCK_SEED_RANDOM;

#define SIZEOF_CK_BLOCK_GENERATE_RANDOM		(sizeof(CI_BLOCK_COMMON)+sizeof(ULONG))

typedef struct 
{
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	CK_BYTE		pucRandom[MAX_BLOCK_DATA_LEN];	
} CK_BLOCK_GENERATE_RANDOM, CK_PTR CK_BLOCK_GENERATE_RANDOM_PTR, CK_PTR PCK_BLOCK_GENERATE_RANDOM;



















typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	union {
		ULONG	ulMechanism;
		ULONG	ulKeyObjectID;
	} uCommon;
	
	ULONG		ulVenderDefinedType;
	ULONG		ulVenderDefinedValueLength;
	ULONG		ulKeyObjectID;
	ULONG		ulCount;
	ULONG		ulReserved;


	UCHAR		pucTlvTemplate[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*6];
} CK_BLOCK_GENERATE_KEY, CK_PTR CK_BLOCK_GENERATE_KEY_PTR, CK_PTR PCK_BLOCK_GENERATE_KEY;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulWrappedKeyID;
	ULONG		ulWrappingKeyID;
	ULONG		ulMechanism;
	ULONG		ulMechanismLen;
	ULONG		ulWrappedKeyLen;
	ULONG		ulReserved;

	UCHAR		pucMechanismBuffer[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*6];
} CK_BLOCK_WRAP_KEY, CK_PTR CK_BLOCK_WRAP_KEY_PTR, CK_PTR PCK_BLOCK_WRAP_KEY;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulUnwrappingKeyID;
	ULONG		ulMechanism;
	ULONG		ulMechanismLen;
	ULONG		ulWrappedKeyLen;	
	ULONG		ulAttributeCount;
	ULONG		ulVenderDefinedType;
	ULONG		ulWrappedKeyID;
	ULONG		ulUnwrappedKeyID;

	UCHAR		pucMechanismBuffer[MAX_BLOCK_IV_LEN];
	UCHAR		pucWrappedKey[MAX_BLOCK_KEY_LEN];
	UCHAR		pucWrappedKeyTemplateTLV[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*8 - MAX_BLOCK_KEY_LEN - MAX_BLOCK_IV_LEN];
} CK_BLOCK_UNWRAP_KEY, CK_PTR CK_BLOCK_UNWRAP_KEY_PTR, CK_PTR PCK_BLOCK_UNWRAP_KEY;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulMechanism;
	ULONG		ulInputParamLen;
	ULONG		ulBaseKeyID;
	ULONG		ulAttributeCount;
	ULONG		ulVenderDefinedType;
	ULONG		ulVenderDefinedValueLength;
	ULONG		ulKeyID;
	ULONG		ulReserved;

	UCHAR		pucParamTemplateTLV[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*8];
} CK_BLOCK_DERIVE_KEY, CK_PTR CK_BLOCK_DERIVE_KEY_PTR, CK_PTR PCK_BLOCK_DERIVE_KEY;






































/* Encrypt, Decrypt, Digest, Verify, Sign, Packet */
typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	ULONG		ulMechanism;
	ULONG		ulKeyId;
	UCHAR		ucIV[MAX_BLOCK_IV_LEN];
}
CK_BLOCK_CRYPT_INIT,		CK_PTR PCK_BLOCK_CRYPT_INIT,
CK_BLOCK_DIGEST_INIT,		CK_PTR PCK_BLOCK_DIGEST_INIT,
CK_BLOCK_VERIFY_INIT,		CK_PTR PCK_BLOCK_VERIFY_INIT,
CK_BLOCK_SIGN_INIT,			CK_PTR PCK_BLOCK_SIGN_INIT,
CK_BLOCK_PACKET_CRYPT_INIT,	CK_PTR PCK_BLOCK_PACKET_CRYPT_INIT;

typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	UCHAR		sucData[MAX_BLOCK_DATA_LEN];
}
CK_BLOCK_CRYPT_UPDATE,		CK_PTR PCK_BLOCK_CRYPT_UPDATE,
CK_BLOCK_DIGEST_UPDATE,		CK_PTR PCK_BLOCK_DIGEST_UPDATE,
CK_BLOCK_VERIFY_UPDATE,		CK_PTR PCK_BLOCK_VERIFY_UPDATE,
CK_BLOCK_SIGN_UPDATE,		CK_PTR PCK_BLOCK_SIGN_UPDATE,
CK_BLOCK_PACKET_CRYPT_UPDATE, CK_PTR PCK_BLOCK_PACKET_CRYPT_UPDATE;


typedef CK_BLOCK_CRYPT_UPDATE
CK_BLOCK_CRYPT,				CK_PTR PCK_BLOCK_CRYPT,
CK_BLOCK_DIGEST,			CK_PTR PCK_BLOCK_DIGEST,
CK_BLOCK_SIGN,				CK_PTR PCK_BLOCK_SIGN,
CK_BLOCK_PACKET_CRYPT, 		CK_PTR PCK_BLOCK_PACKET_CRYPT;


typedef struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	CK_BYTE		sucData[MAX_BLOCK_DATA_LEN];	
}
CK_BLOCK_CRYPT_FINAL, 		CK_PTR PCK_BLOCK_CRYPT_FINAL,
CK_BLOCK_DIGEST_FINAL, 		CK_PTR PCK_BLOCK_DIGEST_FINAL,
CK_BLOCK_VERIFY_FINAL, 		CK_PTR PCK_BLOCK_VERIFY_FINAL,
CK_BLOCK_SIGN_FINAL, 		CK_PTR PCK_BLOCK_SIGN_FINAL,
CK_BLOCK_PACKET_CRYPT_FINAL, CK_PTR PCK_BLOCK_PACKET_CRYPT_FINAL;


typedef struct {
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	ULONG		ulMessageLen;
	ULONG		ulSignatureLen;
	UCHAR		sucData[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*2];
}
CK_BLOCK_VERIFY,		CK_PTR PCK_BLOCK_VERIFY;



typedef struct 
{
	/* CI_BLOCK_COMON */
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	/* for SignEncrypt or DecryptVerify */	
	ULONG		ulMechanism;
	ULONG		ulKeyId;

	ULONG		ulMacMechanism;
	ULONG		ulMacKeyId;

	ULONG		ulIVLen;
	UCHAR		ucIV[MAX_BLOCK_IV_LEN];
} 
CK_BLOCK_PACKET_DUAL_CRYPT_INIT,
CK_PTR PCK_BLOCK_PACKET_DUAL_CRYPT_INIT,
CK_PTR CK_BLOCK_PACKET_DUAL_CRYPT_INIT_PTR;



typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;			// size of (ulIVLen)~(sucData) in bytes
	USHORT		usReturnCode;

	ULONG		ulIVLen;
	ULONG		ulAADLen;
	ULONG		ulPacketLen;
	ULONG		ulSignatureLen;

#define MAX_BLOCK_DUAL_HEAD_LEN	(MAX_BLOCK_SIGN_LEN + MAX_BLOCK_IV_LEN)
#define MAX_BLOCK_DUAL_DATA_LEN	(MAX_BLOCK_DATA_LEN - MAX_BLOCK_DUAL_HEAD_LEN)
	UCHAR		sucIV[MAX_BLOCK_IV_LEN];
	UCHAR		sucSignature[MAX_BLOCK_SIGN_LEN];
	UCHAR		sucPacket[MAX_BLOCK_DUAL_DATA_LEN - sizeof(ULONG)*4];
} 
CK_BLOCK_PACKET_DUAL_CRYPT_UPDATE, 
CK_PTR PCK_BLOCK_PACKET_DUAL_CRYPT_UPDATE,
CK_PTR CK_BLOCK_PACKET_DUAL_CRYPT_UPDATE_PTR; 


typedef CI_BLOCK_COMMON
CK_BLOCK_PACKET_DUAL_CRYPT_FINAL,
CK_PTR PCK_BLOCK_PACKET_DUAL_CRYPT_FINAL,
CK_PTR CK_BLOCK_PACKET_DUAL_CRYPT_FINAL_PTR;



























typedef struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;			// size of (ulIVLen)~(sucData) in bytes
	USHORT		usReturnCode;

	UCHAR		sucID [MAX_MODULE_ID_LEN];
} CK_BLOCK_SETID, CK_PTR CK_BLOCK_SETID_PTR, CK_PTR PCK_BLOCK_SETID;


typedef struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;			// size of (ulIVLen)~(sucData) in bytes
	USHORT		usReturnCode;

	UCHAR		sucData[MAX_BLOCK_DATA_LEN];			// data
} CK_BLOCK_LOOPBACK, CK_PTR	CK_BLOCK_LOOPBACK_PTR, CK_PTR	PCK_BLOCK_LOOPBACK;


typedef struct
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;			// size of (ulIVLen)~(sucData) in bytes
	USHORT		usReturnCode;

	ULONG		ulSeqNum;
	ULONG		ulLastFlag;
	ULONG		ulProgLen;			// actual size of sucData
	ULONG		ulVersion;			// PI version

	UCHAR		sucData[MAX_BLOCK_DATA_LEN - sizeof(ULONG)*4];
} CK_BLOCK_INJECT_PI, *CK_BLOCK_INJECT_PI_PTR, *PCK_BLOCK_INJECT_PI_PTR;



typedef struct
{
#define BLOCK_TIME_LEN			32
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;			// size of (ulIVLen)~(sucData) in bytes
	USHORT		usReturnCode;

	CK_RTC_TIME	rtc;
} CK_BLOCK_CLOCK, *CK_BLOCK_CLOCK_PTR, *PCK_BLOCK_CLOCK;



typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	union {
		ULONG	ulMechanism;
		ULONG	ulResultLen;
	} uCommon;

	UCHAR		pucDiagResult[MAX_BLOCK_DATA_LEN - sizeof(ULONG)];
} CK_BLOCK_DIAG_MODULE, CK_PTR CK_BLOCK_DIAG_MODULE_PTR, CK_PTR PCK_BLOCK_DIAG_MODULE;



typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;

	UCHAR		sucData[MAX_BLOCK_DATA_LEN];
} CK_BLOCK_DEBUG, CK_PTR CK_BLOCK_DEBUG_PTR, CK_PTR PCK_BLOCK_DEBUG;











/*
#define SIZEOF_CK_BLOCK_HIGH_PACKET_ENCRYPT_UPDATE_HEADER			sizeof(CI_BLOCK_COMMON)
#define SIZEOF_CK_BLOCK_RESPONSE_HIGH_PACKET_ENCRYPT_UPDATE_HEADER	(sizeof(CI_BLOCK_COMMON)+MAX_BLOCK_PACKET_IV_LEN)

typedef struct {
	CK_BYTE		pucIV[MAX_BLOCK_PACKET_IV_LEN];
	CK_BYTE		sucCipherText[1];
} HIGH_PACKET_ENCRYPT_UPDATE_RES, *PHIGH_PACKET_ENCRYPT_UPDATE_RES;

typedef struct 
{
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	union {
		CK_BYTE		sucPlainText[1];
		HIGH_PACKET_ENCRYPT_UPDATE_RES resHighPacketEncryptUpdate;
	} uCommon;
} CK_BLOCK_HIGH_PACKET_ENCRYPT_UPDATE, CK_PTR CK_BLOCK_HIGH_PACKET_ENCRYPT_UPDATE_PTR, CK_PTR PCK_BLOCK_HIGH_PACKET_ENCRYPT_UPDATE;

#define SIZEOF_CK_BLOCK_HIGH_PACKET_DECRYPT_UPDATE_HEADER			(sizeof(CI_BLOCK_COMMON)+MAX_BLOCK_PACKET_IV_LEN)
#define SIZEOF_CK_BLOCK_RESPONSE_HIGH_PACKET_DECRYPT_UPDATE_HEADER	sizeof(CI_BLOCK_COMMON)

typedef struct {
	CK_BYTE		pucIV[MAX_BLOCK_PACKET_IV_LEN];
	CK_BYTE		sucCipherText[1];
} HIGH_PACKET_DECRYPT_UPDATE_REQ, *PHIGH_PACKET_DECRYPT_UPDATE_REQ;

typedef struct 
{
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	union {		
		HIGH_PACKET_DECRYPT_UPDATE_REQ reqHighPacketDecryptUpdate;
		CK_BYTE		sucPlainText[1];
	} uCommon;
} CK_BLOCK_HIGH_PACKET_DECRYPT_UPDATE, CK_PTR CK_BLOCK_HIGH_PACKET_DECRYPT_UPDATE_PTR, CK_PTR PCK_BLOCK_HIGH_PACKET_DECRYPT_UPDATE;
*/



/*
typedef struct 
{
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulMechanism;
	ULONG		ulKeyID;
	UCHAR		pucIV[MAX_BLOCK_IV_LEN];
} CK_BLOCK_SIGN_INIT, CK_PTR CK_BLOCK_SIGN_INIT_PTR, CK_PTR PCK_BLOCK_SIGN_INIT;





typedef struct 
{
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	CK_BYTE		sucData[1];	
} CK_BLOCK_SIGN_UPDATE, CK_PTR CK_BLOCK_SIGN_UPDATE_PTR, CK_PTR PCK_BLOCK_SIGN_UPDATE;

typedef struct 
{
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	CK_BYTE		pucDigest[MAX_SIGN_BLOCK_SIZE+UTC_TIME_SIZE];	
} CK_BLOCK_SIGN_FINAL, CK_PTR CK_BLOCK_SIGN_FINAL_PTR, CK_PTR PCK_BLOCK_SIGN_FINAL;


typedef struct 
{
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	ULONG		ulMechanism;
	ULONG		ulKeyID;
	UCHAR		pucIV[MAX_BLOCK_IV_LEN];
} CK_BLOCK_VERIFY_INIT, CK_PTR CK_BLOCK_VERIFY_INIT_PTR, CK_PTR PCK_BLOCK_VERIFY_INIT;

#define SIZEOF_CK_BLOCK_VERIFY_UPDATE_HEADER		(sizeof(CI_BLOCK_COMMON))

typedef struct 
{
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	CK_BYTE		sucData[1];	
} CK_BLOCK_VERIFY_UPDATE, CK_PTR CK_BLOCK_VERIFY_UPDATE_PTR, CK_PTR PCK_BLOCK_VERIFY_UPDATE;

typedef struct 
{
	// CI_BLOCK_COMMON
	UCHAR		ucCmd;
	UCHAR		ucCmdType;
	USHORT		usSessionID;
	USHORT		usDataLen;
	USHORT		usReturnCode;
	
	CK_BYTE		pucDigest[MAX_SIGN_BLOCK_SIZE+UTC_TIME_SIZE];	
} CK_BLOCK_VERIFY_FINAL, CK_PTR CK_BLOCK_VERIFY_FINAL_PTR, CK_PTR PCK_BLOCK_VERIFY_FINAL;
*/




#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif
