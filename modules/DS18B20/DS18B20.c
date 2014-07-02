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
	OW_Send(OW_SEND_RESET, "\xcc\x4e\x00\x00\x3f", 5, NULL, NULL, OW_NO_READ);
	while (TRUE)
	{
		OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ);
//    for (i=0; i<1000000; i++);

		chThdSleepMilliseconds(200);

		union
		{
			uint8_t buf[2];
			uint16_t temp;
		} DS_OUT;
		volatile uint16_t temp_fract;

		OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, DS_OUT.buf, 2, 2);

		temp_fract=(DS_OUT.buf[0]&0b00001111)*625;

    	DS_OUT.temp=DS_OUT.temp>>4;

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

