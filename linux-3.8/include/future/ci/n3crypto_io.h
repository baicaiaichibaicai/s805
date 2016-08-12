#ifndef N3CRYPTO_IO_H
#define N3CRYPTO_IO_H


#define CRYPTOKI_IO_ETHERNET 1

#define N3CRYPTO_USB_ISNOT_OPEN		(-1)
#define RETRY_USB_READ				0



#define N3CRYPTO_USB_CMD_GET_SLOT_LIST			0x81
#define N3CRYPTO_USB_CMD_GET_DEV_VERSION		0x82
#define N3CRYPTO_USB_CMD_TOKEN_PRESENT			0x83
#define N3CRYPTO_USB_CMD_SET_DEBUG_STRING		0x91
#define N3CRYPTO_USB_CMD_GET_DEBUG_STRING		0x92

typedef struct {
	unsigned long int *pulCount;
	unsigned long int *pSlotList;
} USB_SLOT_LIST_t;

typedef struct {
	unsigned char major;
	unsigned char minor;
} USB_LIB_VERSION_t;



/* -------------------------------------------------------------------------- *
 *	Define interface between cryptoki and crypto_module
 *
 *	[file]				[Define]			[platform]		[build_dir]
 *	n3crpto_usbio.h		CRYPTOKI_IO_USB		host/gadget		n3crypto_usb
 *	n3crpto_dummyio.h	CRYPTOKI_IO_DUMMY	host-only		n3crypto_dummy
 *	n3crpto_callio.h	CRYPTOKI_IO_CALL	host-only		n3crypto_call
 *	n3crpto_msgio.h		CRYPTOKI_IO_MSG		host-dummy		n3crypto_msg
 *	n3crpto_socketio.h	CRYPTOKI_IO_SOCKET	client/server	n3crypto_socket
 * -------------------------------------------------------------------------- */

#ifdef CRYPTOKI_IO_DUMMY
# include "n3crypto_dummyio.h"
# define   n3crypto_open					n3crypto_dummy_open
# define   n3crypto_close					n3crypto_dummy_close
# define   n3crypto_controlDev				n3crypto_dummy_controlDev
# define   n3crypto_sendCmdMsg				n3crypto_dummy_sendCmdMsg
#endif // CRYPTOKI_IO_DUMMY



#ifdef CRYPTOKI_IO_CALL
# include "n3crypto_callio.h"
# define   n3crypto_open					n3crypto_call_open
# define   n3crypto_close					n3crypto_call_close
# define   n3crypto_controlDev				n3crypto_call_controlDev
# define   n3crypto_sendCmdMsg				n3crypto_call_sendCmdMsg
#endif // CRYPTOKI_IO_CALL




#ifdef CRYPTOKI_IO_MSG
# include "n3crypto_msgio.h"
# define   n3crypto_open					n3crypto_msg_open
# define   n3crypto_close					n3crypto_msg_close
# define   n3crypto_controlDev				n3crypto_msg_controlDev
# define   n3crypto_sendCmdMsg				n3crypto_msg_sendCmdMsg
#endif // CRYPTOKI_IO_MSG




#ifdef CRYPTOKI_IO_USB
# include "n3crypto_usbio.h"
# define   n3crypto_open					n3crypto_usb_open
# define   n3crypto_close					n3crypto_usb_close
# define   n3crypto_controlDev				n3crypto_usb_controlDev
# define   n3crypto_sendCmdMsg				n3crypto_usb_sendCmdMsg
#endif // CRYPTOKI_IO_USB




#ifdef CRYPTOKI_IO_SOCKET
# include "n3crypto_callio.h"
# define   n3crypto_open					n3crypto_socket_open
# define   n3crypto_close					n3crypto_socket_close
# define   n3crypto_controlDev				n3crypto_socket_controlDev
# define   n3crypto_sendCmdMsg				n3crypto_socket_sendCmdMsg
#endif // CRYPTOKI_IO_SOCKET

#ifdef CRYPTOKI_IO_ETHERNET
# include "n3crypto_ethernetio.h"
# define   n3crypto_open					n3crypto_ethernet_open
# define   n3crypto_close					n3crypto_ethernet_close
# define   n3crypto_controlDev				n3crypto_ethernet_controlDev
# define   n3crypto_sendCmdMsg				n3crypto_ethernet_sendCmdMsg
#endif

#endif	/* N3CRYPTO_IO_H */
