#if !defined _SHIFT_INCLUDED
#define _SHIFT_INCLUDED

#include "..\common\common.h"

#define SHIFT_OUT_PORT		PORTE
#define SHIFT_OUT_DDR		DDRE
#define SHIFT_OUT_BIT		(2)

#define SHIFT_CLK_PORT		PORTE
#define SHIFT_CLK_DDR		DDRE
#define SHIFT_CLK_BIT		(3)

#define SHIFT_LATCH_PORT	PORTE
#define SHIFT_LATCH_DDR		DDRE
#define SHIFT_LATCH_BIT		(4)

#define SHIFT_LATCH_ON		SETBIT(SHIFT_LATCH_PORT, SHIFT_LATCH_BIT)
#define SHIFT_LATCH_OFF		CLEARBIT(SHIFT_LATCH_PORT, SHIFT_LATCH_BIT)

#define SHIFT_DELAY			(150)

extern uint8_t	controls[2];

#define	CONTROL_OFF(idx)	CLEARBIT(controls[idx >> 3], idx & 0x07)
#define	CONTROL_ON(idx)		SETBIT(controls[idx >> 3], idx & 0x07)
#define	TEST_CONTROL(idx)	(TESTBIT(controls[idx >> 3], idx & 0x07))

void	shift_one(uint8_t);
void	shift_all(void);
void	shift_init(void);
void	do_shift(void);

#endif /* _SHIFT_INCLUDED */