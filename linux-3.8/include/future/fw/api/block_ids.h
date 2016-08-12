#ifndef _BLOCK_IDS_H_
#define _BLOCK_IDS_H_

#include <linux/types.h>

#define BLOCK_IDS_SET_ID 5678

struct block_ids_param {
    uint32_t src;
    uint32_t dst;
    uint16_t sport;
    uint16_t dport;
    uint8_t proto;
    uint32_t timeout;
};

int set_block_ids(struct block_ids_param *param);

#endif
