#include "ch.h"
#include "hal.h"
#include "FloorHeater.h"

THD_WORKING_AREA(waFloorHeater, 128);
//__attribute__((noreturn))
THD_FUNCTION(FloorHeater,arg)
{
	(void) arg;
	chRegSetThreadName("FloorHeater");


	while (TRUE)
	{
		chThdSleepSeconds(5);
	}
}

void FloorHeater_Start()
{
#if FloorHeater_PRESENT
	chThdCreateStatic(waFloorHeater, sizeof(waFloorHeater), NORMALPRIO,
			FloorHeater, NULL);
#endif
}
