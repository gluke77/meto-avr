#if !defined __CLOCK_H_INCLUDED_
#define __CLOCK_H_INCLUDED_

#include <avr/io.h>
#include "..\common\common.h"

#define CLOCK_BASE			0x8000

#define SECONDS				(*(volatile uint8_t*)(CLOCK_BASE | 0))
#define SECONDS_ALARM		(*(volatile uint8_t*)(CLOCK_BASE | 1))
#define MINUTES				(*(volatile uint8_t*)(CLOCK_BASE | 2))
#define MINUTES_ALARM		(*(volatile uint8_t*)(CLOCK_BASE | 3))
#define HOURS				(*(volatile uint8_t*)(CLOCK_BASE | 4))
#define HOURS_ALARM			(*(volatile uint8_t*)(CLOCK_BASE | 5))
#define D_WEEK				(*(volatile uint8_t*)(CLOCK_BASE | 6))
#define D_MONTH				(*(volatile uint8_t*)(CLOCK_BASE | 7))
#define MONTH				(*(volatile uint8_t*)(CLOCK_BASE | 8))
#define YEAR				(*(volatile uint8_t*)(CLOCK_BASE | 9))
#define CLOCK_A				(*(volatile uint8_t*)(CLOCK_BASE | 10))
#define CLOCK_B				(*(volatile uint8_t*)(CLOCK_BASE | 11))
#define CLOCK_C				(*(volatile uint8_t*)(CLOCK_BASE | 12))
#define CLOCK_D				(*(volatile uint8_t*)(CLOCK_BASE | 13))

#define CLOCK_TOTAL_WORK	(*(volatile uint32_t*)(CLOCK_BASE | 14))

#define START_CLOCK			{CLOCK_A = _BV(5); CLOCK_B = 7;}

#endif /* __CLOCK_H_INCLUDED_ */