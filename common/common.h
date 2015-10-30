#if !defined __COMMON_H_INCLUDED_
#define __COMMON_H_INCLUDED_

#include <avr/io.h>
#include <avr/interrupt.h>

#define TRUE 1
#define FALSE 0

#define FOSC	(14745600UL)

#define TESTBIT(port, bit)	(port & _BV(bit))
#define SETBIT(port, bit)	(port |= _BV(bit))
#define CLEARBIT(port, bit)	(port &= ~(_BV(bit)))

#define TESTBITL(port, bit)		((port) & (1L << (bit)))
#define SETBITL(port, bit)		((port) |= (1L << (bit)))
#define CLEARBITL(port, bit)	((port) &= ~(1L << (bit)))

#define	EXT_MEM_INIT	{MCUCR = (1<<SRE);} // \
			//			SETBIT(XMCRB, XMM0); \
			//			SETBIT(XMCRB, XMM1); \
			//			SETBIT(XMCRB, XMM2);}

//#define	GLOBAL_INT_ENABLE   SETBIT( SREG, 7 )
//#define	GLOBAL_INT_DISABLE  CLEARBIT( SREG, 7 )

#define	GLOBAL_INT_ENABLE   sei()
#define	GLOBAL_INT_DISABLE  cli()

#define DELAY(ch)	{uint32_t i = ch; while (i--);}

#define F_CPU (14745600UL)

#include <avr/delay.h>

typedef enum
{
	RESULT_OK = 0,
	RESULT_BAD_ACK,
	RESULT_BAD_MSG,
	RESULT_BAD_CRC,
	RESULT_BUFFER_OVERFLOW,
	RESULT_MSG_TOO_BIG,
	RESULT_ALARM,
	RESULT_TIMEOUT,
	RESULT_UNKNOWN_PORT,
	RESULT_IGNORE_CMD
} result_e;

#endif /* __COMMON_H_INCLUDED_*/
