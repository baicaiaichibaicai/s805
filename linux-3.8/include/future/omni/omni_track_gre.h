#ifndef OMNI_TRACK_PROTO_GRE_H
#define OMNI_TRACK_PROTO_GRE_H

#include <asm/byteorder.h>
/* GRE PROTOCOL HEADER */

/* GRE Version field */
#define GRE_VERSION_1701	0x0
#define GRE_VERSION_PPTP	0x1

/* GRE Protocol field */
#define GRE_PROTOCOL_PPTP	0x880B

/* GRE Flags */
#define GRE_FLAG_C		0x80
#define GRE_FLAG_R		0x40
#define GRE_FLAG_K		0x20
#define GRE_FLAG_S		0x10
#define GRE_FLAG_A		0x80

#define GRE_IS_C(f)	((f)&GRE_FLAG_C)
#define GRE_IS_R(f)	((f)&GRE_FLAG_R)
#define GRE_IS_K(f)	((f)&GRE_FLAG_K)
#define GRE_IS_S(f)	((f)&GRE_FLAG_S)
#define GRE_IS_A(f)	((f)&GRE_FLAG_A)
/* GRE is a mess: Four different standards */

struct omni_gre_hdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u16	rec:3,
		srr:1,
		seq:1,
		key:1,
		routing:1,
		csum:1,
		version:3,
		reserved:4,
		ack:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u16	csum:1,
		routing:1,
		key:1,
		seq:1,
		srr:1,
		rec:3,
		ack:1,
		reserved:4,
		version:3;
#else
#error "Adjust your <asm/byteorder.h> defines"
#endif
	__be16	protocol;
};

struct omni_track_gre {
	unsigned int stream_timeout;
	unsigned int timeout;
};

/* modified GRE header for PPTP */
struct omni_gre_hdr_pptp {
	__u8   flags;		/* bitfield */
	__u8   version;		/* should be GRE_VERSION_PPTP */
	__be16 protocol;	/* should be GRE_PROTOCOL_PPTP */
	__be16 payload_len;	/* size of ppp payload, not inc. gre header */
	__be16 call_id;		/* peer's call_id for this session */
	__be32 seq;		/* sequence number.  Present if S==1 */
	__be32 ack;		/* seq number of highest packet recieved by */
				/*  sender in this session */
};

#ifdef __KERNEL__
#include <future/omni/omni_track_tuple_common.h>
#include <future/omni/omni_track_tuple.h>
#include <future/omni_track.h>
struct omni_track;

/* structure for original <-> reply keymap */
struct omni_track_gre_keymap {
	struct list_head list;
	struct omni_track_tuple tuple;
};

/* add new tuple->key_reply pair to keymap */
extern int omni_track_gre_keymap_add(struct omni_track *track, enum omni_track_dir dir,
			 struct omni_track_tuple *t);

/* delete keymap entries */
extern void omni_track_gre_keymap_destroy(struct omni_track *track);

/* get pointer to gre key, if present */
static inline __be32 *gre_key(struct omni_gre_hdr *greh)
{
	if (!greh->key)
		return NULL;
	if (greh->csum || greh->routing)
		return (__be32 *)(greh+sizeof(*greh)+4);
	return (__be32 *)(greh+sizeof(*greh));
}

/* get pointer ot gre csum, if present */
static inline __sum16 *gre_csum(struct omni_gre_hdr *greh)
{
	if (!greh->csum)
		return NULL;
	return (__sum16 *)(greh+sizeof(*greh));
}

extern void omni_track_gre_keymap_flush(void);
extern void omni_track_need_gre(void);

#endif /* __KERNEL__ */
#endif /* OMNI_TRACK_PROTO_GRE_H */
