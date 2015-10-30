#if !defined _MENU_ITEMS_INCLUDED
#define _MENU_ITEMS_INCLUDED

#include <avr\io.h>
#include "../common/common.h"
#include "../common/menu.h"

void menu_items_init(void);

void menu_common(void);

void menu_start(void);

void menu_sensors(void);

void menu_scan_sensors(void);

void menu_controls(void);

void menu_water_mode(void);
void menu_cooler_pump(void);
void menu_extruder_pump(void);
void menu_germo(void);
void menu_pressure(void);
void menu_sg(void);
void menu_drier(void);
void menu_stop(void);
void menu_foil_led(void);
void menu_bunker_motor(void);
void menu_tractor_lock(void);
void menu_siren_mode(void);

void menu_usb(void);
void menu_usart(void);


#endif /* _MENU_ITEMS_INCLUDED */