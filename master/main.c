#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "../common/common.h"
#include "../common/usart.h"
#include "../common/lcd.h"
//#include "../common/ms.h"
#include "usb.h"
#include "../common/beep.h"
#include "../common/modbus.h"
#include "../common/meto.h"
#include "../common/soft_controls.h"
#include "../common/timer.h"
#include "kbd.h"
#include "shift.h"
#include "sensor.h"
#include "clock.h"
//#include "../common/flash.h"
#include "../common/menu.h"
#include "menu_items.h"

char	ch, x, y;
char	buf[50];

uint8_t	timer_id;
uint8_t	min, sec, hr, year, month, day;

modbus_cmd_s	req, ack, cmd;
result_e		res;

void process_kbd(void);
void process_secondary(void);
void process_sensors(void);
void process_usb(void);
void process_soft_controls(void);
void process_water(void);
void process_encoder_counter(void);

void do_encoder(void);

uint8_t		secondary_sensors;
uint16_t	soft_controls;
uint8_t		master_device_id = 0;
uint8_t		secondary_device_id = 0;
uint16_t	usart_error_count = 0;
uint8_t		usart_last_error = 0;
uint32_t	foil_encoder_pulse_count = 0;
uint16_t	g_pnevmo_pulse_duration = 2000;
uint8_t		g_empty_bath_state = 0;

int main(void)
{
	EXT_MEM_INIT;
	
//	usart0_init(USART_RS485_MASTER, 115200);
//	usart0_setprotocol_sec();
	usart1_init(USART_RS485_MASTER, 38400);
	usart1_setprotocol_modbus();

	timer_init();
	shift_init();
	sensor_init();
	lcd_init();
	kbd_init();
	beep_init();
	menu_init();
	menu_items_init();
//	flash_init();
	
	GLOBAL_INT_ENABLE;

	beep_ms(1000);
	delay_ms(500);
	beep_ms(200);
	delay_ms(200);
	beep_ms(200);
	delay_ms(200);

/*
	START_CLOCK;
	
	while (!KEY_PRESSED(0));
	YEAR = 5;
	MONTH = 4;
	D_MONTH = 1;
	HOURS = 10;
	MINUTES = 49;
	SECONDS = 0;
*/	

	lcd_init();
	

	for (;;)
	{
		process_usb();
		menu_doitem();
//		process_kbd();
		process_secondary();
		process_sensors();
		process_soft_controls();
		process_water();
//		do_sensor();
		do_shift();
		do_lcd();

		delay_ms(50);
	}

	return 0;
}

void do_timer(void)
{
	do_kbd();
	do_sensor();
	do_encoder();
}

void process_kbd()
{
	uint8_t key_id;
	
	for (key_id = 0; key_id < KEY_COUNT; key_id++)
		if (KEY_PRESSED(key_id))
		{
			CLEAR_KEY(key_id);
			sprintf(lcd_line1, "PRESSED %d         ", key_id);
			delay_ms(500);
		}
}

void process_soft_controls(void)
{
	static uint16_t old_drier_state = 0xFFFF;
	static uint8_t drier_timer_id = 0;
	
	static uint16_t old_pressure_state = 0xFFFF;
	static uint8_t pressure_timer_id = 0;
	
	static uint16_t old_sg_state = 0xFFFF;
	static uint8_t sg_timer_id = 0;

//////////////////////////////// cooler pump

	if (TEST_SOFT_CONTROL(SOFT_CONTROL_COOLER_PUMP))
		CONTROL_ON(CONTROL_COOLER_PUMP);
	else
		CONTROL_OFF(CONTROL_COOLER_PUMP);

//////////////////////////////// extruder pump
		
	if (TEST_SOFT_CONTROL(SOFT_CONTROL_EXTRUDER_PUMP))
		CONTROL_ON(CONTROL_EXTRUDER_PUMP);
	else
		CONTROL_OFF(CONTROL_EXTRUDER_PUMP);	

//////////////////////////////// germo

	if (TEST_SOFT_CONTROL(SOFT_CONTROL_GERMO))
	{
		if (TEST_SENSOR(SENSOR_GERMO_BEGIN))
		{
			if (TEST_SENSOR(SENSOR_BATH_WATER_LEVEL1))
			{
				SOFT_CONTROL_ON(SOFT_LAMP_GERMO);	// SOFT_CONTROL_GERMO == SOFT_LAMP_GERMO
				CONTROL_ON(CONTROL_GERMO_LOCK);
				CONTROL_ON(CONTROL_GERMO_CARRIAGE);
			}
			else
				SOFT_CONTROL_OFF(SOFT_CONTROL_GERMO);
		}
	}
	else
	{
		SOFT_CONTROL_OFF(SOFT_LAMP_GERMO);
		CONTROL_OFF(CONTROL_GERMO_LOCK);
		CONTROL_OFF(CONTROL_GERMO_CARRIAGE);
	}
	
//////////////////////////////// pressure (air in tube)
	
	if (old_pressure_state != TEST_SOFT_CONTROL(SOFT_CONTROL_PRESSURE))
	{
		old_pressure_state = TEST_SOFT_CONTROL(SOFT_CONTROL_PRESSURE);
		
		if (pressure_timer_id)
		{
			stop_timer(pressure_timer_id);
			pressure_timer_id = 0;
		}
		
		pressure_timer_id = start_timer(g_pnevmo_pulse_duration);
		
		CONTROL_OFF(CONTROL_PRESSURE_ON);
		CONTROL_OFF(CONTROL_PRESSURE_OFF);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_PRESSURE))
		{
			CONTROL_ON(CONTROL_PRESSURE_ON);
			SOFT_CONTROL_ON(SOFT_LAMP_GERMO);
		}
		else
		{
			CONTROL_ON(CONTROL_PRESSURE_OFF);
			SOFT_CONTROL_OFF(SOFT_LAMP_GERMO);
		}
	}
			
	if (pressure_timer_id)
		if (!timer_value(pressure_timer_id))
		{
			stop_timer(pressure_timer_id);
			pressure_timer_id = 0;
			
			CONTROL_OFF(CONTROL_PRESSURE_ON);
			CONTROL_OFF(CONTROL_PRESSURE_OFF);
		}

//////////////////////////////// sg
	
	if (old_sg_state != TEST_SOFT_CONTROL(SOFT_CONTROL_SG))
	{
		old_sg_state = TEST_SOFT_CONTROL(SOFT_CONTROL_SG);
		
		if (sg_timer_id)
		{
			stop_timer(sg_timer_id);
			sg_timer_id = 0;
		}
		
		sg_timer_id = start_timer(g_pnevmo_pulse_duration);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_SG))
		{
			CONTROL_ON(CONTROL_HEAD_DOWN);
			CONTROL_OFF(CONTROL_HEAD_UP);
		}
		else
		{
			CONTROL_ON(CONTROL_HEAD_UP);
			CONTROL_OFF(CONTROL_HEAD_DOWN);
		}
	}
			
	if (sg_timer_id)
		if (!timer_value(sg_timer_id))
		{
			stop_timer(sg_timer_id);
			sg_timer_id = 0;
			
			CONTROL_OFF(CONTROL_HEAD_DOWN);
			CONTROL_OFF(CONTROL_HEAD_UP);
		}
		
//////////////////////////////// drier
	
	if (old_drier_state != TEST_SOFT_CONTROL(SOFT_CONTROL_DRIER))
	{
		old_drier_state = TEST_SOFT_CONTROL(SOFT_CONTROL_DRIER);
		
		if (drier_timer_id)
		{
			stop_timer(drier_timer_id);
			drier_timer_id = 0;
		}
		
		drier_timer_id = start_timer(g_pnevmo_pulse_duration);
		
		CONTROL_OFF(CONTROL_DRIER_ON);
		CONTROL_OFF(CONTROL_DRIER_OFF);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_DRIER))
			CONTROL_ON(CONTROL_DRIER_ON);
		else
			CONTROL_ON(CONTROL_DRIER_OFF);
	}
			
	if (drier_timer_id)
		if (!timer_value(drier_timer_id))
		{
			stop_timer(drier_timer_id);
			drier_timer_id = 0;
			
			CONTROL_OFF(CONTROL_DRIER_ON);
			CONTROL_OFF(CONTROL_DRIER_OFF);
		}

////////////////////////////// cooler tube

	if (TEST_SOFT_CONTROL(SOFT_CONTROL_COOLER_TUBE))
		CONTROL_ON(CONTROL_COOLER_TUBE);
	else
		CONTROL_OFF(CONTROL_COOLER_TUBE);
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void process_sensors(void)
{
	static uint8_t	pressure_state = 0;
	
	static uint16_t old_sensors[2] = {0, 0};
		
	uint16_t new_sensors[2];
	
	modbus_cmd_s	cmd;
	uint8_t			msg[MODBUS_MAX_MSG_LENGTH];

	if (TEST_SENSOR(BUTTON_BATH_GERMO) &&
		TEST_SENSOR(SENSOR_GERMO_BEGIN))
		SOFT_CONTROL_ON(SOFT_CONTROL_GERMO);

	if (TEST_SENSOR(SENSOR_GERMO_END))
		SOFT_CONTROL_OFF(SOFT_CONTROL_GERMO);

	if (TEST_SENSOR(BUTTON_STOP))
	{
		SOFT_CONTROL_OFF(SOFT_CONTROL_SG);
		SOFT_CONTROL_ON(SOFT_LAMP_STOP);
		SOFT_CONTROL_ON(SOFT_CONTROL_GERMO);
		if (2 == GET_WATER_MODE)
			SET_WATER_MODE(1);
	}
	
	if (!TEST_SENSOR(SENSOR_TANK_WATER_LEVEL1))
	{
		CONTROL_OFF(CONTROL_WORK_BATH_PUMP);
		CONTROL_OFF(CONTROL_FILLUP_BATH_PUMP);
		CONTROL_OFF(CONTROL_COOLER_PUMP);
		CONTROL_OFF(CONTROL_EXTRUDER_PUMP);
	}
	
	if (TEST_SENSOR(BUTTON_BATH_PRESSURE) && (0 == pressure_state))
		pressure_state = 1;
		
	if (!TEST_SENSOR(BUTTON_BATH_PRESSURE) && (1 == pressure_state))
	{
		pressure_state = 0;
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_PRESSURE))
			SOFT_CONTROL_OFF(SOFT_CONTROL_PRESSURE);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_PRESSURE);
	}
	
	// todo : process SENSOR_FOIL_ENCODER_HERE
	

	new_sensors[0] = ((uint16_t)sensors[0]) | ((uint16_t)sensors[1] << 8);
	new_sensors[1] = ((uint16_t)sensors[2]) | ((uint16_t)secondary_sensors << 8);
	
	if ((new_sensors[0] != old_sensors[0]) || (new_sensors[1] != old_sensors[1]))
	{
		old_sensors[0] = new_sensors[0];
		old_sensors[1] = new_sensors[1];

		cmd.device_id = master_device_id;
		cmd.cmd_code = MODBUS_READ;
		cmd.cmd_type = MODBUS_ACK;
		cmd.addr = 2;
		cmd.value[0] = new_sensors[0];
		cmd.value[1] = new_sensors[1];
		
		modbus_cmd2msg(&cmd, msg, MODBUS_MAX_MSG_LENGTH);
		usb_cmd(msg, 0, 0, 0);
	}
 }

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void process_secondary(void)
{
#define CMD_NOP			(0)
#define CMD_SETDATA		(1)
#define CMD_SETBITS		(2)
#define CMD_CLEARBITS	(3)

	modbus_cmd_s	cmd;
	uint8_t			msg[MODBUS_MAX_MSG_LENGTH];
	
	static uint16_t old_secondary_soft_controls = 0;
	
	static uint16_t good_count = 0;
	
	uint16_t secondary_soft_controls = soft_controls >> 8;
	
	result_e res;

	cmd.device_id = secondary_device_id;
	cmd.cmd_code = MODBUS_WRITE;
	cmd.cmd_type = MODBUS_REQ;
	cmd.addr = CMD_NOP;
	cmd.value[0] = secondary_soft_controls;
		
	if (old_secondary_soft_controls != secondary_soft_controls)
	{
		old_secondary_soft_controls = secondary_soft_controls;
		
		cmd.addr = CMD_SETDATA;
	}
	
	modbus_cmd2msg(&cmd, msg, MODBUS_MAX_MSG_LENGTH);

	res = usart1_cmd(msg, msg, MODBUS_MAX_MSG_LENGTH, 300);
	
	if (RESULT_OK == res)
	{
		res = modbus_msg2cmd(msg, &cmd);
		
		usart1_msg_ready = 0;
		
		if (RESULT_OK == res)
		{
			soft_controls &= 0x01FF;
			soft_controls |= cmd.value[0] & 0xFC00;
		
			secondary_sensors = (uint8_t)(cmd.value[0] & 0x00FF);
		
			good_count++;
//			beep_ms(5);
		}
		else
		{
			//todo: process bad ack here
			good_count = 0;
			usart_error_count++;
			beep_ms(20);
			usart_last_error = res;
		}
	}
	else
	{
		// todo : process secondary error

		good_count = 0;
		beep_ms(20);
		usart_error_count++;
		usart_last_error = res;
	}
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void process_water(void)
{
	static uint8_t old_water_mode = 0;
	static uint8_t water_action = WATER_ACTION_NOP;
	
	static uint8_t old_empty_bath_state = 1;
	static uint8_t empty_bath_timer_id = 0;
	static uint8_t level_reached = 0;

	uint8_t water_mode = GET_WATER_MODE;
	
	if (old_water_mode != water_mode)
	{
		old_water_mode = water_mode;
		
		switch (water_mode)
		{
		case WATER_MODE_EMPTY:
			water_action = WATER_ACTION_EMPTY;
			break;
			
		case WATER_MODE_MANUAL:
			water_action = WATER_ACTION_MANUAL;
			break;
			
		case WATER_MODE_LEVEL0:
			if (TEST_SENSOR(SENSOR_BATH_WATER_LEVEL0))
				water_action = WATER_ACTION_EMPTY;
			else
				water_action = WATER_ACTION_FILLUP;
			break;
		
		case WATER_MODE_LEVEL1:
			if (TEST_SENSOR(SENSOR_BATH_WATER_LEVEL1))
				water_action = WATER_ACTION_EMPTY;
			else
				water_action = WATER_ACTION_FILLUP;		
			break;
		}
		
		level_reached = 0;
	}	
		
	switch (water_mode)
	{
	case WATER_MODE_EMPTY:
		water_action = WATER_ACTION_EMPTY;
		break;
		
	case WATER_MODE_MANUAL:
		water_action = WATER_ACTION_MANUAL;
		break;
			
	case WATER_MODE_LEVEL0:
		if (!TEST_SENSOR(SENSOR_BATH_WATER_LEVEL0))
			water_action = WATER_ACTION_FILLUP;

		if (TEST_SENSOR(SENSOR_BATH_WATER_LEVEL1))
			water_action = WATER_ACTION_EMPTY;
	
		if ((WATER_ACTION_FILLUP == water_action) && TEST_SENSOR(SENSOR_BATH_WATER_LEVEL0))
		{
			water_action = WATER_ACTION_NOP;
			level_reached = 1;
		}

		break;
			
	case WATER_MODE_LEVEL1:
		if (!TEST_SENSOR(SENSOR_BATH_WATER_LEVEL1))
			water_action = WATER_ACTION_FILLUP;

		if ((WATER_ACTION_FILLUP == water_action) && TEST_SENSOR(SENSOR_BATH_WATER_LEVEL1))
		{
			water_action = WATER_ACTION_NOP;
			level_reached = 1;
		}
				
		break;
	}
		
	switch (water_action)
	{
	case WATER_ACTION_NOP:
		CONTROL_OFF(CONTROL_FILLUP_BATH_PUMP);
		CONTROL_OFF(CONTROL_WORK_BATH_PUMP);
		g_empty_bath_state = 0;
		break;

	case WATER_ACTION_FILLUP:
		if (level_reached)
			CONTROL_OFF(CONTROL_FILLUP_BATH_PUMP);
		else
			if (TEST_SENSOR(SENSOR_TANK_WATER_LEVEL1))
				CONTROL_ON(CONTROL_FILLUP_BATH_PUMP);

		if (TEST_SENSOR(SENSOR_TANK_WATER_LEVEL1))
			CONTROL_ON(CONTROL_WORK_BATH_PUMP);
		
		g_empty_bath_state = 0;
		break;

	case WATER_ACTION_EMPTY:
		CONTROL_OFF(CONTROL_FILLUP_BATH_PUMP);
		CONTROL_OFF(CONTROL_WORK_BATH_PUMP);
		g_empty_bath_state = 1;
		break;
			
	case WATER_ACTION_MANUAL:
		break;
	}

	if (old_empty_bath_state != g_empty_bath_state)
	{
		old_empty_bath_state = g_empty_bath_state;
		
		if (empty_bath_timer_id)
		{
			stop_timer(empty_bath_timer_id);
			empty_bath_timer_id = 0;
		}
		
		empty_bath_timer_id = start_timer(g_pnevmo_pulse_duration);
		
		CONTROL_OFF(CONTROL_EMPTY_BATH_ON);
		CONTROL_OFF(CONTROL_EMPTY_BATH_OFF);
		
		if (g_empty_bath_state)
			CONTROL_ON(CONTROL_EMPTY_BATH_ON);
		else
			CONTROL_ON(CONTROL_EMPTY_BATH_OFF);
	}
			
	if (empty_bath_timer_id)
		if (!timer_value(empty_bath_timer_id))
		{
			stop_timer(empty_bath_timer_id);
			empty_bath_timer_id = 0;
			
			CONTROL_OFF(CONTROL_EMPTY_BATH_ON);
			CONTROL_OFF(CONTROL_EMPTY_BATH_OFF);
		}
	
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void process_usb(void)
{
	modbus_cmd_s	cmd;
	uint8_t			msg[MODBUS_MAX_MSG_LENGTH];
	
	usb_getmsg();

	if (!usb_msg_ready)
		return;
		
	usb_msg_ready = 0;
	
	if (RESULT_OK != modbus_msg2cmd(usb_inbuf, &cmd))
		return;
		
	if (master_device_id != cmd.device_id)
		return;
		
	if (MODBUS_READ == cmd.cmd_code)
	{
		if (0x0000 == cmd.addr)
		{
			cmd.device_id = master_device_id;
			cmd.cmd_code = MODBUS_READ;
			cmd.cmd_type = MODBUS_ACK;
			cmd.addr = 3;
			cmd.value[0] = soft_controls;
			cmd.value[1] = ((uint16_t)sensors[0]) | ((uint16_t)sensors[1] << 8);
			cmd.value[2] = ((uint16_t)sensors[2]) | ((uint16_t)secondary_sensors << 8);

			modbus_cmd2msg(&cmd, msg, MODBUS_MAX_MSG_LENGTH);
			usb_cmd(msg, 0, 0, 0);
		}
	}
	else if(MODBUS_WRITE == cmd.cmd_code)
	{
		switch (cmd.addr)
		{
		case 0x0000:
			soft_controls = cmd.value[0];
			modbus_cmd2msg(&cmd, msg, MODBUS_MAX_MSG_LENGTH);
//			usb_cmd(msg, 0, 0, 0);
			break;
			
		case 0x0100:
			soft_controls |= cmd.value[0];
			modbus_cmd2msg(&cmd, msg, MODBUS_MAX_MSG_LENGTH);
//			usb_cmd(msg, 0, 0, 0);
			break;

		case 0x0200:
			soft_controls &= ~cmd.value[0];
			modbus_cmd2msg(&cmd, msg, MODBUS_MAX_MSG_LENGTH);
//			usb_cmd(msg, 0, 0, 0);
			break;
			
		default:	
			break;
		}
	}
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void do_encoder(void)
{
	static uint8_t old_encoder_state = 0;
	uint8_t			encoder_state;
	
	encoder_state = TEST_SENSOR(SENSOR_FOIL_ENCODER);
	
	if (old_encoder_state != encoder_state)
	{
		old_encoder_state = encoder_state;
		foil_encoder_pulse_count++;
	}
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void process_encoder_counter(void)
{
	static uint32_t old_msec = 0;
	uint32_t msec;
	uint32_t delay;
	
	msec = timer_mseconds_total;
	
	if (old_msec > msec)
		delay = 0xFFFFFFFF - old_msec + msec;
	else
		delay = msec - old_msec;
		
	old_msec = msec;
	
	// estimate delay/foil_encoder_pulse_count here
	
	foil_encoder_pulse_count = 0;
}

#define DEFAULT_ENCODER_MAX_TIME_COUNTER	(100)
#define ENCODER_MAX_PULSE_COUNTER			(10)

void new_do_encoder(void)
{
	static uint8_t time_counter = 0;
	static uint8_t pulse_counter = 0;
	static uint8_t is_running = 0;
	static uint8_t max_time_counter = DEFAULT_ENCODER_MAX_TIME_COUNTER;
	
	static uint8_t old_encoder_state = 0;
	uint8_t			encoder_state;
	
	static uint8_t	array_pos = 0;
	static uint8_t	array[ENCODER_MAX_PULSE_COUNTER];
	
	uint8_t		idx;
	uint16_t	sum;
	
	encoder_state = TEST_SENSOR(SENSOR_FOIL_ENCODER);	

	time_counter++;
	
	if (!old_encoder_state && encoder_state)
	{
		old_encoder_state = encoder_state;
		
		if (is_running)
		{
			if (pulse_counter < ENCODER_MAX_PULSE_COUNTER)
				pulse_counter++;
		
			array[array_pos] = time_counter;
			
			array_pos++;
			if (array_pos >= ENCODER_MAX_PULSE_COUNTER)
				array_pos = 0;
				
			if (ENCODER_MAX_PULSE_COUNTER <= pulse_counter)
			{
				for (idx = 0; idx < ENCODER_MAX_PULSE_COUNTER; idx++)
					sum += array[idx];
					
				max_time_counter = (uint8_t)((sum << 1) / ENCODER_MAX_PULSE_COUNTER);
			}
		}
		
		is_running = 1;
		time_counter = 0;
	}
	else if (old_encoder_state && !encoder_state)
		old_encoder_state = encoder_state;

	if (time_counter > max_time_counter)
	{
		is_running = 0;
		pulse_counter = 0;
		max_time_counter = DEFAULT_ENCODER_MAX_TIME_COUNTER;
		array_pos = 0;
		
		if (SOFT_CONTROL_ON(SOFT_CONTROL_SG))
		{
			CONTROL_ON(CONTROL_HEAD_UP);
			do_shift();
			SOFT_CONTROL_OFF(SOFT_CONTROL_SG);
		}
	}

}
