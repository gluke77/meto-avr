#include <avr\io.h>
#include "..\common\common.h"
#include "kbd.h"
//#include "../common/beep.h"

volatile uint8_t	key_pressed;

volatile uint8_t	kbd_value_1;
volatile uint8_t	kbd_value_2;

void kbd_init(void)
{
	kbd_value_1 = 0xFF;
	kbd_value_2 = 0xFF;
	key_pressed = 0x00;
}


void kbd_scan(void)
{
	uint8_t	kbd_value;
	uint8_t	key_id;

	kbd_value = KBD_PORT;
	for (key_id = 0; key_id < 8; key_id++)
	{
		if (TESTBIT(kbd_value_1, key_id) && 
			TESTBIT(kbd_value_2, key_id) &&
			!TESTBIT(kbd_value, key_id))
			
			CLEARBIT(kbd_value_1, key_id);
			
		else if (!TESTBIT(kbd_value_1, key_id) && //key pressed
			TESTBIT(kbd_value_2, key_id) &&
			!TESTBIT(kbd_value, key_id))
		{
			CLEARBIT(kbd_value_2, key_id);
		}
		else if (!TESTBIT(kbd_value_1, key_id) && 
			TESTBIT(kbd_value_2, key_id) &&
			TESTBIT(kbd_value, key_id))
			 
			SETBIT(kbd_value_1, key_id);
			
		else if (!TESTBIT(kbd_value_1, key_id) && 
			!TESTBIT(kbd_value_2, key_id) &&
			TESTBIT(kbd_value, key_id))
			 
			SETBIT(kbd_value_1, key_id);
			
		else if (TESTBIT(kbd_value_1, key_id) && 
			!TESTBIT(kbd_value_2, key_id) &&
			!TESTBIT(kbd_value, key_id))
			 
			CLEARBIT(kbd_value_1, key_id);

		else if (TESTBIT(kbd_value_1, key_id) && // key released
			!TESTBIT(kbd_value_2, key_id) &&
			TESTBIT(kbd_value, key_id))
		{	 
			SETBIT(kbd_value_2, key_id);
			
			SETBIT(key_pressed, key_id);
		}
		else if (!TESTBIT(kbd_value_1, key_id) && // key still pressed
			!TESTBIT(kbd_value_2, key_id) &&
			!TESTBIT(kbd_value, key_id))
		{	 
		}
	}
}

void kbd_clear(void)
{
	key_pressed = 0x00;
}

void do_kbd(void)
{
	static uint8_t kbd_timeout = KBD_TIMEOUT;
	
	if (!kbd_timeout--)
	{
		kbd_timeout = KBD_TIMEOUT;
		kbd_scan();
	}
}