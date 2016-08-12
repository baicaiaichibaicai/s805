/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_MATCH_PICS_H
#define _PROFILE_WEB_MATCH_PICS_H


enum {
	PICS_META,
	PICS_LABLE,
	PICS_ICEC,
	PICS_SAFENET,
	PICS_NEWICRA,	/* 모든 방식을 safenet 표현으로 mapping */
	PICS_ICRA,		/* RSACi방식이라고 함, SAFENET 과 비슷 */
	PICS_SAFESURF,
	PICS_TYPE_MAX
};

typedef struct _PatternMatchData {
    char pat[128];
    int plen;
    int type;
} PatternMatchData;


int url_rate_filter(unsigned char *, unsigned char *, unsigned int, int);

#endif/*_PROFILE_WEB_MATCH_PICS_H*/

