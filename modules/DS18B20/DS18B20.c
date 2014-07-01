#include "ch.h"
#include "hal.h"
#include "onewire.h"
#include "DS18B20.h"

THD_WORKING_AREA(waDS18B20, 128);
//__attribute__((noreturn))
THD_FUNCTION(DS18B20,arg)
{
	(void) arg;
	chRegSetThreadName("DS18B20");

	OW_Init();
	while (TRUE)
	{
		OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ);
//    for (i=0; i<1000000; i++);

		chThdSleepMilliseconds(800);

    	uint8_t buf[2];
    	OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, buf,2, 2);

		chThdSleepSeconds(5);
	}
}

void DS18B20_Start()
{
#if DS18B20_PRESENT
	chThdCreateStatic(waDS18B20, sizeof(waDS18B20), NORMALPRIO,
			DS18B20, NULL);
#endif
}

