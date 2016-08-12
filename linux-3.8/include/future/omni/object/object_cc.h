#ifndef _OBJ_COUNTRY_H_
#define _OBJ_COUNTRY_H_

#define CC_BIN_PATH "/var/geoip/geoip.bin"

extern int omni_object_cc_init(void);
extern om_cc_data *cc_bin;
extern size_t cc_entry_count;
extern int cc_entry_memory_usage;

#endif  /* _OBJ_COUNTRY_H_ */
