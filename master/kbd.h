#if !defined _KBD_INCLUDED
	#define _KBD_INCLUDED
	
#include "..\common\common.h"

#define KBD_PORT		(*(volatile uint8_t *)(0x80FF))

#define KEY_COUNT		(6)

#define KBD_TIMEOUT		(11)

#define KEY_MENU	(5)
#define KEY_ENTER	(3)
#define KEY_LEFT	(2)
#define KEY_RIGHT	(0)
#define KEY_UP		(4)
#define KEY_DOWN	(1)

void kbd_scan(void);
void kbd_init(void);
uint8_t kbd_keypressed(void);
void kbd_clear(void);
void do_kbd(void);

#define KEY_PRESSED(key_id)		TESTBIT(key_pressed, key_id )
#define CLEAR_KEY(key_id)		CLEARBIT(key_pressed, key_id )
#define CLEAR_KEY_PRESSED(key_id)	CLEAR_KEY(key_id)

extern volatile uint8_t	key_pressed;
	
#endif /* _KBD_INCLUDED */