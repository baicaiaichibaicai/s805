/*
 ** $Id: mwm.h 3850 2007-10-04 00:04:17Z ikhwang $
 **
 **  mwm.h
 **
 ** Copyright (C) 2002 Sourcefire,Inc
 ** Marc Norton
 **
 ** Modifed Wu-Manber style Multi-Pattern Matcher
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **
 **
 */  

#ifndef __MWM_H__
#define __MWM_H__

#ifndef UINT64
# define UINT64 unsigned long long
#endif

/*
 ** Enables display of pattern group stats
 */ 
//#define SHOW_STATS
#define MWM_CALLOC(a,b) t_vzalloc((a)*(b))
#define MWM_MALLOC(a) kmalloc((a), GFP_ATOMIC)
#define MWM_FREE kfree

#define is_lower_alpha(C) ((C)>='a' && (C)<='z')

#define HASHTABLESIZE (1024*1024)
#define HASHBYTES16    2

void *t_vzalloc(size_t a);

void *t_vmalloc(size_t a);

void t_vfree(void* p);

/* 
 ** Causes mbmAddPattern to check for and not allow duplicate patterns. 
 ** By default we allow multiple duplicate patterns, since the AND clause
 ** may case the whole signature to be different. We trigger each pattern
 ** to be processesed by default.
 */ 
/*
#define REQUIRE_UNIQUE_PATTERNS
 */ 

/*
 *
 *  Boyer-Moore-Horsepool for small pattern groups
 *    
 */ 
typedef struct {
    unsigned char *P;
    int M;
    int bcShift[256];
} HBM_STRUCT;

/*
 **  This struct is used internally my mwm.c
 */ 
typedef struct _mwm_pattern_struct {
    struct _mwm_pattern_struct *next;
    unsigned char *psPat;	// pattern array, no case
    unsigned char *psPatCase;	// pattern array, case(exact)
    unsigned psLen;		// length of pattern in bytes
    int psID;
    unsigned psNoCase;		// Pattern match is case insensitive if true
    HBM_STRUCT * psBmh;
} MWM_PATTERN_STRUCT;

/*
 *  Pattern Matching Methods - Boyer-Moore-Horspool or Modified Wu Manber
 */ 
#define MTH_MWM 0
#define MTH_BM  1

#define HASH_TYPE int     

#define BWSHIFTABLESIZE (1024*1024)

/*
 ** Pattern GROUP Structure, this struct is is used publicly, but by reference only
 */ 
typedef struct _mwm_struct {
    int msMethod;		/* MTH_BM, MTH_MWM */
    MWM_PATTERN_STRUCT * plist;

    /*  Array of Patterns */ 
    int msMaxPatterns;
    MWM_PATTERN_STRUCT * msPatArray;

    /* Array of Group Counts, # of patterns in each hash group */ 
    unsigned int *msNumArray;

    /* One byte patterns */ 
    unsigned int msNumArray1[256];

    /* Number of Patterns loaded */ 
    int msNumPatterns;

    /* Wu-Manber Hash Tables */ 
    unsigned msNumHashEntries;
    HASH_TYPE * msHash;	// 2+ character Pattern Big Hash Table
    HASH_TYPE msHash1[256];	// One character Pattern Hash Table

    /* Bad Character Shift Table */ 
    int msShift[256];
    unsigned msShiftLen;

    /* Bad Word Shift Table */ 
    unsigned char *msShift2;
    int msLargeShifts;

    /* Case insensitive search */ 
    int msNoCase;

    /* search function */ 
    int (*search) (struct _mwm_struct * ps, unsigned char *Tx, int n, unsigned char *Tc,
	    int (*match) (int id, int index, void *data), void *data);

    /* Print Group Details */ 
    int msDetails;

    /* Pattern Group Stats  */ 
    int msSmallest;
    int msLargest;
    int msAvg;
    int msTotal;
    int *msLengths;
} MWM_STRUCT;

/*
 ** PROTOTYPES
 */ 
void *mwmNew(void);
void mwmFree(void *pv);
int mwmAddPatternEx(void *pv, unsigned char *P, int m, unsigned noCase, int ID);
void mwmLargeShifts(void *pv, int flag);
int mwmPrepPatterns(void *pv);

int mwmSearch(void *pv, unsigned char *T, int n, int (*action) (int id, int index, void *data),
	void *data);
UINT64 mwmGetPatByteCount(void);
void mwmResetByteCount(void);

/* Not so useful, but not ready to be dumped  */ 
int mwmAddPattern(void *pv, unsigned char *P, int m, unsigned id);
int mwmGetNumPatterns(void *pv);

#endif	/*  */

