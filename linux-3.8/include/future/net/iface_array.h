#include <future/general.h>

#define ZEN_SLOT_MAX				8

/* The number of slot in board */
#define SLOT_NUM_DEFAULT			1
#define SLOT_NUM_CAR2051			2	// ZEN2000,  3000R	Caswell
#define SLOT_NUM_FW8759 			2	// ZEN4000
#define SLOT_NUM_FW8771 			3	// ZEN5000
#define SLOT_NUM_CAR4020			4	// ZEN6000
#define SLOT_NUM_FW8877 			5	// ZEN7000,  8000
#define SLOT_NUM_FW8896 			7	// ZEN10000, 20000
#define SLOT_NUM_FW7573 			2	// ZEN3000,  3000S	Lanner

/* Ethernet Controller Device number */
#define DEVICE_82599_FIBER			0x10fb
#define DEVICE_82580_COPPER 		0x150e
#define DEVICE_82580_FIBER  		0x150f
#define DEVICE_I350_COPPER			0x1521
#define DEVICE_I350_FIBER			0x1522
#define DEVICE_I210_COPPER			0x1533
#define DEVICE_I211_COPPER			0x1539
#define DEVICE_I217_COPPER  		0x153a
#define DEVICE_X710_FIBER			0x1572
#define DEVICE_I354_COPPER			0x1f41

enum {
	BOARD_INDEX_DEFAULT = 0,
	BOARD_INDEX_CAR2051,
	BOARD_INDEX_FW8759,
	BOARD_INDEX_FW8771,
	BOARD_INDEX_CAR4020,
	BOARD_INDEX_FW8877,
	BOARD_INDEX_FW8896,
	BOARD_INDEX_FW7573
};

/* NIC Slot Information struct */
struct nic_slot_info {
	/* Show /proc/ferret/network/iface_array */
	int index;
	int use;
	int num_of_iface;
	int element[NR_ETH];
	unsigned short device;
	/* Use ethernet bypass driver */
	int bypass;     		// the number of bypass pair
	int mixed_iface;		// Mixed interface in slot
};

struct iface_pci_name {
	char name[13];			// Interface PCI name
	char bridge[13];		// Interface PCI bridge name
	char bridge2[13];		// NIP53040 PCI bridge name
	unsigned short device;  // PCI device number
};

struct nic_slot_pci_name {
	unsigned char low[13];
	unsigned char high[13];
};

#ifdef CONFIG_X86
extern int e1000_port[];
#endif
extern int MAX_ETH_DEV;
extern struct iface_pci_name g_iface_pci[];

int iface_array_proc_init(void);
