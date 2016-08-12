#ifndef	_PKCS11_UT_H_
#define _PKCS11_UT_H_

#include "pkcs11.h"

#define PKCS11_THREAD_MEM_NUM	16
#define PKCS11_THREAD_NUM		8
#define PKCS11_BUF_PINLEN		256
#define PKCS11_CRYPT_BLK_LEN	32
#define PKCS11_HASH_MAX_LEN		64
#define PKCS11_IV_MAX_LEN		32

// 150915, NKHA - modify max size of APUD
#ifdef SMARTPHONE_PKCS11
#define PKCS11_CRYPT_BLK_NUM	63
#define PKCS11_APDU_MSG_LEN		(PKCS11_CRYPT_BLK_NUM * PKCS11_CRYPT_BLK_LEN)
#else
// should be same as MAX_BLOCK_DATA_LEN @ commandblock.h
#define PKCS11_APDU_MSG_LEN		(2048-128)
#endif

// define for Object Template
#define PKCS11_OBJ_ID_LEN		32
#define PKCS11_OBJ_LABEL_LEN	32
#define PKCS11_OBJ_VALUE		64

// define for crypto_type
#define PKCS11_CRYPT_BLK_ENC		1
#define PKCS11_CRYPT_BLK_DEC		2
#define PKCS11_CRYPT_BLK_ENC_GCM	3
#define PKCS11_CRYPT_BLK_DEC_GCM	4
#define PKCS11_CRYPT_PKT_ENC		5
#define PKCS11_CRYPT_PKT_DEC		6
#define PKCS11_CRYPT_PKT_ENC_GCM	7
#define PKCS11_CRYPT_PKT_DEC_GCM	8
#define PKCS11_CRYPT_PKT_ENC_SIGN	9
#define PKCS11_CRYPT_PKT_DEC_VERIFY	10
#define PKCS11_CRYPT_DIGEST			11
#define PKCS11_CRYPT_VERIFY			12
#define PKCS11_CRYPT_SIGN			13

#define UNUSED_SESSION_HANDLE	(-1)
#define UNUSED_OBJECT_HANDLE	(-1)
#define UNUSED_SLOTID           (-1)

#define SCRIPT_STRLINE_LEN		1024

#define DIM(x)							sizeof(x)/sizeof(x[0])
#define PKCS11_CHK_RANGE(x,rng,msg);	if (x>rng) {LOGE("%s", msg); return (-1);}
#define PKCS11_CHK_RV(x,msg);			if (x!=CKR_OK) {LOGE("rv=0x%x: %s", x,msg); return rv;}
#define PKCS11_CHK_RV_S(x,msg,s);		if (x!=CKR_OK) {LOGE("rv=0x%x: %s(hSes=%d)", x,#msg,s); return rv;}
#define PKCS11_CHK_RV_SO(x,msg,s,o);	if (x!=CKR_OK) {LOGE("rv=0x%x: %s(hSes=%d, hObj=0x%x)", x,#msg,s,o); return rv;}

// object template for secret, private, or public key, data, and certificate
typedef struct {
	CK_OBJECT_CLASS		class_;
	CK_BBOOL			bToken;
	CK_ULONG			keyType;
	CK_BBOOL			bEncrypt;
	CK_BBOOL			bDecrypt;
	CK_BYTE				id [PKCS11_OBJ_ID_LEN];
	CK_BYTE				value [PKCS11_OBJ_VALUE];
	CK_BYTE				label[PKCS11_OBJ_LABEL_LEN];
} ObjectTemplate_t;


typedef enum {
	PKCS11_CMD_general = 1,
	PKCS11_CMD_session,
	PKCS11_CMD_object,
	PKCS11_CMD_crypt,
	PKCS11_CMD_key,
	PKCS11_CMD_extra,
	PKCS11_CMD_n3crypto,
} PKCS11_CMD_TYPE_t;

extern int noarg_forhelp;

// PKCS11 APIs - vendor-specific funcs
#ifdef SMARTPHONE_PKCS11
int run_C_SendApduMessage (void);
int run_C_EraseKeys (void);
int run_C_NSRI_VerifyValue (void);
int run_C_NSRI_CryptMsg (void);
int run_C_SetTokenLost (void);
int run_C_GetTokenLost (void);
int run_C_NANDRead (void);
int run_C_NANDWrite (void);
#endif /* endof #ifdef SMARTPHONE_PKCS11 */

int run_C_DeriveKey(void);
int run_C_WrapKey(void);
int run_C_UnwrapKey(void);

int run_C_PacketDecryptInit (void);
int run_C_PacketDecryptUpdate (void);
int run_C_PacketDecryptUpdateGCM (void);
int run_C_PacketDecryptFinal (void);

int run_C_PacketDecryptVerifyInit (void);
int run_C_PacketDecryptVerifyUpdate (void);
int run_C_PacketDecryptVerifyFinal (void);

int run_C_PacketEncryptInit (void);
int run_C_PacketEncryptUpdate (void);
int run_C_PacketEncryptUpdateGCM (void);
int run_C_PacketEncryptFinal (void);

int run_C_PacketSignEncryptInit (void);
int run_C_PacketSignEncryptUpdate (void);
int run_C_PacketSignEncryptFinal (void);


int run_C_NSRI_GetLibVersion (void);
int run_C_SetDebugLevel (void);

// PKCS11 APIs with session, login, and object
int run_C_Decrypt (void);
int run_C_DecryptInit (void);
int run_C_DecryptUpdate (void);
int run_C_DecryptUpdateGCM (void);
int run_C_DecryptFinal (void);

int run_C_Encrypt (void);
int run_C_EncryptInit (void);
int run_C_EncryptUpdate (void);
int run_C_EncryptUpdateGCM (void);
int run_C_EncryptFinal (void);

int run_C_Digest (void);
int run_C_DigestInit (void);
int run_C_DigestUpdate (void);
int run_C_DigestFinal (void);

int run_C_Sign (void);
int run_C_SignInit (void);
int run_C_SignUpdate (void);
int run_C_SignFinal (void);

int run_C_Verify (void);
int run_C_VerifyInit (void);
int run_C_VerifyUpdate (void);
int run_C_VerifyFinal (void);

int run_C_FindObjectsInit (void);
int run_C_FindObjects (void);
int run_C_FindObjectsFinal (void);

int run_C_GetObjectSize (void);
int run_C_SetAttributeValue (void);
int run_C_GetAttributeValue (void);
int run_C_GetAttributeValueNull (void);		// 150825, NKHA - get ulValueLen
int run_C_DestroyObject (void);
int run_C_CreateObject (void);
int run_C_CopyObject (void);				// 150910, NKHA

// PKCS11 APIs with session and login
int run_C_SeedRandom (void);
int run_C_GenerateRandom (void);
int run_C_SetPIN (void);
int run_C_InitPIN (void);
int run_C_Login (void);
int run_C_Logout (void);

// PKCS11 APIs with session
int run_C_GetSessionInfo (void);
int run_C_CloseAllSessions (void);
int run_C_CloseSession (void);


// PKCS11 APIs without session
int run_C_InitToken (void);
int run_C_Initialize (void);
int run_C_Finalize (void);
int run_C_GetInfo (void);
int run_C_GetFunctionList (void);
int run_C_GetSlotList(void);
int run_C_GetSlotInfo();
int run_C_GetTokenInfo(void);
int run_C_OpenSession (void);

// Extra functions
int do_CmdHelp(void);
int do_CmdExit(void);
int do_Pause(void);
int do_Echo(void);
int	do_Compare (void);
int	do_Sleep (void);
int	do_FindObjectById (void);
int do_TestAPI (void);
int do_ThreadEx (void);
int do_ThreadInit (void);
int do_ThreadWait (void);
int do_ShowTokenState (void);

int create_thread_context (FILE *fp);
int destroy_thread_context (void);

int do_HelpMsgShow (char *msg);

int run_TestAPI(void);
int run_C_NSRI_InitParams(void);
int run_C_SetSerialID (void);

int do_SetDebugOption (void);
int do_ResetSDC(void);
int do_GetSerialID (void);
int do_SetSerialID (void);
int do_ReadFlash (void);		// 20130717 NKHA, to save memory data in IC chip into a file
int do_SDIO(void);				// 20130805 NKHA, to check SMART_IO.CRD IO
int do_SDSCGetSCIOType(void);	// 20130807 NKHA, to check SDSCGetSCIOType
int do_SetLibPath(void);		// 20131018 NKHA, to set dll path

int run_C_NSRI_TryConnectDev(void);	// 20131104 NKHA, to try connecting secure SD token


int find_Object (CK_BYTE_PTR keyId, CK_SESSION_HANDLE hSes, CK_OBJECT_HANDLE_PTR phObj);
CK_RV initToken (CK_SLOT_ID *slotID, CK_ULONG *pulSlotCount);
CK_RV accessToken (CK_SLOT_ID slotID, CK_BYTE_PTR PIN, CK_ULONG	pinLen, CK_SESSION_HANDLE *phSes);
CK_RV releaseToken (CK_SESSION_HANDLE hSes);


void show_attribute (CK_ATTRIBUTE_PTR attr, CK_ULONG ulCount);
void set_session_handle (int idx, CK_SESSION_HANDLE value);
CK_SESSION_HANDLE get_session_handle (int idx);


#endif /* _PKCS11_UT_H_ */
