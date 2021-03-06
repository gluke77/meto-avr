#if !defined _MENU_ITEMS_INCLUDED
#define _MENU_ITEMS_INCLUDED

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
void menu_cooler_tube(void);
void menu_foil_led(void);
void menu_bunker_motor(void);
void menu_tractor_lock(void);
void menu_siren_mode(void);

void menu_usb(void);
void menu_usart(void);

void menu_empty_bath(void);
void menu_work_pump(void);
void menu_fillup_pump(void);
void menu_debug(void);
void menu_clock(void);
void menu_version(void);

#endif /* _MENU_ITEMS_INCLUDED */