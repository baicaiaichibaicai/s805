#define ABN484 0x4840
#define ABN482 0x4820
#define W20 0x20
#define W10 0x10
#define A484_EEP_W20_434C       0x434C   // A484 9555 0x24 addressing
#define A484_EEP_W20_434D       0x434D   // A484 9555 0x25 addressing
#define A484_EEP_W20_434E       0x434E   // A484 9555 0x26 addressing
#define A484_EEP_W20_434F       0x434F   // A484 9555 0x27 addressing
#define A484_EEP_W10_430F       0x430F   // A484 9555 0x23 addressing
#define A484_EEP_W10_430E       0x430E   // A484 9555 0x22 addressing
#define A484_EEP_W10_430D       0x430D   // A484 9555 0x21 addressing
#define A484_EEP_W10_430C       0x430C   // A484 9555 0x20 addressing
#define A484_EEP_W20_030C       0x030C   // A484 9559 0x4c addressing
#define A484_EEP_W20_030D       0x030D   // A484 9559 0x4d addressing
#define A484_EEP_W20_030E       0x030E   // A484 9559 0x4e addressing
#define A484_EEP_W20_030F       0x030F   // A484 9559 0x4f addressing
#define A482_EEP_W20_C180	0xC180   // A482 9555 0x24 addressing
#define A482_EEP_W20_C181       0xC181   // A482 9555 0x25 addressing
#define A482_EEP_W20_C1C0       0xC1C0   // A482 9555 0x26 addressing
#define A482_EEP_W20_C1C1       0xC1C1   // A482 9555 0x27 addressing
#define A482_EEP_W10_C100       0xC100   // A482 9555 0x20 addressing
#define A482_EEP_W10_C101       0xC101   // A482 9555 0x21 addressing
#define A482_EEP_W10_C140       0xC140   // A482 9555 0x22 addressing
#define A482_EEP_W10_C141       0xC141   // A482 9555 0x23 addressing

struct _zen_bypass_info {
	char serial[8];
	char board[16];
	int is_onboard;
	int num_of_slot;
	int allbp[52];
};

extern void bypass_enable(void);
