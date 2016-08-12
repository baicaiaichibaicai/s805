/*
 * Future Systems
 */
#ifndef _PROFILE_PROFILES_H
#define _PROFILE_PROFILES_H


#define PROFILE_BASE_CTL 100

typedef struct {
	unsigned char cmd;
	int profile_cnt;
	void **profile;
} profile_root;

#endif/*_PROFILE_PROFILES_H*/

