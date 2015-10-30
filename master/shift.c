#include <avr\io.h>
#include "..\common\common.h"
#include "shift.h"

uint8_t	controls[2];
uint8_t old_controls[2];

void	shift_one(uint8_t data)
{
	int	i;
	for (i = 0; i < 8; i++)
	{
		
		if (TESTBIT(data, i))
			SETBIT(SHIFT_OUT_PORT, SHIFT_OUT_BIT);
		else 
			CLEARBIT(SHIFT_OUT_PORT, SHIFT_OUT_BIT);
		
		SETBIT(SHIFT_CLK_PORT, SHIFT_CLK_BIT);
		DELAY(SHIFT_DELAY);
		CLEARBIT(SHIFT_CLK_PORT, SHIFT_CLK_BIT);
		CLEARBIT(SHIFT_OUT_PORT, SHIFT_OUT_BIT);
		DELAY(SHIFT_DELAY);
	}
}

void do_shift(void)
{
	if ((old_controls[0] != controls[0]) ||
		(old_controls[1] != controls[1]))
	{
		old_controls[0] = controls[0];
		old_controls[1] = controls[1];
		shift_all();
	}
}

void	shift_all(void)
{
	shift_one(controls[0]);
	shift_one(controls[1]);
	SHIFT_LATCH_ON;
	DELAY(SHIFT_DELAY);
	SHIFT_LATCH_OFF;
}

void	shift_init(void)
{
	SETBIT(SHIFT_OUT_DDR, SHIFT_OUT_BIT);
	SETBIT(SHIFT_CLK_DDR, SHIFT_CLK_BIT);
	SETBIT(SHIFT_LATCH_DDR, SHIFT_LATCH_BIT);
	CLEARBIT(SHIFT_OUT_PORT, SHIFT_OUT_BIT);
	CLEARBIT(SHIFT_CLK_PORT, SHIFT_CLK_BIT);
	CLEARBIT(SHIFT_LATCH_PORT, SHIFT_LATCH_BIT);
	controls[0] = 0x00;
	controls[1] = 0x00;
	old_controls[0] = 0xFF;
	old_controls[1] = 0xFF;	
}