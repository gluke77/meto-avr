#if !defined _METO_INCLUDED
#define _METO_INCLUDED

#if !defined _PC
	#include <avr\io.h>
#endif /* _PC */
#include "common.h"

// SENSORS

#if defined _MASTER

#define SENSOR_BATH_WATER_LEVEL0		(0)
#define SENSOR_BATH_WATER_LEVEL1		(1)
#define SENSOR_TANK_WATER_LEVEL0		(2)
#define SENSOR_TANK_WATER_LEVEL1		(3)

#define SENSOR_HEAD_WATER		(4)
#define SENSOR_INNER_WATER		(5)		// 2x inner extruder complex
#define SENSOR_OUTER_WATER		(6)		// 3x outer extruder complex

#define SENSOR_GERMO_BEGIN		(7)
#define SENSOR_GERMO_END		(8)

#define BUTTON_BATH_PRESSURE	(9)
#define BUTTON_BATH_GERMO		(10)

#define BUTTON_STOP				(11)
#define SENSOR_FOIL_ENCODER		(12)
#define SENSOR_PRESSURE			(13)
#define SENSOR_HEAD_UP			(14)
#define SENSOR_HEAD_DOWN		(15)

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

#define	CONTROL_FILLUP_BATH_PUMP	(0)
#define CONTROL_WORK_BATH_PUMP		(1)
#define	CONTROL_COOLER_PUMP			(2)
#define CONTROL_EXTRUDER_PUMP		(3)

// PNEVMO

#define CONTROL_EMPTY_BATH_ON		(4)
#define CONTROL_EMPTY_BATH_OFF		(5)
#define CONTROL_GERMO_CARRIAGE		(6)
#define CONTROL_GERMO_LOCK			(7)
#define CONTROL_DRIER_ON			(8)
#define CONTROL_DRIER_OFF			(9)

#define CONTROL_PRESSURE_ON			(10)
#define CONTROL_PRESSURE_OFF		(11)

#define CONTROL_HEAD_UP				(12)
#define CONTROL_HEAD_DOWN			(13)

//

#define CONTROL_STOP				(14)

#define LAMP_STOP					(16) // these are on secondary

#define LAMP_BATH_PRESSURE			(17) 
#define LAMP_BATH_GERMO				(18)

#define CONTROL_TRACTOR_LOCK		(19)
#define CONTROL_BUNKER_MOTOR		(20)
#define CONTROL_FOIL_LED			(21)

#define	LAMP_RED					(22)
#define	CONTROL_SIREN				(23)

#endif /* _MASTER */

#if defined _SECONDARY

#define LAMP_STOP					(0) // these are on secondary

#define LAMP_BATH_PRESSURE			(1) 
#define LAMP_BATH_GERMO				(2)

#define CONTROL_TRACTOR_LOCK		(3)
#define CONTROL_BUNKER_MOTOR		(4)
#define CONTROL_FOIL_LED			(5)

#define	LAMP_RED					(6)
#define	CONTROL_SIREN				(7)

#endif /* _SECONDARY */

#endif /* _METO_INCLUDED */
