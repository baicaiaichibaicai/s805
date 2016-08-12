/* vi: set ts=4 sw=4: */
#ifndef _PKCS11T_N3CRYPTO_H_
#define _PKCS11T_N3CRYPTO_H_

#include "pkcs11t_debug.h"


/* ------------------------------------------------------------------------- *
	Vendor-defined Return Value
 * ------------------------------------------------------------------------- */
#define	CKR_OBJECT_ALREADY_CREATED		(CKR_VENDOR_DEFINED | 0x1000)


/* ------------------------------------------------------------------------- *
	Vendor-defined Object Handles
 * ------------------------------------------------------------------------- */
#define CKA_OBJECT_HANDLE				(CKA_VENDOR_DEFINED + 0x100)
#define CKO_OBJECT_HANDLE_MAX			(CKO_VENDOR_DEFINED + 0x1000)
#define CKO_OBJECT_HANDLE_MIN			(CKO_VENDOR_DEFINED + 0x0100)


#define CKO_CRYPT_SEC_KEY_BASE			(CKO_VENDOR_DEFINED + 0xb10)

#define CKO_CRYPT_RSA_CERT_BASE			(CKO_VENDOR_DEFINED + 0x200)
#define CKO_CRYPT_RSA_PUB_KEY_BASE		(CKO_VENDOR_DEFINED + 0x500)
#define CKO_CRYPT_RSA_PRI_KEY_BASE		(CKO_VENDOR_DEFINED + 0x900)
#define CKO_SIGN_RSA_CERT_BASE			(CKO_VENDOR_DEFINED + 0x300)
#define CKO_SIGN_RSA_PUB_KEY_BASE		(CKO_VENDOR_DEFINED + 0x600)
#define CKO_SIGN_RSA_PRI_KEY_BASE		(CKO_VENDOR_DEFINED + 0xa00)

#define CKO_CRYPT_ECC_CERT_BASE			(CKO_VENDOR_DEFINED + 0x210)
#define CKO_CRYPT_ECC_PUB_KEY_BASE		(CKO_VENDOR_DEFINED + 0x510)
#define CKO_CRYPT_ECC_PRI_KEY_BASE		(CKO_VENDOR_DEFINED + 0x910)
#define CKO_SIGN_ECC_CERT_BASE			(CKO_VENDOR_DEFINED + 0x310)
#define CKO_SIGN_ECC_PUB_KEY_BASE		(CKO_VENDOR_DEFINED + 0x610)
#define CKO_SIGN_ECC_PRI_KEY_BASE		(CKO_VENDOR_DEFINED + 0xa10)



/* fixed handle of secret keys */
#define CKO_CRYPT_SEC_QKEY				(CKO_CRYPT_SEC_KEY_BASE + 0)
#define CKO_CRYPT_SEC_AKEY				(CKO_CRYPT_SEC_KEY_BASE + 1)

/* fixed handle of private keys */
#define CKO_CRYPT_RSA_PRI_KEY			(CKO_CRYPT_RSA_PRI_KEY_BASE + 2)
#define CKO_CRYPT_ECC_PRI_KEY			(CKO_CRYPT_ECC_PRI_KEY_BASE + 2)
#define CKO_SIGN_RSA_PRI_KEY			(CKO_SIGN_RSA_PRI_KEY_BASE  + 2)
#define CKO_SIGN_ECC_PRI_KEY			(CKO_SIGN_ECC_PRI_KEY_BASE  + 2)

/* fixed handle of public keys */
#define CKO_CRYPT_RSA_PUB_KEY			(CKO_CRYPT_RSA_PUB_KEY_BASE + 2)
#define CKO_CRYPT_ECC_PUB_KEY			(CKO_CRYPT_ECC_PUB_KEY_BASE + 2)
#define CKO_SIGN_RSA_PUB_KEY			(CKO_SIGN_RSA_PUB_KEY_BASE  + 2)
#define CKO_SIGN_ECC_PUB_KEY			(CKO_SIGN_ECC_PUB_KEY_BASE  + 2)

/* fixed handle of certificates */
#define CKO_CRYPT_RSA_CERT_USER			(CKO_CRYPT_RSA_CERT_BASE + 2)
#define CKO_CRYPT_ECC_CERT_USER			(CKO_CRYPT_ECC_CERT_BASE + 2)
#define CKO_SIGN_RSA_CERT_ROOT			(CKO_SIGN_RSA_CERT_BASE + 0)
#define CKO_SIGN_RSA_CERT_UCA 			(CKO_SIGN_RSA_CERT_BASE + 1)
#define CKO_SIGN_RSA_CERT_USER			(CKO_SIGN_RSA_CERT_BASE + 2)
#define CKO_SIGN_RSA_CERT_KMTS			(CKO_SIGN_RSA_CERT_BASE + 4)
#define CKO_SIGN_ECC_CERT_ROOT			(CKO_SIGN_ECC_CERT_BASE + 0)
#define CKO_SIGN_ECC_CERT_USER			(CKO_SIGN_ECC_CERT_BASE + 2)
#define CKO_SIGN_ECC_CERT_UCA 			(CKO_SIGN_ECC_CERT_BASE + 1)
#define CKO_SIGN_ECC_CERT_KMTS			(CKO_SIGN_ECC_CERT_BASE + 4)


#define MASK_OBJECT_HANDLE_INDEX(x)		(x & 0xffff)

/* ------------------------------------------------------------------------- *
	Vendor-defined Attributes
 * ------------------------------------------------------------------------- */
#define CKA_EC_COFACTOR					(CKA_VENDOR_DEFINED + 0x0011)
#define CKA_EC_ORDER					(CKA_VENDOR_DEFINED + 0x0012)
#define CKA_EC_CONST_A 					(CKA_VENDOR_DEFINED + 0x0013)
#define CKA_EC_CONST_B 					(CKA_VENDOR_DEFINED + 0x0014)
#define CKA_EC_BASE_X  					(CKA_VENDOR_DEFINED + 0x0015)
#define CKA_EC_BASE_Y  					(CKA_VENDOR_DEFINED + 0x0016)
#define CKA_EC_Q_X  					(CKA_VENDOR_DEFINED + 0x0017)
#define CKA_EC_Q_Y  					(CKA_VENDOR_DEFINED + 0x0018)






/* ------------------------------------------------------------------------- *
	Vendor-defined Mechanisms
 * ------------------------------------------------------------------------- */
#define CKK_NSRI						(CKK_VENDOR_DEFINED + 0x01000001)

#define CKM_NSRI_ECHO					(CKM_VENDOR_DEFINED + 0x0010)
#define CKM_NSRI_CIPC					(CKM_VENDOR_DEFINED + 0x0100)
#define CKM_NSRI_CIPD					(CKM_VENDOR_DEFINED + 0x0200)
#define CKM_NSRI_IKE_R1					(CKM_VENDOR_DEFINED + 0x0300)
#define CKM_NSRI_IKE_E1					(CKM_VENDOR_DEFINED + 0x0400)
#define CKM_NSRI_HASH					(CKM_VENDOR_DEFINED + 0x0500)
#define CKM_NSRI_ARIA					(CKM_VENDOR_DEFINED + 0x0600)
#define CKM_NSRI_CIPE					(CKM_VENDOR_DEFINED + 0x0700)
#define CKM_NSRI_IKE_S1					(CKM_VENDOR_DEFINED + 0x0800)

#define CKM_NSRI_CIPC_ECB				(CKM_NSRI_CIPC + 0x01)
#define CKM_NSRI_CIPC_CBC				(CKM_NSRI_CIPC + 0x02)
#define CKM_NSRI_CIPC_OFB				(CKM_NSRI_CIPC + 0x03)
#define CKM_NSRI_CIPC_CFB				(CKM_NSRI_CIPC + 0x04)
#define CKM_NSRI_CIPC_CTR				(CKM_NSRI_CIPC + 0x05)
#define CKM_NSRI_CIPC_GCM				(CKM_NSRI_CIPC + 0x06)

#define CKM_NSRI_CIPD_ECB				(CKM_NSRI_CIPD + 0x01)
#define CKM_NSRI_CIPD_CBC				(CKM_NSRI_CIPD + 0x02)
#define CKM_NSRI_CIPD_OFB				(CKM_NSRI_CIPD + 0x03)
#define CKM_NSRI_CIPD_CFB				(CKM_NSRI_CIPD + 0x04)
#define CKM_NSRI_CIPD_CTR				(CKM_NSRI_CIPD + 0x05)
#define CKM_NSRI_CIPD_GCM				(CKM_NSRI_CIPD + 0x06)

#define CKM_NSRI_CIPE_CBC				(CKM_NSRI_CIPE + 0x01)
#define CKM_NSRI_CIPE_ECB				(CKM_NSRI_CIPE + 0x02)
#define CKM_NSRI_CIPE_OFB				(CKM_NSRI_CIPE + 0x03)
#define CKM_NSRI_CIPE_CFB				(CKM_NSRI_CIPE + 0x04)
#define CKM_NSRI_CIPE_CTR				(CKM_NSRI_CIPE + 0x07)

#define CKM_NSRI_IKE_R1_UMPROC1			(CKM_NSRI_IKE_R1 + 0x01)
#define CKM_NSRI_IKE_R1_UMPROC2			(CKM_NSRI_IKE_R1 + 0x02)
#define CKM_NSRI_IKE_R1_UMPROC3			(CKM_NSRI_IKE_R1 + 0x03)
#define CKM_NSRI_IKE_R1_UMPROC4			(CKM_NSRI_IKE_R1 + 0x04)
#define CKM_NSRI_IKE_R1_REKEY1			(CKM_NSRI_IKE_R1 + 0x05)
#define CKM_NSRI_IKE_R1_REKEY2			(CKM_NSRI_IKE_R1 + 0x06)
#define CKM_NSRI_IKE_R1_REKEY3			(CKM_NSRI_IKE_R1 + 0x07)

#define CKM_NSRI_IKE_E1_UMPROC1			(CKM_NSRI_IKE_E1 + 0x01)
#define CKM_NSRI_IKE_E1_UMPROC2			(CKM_NSRI_IKE_E1 + 0x02)
#define CKM_NSRI_IKE_E1_UMPROC3			(CKM_NSRI_IKE_E1 + 0x03)
#define CKM_NSRI_IKE_E1_UMPROC4			(CKM_NSRI_IKE_E1 + 0x04)

#define CKM_NSRI_IKE_S1_UMPROC1			(CKM_NSRI_IKE_S1 + 0x01)
#define CKM_NSRI_IKE_S1_UMPROC2			(CKM_NSRI_IKE_S1 + 0x02)
#define CKM_NSRI_IKE_S1_UMPROC3			(CKM_NSRI_IKE_S1 + 0x03)
#define CKM_NSRI_IKE_S1_UMPROC4			(CKM_NSRI_IKE_S1 + 0x04)

#define CKM_NSRI_HASH_HMAC				(CKM_NSRI_HASH + 0x01)
#define CKM_NSRI_HASH_NOPAD				(CKM_NSRI_HASH + 0x02)
#define CKM_NSRI_HASH_NOPAD_HMAC		(CKM_NSRI_HASH + 0x03)
#define CKM_SHA256_NOPAD				(CKM_NSRI_HASH + 0x04)
#define CKM_SHA256_NOPAD_HMAC			(CKM_NSRI_HASH + 0x05)

#define CKM_ARIA_ECB128					(CKM_NSRI_ARIA + 0x01)
#define CKM_ARIA_ECB192					(CKM_NSRI_ARIA + 0x02)
#define CKM_ARIA_ECB256					(CKM_NSRI_ARIA + 0x03)
#define CKM_ARIA_CTR					(CKM_NSRI_ARIA + 0x04)
#define CKM_ARIA_CTR128					(CKM_NSRI_ARIA + 0x05)
#define CKM_ARIA_CTR192					(CKM_NSRI_ARIA + 0x06)
#define CKM_ARIA_CTR256					(CKM_NSRI_ARIA + 0x07)
#define CKM_ARIA_OFB					(CKM_NSRI_ARIA + 0x08)
#define CKM_ARIA_OFB128					(CKM_NSRI_ARIA + 0x09)
#define CKM_ARIA_OFB192					(CKM_NSRI_ARIA + 0x0a)
#define CKM_ARIA_OFB256					(CKM_NSRI_ARIA + 0x0b)


#define	SLOT_ID_UNKNOWN					0xffff
#define	SESSION_ID_UNKNOWN				0xffff


typedef struct
{
	CK_OBJECT_HANDLE	hOtherPubKey;		// handle of other peer's public key
	CK_OBJECT_HANDLE	hMyPriKey;			// handle of my private key
	CK_BYTE				otherPE[256];		// value of other peer's PE(r||ID)
	CK_BYTE				otherAuth[32];
	CK_BYTE				myID[32];
	CK_BYTE				myPE[256];			// value of my PE(r||ID)
	CK_BYTE				myAuth[32];
	CK_OBJECT_HANDLE	hEIV;				// encryption IV handle
	CK_OBJECT_HANDLE	hDIV;				// decryption IV handle
	CK_OBJECT_HANDLE	hESK;				// encryption session key handle
	CK_OBJECT_HANDLE	hDSK;				// decryption session key handle
	CK_OBJECT_HANDLE	hEMK;				// signing key handle
	CK_OBJECT_HANDLE	hDMK;				// verifying key handle

#ifdef DEBUG_LIGHT
	/* Debugging */
	CK_ULONG			bDebug;				// enable debug if TRUE
	CK_BYTE				myNonce[32];
#endif
} CK_MECH_IKE_PARAMETER, CK_PTR PCK_MECH_IKE_PARAMETER;




typedef struct
{
	CK_ULONG			ulLen;			// length of aad or tag in bytes
	union {
		CK_BYTE			aad[32];		// in buffer to save aad, set byte 0 first
		CK_BYTE			tag[32];		// out buffer to save tag, set byte 0 first
	} gcm;


#define	LENGTH_GCM_IV	8				// length of iv in bytes (different every packet)
#define LENGTH_GCM_SALT	16				// length of salt in bytes (same in IPSec session)

	CK_ULONG			ulIVLen;		// length of IV in bytes
	CK_BYTE				iv[32];			// in buffer to save iv, set byte 0 first

	CK_ULONG			reserved;		// reserved for word alignment
	CK_ULONG			ulDataLen;		// length of packet data in bytes
} CK_GCM_PARAM, CK_PTR CK_GCM_PARAM_PTR;


typedef struct
{
#define PI_ALGORITHM_NAME           "nsr_crypto_algorithms.bin"
#define PI_DATA_OFFSET              0x800
#define PI_USB_G_N3CRYPTO_DRIVER    0x0001
#define PI_USB_N3CRYPTO_LB_DRIVER   0x0002
#define PI_PKCS11_N3CRYPTO_DRIVER   0x0003
#define PI_CRYPTO_ALGORITHM         0x0004

	char		filename[256];
	CK_ULONG	size;
	CK_ULONG	offset;
	CK_ULONG	type;
	CK_ULONG	reserved;
} CK_PI, *CK_PI_PTR;				// size=272



typedef struct
{
#define PI_MAGIC_CODE				0x19850104
#define PI_FILE_NUM					7
	CK_ULONG	magicNum;
	CK_VERSION	pi_version;
	CK_BYTE		reserved1[2];
	CK_ULONG	pi_count;
	CK_ULONG	reserved2;

	CK_PI		fileHdr[PI_FILE_NUM];
} CK_PI_HEADER, *CK_PI_HEADER_PTR;	// size=12+272*7=1916=0x77c



typedef struct {
	CK_ULONG	year;
	CK_ULONG	month;
	CK_ULONG	day;
	CK_ULONG	hour;
	CK_ULONG	min;
	CK_ULONG	sec;
} CK_RTC_TIME, *CK_RTC_TIME_PTR;



#endif /*_PKCS11T_N3CRYPTO_H_*/
