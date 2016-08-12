
void ms96d9_bypass_disable(int port);
void ms96d9_bypass_enable(int port);
void ms96d9_bypass_init(int port);
int is_atom_core(void);

#define ATOM_PROCESSOR	"Atom(TM) CPU D525"
#define DRV_NAME	 "GPIO controller"
#define LPC_CONFIG_BASE		0x00
#define GPIO_BA_OFFSET		0x48
#define SEND_BIT	0x15
#define PAIR1_BIT	0x6
#define PAIR2_BIT	0x2
#define PAIR3_BIT	0x1
#define CFG_1_BIT  	0x7
#define CFG_2_BIT	0x6
#define CFG_3_BIT	0x3
#define GP_LVL			0x0C
#define GP_LVL2			0x38
#define SIZE_LPC_REG		0x40
