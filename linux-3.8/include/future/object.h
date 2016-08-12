/***************************************************************************
* Copyright (c) 1990-2000, Future Systems, Inc. / Seoul, Republic of Korea *
* All Rights Reserved.                                                     *
*                                                                          *
* This document contains proprietary and confidential information. No      *
* parts of this document or the computer program it embodies may be in	   *
* any way copied, duplicated, reproduced, translated into a different      *
* programming language, or distributed to any person, company, or          *
* corporation without the prior written consent of Future Systems, Inc.    *
***************************************************************************/

/*---------------------- [Comment in details]-------------------------------
Description: Gate2 Object 정의(object.h)
Version: 1.0

****************************************************************************
                                 주 의
  이 파일에서 Object 부분이 변경되면 admin/object.c 파일의 parse_???_obj()
  Function들을 변경해야 함.
****************************************************************************

--------------------------------------------------------------------------*/
#ifndef _FUTURE_OBJECT_H
#define _FUTURE_OBJECT_H

#include <asm/byteorder.h>
#include <linux/types.h>
#include <linux/mc146818rtc.h>

typedef struct
{
    u_char      center_info[8]		;	//          UUID
    u_short     type			;  	//     GID  UUID
    u_short     sub_type		;  	//     GID  UUID
    u_int	id			;  	// ID  GID  UUID
} obj_id_struct;

typedef struct {
    u_short		version		;
    u_short		length		;	// key[] length
    char		id[8]		;	// Key ID
    char		key[32]		;	// Key Material, Real Key
    char		auth[4]		;
} __attribute__((packed)) wrapped_key;

typedef struct {
    wrapped_key		rng_key		;	// random number 생성 관련
    char		initial_seed[16];	// random number 생성 관련
    u_short		mac_algo	;	// wrapped_key auth algo
    u_short		enc_algo	;	// wrapped_key derivation algo
    u_int		not_before	;	// 유효개시일. UTC
    u_int		not_after	;	// 유효만료일. UTC
    char		id[16]		;	// MasterKey ID
    wrapped_key		key		;	// MasterKey Material
} __attribute__((packed)) mk_token;

#endif/*_FUTURE_OBJECT_H*/
