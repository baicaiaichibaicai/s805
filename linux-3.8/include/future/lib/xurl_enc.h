/***************************************************************************
* Copyright (c) 1990-2000, Future Systems, Inc. / Seoul, Republic of Korea *
* All Rights Reserved.                                                     *
*                                                                          *
* This document contains proprietary and confidential information. No      *
* parts of this document or the computer program it embodies may be in     *
* any way copied, duplicated, reproduced, translated into a different      *
* programming language, or distributed to any person, company, or          *
* corporation without the prior written consent of Future Systems, Inc.    *
***************************************************************************/

/*---------------------- [Comment in details]-------------------------------
Description: (xurl_enc.h)
Version: 1.0

C0000 - Create by drkong@hanmir.com                               2002/5/20
--------------------------------------------------------------------------*/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ENCRYPT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ENCRYPT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32
#ifdef ENCRYPT_EXPORTS
#define ENCRYPT_API __declspec(dllexport)
#else
#define ENCRYPT_API __declspec(dllimport)
#endif
#else //UNIX,LINUX
#define	ENCRYPT_API extern
#endif

#include "xurl_md5.h"

/*
 * Function for encrypting message.
 */
ENCRYPT_API void GetEncryptedMessage(const unsigned char* mesg, /* string to encryption */
				 int length,			/* length of the string */
				 unsigned char *result);	/* digest to store */


ENCRYPT_API void Init(void);
ENCRYPT_API void Append(const md5_state_t* data, int len);
ENCRYPT_API void Finish(md5_state_t data[]);

