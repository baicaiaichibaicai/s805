#ifndef _LP_LIST_TYPES
#define _LP_LIST_TYPES

#include <linux/llist.h>

typedef struct _LPListItem {
    int		index;
    struct llist_node list;
} LPListItem;


typedef struct LPList {
    int size;
    struct llist_head head;
} LPList;

#endif 

