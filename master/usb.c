#include <avr\io.h>
#include "../common/common.h"
#include "usb.h"
#include <string.h>
#include <../common/timer.h>

volatile uint8_t	usb_inbuf[USB_INBUF_SIZE];
volatile uint8_t	usb_msg_ready = 0;
volatile uint8_t	usb_inbuf_pos = 0;
volatile uint8_t	usb_on = 0;

int usb_putchar(char ch)
{
#if defined _USE_TIMED_USB
	return usb_putchar_timed(ch, USB_TIMEOUT);
#else
	while (TESTBIT(USB_STATUS, USB_TXE));
	USB_DATA = ch;
	return 0;
#endif /* _USE_TIMED_USB */
}

#if defined _USE_TIMED_USB
int usb_putchar_timed(char ch, uint16_t delay)
{
	uint8_t timer_id = 0;
	
	if (!usb_on)
		return 0;
		
	timer_id = start_timer(delay);
	
	while (TESTBIT(USB_STATUS, USB_TXE))
		if (timer_id)
			if (!timer_value(timer_id))
			{
				stop_timer(timer_id);
				timer_id = 0;
				usb_on = 0;
				return 0;
			}
	
	stop_timer(timer_id);
	timer_id = 0;
	
	USB_DATA = ch;
	return 0;
}
#endif /* _USE_TIMED_USB */

void usb_puts(const char * buf)
{
	while (*buf)
		usb_putchar(*buf++);
}

int	usb_getchar(void)
{
#if defined _USE_TIMED_USB
	return usb_getchar_timed(USB_TIMEOUT);
#else
	while (TESTBIT(USB_STATUS, USB_RXE));
	return USB_DATA;
#endif /* _USE_TIMED_USB */
}

#if defined _USE_TIMED_USB
int usb_getchar_timed(uint16_t delay)
{
	uint8_t timer_id = 0;

	if (!usb_on)
		return -1;

	timer_id = start_timer(delay);
	
	while (TESTBIT(USB_STATUS, USB_RXE))
		if (timer_id)
			if (!timer_value(timer_id))
			{
				stop_timer(timer_id);
				timer_id = 0;
//				usb_on = 0;
				return -1;
			}

	stop_timer(timer_id);
	timer_id = 0;

	return USB_DATA;
}
#endif /* _USE_TIMED_USB */

int usb_data_ready(void)
{
#if defined _USE_TIMED_USB
	if (TESTBIT(USB_STATUS, USB_RXE))
		return 0;
	else
	{
		usb_on = 1;
		return 1;
	}
#else
	return !TESTBIT(USB_STATUS, USB_RXE);
#endif /* _USE_TIMED_USB */
}

void usb_getmsg(void)
{
	uint8_t	byte;
	
	while ((!usb_msg_ready) && usb_data_ready())
	{
		byte = USB_DATA;
	
		if (':' == byte)
		{
			memset(usb_inbuf, 0x00, USB_INBUF_SIZE);
			usb_inbuf[0] = ':';
			usb_inbuf_pos = 1;
			usb_msg_ready = 0;
		}
		else if (usb_inbuf_pos)
			usb_inbuf[usb_inbuf_pos++] = byte;
		
		if (':' == usb_inbuf[0] &&
			'\r' == usb_inbuf[usb_inbuf_pos-2] &&
			'\n' == usb_inbuf[usb_inbuf_pos-1])
		{
			usb_msg_ready = 1;
			usb_inbuf_pos = 0;
		}
		else if (USB_INBUF_SIZE == usb_inbuf_pos)
		{
			memset(usb_inbuf, 0x00, USB_INBUF_SIZE);
			usb_inbuf_pos = 0;
			usb_msg_ready = 0;
		}
	}
}

result_e usb_cmd(uint8_t * req, uint8_t * ack, size_t ack_size, uint16_t delay)
{
	result_e		res;
	uint8_t			timer_id;
	uint8_t			idx;
	
	if (!usb_on)
		return RESULT_TIMEOUT;
	
	for (idx = 0; req[idx] != '\n'; idx++)
		usb_putchar(req[idx]);
	usb_putchar('\n');
		
	if (NULL == ack)
		return RESULT_OK;
	
	res = RESULT_TIMEOUT;
		
	timer_id = start_timer(delay);
		
	while (timer_value(timer_id))
	{
		usb_getmsg();
		if (usb_msg_ready)
		{
			res = RESULT_OK;
			usb_msg_ready = 0;
			memcpy(ack, usb_inbuf, (ack_size < USB_INBUF_SIZE)?ack_size:USB_INBUF_SIZE);
			stop_timer(timer_id);
			break;
		}
	}
	
	stop_timer(timer_id);
	
	return res;
}


