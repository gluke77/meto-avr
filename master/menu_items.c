#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\eeprom.h>
#include <string.h>
#include <stdio.h>
#include "../common/common.h"
#include "../common/menu.h"
#include "menu_items.h"
#include "../common/timer.h"
#include "kbd.h"
#include "../common/lcd.h"
#include "sensor.h"
#include "../common/soft_controls.h"
#include "shift.h"
#include "../common/beep.h"
#include "usb.h"
#include "../common/meto.h"

void menu_items_init(void)
{
	uint8_t		idx;
	
	menu_items[MENU_MODE_START][0] = menu_start;
	
//	menu_items[MENU_MODE_SENSORS][0] = menu_sensors;
	
	menu_items[MENU_MODE_SCAN_SENSORS][0] = menu_scan_sensors;

	idx = 0; 

	menu_items[MENU_MODE_CONTROLS][idx++] = menu_controls;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_water_mode;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_cooler_pump;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_extruder_pump;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_germo;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_pressure;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_sg;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_drier;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_cooler_tube;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_foil_led;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_bunker_motor;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_tractor_lock;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_siren_mode;

	menu_items[MENU_MODE_LINK][0] = menu_usb;
	menu_items[MENU_MODE_LINK][1] = menu_usart;
	menu_items[MENU_MODE_LINK][2] = menu_empty_bath;
	menu_items[MENU_MODE_LINK][3] = menu_work_pump;
	menu_items[MENU_MODE_LINK][4] = menu_fillup_pump;
}

void menu_common(void)
{
	if (KEY_PRESSED(KEY_UP))
	{
		menu_item_prev();
		CLEAR_KEY_PRESSED(KEY_UP);
		beep_ms(50);
	}
	
	if (KEY_PRESSED(KEY_DOWN))
	{
		menu_item_next();
		CLEAR_KEY_PRESSED(KEY_DOWN);
		beep_ms(50);
	}

	if (KEY_PRESSED(KEY_MENU))
	{
		menu_mode_next();
		CLEAR_KEY_PRESSED(KEY_MENU);
		beep_ms(50);
	}
}

#define FREQ_UPPER_ADDR	(0)
#define FREQ_LOWER_ADDR	(2)
#define FREQ_ADDR		(4)
#define PWM_SHIFT_ADDR	(6)
#define	PWM_BASE_ADDR	(8)

#define ADC0_DELAY_ADDR	(9)
#define ADC0_COUNT_ADDR	(10)
#define ADC1_DELAY_ADDR	(12)
#define ADC1_COUNT_ADDR	(13)
#define ADC2_DELAY_ADDR	(15)
#define ADC2_COUNT_ADDR	(16)
#define ADC3_DELAY_ADDR	(18)
#define ADC3_COUNT_ADDR	(19)

#define PFC_MODE_ADDR	(21)
#define INT_TIMEOUT_ADDR	(22)
#define KEEP_MODE_ADDR	(24)
#define KEEP_STEP_ADDR	(25)
#define KEEP_DELTA_ADDR	(26)
#define MAX_PWM_ADDR	(27)
#define MIN_PWM_ADDR	(28)
#define BAUD_LO_ADDR	(29)
#define BAUD_HI_ADDR	(31)
#define MODBUS_ID_ADDR	(33)
#define TEMP_ALARM_ADDR	(34)
#define TEMP_STOP_ADDR	(36)
/*
void loadFromEE(void)
{
	g_freq_upper = (uint32_t)eeprom_read_word(FREQ_UPPER_ADDR);
	g_freq_lower = (uint32_t)eeprom_read_word(FREQ_LOWER_ADDR);
	dds_setfreq((uint32_t)eeprom_read_word(FREQ_ADDR));
	g_pwm_base = eeprom_read_byte(PWM_BASE_ADDR);
	g_pwm_shift = eeprom_read_byte(PWM_SHIFT_ADDR);
	
	adc_set_delay(0, eeprom_read_byte(ADC0_DELAY_ADDR));
	adc_set_count(0, eeprom_read_word(ADC0_COUNT_ADDR));
	adc_set_delay(1, eeprom_read_byte(ADC1_DELAY_ADDR));
	adc_set_count(1, eeprom_read_word(ADC1_COUNT_ADDR));	
	adc_set_delay(2, eeprom_read_byte(ADC2_DELAY_ADDR));
	adc_set_count(2, eeprom_read_word(ADC2_COUNT_ADDR));
	adc_set_delay(3, eeprom_read_byte(ADC3_DELAY_ADDR));
	adc_set_count(3, eeprom_read_word(ADC3_COUNT_ADDR));
	
	set_pfc_mode(eeprom_read_byte(PFC_MODE_ADDR));
	
	g_int_timeout = eeprom_read_word(INT_TIMEOUT_ADDR);
	g_keep_mode = eeprom_read_byte(KEEP_MODE_ADDR);
	g_keep_freq_step = eeprom_read_byte(KEEP_STEP_ADDR);
	g_keep_freq_max_delta = eeprom_read_byte(KEEP_DELTA_ADDR);
	g_max_pwm = eeprom_read_byte(MAX_PWM_ADDR);
	g_min_pwm = eeprom_read_byte(MIN_PWM_ADDR);
	
	g_baudrate = (uint32_t)eeprom_read_word(BAUD_LO_ADDR);
	g_baudrate |= ((uint32_t)(eeprom_read_word(BAUD_HI_ADDR))) << 16;
	
	g_modbus_id = eeprom_read_byte(MODBUS_ID_ADDR);
	
	g_temp_alarm = eeprom_read_word(TEMP_ALARM_ADDR);
	g_temp_stop = eeprom_read_word(TEMP_STOP_ADDR);
}

void storeToEE(void)
{
	eeprom_write_word(FREQ_UPPER_ADDR, (uint16_t)g_freq_upper);
	eeprom_write_word(FREQ_LOWER_ADDR, (uint16_t)g_freq_lower);
	eeprom_write_word(FREQ_ADDR, (uint16_t)g_dds_freq);
	eeprom_write_byte(PWM_BASE_ADDR, g_pwm_base);
	eeprom_write_byte(PWM_SHIFT_ADDR, g_pwm_shift);
	
	eeprom_write_byte(ADC0_DELAY_ADDR, adc_get_delay(0));
	eeprom_write_word(ADC0_COUNT_ADDR, adc_get_count(0));
	eeprom_write_byte(ADC1_DELAY_ADDR, adc_get_delay(1));
	eeprom_write_word(ADC1_COUNT_ADDR, adc_get_count(1));
	eeprom_write_byte(ADC2_DELAY_ADDR, adc_get_delay(2));
	eeprom_write_word(ADC2_COUNT_ADDR, adc_get_count(2));
	eeprom_write_byte(ADC3_DELAY_ADDR, adc_get_delay(3));
	eeprom_write_word(ADC3_COUNT_ADDR, adc_get_count(3));
	
	eeprom_write_byte(PFC_MODE_ADDR, g_pfc_mode);
	eeprom_write_word(INT_TIMEOUT_ADDR, g_int_timeout);
	eeprom_write_byte(KEEP_MODE_ADDR, g_keep_mode);
	eeprom_write_byte(KEEP_STEP_ADDR, g_keep_freq_step);
	eeprom_write_byte(KEEP_DELTA_ADDR, g_keep_freq_max_delta);
	eeprom_write_byte(MAX_PWM_ADDR, g_max_pwm);
	eeprom_write_byte(MIN_PWM_ADDR, g_min_pwm);
	eeprom_write_word(BAUD_LO_ADDR, (uint16_t)(g_baudrate & 0x0000FFFF));
	eeprom_write_word(BAUD_HI_ADDR, (uint16_t)(g_baudrate >> 16));
	
	eeprom_write_byte(MODBUS_ID_ADDR, g_modbus_id);
	eeprom_write_word(TEMP_ALARM_ADDR, g_temp_alarm);
	eeprom_write_word(TEMP_STOP_ADDR, g_temp_stop);
}

void reset_settings(void)
{
	g_freq_upper = 21900;
	g_freq_lower = 21300;
	dds_setfreq(21450);

	g_pwm_base = 77;
	g_pwm_shift = 0;
	
	g_int_timeout = 50;
	g_keep_mode = 1;
	
	g_keep_freq_step = 1;
	g_keep_freq_max_delta = 5;
	
	g_max_pwm = 255;
	g_min_pwm = 0;
	
	g_baudrate = 115200;
	g_modbus_id = 1;
	
	g_temp_alarm = 75;
	g_temp_stop = 80;
	
	adc_set_delay(0, 1);
	adc_set_count(0, 800);

	adc_set_delay(1, 1);
	adc_set_count(1, 128);

	adc_set_delay(2, 1);
	adc_set_count(2, 800);

	adc_set_delay(3, 1);
	adc_set_count(3, 400);
	
	set_pfc_mode(PFC_AUTO);
}

void menu_modbus_id(void)
{
	sprintf(lcd_line1, "ÀÄÐÅÑ=%-14d", g_modbus_id);

	if (KEY_PRESSED(KEY_RIGHT))
	{
		g_modbus_id++;
			
		CLEAR_KEY_PRESSED(KEY_RIGHT);
	}

	if (KEY_PRESSED(KEY_LEFT))
	{
		g_modbus_id--;
		
		CLEAR_KEY_PRESSED(KEY_LEFT);
	}

	menu_common();
}

uint32_t	baud[] = {2400, 4800, 9600, 19200, 38400, 57600, 115200};

void menu_baudrate(void)
{
	uint8_t		idx;
	
	for (idx = 0; idx < 7; idx++)
		if (g_baudrate == baud[idx])
			break;
	
	sprintf(lcd_line1, "ÑÊÎÐÎÑÒÜ=%-11ld", g_baudrate);

	if (KEY_PRESSED(KEY_RIGHT))
	{
		idx++;
		
		if (7 == idx)
			idx = 0;
			
		CLEAR_KEY_PRESSED(KEY_RIGHT);
	}

	if (KEY_PRESSED(KEY_LEFT))
	{
		if (0 == idx)
			idx = 7;
			
		idx--;
		
		CLEAR_KEY_PRESSED(KEY_LEFT);
	}
	
	g_baudrate = baud[idx];

	menu_common();
}

*/

extern uint8_t secondary_sensors;

void menu_start(void)
{
	sprintf(lcd_line0, "ÌÅÒÀËËÎÏÎËÈÌÅÐ-4");
	sprintf(lcd_line1, "ÌÏ123-2006      ");
	
	menu_common();
}

void menu_sensors(void)
{
	uint8_t	idx;
	
	sprintf(lcd_line0, "ÂÕÎÄÛ           ");
	
	for (idx = 0; idx < 3; idx++)
		lcd_line0[13 + idx] = (TESTBIT(secondary_sensors, idx))?'1':'.';
		
	for (idx = 0; idx < 16; idx++)
		lcd_line1[idx] = (TEST_SENSOR(idx))?'1':'.';

	menu_common();
}

char * sensor_text[][3] =
	{
		{"ÂÎÄÀ Â ÂÀÍÍÎÉ-0  ", "ÄÀ", "ÍÅÒ"},
		{"ÂÎÄÀ Â ÂÀÍÍÎÉ-1  ", "ÄÀ", "ÍÅÒ"},
		{"ÂÎÄÀ Â ÁÀÊÅ-0    ", "ÄÀ", "ÍÅÒ"},
		{"ÂÎÄÀ Â ÁÀÊÅ-1    ", "ÄÀ", "ÍÅÒ"},
		{"ÎÕËÀÆÄÅÍÈÅ ÑÃ    ", "ÄÀ", "ÍÅÒ"},
		{"ÎÕËÀÆÄÅÍÈÅ Ý2    ", "ÄÀ", "ÍÅÒ"},
		{"ÎÕËÀÆÄÅÍÈÅ Ý3    ", "ÄÀ", "ÍÅÒ"},
		{"ÍÀ×ÀËÎ ÃÅÐÌÅÒÈÇ. ", "ÄÀ", "ÍÅÒ"},
		{"ÊÎÍÅÖ ÃÅÐÌÅÒÈÇ.  ", "ÄÀ", "ÍÅÒ"},
		{"ÊÍÎÏÊÀ ÄÀÂËÅÍÈß  ", "ÄÀ", "ÍÅÒ"},
		{"ÊÍÎÏÊÀ ÃÅÐÌÅÒÈÇ. ", "ÄÀ", "ÍÅÒ"},
		{"ÊÍÎÏÊÀ ÑÒÎÏ      ", "ÄÀ", "ÍÅÒ"},
		{"ÝÍÊÎÄÅÐ          ", "ÄÀ", "ÍÅÒ"},
		{"ÂÎÇÄÓÕ Â ÑÈÑÒÅÌÅ ", "ÄÀ", "ÍÅÒ"},
		{"ÑÃ ÏÎÄÍßÒÀ       ", "ÄÀ", "ÍÅÒ"},
		{"ÑÃ ÎÏÓÙÅÍÀ       ", "ÄÀ", "ÍÅÒ"},
		{"ÏÇÏ Ý2           ", "ÄÀ", "ÍÅÒ"},
		{"ÏÇÊ Ý2           ", "ÄÀ", "ÍÅÒ"},
		{"ÏÇÏ Ý3           ", "ÄÀ", "ÍÅÒ"},
		{"ÏÇÊ Ý3           ", "ÄÀ", "ÍÅÒ"},
		{"ÏÇÌ Ý3           ", "ÄÀ", "ÍÅÒ"},
		{"ÊÎÍÅÖ ÔÎËÜÃÈ     ", "ÄÀ", "ÍÅÒ"},
		{"ÇÀÌÅÍÀ ÁÎÁÈÍÛ    ", "ÄÀ", "ÍÅÒ"},
		{"ÑÌÎÒÊÀ ÔÎËÜÃÈ    ", "ÄÀ", "ÍÅÒ"}
	};

#define SENSOR_COUNT	(24)

uint8_t	sensor_id[] =
	{
		SENSOR_BATH_WATER_LEVEL0,
		SENSOR_BATH_WATER_LEVEL1,
		SENSOR_TANK_WATER_LEVEL0,
		SENSOR_TANK_WATER_LEVEL1,
		SENSOR_HEAD_WATER,
		SENSOR_E2_WATER,
		SENSOR_E3_WATER,
		SENSOR_GERMO_BEGIN,
		SENSOR_GERMO_END,
		BUTTON_BATH_PRESSURE,
		BUTTON_BATH_GERMO,
		BUTTON_STOP,
		SENSOR_FOIL_ENCODER,
		SENSOR_PRESSURE,
		SENSOR_HEAD_UP,
		SENSOR_HEAD_DOWN,
		SENSOR_E2P_EMPTY,
		SENSOR_E2K_EMPTY,
		SENSOR_E3P_EMPTY,
		SENSOR_E3K_EMPTY,
		SENSOR_E3M_EMPTY,
		SENSOR_END_OF_FOIL,
		SENSOR_PRI_REEL,
		SENSOR_SEC_REEL
	};

extern uint8_t		g_encoder_on;
extern uint32_t	g_pl_state;

void menu_scan_sensors(void)
{
	static uint8_t scan_mode_on = 1;
	static uint8_t sensor = 0;
	static uint8_t timer_id = 0;
	
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		scan_mode_on ^= 0x01;
		beep_ms(50);
	}
	
	if ((scan_mode_on) && (0 == timer_id))
			timer_id = start_timer(1000);


	if (0 != timer_id)
	{
		if (!timer_value(timer_id))
		{
			stop_timer(timer_id);
			timer_id = 0;
			
			if (scan_mode_on)
				sensor++;
			
			if (SENSOR_COUNT <= sensor)
				sensor = 0;
		}
	}

	if (!scan_mode_on)
	{
		if (KEY_PRESSED(KEY_UP))
		{
			CLEAR_KEY_PRESSED(KEY_UP);
			
			if (0 == sensor)
				sensor = SENSOR_COUNT;
				
			sensor--;
		}
		
		if (KEY_PRESSED(KEY_DOWN))
		{
			CLEAR_KEY_PRESSED(KEY_DOWN);
			
			sensor++;
			
			if (SENSOR_COUNT <= sensor)
				sensor = 0;
		}
	}
	
	sprintf(lcd_line0, "%s               ", sensor_text[sensor][0]);
		
	if (SENSOR_FOIL_ENCODER == sensor_id[sensor])
		sprintf(lcd_line1, "%s               ",
			(g_encoder_on)?sensor_text[sensor][1]:sensor_text[sensor][2]);
			
	else if ((SENSOR_E2P_EMPTY == sensor_id[sensor]) ||
			(SENSOR_E2K_EMPTY == sensor_id[sensor]) ||
			(SENSOR_E3P_EMPTY == sensor_id[sensor]) ||
			(SENSOR_E3K_EMPTY == sensor_id[sensor]) ||
			(SENSOR_E3M_EMPTY == sensor_id[sensor]))
	
		sprintf(lcd_line1, "%s                ",
			(TESTBITL(g_pl_state, sensor_id[sensor]))?sensor_text[sensor][1]:sensor_text[sensor][2]);
	
	else if (sensor_id[sensor] < 24)
		sprintf(lcd_line1, "%s               ",
			(TEST_SENSOR(sensor_id[sensor]))?sensor_text[sensor][1]:sensor_text[sensor][2]);
	
	else 
		sprintf(lcd_line1, "%s               ",
			(TESTBIT(secondary_sensors, sensor_id[sensor] - 24))?sensor_text[sensor][1]:sensor_text[sensor][2]);

	menu_common();
}

void menu_controls(void)
{
	sprintf(lcd_line0,"ÂÛÕÎÄÛ          ");
	
	lcd_line1[16] = 0;
	
	lcd_line1[SOFT_CONTROL_WATER_MODE0] = '0' + GET_WATER_MODE;
	lcd_line1[SOFT_CONTROL_WATER_MODE1] = ' ';
	
	lcd_line1[SOFT_CONTROL_COOLER_PUMP] = (TEST_SOFT_CONTROL(SOFT_CONTROL_COOLER_PUMP))?'Õ':'.';
	lcd_line1[SOFT_CONTROL_EXTRUDER_PUMP] = (TEST_SOFT_CONTROL(SOFT_CONTROL_EXTRUDER_PUMP))?'Ý':'.';
	lcd_line1[SOFT_CONTROL_COOLER_TUBE] = (TEST_SOFT_CONTROL(SOFT_CONTROL_COOLER_TUBE))?'Ï':'.';
	lcd_line1[SOFT_CONTROL_PRESSURE] = (TEST_SOFT_CONTROL(SOFT_CONTROL_PRESSURE))?'Â':'.';
	lcd_line1[SOFT_CONTROL_SG] = (TEST_SOFT_CONTROL(SOFT_CONTROL_SG))?'Ñ':'.';
	lcd_line1[SOFT_CONTROL_DRIER] = (TEST_SOFT_CONTROL(SOFT_CONTROL_DRIER))?'Î':'.';
	lcd_line1[SOFT_CONTROL_GERMO] = (TEST_SOFT_CONTROL(SOFT_CONTROL_GERMO))?'Ã':'.';
	
	lcd_line1[SOFT_LAMP_STOP - 1] = (TEST_SOFT_CONTROL(SOFT_LAMP_STOP))?'S':'.';
	lcd_line1[SOFT_CONTROL_FOIL_LED - 1] = (TEST_SOFT_CONTROL(SOFT_CONTROL_FOIL_LED))?'Ë':'.';
	lcd_line1[SOFT_CONTROL_BUNKER_MOTOR - 1] = (TEST_SOFT_CONTROL(SOFT_CONTROL_BUNKER_MOTOR))?'Á':'.';
	lcd_line1[SOFT_CONTROL_TRACTOR_LOCK - 1] = (TEST_SOFT_CONTROL(SOFT_CONTROL_TRACTOR_LOCK))?'Ò':'.';
	
	
	lcd_line1[SOFT_SIREN_MODE0 - 1] = ' ';
	lcd_line1[SOFT_SIREN_MODE1 - 1] = '0' + GET_SIREN_MODE;
	
	lcd_line1[15] = ' ';
	
	menu_common();
}


void menu_water_mode(void)
{	
	uint16_t water_mode;
	
	sprintf(lcd_line0,"ÓÐÎÂÅÍÜ ÂÎÄÛ    ");
	sprintf(lcd_line1,"%d                ", GET_WATER_MODE);
	
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		water_mode = GET_WATER_MODE;
		water_mode++;
		if (2 < water_mode)
			water_mode = 0;
		SET_WATER_MODE(water_mode);
		beep_ms(50);
	}
	
	menu_common();
}

void menu_cooler_pump(void)
{	
	sprintf(lcd_line0,"ÍÀÑÎÑ ÕÎËÎÄ.ÓÑÒ.");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_COOLER_PUMP))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_COOLER_PUMP))
			SOFT_CONTROL_OFF(SOFT_CONTROL_COOLER_PUMP);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_COOLER_PUMP);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_extruder_pump(void)
{	
	sprintf(lcd_line0,"ÍÀÑÎÑ ÎÕËÀÆÄÅÍÈß");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_EXTRUDER_PUMP))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_EXTRUDER_PUMP))
			SOFT_CONTROL_OFF(SOFT_CONTROL_EXTRUDER_PUMP);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_EXTRUDER_PUMP);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_germo(void)
{	
	sprintf(lcd_line0,"ÃÅÐÌÅÒÈÇÀÖÈß    ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_GERMO))?"ÂÊËÞ×ÅÍÀ":"ÂÛÊËÞ×ÅÍÀ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_GERMO))
			SOFT_CONTROL_OFF(SOFT_CONTROL_GERMO);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_GERMO);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_pressure(void)
{	
	sprintf(lcd_line0,"ÂÎÇÄÓÕ Â ÒÐÓÁÅ  ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_PRESSURE))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_PRESSURE))
			SOFT_CONTROL_OFF(SOFT_CONTROL_PRESSURE);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_PRESSURE);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_sg(void)
{	
	sprintf(lcd_line0,"ÑÂÀÐ. ÃÎËÎÂÊÀ.  ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_SG))?"ÏÐÈÆÀÒÀ":"ÏÎÄÍßÒÀ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_SG))
			SOFT_CONTROL_OFF(SOFT_CONTROL_SG);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_SG);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_drier(void)
{	
	sprintf(lcd_line0,"ÎÑÓØÈÒÅËÜ       ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_DRIER))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_DRIER))
			SOFT_CONTROL_OFF(SOFT_CONTROL_DRIER);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_DRIER);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_cooler_tube(void)
{	
	sprintf(lcd_line0,"ÏÐÅÄÂ.ÎÕËÀÆÄÅÍÈÅ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_COOLER_TUBE))?"ÂÊËÞ×ÅÍÎ":"ÂÛÊËÞ×ÅÍÎ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_COOLER_TUBE))
			SOFT_CONTROL_OFF(SOFT_CONTROL_COOLER_TUBE);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_COOLER_TUBE);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_foil_led(void)
{	
	sprintf(lcd_line0,"ÊÎÍÒÐÎËÜ ÔÎËÜÃÈ ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_FOIL_LED))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_FOIL_LED))
			SOFT_CONTROL_OFF(SOFT_CONTROL_FOIL_LED);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_FOIL_LED);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_bunker_motor(void)
{	
	sprintf(lcd_line0,"ÄÂÈÃ. ÍÀÊÎÏÈÒÅËß");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_BUNKER_MOTOR))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_BUNKER_MOTOR))
			SOFT_CONTROL_OFF(SOFT_CONTROL_BUNKER_MOTOR);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_BUNKER_MOTOR);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_tractor_lock(void)
{	
	sprintf(lcd_line0,"ÏÐÈÆÈÌ ÒÓ       ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_TRACTOR_LOCK))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_TRACTOR_LOCK))
			SOFT_CONTROL_OFF(SOFT_CONTROL_TRACTOR_LOCK);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_TRACTOR_LOCK);
			
		beep_ms(50);
	}

	
	menu_common();
}

void menu_siren_mode(void)
{	
	uint16_t siren_mode;
	
	sprintf(lcd_line0,"ÐÅÆÈÌ ÑÈÐÅÍÛ    ");
	sprintf(lcd_line1,"%d                ", GET_SIREN_MODE);
	
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		siren_mode = GET_SIREN_MODE;
		siren_mode++;
		if (3 < siren_mode)
			siren_mode = 0;
		SET_SIREN_MODE(siren_mode);
		
		beep_ms(50);
	}
	
	menu_common();
}


void menu_usb(void)
{
	sprintf(lcd_line0,"ÑÂßÇÜ Ñ ÊÎÌ.ÏÀÍ.");

	sprintf(lcd_line1,"%s                ",
		(usb_on)?"ÅÑÒÜ":"ÍÅÒ");
		
	if (!usb_on)
		if (KEY_PRESSED(KEY_ENTER))
		{
			CLEAR_KEY_PRESSED(KEY_ENTER);
			
			usb_on = 1;
			
			beep_ms(50);
		}
		
	menu_common();
}

extern uint16_t usart_error_count;
extern uint8_t usart_last_error;

void menu_usart(void)
{
	sprintf(lcd_line0,"ÎØÈÁÊÈ ÂÅÄÎÌÎÃÎ ");
	sprintf(lcd_line1,"%d %d               ", usart_error_count, usart_last_error);
		
	menu_common();
}

extern uint8_t g_empty_bath_state;

void menu_empty_bath(void)
{
	sprintf(lcd_line0,"ÑËÈÂ ÂÀÍÍÛ      ");
	sprintf(lcd_line1,"%s                ", 
		g_empty_bath_state?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		g_empty_bath_state ^= 0x01;
		SET_WATER_MODE(WATER_MODE_MANUAL);
	
		beep_ms(50);
	}
	
	menu_common();
}

void menu_work_pump(void)
{
	sprintf(lcd_line0,"ÍÀÑÎÑ ÖÈÐÊÓËßÖÈÈ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_CONTROL(CONTROL_WORK_BATH_PUMP))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_CONTROL(CONTROL_WORK_BATH_PUMP))
			CONTROL_OFF(CONTROL_WORK_BATH_PUMP);
		else
			CONTROL_ON(CONTROL_WORK_BATH_PUMP);
			
		SET_WATER_MODE(WATER_MODE_MANUAL);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_fillup_pump(void)
{
	sprintf(lcd_line0,"ÍÀÑÎÑ ÍÀÏÎËÍÅÍÈß");
	sprintf(lcd_line1,"%s                ", 
		(TEST_CONTROL(CONTROL_FILLUP_BATH_PUMP))?"ÂÊËÞ×ÅÍ":"ÂÛÊËÞ×ÅÍ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_CONTROL(CONTROL_FILLUP_BATH_PUMP))
			CONTROL_OFF(CONTROL_FILLUP_BATH_PUMP);
		else
			CONTROL_ON(CONTROL_FILLUP_BATH_PUMP);
			
		SET_WATER_MODE(WATER_MODE_MANUAL);
			
		beep_ms(50);
	}
	
	menu_common();
}

