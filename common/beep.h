#if !defined _BEEP_INCLUDED
#define _BEEP_INCLUDED

#include <avr/io.h>
#include "common.h"

#if defined _MASTER

	#define BEEP_PORT		PORTD
	#define BEEP_DDR		DDRD
	#define BEEP_BIT		(1)

#endif /* _MASTER */

#if defined _SECONDARY

	#define BEEP_PORT		PORTB
	#define BEEP_DDR		DDRB
	#define BEEP_BIT		(7)

#endif /* _SECONDARY */

#define BEEP_INIT	SETBIT(BEEP_DDR, BEEP_BIT)
#define BEEP_ON		SETBIT(BEEP_PORT, BEEP_BIT)
#define BEEP_OFF	CLEARBIT(BEEP_PORT, BEEP_BIT)
#define CHECK_BEEP	TESTBIT(BEEP_PORT, BEEP_BIT)

void beep_init(void);
void beep_ms(uint16_t /* delay */);
void beep_timer_ms(uint16_t /* delay */);

#endif /* _BEEP_INCLUDED */