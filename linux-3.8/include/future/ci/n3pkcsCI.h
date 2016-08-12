/* vi:set ts=4 sw=4: */

#ifndef _N3_PKCS_CI_H_
#define _N3_PKCS_CI_H_

#define CMD_CLASS_MASK						0xf0

#define CMD_CLASS_FAST_CRYPTO_SINGLE		0xf0
#define CMD_CLASS_FAST_CRYPTO_UPDATE		0xe0
#define CMD_CLASS_SLOW_INIT					0xd0
#define CMD_CLASS_SLOW_FINAL 				0xc0
#define CMD_CLASS_KEYMGMT					0xb0
#define CMD_CLASS_OBJMGMT 					0xa0
#define CMD_CLASS_SESMGMT					0x90
#define CMD_CLASS_TOKMGMT					0x80
#define CMD_CLASS_GENERAL					0x70
#define CMD_CLASS_LOG						0x60
#define CMD_CLASS_DIAG						0x50
#define CMD_CLASS_CONFIG					0x40
#define CMD_CLASS_INJECTION					0x30
#define CMD_CLASS_USERDEFINED				0x00


/* CI command codes */
#define CI_SignDirect						0x10
#define CI_VerifyDirect						0x11
#define CI_SignEncryptUpdateBig				0x12
#define CI_DecryptVerifyUpdateBig			0x13
#define CI_RSA_publicOp						0x14
#define CI_RSA_privateOp					0x15
#define CI_RSA_SetC							0x16
#define CI_SetFeature						0x17
#define CI_Reboot							0x18
#define CI_GetMechanismList					0x19
#define CI_GetMechanismInfo					0x1a
#define CI_GetSlotInfo						0x1d
#define CI_GetBufferSize					0x1e
#define CI_GetSlotList						0x1f

#define CI_AuthenFWTerminal					0x20
#define CI_FWProgInjection					0x21
#define CI_ForceZeroization					0x22
#define CI_BRAMZeroization					0x23			// erase SRAM
#define CI_PISO_Login						0x24
#define CI_CSPInjection						0x25			// Algorithm
/* 0x26~0x2f: reserved */

#define CI_DebugInfo						0x30
#define CI_NAND_Read						0x31
#define CI_NAND_Write						0x32
#define CI_UFI								0x33
/* 0x33:0x3f: reserved */

#define CI_GetTimer							0x40
#define CI_SetTimer							0x41			// Set RTC
#define CI_EmergencyClear					0x42
#define CI_MemRead							0x43
#define CI_MemWrite							0x44
#define CI_IORead							0x45
#define CI_IOWrite							0x46
#define CI_PI								0x47
#define CI_Load								0x48
#define CI_Ver								0x49
#define CI_SetSerialID						0x4a
/* 0x4b:0x4f: reserved */

#define CI_DiagModule						0x50
#define CI_Loop								0x51
/* 0x52:0x5f: reserved */

#define CI_LogReadInit						0x60
#define CI_LogRead							0x61
#define CI_LogWrite							0x62
#define CI_InitUserPIN						0x63
#define CI_DeleteUserPIN					0x64
#define CI_GetPINStatus						0x65
/* 0x66:0x6f: reserved */

#define CI_Initialize						0x70
#define CI_Finalize							0x71
#define CI_GetInfo							0x72
/* 0x73:0x7f: reserved */

#define CI_GetTokenInfo						0x80
#define CI_InitToken						0x81
#define CI_InitPIN							0x82
#define CI_SetPIN							0x83
/* 0x84:0x8f: reserved */

#define CI_OpenSession						0x90
#define CI_CloseSession						0x91
#define CI_CloseAllSessions					0x92
#define CI_GetSessionInfo					0x93
#define CI_GetOperationState				0x94
#define CI_SetOperationState				0x95
#define CI_Login							0x96
#define CI_Logout							0x97
/* 0x98:0x9f: reserved */

#define CI_CreateObject						0xa0
#define CI_CopyObject						0xa1
#define CI_DestroyObject					0xa2
#define CI_GetObjectSize					0xa3
#define CI_GetAttributeValue				0xa4
#define CI_SetAttributeValue				0xa5
#define CI_FindObjectsInit					0xa6
#define CI_FindObjects						0xa7
#define CI_FindObjectsFinal					0xa8
/* 0xa9:0xaf: reserved */

#define CI_SeedRandom						0xb0
#define CI_GenerateRandom					0xb1
#define CI_GenerateKey						0xb2
#define CI_GenerateKeyPair					0xb3
#define CI_WrapKey							0xb4
#define CI_UnwrapKey						0xb5
#define CI_DeriveKey						0xb6
/* 0xb7:0xbf: reserved */

#define CI_EncryptInit						0xc0
#define CI_DecryptInit						0xc1
#define CI_DigestInit						0xc2
#define CI_SignInit							0xc3
#define CI_VerifyInit						0xc4
#define CI_SignEncryptInit					0xc5
#define CI_DecryptVerifyInit				0xc6
#define CI_PacketEncryptInit				0xc7
#define CI_PacketDecryptInit				0xc8
#define CI_PacketSignEncryptInit			0xc9
#define CI_PacketDecryptVerifyInit			0xca
/* 0xcb:0xcf: reserved */

#define CI_EncryptUpdate					0xd0
#define CI_DecryptUpdate					0xd1
#define CI_EncryptUpdateGCM					0xd2
#define CI_DecryptUpdateGCM					0xd3
/* d4 reserved */
#define CI_SignEncryptUpdate				0xd5
#define CI_DecryptVerifyUpdate				0xd6
#define CI_PacketEncryptUpdate				0xd7
#define CI_PacketDecryptUpdate				0xd8
#define CI_PacketSignEncryptUpdate			0xd9
#define CI_PacketDecryptVerifyUpdate		0xda
#define CI_PacketEncryptUpdateGCM			0xdb
#define CI_PacketDecryptUpdateGCM			0xdc
#define CI_DigestUpdate						0xdd
#define CI_SignUpdate						0xde
#define CI_VerifyUpdate						0xdf


#define CI_EncryptFinal						0xe0
#define CI_DecryptFinal						0xe1
#define CI_DigestFinal						0xe2
#define CI_SignFinal						0xe3
#define CI_VerifyFinal						0xe4
#define CI_SignEncryptFinal					0xe5
#define CI_DecryptVerifyFinal				0xe6
#define CI_PacketEncryptFinal				0xe7
#define CI_PacketDecryptFinal				0xe8
#define CI_PacketSignEncryptFinal			0xe9
#define CI_PacketDecryptVerifyFinal			0xea
/* 0xeb:0xef: reserved */
#define CI_HighPacketSignEncryptUpdate		0xec
#define	CI_HighPacketDecryptVerifyUpdate 	0xed

#define CI_Encrypt							0xf0
#define CI_Decrypt							0xf1
#define CI_Digest							0xf2
#define CI_Sign								0xf3
#define CI_Verify							0xf4
#define CI_SignEncrypt						0xf5
#define CI_DecryptVerify					0xf6
#define CI_PacketEncrypt					0xf7
#define CI_PacketDecrypt					0xf8
#define CI_PacketSignEncrypt				0xf9
#define CI_PacketDecryptVerify				0xfa
/* 0xfb:0xff: reserved */



#endif
