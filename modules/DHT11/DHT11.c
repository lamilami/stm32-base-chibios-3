#include "ch.h"
#include "hal.h"
#include "core.h"

#if DHT11_PRESENT

#include "DHT11_hal.h"
#include "DHT11.h"

#include <stdlib.h>

static thread_reference_t Update_Thread;

//volatile uint16_t out_temp[4];
static core_base_struct_t Core_DHT11;
volatile static DHT11_Inner_Val Inner_Val_DHT11;

void DHT11_Init()
{
	Core_DHT11.type = DHT11;
    Core_DHT11.Auto_Update_Sec = 180;
//	Core_DHT11.direction = RW;
	Core_DHT11.next = NULL;
	Core_DHT11.description = "DHT11 Hum&Temp Sensor";
	Core_DHT11.inner_values[0] = &Inner_Val_DHT11;
	Core_DHT11.ival_size = sizeof(DHT11_Inner_Val);
	Core_DHT11.ival_rw_size = sizeof(DHT11_Inner_Val_RW);

//	Inner_Val_DHT11.RW.Auto_Update_Sec = 180;
	Inner_Val_DHT11.cont_errors = 0;
	Inner_Val_DHT11.global_errors_32 = 0;
	Inner_Val_DHT11.humidity = 0xFFFF;
	Inner_Val_DHT11.temp = 0xFFFF;

	Core_Module_Register(&Core_DHT11);
}

THD_WORKING_AREA(waDHT11_thread, 256);
//__attribute__((noreturn))
THD_FUNCTION(DHT11_thread,arg)
{
	(void) arg;
//	thread_t *answer_thread;
	//	chRegSetThreadName("DS18B20");

	static uint32_t global_errors = 0;
	static uint16_t cont_errors = 0;
//	static uint16_t old_temp = 0xffff;

	{
		//Waiting DHT11 to initialize
		chThdSleepSeconds(1);
	}

	static int8_t humidity, temperature;

	DHTD1.ext_pin = DHT11_PIN;
	DHTD1.ext_port = GPIOA;
	DHTD1.ext_drv = &EXTD1;
	DHTD1.ext_mode = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA;
	DHTD1.refresh_period = 1000;
	dht11Init(&DHTD1);

	dht11Update(&DHTD1, NULL);

	chEvtGetAndClearEvents(ALL_EVENTS);

	while (TRUE)
	{
		eventmask_t evt = 0;
		chThdSleepSeconds(1);
		dht11GetHumidity(&DHTD1, &humidity);
		dht11GetTemperature(&DHTD1, &temperature);

		if (DHTD1.state != DHT11_READ_OK)
		{
			global_errors++;
			cont_errors++;
			humidity = 0;
			temperature = 99;
		}
		else
		{
			cont_errors = 0;
		}

		chSysLock();

		Inner_Val_DHT11.temp = temperature << 2;
		Inner_Val_DHT11.humidity = humidity;
		Inner_Val_DHT11.cont_errors = MAX(Inner_Val_DHT11.cont_errors,cont_errors);
		Inner_Val_DHT11.global_errors_32 = global_errors;

		if (evt == EVENTMASK_REREAD)
		{
			_core_wakeup_i(&Update_Thread, MSG_OK);
		}

		chSysUnlock();

		evt = chEvtWaitOneTimeout(ALL_EVENTS, S2ST(Core_DHT11.Auto_Update_Sec-1));

		dht11Update(&DHTD1, NULL);

//		if (DHT11_READ_REQUEST != dht11Update(&DHTD1, NULL))
//		{
//			global_errors++;
//			cont_errors++;
//		}
//		else
//		{
//			cont_errors = 0;
//		}
	}
}

void DHT11_Start()
{
#if DHT11_PRESENT
	DHT11_Init();
	thread_t* thd = chThdCreateStatic(waDHT11_thread, sizeof(waDHT11_thread), HIGHPRIO, DHT11_thread, NULL);
	Core_Register_Thread(DHT11, thd, &Update_Thread);
	chThdYield();
#endif
}
#endif
