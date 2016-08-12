#ifndef _N3CRYPTO_ETHERNET_H_
#define _N3CRYPTO_ETHERNET_H_

#include <linux/skbuff.h>

int n3crypto_ethernet_open(void);
int n3crypto_ethernet_sendCmdMsg
(
	int slotID, 
	void *pInData, 
	unsigned int nInDataLen,
	void *pOutData, 
	unsigned int *pulOutDataLen
);
int n3crypto_ethernet_sendCmdMsg_async
(
 	struct sk_buff *skb,
	int len
);
int n3crypto_ethernet_controlDev(int command, void *pData, void *pArg1);
int n3crypto_ethernet_close(void);


#endif // _N3CRYPTO_ETHERNET_H_

