#include <avr\io.h>
#include "..\common\common.h"
#include "sensor.h"
#include "shift.h"

volatile uint8_t	sensors[SENSOR_BANK_COUNT];
volatile uint8_t	tmp_sensors[SENSOR_BANK_COUNT];
volatile uint8_t	sensor_value_1[SENSOR_BANK_COUNT];

uint8_t				inverse_mask[SENSOR_BANK_COUNT];

void sensor_init(void)
{
	uint8_t	idx;
	
	inverse_mask[0] = 0x00; //0x02; // 00000010
	inverse_mask[1] = 0x00; //0xAA; // 10101010
	inverse_mask[2] = 0x00;
	
	for (idx = 0; idx < SENSOR_BANK_COUNT; idx++)
	{
		sensor_value_1[idx] = 0xFF;
		tmp_sensors[idx] = 0x00;
		sensors[idx] = 0x00;
	}

	SENSOR_PORT_DDR = 0;
	SETBIT(SENSOR_SCAN_DDR0, SENSOR_SCAN_BIT0);
	SETBIT(SENSOR_SCAN_DDR1, SENSOR_SCAN_BIT1);
	SETBIT(SENSOR_SCAN_DDR2, SENSOR_SCAN_BIT2);
}

void sensor_select(uint8_t bank)
{
	if (SENSOR_BANK_COUNT <= bank)
		bank = 0;
	
	if (bank == 0)
		CLEARBIT(SENSOR_SCAN_PORT0, SENSOR_SCAN_BIT0);
	else
		SETBIT(SENSOR_SCAN_PORT0, SENSOR_SCAN_BIT0);
		
	if (bank == 1)
		CLEARBIT(SENSOR_SCAN_PORT1, SENSOR_SCAN_BIT1);
	else
		SETBIT(SENSOR_SCAN_PORT1, SENSOR_SCAN_BIT1);
		
	if (bank == 2)
		CLEARBIT(SENSOR_SCAN_PORT2, SENSOR_SCAN_BIT2);
	else
		SETBIT(SENSOR_SCAN_PORT2, SENSOR_SCAN_BIT2);
}

void sensor_scan(void)
{
	uint8_t	bank_id;
	uint8_t	sensor_value;
	uint8_t	key_id;
	
	for (bank_id = 0; bank_id < SENSOR_BANK_COUNT; bank_id++)
	{
		sensor_select(bank_id);
		DELAY(500);
		sensor_value = SENSOR_PORT;

		for (key_id = 0; key_id < 8; key_id++)
		{
			if (TESTBIT(sensor_value_1[bank_id], key_id) && 
				TESTBIT(tmp_sensors[bank_id], key_id) &&
				!TESTBIT(sensor_value, key_id))
				
				CLEARBIT(sensor_value_1[bank_id], key_id);
				
			else if (!TESTBIT(sensor_value_1[bank_id], key_id) && //key pressed
				TESTBIT(tmp_sensors[bank_id], key_id) &&
				!TESTBIT(sensor_value, key_id))

				CLEARBIT(tmp_sensors[bank_id], key_id);

			else if (!TESTBIT(sensor_value_1[bank_id], key_id) && 
				TESTBIT(tmp_sensors[bank_id], key_id) &&
				TESTBIT(sensor_value, key_id))
				 
				SETBIT(sensor_value_1[bank_id], key_id);
				
			else if (!TESTBIT(sensor_value_1[bank_id], key_id) && 
				!TESTBIT(tmp_sensors[bank_id], key_id) &&
				TESTBIT(sensor_value, key_id))
				 
				SETBIT(sensor_value_1[bank_id], key_id);
				
			else if (TESTBIT(sensor_value_1[bank_id], key_id) && 
				!TESTBIT(tmp_sensors[bank_id], key_id) &&
				!TESTBIT(sensor_value, key_id))
				 
				CLEARBIT(sensor_value_1[bank_id], key_id);

			else if (TESTBIT(sensor_value_1[bank_id], key_id) && // key released
				!TESTBIT(tmp_sensors[bank_id], key_id) &&
				TESTBIT(sensor_value, key_id))

				SETBIT(tmp_sensors[bank_id], key_id);
		}
	}
}

void sensor_scan1(uint8_t bank_id)
{
	uint8_t	sensor_value;
	uint8_t	key_id;
	
	sensor_value = SENSOR_PORT;

	for (key_id = 0; key_id < 8; key_id++)
	{
		if (TESTBIT(sensor_value_1[bank_id], key_id) && 
			TESTBIT(tmp_sensors[bank_id], key_id) &&
			!TESTBIT(sensor_value, key_id))
				
			CLEARBIT(sensor_value_1[bank_id], key_id);
				
		else if (!TESTBIT(sensor_value_1[bank_id], key_id) && //key pressed
			TESTBIT(tmp_sensors[bank_id], key_id) &&
			!TESTBIT(sensor_value, key_id))

			CLEARBIT(tmp_sensors[bank_id], key_id);

		else if (!TESTBIT(sensor_value_1[bank_id], key_id) && 
			TESTBIT(tmp_sensors[bank_id], key_id) &&
			TESTBIT(sensor_value, key_id))
				 
			SETBIT(sensor_value_1[bank_id], key_id);
				
		else if (!TESTBIT(sensor_value_1[bank_id], key_id) && 
			!TESTBIT(tmp_sensors[bank_id], key_id) &&
			TESTBIT(sensor_value, key_id))
				 
			SETBIT(sensor_value_1[bank_id], key_id);
				
		else if (TESTBIT(sensor_value_1[bank_id], key_id) && 
			!TESTBIT(tmp_sensors[bank_id], key_id) &&
			!TESTBIT(sensor_value, key_id))
				 
			CLEARBIT(sensor_value_1[bank_id], key_id);

		else if (TESTBIT(sensor_value_1[bank_id], key_id) && // key released
			!TESTBIT(tmp_sensors[bank_id], key_id) &&
			TESTBIT(sensor_value, key_id))

			SETBIT(tmp_sensors[bank_id], key_id);
	}

	sensors[bank_id] = (tmp_sensors[bank_id] & ~(inverse_mask[bank_id])) |
		(~(tmp_sensors[bank_id]) & inverse_mask[bank_id]);

}

void do_sensor(void)
{
	static uint8_t	current_sensor_bank = 0xFF;
	static uint8_t sensor_timeout = SENSOR_TIMEOUT;
	
//	if (sensor_timeout--)
//		return;
		
	sensor_timeout = SENSOR_TIMEOUT;
	
	if (current_sensor_bank != 0xFF)
		sensor_scan1(current_sensor_bank);
	
	current_sensor_bank++;
		
	if (SENSOR_BANK_COUNT == current_sensor_bank)
		current_sensor_bank = 0;
		
	sensor_select(current_sensor_bank);
}