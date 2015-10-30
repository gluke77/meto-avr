#if !defined _USB_INCLUDED
#define _USB_INCLUDED

#include <avr\io.h>
#include "../common/common.h"
#include <string.h>

#define _USE_TIMED_USB	1 // comment out this for sync usb

#define USB_DATA		(*(volatile uint8_t *)(0x8080))
#define USB_STATUS		(*(volatile uint8_t *)(0x8081))

#define USB_RXE			(0)
#define USB_TXE			(1)
#define USB_SUSPEND		(2)
#define USB_CONFIGURED	(3)

#define USB_SEND_NOW	(USB_STATUS = 0xFF)

#define	USB_INBUF_SIZE	(64)

extern volatile uint8_t	usb_inbuf[USB_INBUF_SIZE];
extern volatile uint8_t	usb_inbuf_pos;
extern volatile uint8_t	usb_msg_ready;
extern volatile uint8_t 	usb_on;

int usb_putchar(char /* ch */);
int usb_getchar(void);
int usb_data_ready(void);

void usb_puts(const char * /* buf */);

void usb_getmsg(void);

result_e usb_cmd(uint8_t * /* req */, uint8_t * /* ack */, size_t /* ack_size */, uint16_t /* delay */);

#if defined _USE_TIMED_USB

#define USB_TIMEOUT	(100)

int usb_putchar_timed(char /* ch */, uint16_t /* delay */);
int usb_getchar_timed(uint16_t /* delay */);

#endif /* _USE_TIMED_USB */

#endif /* _USB_INCLUDED */