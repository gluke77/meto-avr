#if !defined _METO_INCLUDED
#define _METO_INCLUDED

#if !defined _PC
	#include <avr\io.h>
#endif /* _PC */
#include "common.h"

// SENSORS

#if defined _MASTER

#define SENSOR_BATH_WATER_LEVEL0		(11)
#define SENSOR_BATH_WATER_LEVEL1		(9)
#define SENSOR_TANK_WATER_LEVEL0		(15)
#define SENSOR_TANK_WATER_LEVEL1		(13)

#define SENSOR_HEAD_WATER		(12)
#define SENSOR_E2_WATER			(14)	// 2x inner extruder complex
#define SENSOR_E3_WATER			(1)		// 3x outer extruder complex

#define SENSOR_GERMO_BEGIN		(8)
#define SENSOR_GERMO_END		(10)

#define BUTTON_BATH_PRESSURE	(5)
#define BUTTON_BATH_GERMO		(3)

#define BUTTON_STOP				(23)
#define SENSOR_FOIL_ENCODER		(17)
#define SENSOR_PRESSURE			(22)
#define SENSOR_HEAD_UP			(0)
#define SENSOR_HEAD_DOWN		(2)

#define SENSOR_E2P_EMPTY		(19)
#define SENSOR_E2K_EMPTY		(21)
#define SENSOR_E3M_EMPTY		(16)
#define SENSOR_E3P_EMPTY		(18)
#define SENSOR_E3K_EMPTY		(20)

#define SENSOR_END_OF_FOIL		(24)	// these are on secondary
#define SENSOR_PRI_REEL			(25)
#define SENSOR_SEC_REEL			(26)

#endif /* _MASTER */

#if defined _SECONDARY

#define SENSOR_END_OF_FOIL		(0)	// these are on secondary
#define SENSOR_PRI_REEL			(1)
#define SENSOR_SEC_REEL			(2)
#define ERROR_SENSOR_END_OF_FOIL	(3)	
#define ERROR_SENSOR_PRI_REEL		(4)
#define ERROR_SENSOR_SEC_REEL		(5)

#endif /* _SECONDARY */

// CONTROLS

#if defined _MASTER

// PUMPS

#define	CONTROL_FILLUP_BATH_PUMP	(6)
#define CONTROL_WORK_BATH_PUMP		(5)
#define	CONTROL_COOLER_PUMP			(7)
#define CONTROL_EXTRUDER_PUMP		(4)

// PNEVMO

#define CONTROL_EMPTY_BATH_ON		(12)
#define CONTROL_EMPTY_BATH_OFF		(13)
#define CONTROL_GERMO_CARRIAGE		(0)
#define CONTROL_GERMO_LOCK			(1)
#define CONTROL_DRIER_ON			(2)
#define CONTROL_DRIER_OFF			(3)

#define CONTROL_PRESSURE_ON			(10)
#define CONTROL_PRESSURE_OFF		(11)

#define CONTROL_HEAD_UP				(9)
#define CONTROL_HEAD_DOWN			(8)

//

#define CONTROL_COOLER_TUBE			(15)

// these are on secondary

#define LAMP_BATH_GERMO				(16) 
#define LAMP_BATH_PRESSURE			(17)

#define LAMP_STOP					(18)

#define CONTROL_TRACTOR_LOCK		(19)
#define CONTROL_BUNKER_MOTOR		(20)
#define CONTROL_FOIL_LED			(21)

#define	LAMP_RED					(22)
#define	CONTROL_SIREN				(23)

#endif /* _MASTER */

#if defined _SECONDARY

#define LAMP_BATH_GERMO				(0) 
#define LAMP_BATH_PRESSURE			(1)

#define LAMP_STOP					(2)

#define CONTROL_TRACTOR_LOCK		(3)
#define CONTROL_BUNKER_MOTOR		(4)
#define CONTROL_FOIL_LED			(5)

#define	LAMP_RED					(6)
#define	CONTROL_SIREN				(7)

#endif /* _SECONDARY */

#endif /* _METO_INCLUDED */
