
enum block_type {
	BLOCK_NONE = 0,
	BLOCK_CLI,
	BLOCK_IPS,
	BLOCK_IDS,
	BLOCK_WIPS,
	BLOCK_DDOS,
	BLOCK_APP,
	BLOCK_AS,
	BLOCK_AV,
	BLOCK_RTM,
	BLOCK_DMC,
	BLOCK_PAN,
	BLOCK_TRACK_MAX
} ;

struct omni_block_track_info {
	unsigned long block_key;
	unsigned int pkt_len;
	enum block_type type;

	__be16 pkt_id;

	u32 timeout;
	u32 app_id;

	u_int32_t unique_id;
	u_int8_t pkt_dir;

	u8 proto;
};

struct omni_track_l4block
{
	enum block_type type;
	void (*init)(struct omni_track *track,
			struct omni_block_track_info *info);
	void (*acct)(struct omni_track *track,
			enum omni_track_info trinfo,
			const struct sk_buff *skb);
	void (*close)(struct omni_track *track);
};

extern struct omni_track_l4block *omni_track_block[BLOCK_TRACK_MAX];
extern struct omni_track_l4block omni_track_block_pan;
extern struct omni_track_l4block omni_track_block_ips;
extern struct omni_track_l4block omni_track_block_ddos;
extern struct omni_track_l4block omni_track_block_app;
extern struct omni_track_l4block omni_track_block_ids;
extern struct omni_track_l4block omni_track_block_as;
extern struct omni_track_l4block omni_track_block_av;

extern struct omni_track_l4block *omni_track_block_find(enum block_type type);

extern void omni_block_track_info_make(
		struct omni_block_track_info *info,
		u32 timeout, enum block_type type, unsigned int pkt_len,
		u_int8_t pkt_dir, u32 app_id, __be16 pkt_id,
		unsigned long block_key, u_int32_t unique_id);

extern int omni_init_block_track(struct omni_track_tuple *tuple, struct omni_block_track_info *info);
extern int omni_unblock_track(struct omni_track_tuple *t);

extern void omni_track_block_acc(struct omni_track *track, bool type);
extern int omni_block_track_count(void);

extern unsigned int g_omni_block_track_use __read_mostly;

#define BLOCK_DEC 0
#define BLOCK_INC 1
