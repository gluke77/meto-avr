#if !defined _SENSOR_INCLUDED
	#define _SENSOR_INCLUDED
	
#include "..\common\common.h"

#define SENSOR_PORT			PINF
#define SENSOR_PORT_DDR		DDRF
#define SENSOR_SCAN_PORT0	PORTE
#define SENSOR_SCAN_DDR0	DDRE
#define SENSOR_SCAN_PORT1	PORTD
#define SENSOR_SCAN_DDR1	DDRD
#define SENSOR_SCAN_PORT2	PORTD
#define SENSOR_SCAN_DDR2	DDRD
#define SENSOR_SCAN_BIT0	(5)
#define SENSOR_SCAN_BIT1	(6)
#define SENSOR_SCAN_BIT2	(7)

#define SENSOR_BANK_COUNT	(3)

#define SENSOR_TIMEOUT		(7)

#define TEST_SENSOR(idx)	(!TESTBIT(sensors[idx >> 3], (idx & 0x07)))
#define SENSOR_ON(idx)		(CLEARBIT(sensors[idx >> 3], (idx & 0x07)))
#define SENSOR_OFF(idx)		(SETBIT(sensors[idx >> 3], (idx & 0x07)))

#define TEST_HARD_SENSOR(idx)	(!TESTBIT(hard_sensors[idx >> 3], (idx & 0x07)))

extern volatile uint8_t	hard_sensors[SENSOR_BANK_COUNT];

extern volatile uint8_t	sensors[SENSOR_BANK_COUNT];
extern volatile uint8_t	stop_germo_led;
						
void sensor_select(uint8_t);
void sensor_init(void);
void sensor_scan(void);
void sensor_scan1(uint8_t);
void do_sensor(void);
	
#endif /* _SENSOR_INCLUDED */