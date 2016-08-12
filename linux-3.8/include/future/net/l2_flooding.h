/***************************************************************************
* Copyright (c) 1990-2016, Future Systems, Inc. / Seoul, Republic of Korea *
* All Rights Reserved.                                                     *
*                                                                          *
* Author: pierre (pierre@kms.future.co.kr)                                 *
* First draft writing date: 2016-03-18                                     *
*                                                                          *
* This document contains proprietary and confidential information. No      *
* parts of this document or the computer program it embodies may be in     *
* any way copied, duplicated, reproduced, translated into a different      *
* programming language, or distributed to any person, company, or          *
* corporation without the prior written consent of Future Systems, Inc.    *
***************************************************************************/
#ifndef _L2_FLOODINF_H_
#define _L2_FLOODING_H_

#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/gfp.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/kthread.h>

//broadcast mac on 64b
//note: in the kernel, each interface has a different broadcast address.
//but looking for the broadcast address of a frame every time we have a packet
//would be to memory expensive. Furthermore, we are only interested in ethernet
//frames by definition, and it seems that the broadcast mac is always 0xFF..FF
//for them, thus we define value at compile time to speed up things
#define BROADCAST_MAC 		0x0000ffffffffffff

//loop over all network devices
#define LOOP_NDEV(v) for_each_netdev(&init_net, v)

//test the string x is the shape ethi
#define CHECK_ETH(x) (x[0]=='e'&&x[1]=='t'&&x[2]=='h')


#define CHECK_MAC(x,y) (x[0]==y[0]&& \
	x[1]==y[1]&& \
	x[2]==y[2]&& \
	x[3]==y[3]&& \
	x[4]==y[4]&& \
	x[5]==y[5])

typedef uint32_t rate_t;


//check that rate_t is unsigned at compilation time
#define CHECK_PPS_TYPE() BUILD_BUG_ON(((rate_t)-1)==((float)-1))

/*******************************************************************************
 *******************************************************************************
 * GENERIC FUNCTIONS
 *******************************************************************************
 ******************************************************************************/
//function to execute thread every second
int l2_flooding_update(void *);


struct flood_element {
	atomic_t rate;
	rate_t limit;
};
/*******************************************************************************
 *******************************************************************************
 * END OF GENERIC FUNCTIONS
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
 * MAC FLOOD GUARD
 *******************************************************************************
 ******************************************************************************/
//convert a mac address to an unsigned long for our key tree
//our first intuition was that it is faster to invert the fields of the mac
//address and perform the lookup (most of the mac address we will forbid
//will have different LSBs but similar MSBs (locality of vendor/geography/...)
//The linux radix-tree put the MSBs as the root, thus the LSBs of the
//MAC adress should become the MSBs in our tree), but it seems that inverting
//the order of the bits is too costly. maybe we didn't understand something
//in linux radix tree implementation?
//note: this only works in 64b platforms (sizeof(unsigned long)=8>6)
//but for both endianess (since it uses every two char, it doesn't change the
//endianness of the notation chosen by the user that we know or don't know)
#define MAC_TO_TREE_KEY(x) (*((unsigned long*)x)&BROADCAST_MAC)

#define MACF_PROC_NAME 	"mac_flood_guard"

#define MACF_MAX_NB 	100

#define CHK_HEX_N(x) ((x>='0'&&x<='9')||(x>='A'&&x<='F'))
#define CONV_HEX_NUM(x) ((x>='A')?(x-'A'+10):(x-'0'))
#define MAC_TO_CHAR(x) ((CONV_HEX_NUM(x[0])<<4)+CONV_HEX_NUM(x[1]))

#define MACF_MSG_HEADER "[MAC FLOOD GUARD]"

//check  we registered a mac address and we are above threshold
bool mac_flood_checker(struct sk_buff*);


//get data from user space
int macf_write(struct file *, const char __user*, size_t, void*);

//send data to user space
int macf_read(char*, char**, off_t, int, int*, void*);
/*******************************************************************************
 *******************************************************************************
 * END OF MAC FLOOD GUARD
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
 * STORM CONTROL
 *******************************************************************************
 ******************************************************************************/
#define STORM_PROC_NAME 	"storm_control"
#define MAXIMUM_STORM_ARG_LEN 	500
#define MAX_RATE_VALUE 		(((rate_t)-1)-1) //max rate_t val-2, 
#define STORM_NOT_CHANGE 	((rate_t)-1) //-1 is kept for do not change
#define STORM_MSG_HEADER 	"[STORM CONTROL]"

#define FE_SIZE 		sizeof(struct flood_element)

//see below struct storm_control for more explanation about those flags
#define SC_ACTIVE_BIT 		0xE0 //if one of those bits is set, the
//interface wants broad/uni/multi
//filtering
#define SC_ACTIVE_BROADCAST_BIT 	0x80 //wants broadcast filtering
#define SC_ACTIVE_MULTICAST_BIT 	0x40 //wants multicast filtering
#define SC_ACTIVE_UNICAST_BIT		0x20 //unicast is the default
#define SC_INTERFACE_MASK 		0x1F  //mask to extract interface number
#define SC_INTERFACE_NOT_DETECT 	SC_INTERFACE_MASK
//all those bits are set to one if the interface is
//not recognized

#define SC_GET_ALL_FLAGS(x)		(x>>5)


#ifdef CONFIG_MZEN
#define SC_NB_INTERFACE 		4
//max number of interface to take care of
//notice that if the number of interface is
//changed, we need to change all the masks
//above
//we block the number of interface to 4 in
//MZEN
//NOTE: this corresponds to eth[0..3],
//and can NOT correspond to ethh[4..7] for
//example; else we need another translation
//table
#else
#define SC_NB_INTERFACE 		SC_INTERFACE_MASK
#endif


/*******************************************************************************
 * struct storm_control - hold data to storm control conf and actual rate
 *
 * All fields are self explanatory except interface_translation_table.
 * interface_translation_table: translation table  ifindex->0b(bm ux xx xx)
 *  where:
 *  	- x xx xx is the interface number (<31). Note that if there is more
 *  	  than 2^5-1 eth interface on the system, it will crash and we have to
 *  	  use a 16b integer instead
 *  	- if x xx xx = 0x1F , then it means that the interface is NOT active
 *  	  (not an ethi)
 *  	- if u bit is set, the interface is unicast active
 *  	- if m bit is set, the interface is multicast active
 *  	- if b bit is set, the interface is broadcast active
 *  	- if u=m=b=0, no active threshold is set
 *
 * The bits are updated everytime valid data is written to the procfs interface.
 *
 * Note: to speed up translation, a lot of values will be just 0. it allows
 *       to bypass the storm control functionality for interfaces that doesn't
 *       support it.
 * Note about performance: a lot of thought has been put into performance here.
 *   	1. Using the translation table to set bits forces us to do 2 & ops
 *   	   more.
 *   	2. It allows us to just store everything in 1/8th of a register
 *   		(see storm_control_checker function))
 *   	-> it's a classical memory/computing power, after testing the cache/
 *   	register pressure is already high enough to not create an extra
 *   	overhead (which will be way more important than 2 &s).
 *   	-> if one core = <8 interfaces (which will be the case in the device
 *   	   it seems?) it means that all the actual translation table will be
 *   	   stored into one register which is OK.
 ******************************************************************************/
struct storm_control {
	//not that the first element of those array will always be inactive (non
	//ethi interfaces)
	struct flood_element *broadcast; 	//broadcast parameters
	struct flood_element *multicast; 	//multicast parameters
	struct flood_element *unicast;	//unicast parameters

	uint8_t *interface_translation_table;   //see above
	bool 	is_storm_control; 		//check if storm control is in use
};


//free argument
void free_storm(struct storm_control *);

//check an interface name
int get_eth_number(char *);

//check MAC address flood / interface (storm control)
bool storm_control_checker(struct sk_buff*);

//initialize the storm translation table
bool storm_translation_table_init(void);

//remove limit on specific interface
bool remove_storm_interface(int, struct flood_element*, const char*, uint8_t);

//add a single limit to a storm element
bool add_storm_interface(struct flood_element*, int, rate_t, const char*,
	uint8_t);

//return the biggest ifindex of all interfaces
uint16_t get_max_ifidx_interface(void);

//get data from user space
int storm_write(struct file *, const char __user*, size_t, void*);

//send data to user space
int storm_read(char*, char**, off_t, int, int*, void*);

//add a list of limit to a net_device and start/stop thread if necessary
bool add_storm_interfaces(struct net_device *, rate_t, rate_t,rate_t);
/*******************************************************************************
 *******************************************************************************
 * END OF STORM CONTROL FUNCTIONS
 *******************************************************************************
 ******************************************************************************/
#endif // _L2_FLOODING_H_

