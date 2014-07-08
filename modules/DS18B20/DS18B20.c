#include "ch.h"
#include "hal.h"
#include "onewire.h"
#include "DS18B20.h"
#include "core.h"

volatile uint16_t out_temp[4];

volatile core_base_struct_t Core_DS18B20;

void DS18B20_Init ()
{
	Core_DS18B20.id = 1;
	Core_DS18B20.type = Temp;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
	Core_DS18B20.thread = chThdGetSelfX();
	Core_DS18B20.direction = RW;
	Core_DS18B20.next = NULL;
	Core_DS18B20.description = "4 Floor Temp Sensors DS18B20";
	Core_Base.next = &Core_DS18B20;
}

THD_WORKING_AREA(waDS18B20, 128);
//__attribute__((noreturn))
THD_FUNCTION(DS18B20,arg)
{
	(void) arg;
	thread_t *answer_thread;
	chRegSetThreadName("DS18B20");

	DS18B20_Init ();

	OW_Init();
	uint8_t buf[32];
//	OW_Scan(buf,4);

	OW_Send(OW_SEND_RESET, "\xcc\x4e\x00\x00\x3f", 5, NULL, NULL, OW_NO_READ);
	while (TRUE)
	{
//		answer_thread = chMsgWait();
//		chMsgGet(answer_thread);

		OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ);
//    for (i=0; i<1000000; i++);

		chThdSleepMilliseconds(200);

		union
		{
			uint8_t buf[2];
			uint16_t temp;
		} DS_OUT[4];

		volatile uint16_t temp_fract;

//		OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, DS_OUT.buf, 2, 2);
		OW_Send(OW_SEND_RESET, "\x55\x28\x08\x38\xbf\x03\x00\x00\xea\xbe\xff\xff", 12, DS_OUT[0].buf,2, 10);
//		temp_fract=(DS_OUT[0].buf[0]&0b00001111)*625;
		out_temp[0]=DS_OUT[0].temp>>4;

		OW_Send(OW_SEND_RESET, "\x55\x28\xd8\x2a\xbf\x03\x00\x00\xd1\xbe\xff\xff", 12, DS_OUT[1].buf,2, 10);
		out_temp[1]=DS_OUT[1].temp>>4;

		OW_Send(OW_SEND_RESET, "\x55\x28\x0b\x51\xbf\x03\x00\x00\x51\xbe\xff\xff", 12, DS_OUT[2].buf,2, 10);
		out_temp[2]=DS_OUT[2].temp>>4;

		OW_Send(OW_SEND_RESET, "\x55\x28\x97\xbb\xd5\x03\x00\x00\x71\xbe\xff\xff", 12, DS_OUT[3].buf,2, 10);
		out_temp[3]=DS_OUT[3].temp>>4;

		Core_DS18B20.current_value=(out_temp[0]+out_temp[1]+out_temp[2]+out_temp[3])/4;

//    	msg_t OutTemp =

		LEDSwap();
      	chThdSleepSeconds(3);
//    	chMsgRelease (answer_thread, (msg_t) DS_OUT.temp);
	}
}

void DS18B20_Start()
{
#if DS18B20_PRESENT
	chThdCreateStatic(waDS18B20, sizeof(waDS18B20), NORMALPRIO,
			DS18B20, NULL);
#endif
}

