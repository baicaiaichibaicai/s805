#ifndef _FS_ETH_H_
#define	_FS_ETH_H_


#define	NKHA_DEBUG			0
#define	DEBUG_RCV			0
#define	DEBUG_RCV1			0

#define	USE_MAC				0
#define	MC_CALL_CRYPTO		0

#define USE_VM_CRYPTO		0

#define	TEST				0
#define	TIME				0
#define	THREAD				1
#define WAIT_FOR			0



#define FS_HEADER_LEN 		14
#define MAC_HEADER_LEN 		14

#define	QUEUE_SIZE			2048*100



#if USE_VM_CRYPTO
/* NIC interfaces for VM crypto */
#define	NETDEVICE_MAIN		"eth1"
#define	NETDEVICE_BIIKBU	"eth1"
#else
/* NIC interfaces for mZEN */
#define	NETDEVICE_MAIN		"eth3"
#define	NETDEVICE_BIIKBU	"eth1"
#endif



#pragma pack(push, 1)

//future
typedef struct
{
#define	TYPE_OF_REQ_IKE			0x0100
#define TYPE_OF_REQ_IPSEC_EN	0x0140
#define TYPE_OF_REQ_IPSEC_DE	0x0160
#define TYPE_OF_REQ_IPSEC		0x0150
#define TYPE_OF_RSP_IKE			0x0180
#define TYPE_OF_RSP_IPSEC_EN	0x01C0
#define TYPE_OF_RSP_IPSEC_DE	0x01E0
#define TYPE_OF_RSP_IPSEC		0x01D0
	short 			type;
	/*
	short			reserved;
	short			pin;
	int				thread_id;
	*/
	u64				track;
	unsigned short	length;

#define ETH_FS_TX_PKT			0xAAAA		// toward crypto module
#define ETH_FS_RX_IKE			0xAABB		// from crypto module
#define ETH_FS_RX_IPSEC_ASYNC	0xAACC		// from crypto module
#define ETH_FS_RX_IPSEC_SYNC	0xAADD		// from crypto module
	unsigned short	ether_type;
} FUTURE_HEADER;

#pragma pack(pop)


#endif // _FS_ETH_H_
