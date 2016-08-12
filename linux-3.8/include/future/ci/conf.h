/* vi: set ts=4 sw=4: */
/*
 *	File: conf.h
 *	Auth: Eunchan Kim
 *	Date: 2014.03.26
 *
 *
 *	Description
 *	v0.01 - 2015.07.21, NKHA
 *			not implemnted Dual-function, HMAC, Sign/verify, Digest
 *
 */

#ifndef _CONF_H_
#define _CONF_H_


#define PLATFORM_LINUX			// sel PLATFORM_LINUX or PLATFORM_WINDOWS
#define DEBUG							1


// SRAM configuration
//	BB_SRAM_SIZE        +----------------+ 0x0008_00000
//                      |                |
//                      |    320KB       | BB_SRAM_KEYS_SIZE
//                      |                |
//	BB_SRAM_KEYS_OFFSET +----------------+ 0x0000_30000
//                      |                |
//                      |    176KB       | BB_SRAM_ALGO_SIZE
//                      |                |
//	BB_SRAM_ALGO_OFFSET +----------------+ 0x0000_04000
//                      |                |
//                      |     16KB       | BB_SRAM_INFO_SIZE
//                      |                |
//	BB_SRAM_INFO_OFFSET +----------------+ 0x0000_00000
//
#define BB_SRAM_BASE_ADDR				0x100000	// fixme
#define BB_SRAM_SIZE					0x80000		// 512KB
#define BB_SRAM_INFO_OFFSET				0
#define BB_SRAM_ALGO_OFFSET				0x4000
#define BB_SRAM_KEYS_OFFSET				0x30000
#define BB_SRAM_INFO_SIZE				(BB_SRAM_ALGO_OFFSET - BB_SRAM_INFO_OFFSET)	//  16KB
#define BB_SRAM_ALGO_SIZE				(BB_SRAM_KEYS_OFFSET - BB_SRAM_ALGO_OFFSET)	// 176KB
#define BB_SRAM_KEYS_SIZE				(BB_SRAM_SIZE - BB_SRAM_KEYS_OFFSET)		// 320KB


// BOOT_ROM for Serial ID, ...
#define CM_EPROM_BASE_ADDR				0x00000000
#define CM_EPROM_SIZE					0x400		// 1KB



#define	DEFAULT_NODE_COUNT				100

// Versions required at CK_INFO
#define	CRYPTO_INTERFACE_MAJOR_VER		3
#define CRYPTO_INTERFACE_MINOR_VER		0
#define	CRYPTO_LIBRARY_MAJOR_VER		2
#define CRYPTO_LIBRARY_MINOR_VER		20

// versions required at CK_SLOT_INFO
#define CRYPTO_HW_MAJOR_VER				1
#define CRYPTO_HW_MINOR_VER				0




#define UTC_TIME_SIZE					16

#define MAX_MODULE_ID_LEN				16
#define MAX_MODEL_LEN				 	16
#define MAX_LABEL_LEN					32
#define MAX_LIB_DESCRIPT_LEN			32
#define MAX_MANUFACTURER_ID_LEN			32

#define MAX_USER_COUNT					1
#define MIN_UID_LEN						6
#define MAX_UID_LEN						128
#define MIN_PIN_LEN						6
#define MAX_PIN_LEN						16		// orig 128
#define MAX_HASHED_PIN_LEN				64		// 32 for SHA-256
#define DEFAULT_USER_PIN				"00000000"
#define DEFAULT_SO_PIN					"ZERO MODULE"

#define MAX_USER_LOGIN_RETRY_CNT		5
#define MAX_SO_LOGIN_RETRY_CNT			5

#define MAX_LOG_COUNT					128

#define MAX_APPID_LEN					32
#define MAX_SLOT_ID_COUNT				32


#define	CRYPTO_GCM_AAD_LENGTH			32
#define CRYPTO_GCM_IV_LENGTH			32



#define MAX_SESSION_NUM					65536
#define MAX_OBJECT_NUM					65535
#define MAX_FIND_OBJECT_NUM				2




/* management scheme of session list */
#define SESSION_LINKED_LIST				1
#define SESSION_ARRAY					2
#define SESSION_HASH_TABLE				3
#define	_USE_SESSION					SESSION_ARRAY

#define IND_SESSION_LOGIN


/* RNG mechanism */
#define	RNG_MECH_PLATFORM				1
#define RNG_MECH_CIPC					2
#define RNG_MECH_CIPD					3
#define RNG_MECH_CIPE					3
#define RNG_MECH						RNG_MECH_CIPC



/* RSA mode used for IKE */
#define IKE_R1_RSA_MODE					"if-modn"
#define	IKE_R1_RSA_CRYPT_MECH			"rsa-pkcs1v2-sha256-oaep"
#define IKE_R1_RSA_SIGN_MECH			"rsa-pkcs1-pss"
#define MECH_HASH_PIN					"sha256"


#define HAVE_LOCALTIME





/* SHOULD check definitions */
#undef	USE_FIXED_HANDLE			// check pkcs_object.c

/* 140923, NKHA - to emulate SRAM with a file */
#define	USE_FILE_INSTEAD_OF_SRAM
#define	SRAM_FILE_NAME				"/etc/sram.conf"
#define ROM_FILE_NAME				"/etc/rom.conf"

/* default debug message level */
#define SSH_DEFAULT_DEBUG_LEVEL		"*=3"

/* 141016, NKHA - define /proc/... name */
#define N3CRYPTO_PROC_DIR_NAME		"n3crypto"


/* host-side device driver name */
#define N3CRYPTO_HOST_DRIVER_NAME	"n3crypto_host_dd0"
#define N3CRYPTO_DEVICE_DRIVER_NAME	"n3crypto"


/* Set to directly call send/control APIs in host device driver */
#define USE_DIRECT_HOST_USB_API		0



/* Default attribute values of a created object - 141121, NKHA */
#define CK_DEFAULT_ATTR_TOKEN		0
#define CK_DEFAULT_ATTR_PRIVATE		1
#define CK_DEFAULT_ATTR_MODIFIABLE	1
#define CK_DEFAULT_ATTR_COPYABLE	1	// 150906, NKHA - add
#define CK_DEFAULT_ATTR_SENSITIVE	0	// default setting on sensitive




/* default IDs */
#define DEFAULT_MODEL_ID			"N3Crypto-NIP-16 "
#define DEFAULT_LABEL_ID			"NSR Crypt-Module"
#define DEFAULT_MODULE_ID			"S1-NIP1500000001"
#define DEFAULT_MANUFACTURER_ID		"NSR"


/* Cipher for RNG */
#define DEFAULT_CIPHER_RNG			"cipe-rng"

#endif /* _CONF_H_ */
