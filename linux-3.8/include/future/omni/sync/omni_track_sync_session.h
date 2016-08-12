#ifndef __OMNI_TRACK_SYNC_SESSION_H__
#define __OMNI_TRACK_SYNC_SESSION_H__

#include <future/omni/omni_track_cache.h>
#include <future/omni/omni_track_expect.h>

int omni_track_sync_sender(struct omni_track *track);
int omni_track_sync_decider(struct omni_track *track);
int sync_session(void *data);
int omni_track_sync_info_proc_read(char *page, char **start, off_t off, int count, int *eof,void *data);
int omni_track_sync_info_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
int omni_track_sync_sess_proc_read(char *page, char **start,off_t off, int count, int *eof,void *data);
int omni_track_sync_sess_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);

void omni_track_sync_init(void);
void omni_track_sync_request(void);

int is_sync_sess_runnig(void);

#define OMNI_TRACK_SYNC_PORT 11100
 
struct omni_sync_common_hdr {
	__u8    next:4,
            status:4;
    __u8    type;     
};

struct omni_sync_new_data {
	struct omni_track_tuple_hash tuplehash[OMNI_TRACK_DIR_MAX];
	__be64 status;
	__be64 tr_timeout;
	__be32 is_ipsec;
	struct omni_cache_l1 cache_l1;
	union omni_track_proto proto;
	struct omni_nat nat;
};

struct omni_sync_delete_data {
	struct omni_track_tuple_hash tuplehash[OMNI_TRACK_DIR_MAX];
};

struct omni_sync_update_data {
	struct omni_track_tuple_hash tuplehash[OMNI_TRACK_DIR_MAX];
    __be64 status;
	__be64 tr_timeout;
	__be32 is_ipsec;
	struct omni_cache_l1 cache_l1;
	union omni_track_proto proto;
	struct omni_nat nat;
};

struct omni_sync_helper_hdr {
	__u8    next:4,
            status:4;
    __u8    type;     
};

struct omni_sync_helper_data {
	__be32    status;
    __be32    tuples;
    __be32    policy;
    __be32    timeout;
    __be32    map;
};

struct omni_sync_expect_hdr {
	__u8    next:4,
            status:4;
    __u8    type;     
};

struct omni_sync_expect_data {
	struct omni_track_expect exp;
};

struct sync_sess_conf {
    int use;
    int mode;
};

struct omni_track_sync_count {
    unsigned int new;
    unsigned int update;
    unsigned int delete;
};

enum sync_type {
    SYNC_UPDATE,
    SYNC_DELETE,
    SYNC_NEW,
    SYNC_ALL,
};

enum sess_type {
    SESS_PARENT,
    SESS_CHILD,
};

#endif
