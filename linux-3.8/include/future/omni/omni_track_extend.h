#ifndef _OMNI_TRACK_EXTEND_H
#define _OMNI_TRACK_EXTEND_H

#include <linux/slab.h>
#include <future/omni_track.h>

enum omni_track_ext_id {
	OMNI_TRACK_EXT_HELPER,
#if IS_ENABLED(CONFIG_OMNI_NAT)
	OMNI_TRACK_EXT_NAT,
#endif/*CONFIG_OMNI_NAT*/
#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_APP_EXT)
	OMNI_TRACK_EXT_APP,
#endif/*CONFIG_FERRET_FW_PROFILES_APP_EXT*/
#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_WEB)
	OMNI_TRACK_EXT_WEB,
#endif/*CONFIG_FERRET_FW_PROFILE_WEB*/
	OMNI_TRACK_EXT_NUM,
};

#define OMNI_TRACK_EXT_HELPER_TYPE struct omni_track_help
#define OMNI_TRACK_EXT_NAT_TYPE struct omni_track_nat
#define OMNI_TRACK_EXT_APP_TYPE struct omni_ext_app_profile
#define OMNI_TRACK_EXT_WEB_TYPE struct omni_ext_web_profile


/* Extensions: optional stuff which isn't permanently in struct. */
struct omni_track_ext {
	struct rcu_head rcu;
	u8 offset[OMNI_TRACK_EXT_NUM];
#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_WEB)
	u16 len;
#else
	u8 len;
#endif/*CONFIG_FERRET_FW_PROFILES_WEB*/
	char data[0];
};

static inline bool __omni_track_ext_exist(const struct omni_track_ext *omni_ext, u8 id)
{
	return !!omni_ext->offset[id];
}

static inline bool omni_track_ext_exist(const struct omni_track *track, u8 id)
{
	return (track->omni_ext && __omni_track_ext_exist(track->omni_ext, id));
}

static inline void *__omni_track_ext_find(const struct omni_track *track, u8 id)
{
	if (!omni_track_ext_exist(track, id))
		return NULL;

	return (void *)track->omni_ext + track->omni_ext->offset[id];
}
#define omni_track_ext_find(omni_ext, id)	\
	((id##_TYPE *)__omni_track_ext_find((omni_ext), (id)))

/* Destroy all relationships */
extern void __omni_track_ext_destroy(struct omni_track *track);
static inline void omni_track_ext_destroy(struct omni_track *track)
{
	if (track->omni_ext)
		__omni_track_ext_destroy(track);
}

/* Free operation. If you want to free a object referred from private area,
 * please implement __nf_ct_ext_free() and call it.
 */
static inline void omni_track_ext_free(struct omni_track *track)
{
	if (track->omni_ext)
		kfree(track->omni_ext);
}

/* Add this type, returns pointer to data or NULL. */
void *__omni_track_ext_add_length(struct omni_track *track, enum omni_track_ext_id id,
			     size_t var_alloc_len, gfp_t gfp);

#define omni_track_ext_add(track, id, gfp) \
	((id##_TYPE *)__omni_track_ext_add_length((track), (id), 0, (gfp)))
#define omni_track_ext_add_length(track, id, len, gfp) \
	((id##_TYPE *)__omni_track_ext_add_length((track), (id), (len), (gfp)))

#define OMNI_TRACK_EXT_F_PREALLOC	0x0001

struct omni_track_ext_type {
	/* Destroys relationships (can be NULL). */
	void (*ext_destroy)(struct omni_track *track);
	/* Called when realloacted (can be NULL).
	   Contents has already been moved. */
	void (*move)(void *new, void *old);

	enum omni_track_ext_id id;

	unsigned int flags;

#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_WEB)
	/* Length and min alignment. */
	u16 len;
	u16 align;
	/* initial size of omni_track_ext. */
	u16 alloc_size;
#else
	/* Length and min alignment. */
	u8 len;
	u8 align;
	/* initial size of omni_track_ext. */
	u8 alloc_size;
#endif/*CONFIG_FERRET_FW_PROFILES_WEB*/
};

int omni_track_extend_register(struct omni_track_ext_type *type);
void omni_track_extend_unregister(struct omni_track_ext_type *type);
#endif /*_OMNI_TRACK_EXTEND_H*/
