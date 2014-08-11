#include "ch.h"
#include "hal.h"
#include "DHT11_hal.h"
#include "DHT11.h"
#include "core.h"

#include <stdlib.h>

//volatile uint16_t out_temp[4];
volatile static core_base_struct_t Core_DHT11;
volatile static struct
{
	volatile uint16_t temp;
	volatile uint16_t humidity;
	volatile uint16_t cont_errors;
	union
	{
		volatile uint16_t global_errors[2];
		volatile uint32_t global_errors_32;
	};
	union
	{
		volatile uint16_t critical_errors[2];
		volatile uint32_t critical_errors_32;
	};
} Inner_Val;

void DHT11_Init(void *arg)
{
	Core_DHT11.id = (uint32_t) arg;
	Core_DHT11.type = Humidity;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
	Core_DHT11.thread = chThdGetSelfX();
	Core_DHT11.direction = RW;
	Core_DHT11.next = NULL;
	Core_DHT11.description = "4 Floor Temp Sensors DS18B20";
	Core_DHT11.current_value = 0xffff;
	Core_DHT11.set_value = 0x68;     //Initial Floor Temp value 0x68 = 26 deg. Celsius
	Core_DHT11.inner_values = &Inner_Val;
	Core_DHT11.ival_size = sizeof(Inner_Val);
	/*	chSysLock();
	 Core_Base.next = &Core_DS18B20;
	 chSysUnlock();*/

	Core_Module_Register(&Core_DHT11);
}

THD_WORKING_AREA(waDHT11, 256);
//__attribute__((noreturn))
THD_FUNCTION(DHT11,arg)
{
	(void) arg;
//	thread_t *answer_thread;
	//	chRegSetThreadName("DS18B20");

	DHT11_Init(arg);

	/*	volatile core_base_struct_t Core_DS18B20 =
	 { 2,Temp,chThdGetSelfX(),RW,0,0,&Inner_Val,sizeof(Inner_Val),"4 Floor Temp Sensors DS18B20",NULL
	 };*/

//	Core_Module_Register (&Core_DS18B20);
	static ucnt_t global_errors = 0;
	static ucnt_t critical_errors = 0;
	static uint16_t cont_errors = 0;
//	static uint16_t old_temp = 0xffff;

	{
		//Waiting DHT11 to initialize
		chThdSleepSeconds(1);
	}

	static dht11_t DHTD1;
	static int8_t humidity, temperature;
	static EXTConfig extcfg =
	{
	{
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL } } };

	/*
	 * Activates the EXT driver 1.
	 */
	extStart(&EXTD1, &extcfg);

	DHTD1.ext_pin = 0;
	DHTD1.ext_port = GPIOA;
	DHTD1.ext_drv = &EXTD1;
	DHTD1.ext_mode = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA;
	DHTD1.refresh_period = 1000;
	dht11Init(&DHTD1);

	dht11Update(&DHTD1, NULL);

	while (TRUE)
	{
		chThdSleepSeconds(3);

		dht11GetHumidity(&DHTD1, &humidity);
		dht11GetTemperature(&DHTD1, &temperature);
		dht11Update(&DHTD1, NULL);

		chSysLock();
		Inner_Val.temp = temperature<<2;
		Inner_Val.humidity = humidity;
		Inner_Val.cont_errors = MAX(Inner_Val.cont_errors,cont_errors);
		Inner_Val.global_errors_32 = global_errors;
		Inner_Val.critical_errors_32 = critical_errors;
		chSysUnlock();
	}
}

void DHT11_Start(uint8_t id)
{
#if DHT11_PRESENT
	chThdCreateStatic(waDHT11, sizeof(waDHT11), HIGHPRIO, DHT11, (void*) (uint32_t) id);
#endif
}
