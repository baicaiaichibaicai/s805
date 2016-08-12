#ifndef _BLOCK_WIPS_H_
#define _BLOCK_WIPS_H_

#include <linux/types.h>

#define BLOCK_WIPS_SET_ID 5679

struct block_wips_param {
    uint32_t src;
    uint32_t dst;
    uint16_t sport;
    uint16_t dport;
    uint8_t proto;
    uint32_t timeout;
};

int set_block_wips(struct block_wips_param *param);

#endif
