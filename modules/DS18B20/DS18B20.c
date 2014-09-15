#include "ch.h"
#include "hal.h"
#include "onewire.h"
#include "DS18B20.h"
#include "core.h"

#include <stdlib.h>

#if DS18B20_PRESENT

static thread_reference_t Update_Thread;

//volatile uint16_t out_temp[4];
static core_base_struct_t Core_DS18B20;
volatile static DS18B20_Inner_Val Inner_Val_DS18B20={};

const char* sens_addr[DS18B20_NUMBER_OF_SENSORS] =
//{ DS18B20_SENSOR_1, DS18B20_SENSOR_2, DS18B20_SENSOR_3, DS18B20_SENSOR_4 };
{ DS18B20_SENSOR_1 };

void DS18B20_Init()
{
//	Core_DS18B20.id = (uint32_t) arg;
	Core_DS18B20.type = Temp;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
//	Core_DS18B20.thread = chThdGetSelfX();
//	Core_DS18B20.direction = RW;
	Core_DS18B20.next = NULL;
	Core_DS18B20.description = "4 Floor Temp Sensors DS18B20";
//	Core_DS18B20.current_value = 0xffff;
//	Core_DS18B20.set_value = 0x68;     //Initial Floor Temp value 0x68 = 26 deg. Celsius
	Core_DS18B20.inner_values = &Inner_Val_DS18B20;
	Core_DS18B20.ival_size = sizeof(DS18B20_Inner_Val);
	Core_DS18B20.ival_rw_size = sizeof(DS18B20_Inner_Val_RW);
	/*	chSysLock();
	 Core_Base.next = &Core_DS18B20;
	 chSysUnlock();*/
	Inner_Val_DS18B20.RW.Auto_Update_Sec = 180;
	register int i;
	for (i = 0; i < DS18B20_NUMBER_OF_SENSORS; i++)
	{
		Inner_Val_DS18B20.temp[i] = 0xFFFF;
	}

	Core_Module_Register(&Core_DS18B20);
}

THD_WORKING_AREA(waDS18B20, 256);
//__attribute__((noreturn))
THD_FUNCTION(DS18B20,arg)
{
	(void) arg;
//	thread_t *answer_thread;

	static ucnt_t global_errors = 0;
	static uint16_t cont_errors = 0;
//	static uint16_t old_temp = 0xffff;

	OW_Init();
	uint8_t buf[DS18B20_NUMBER_OF_SENSORS*8];
	OW_Scan(buf,DS18B20_NUMBER_OF_SENSORS);

//	DS18B20_Init (arg,Core_DS18B20);

	while (OW_Send(OW_SEND_RESET, (uint8_t *) "\xcc\x4e\x00\x00\x3f", 5, NULL, 0, OW_NO_READ) != OW_OK)
	{
		//Waiting DS18B20 to initialize
		chThdSleepSeconds(1);
	}

	while (TRUE)
	{

		eventmask_t evt = chEvtWaitOneTimeout(ALL_EVENTS, S2ST(Inner_Val_DS18B20.RW.Auto_Update_Sec));


		while (OW_Send(OW_SEND_RESET, (uint8_t *) "\xcc\x44", 2, NULL, 0, OW_NO_READ) != OW_OK)
		{
			//Waiting DS18B20 to initialize
			chThdSleepMilliseconds(1);
			global_errors++;
		}

		chThdSleepMilliseconds(200);

		union
		{
			uint8_t buf[2];
			int16_t temp;
		} DS_OUT[DS18B20_NUMBER_OF_SENSORS];

//		sens_addr[0] = "\x55";

		register int i;
		for (i = 0; i < DS18B20_NUMBER_OF_SENSORS; i++)
		{

			if (OW_Send(OW_SEND_RESET, (uint8_t *) sens_addr[i], 12, DS_OUT[i].buf, 2, 10) == OW_OK)
			{
				//			cntr++;
				DS_OUT[i].temp = DS_OUT[i].temp >> 2;
				chSysLock();
				if ((Inner_Val_DS18B20.temp[i] != 0xffff) && (abs(DS_OUT[i].temp - Inner_Val_DS18B20.temp[i]) > (10 << 2)))
				{
					global_errors++;
					cont_errors++;
				}
				else
				{
					cont_errors = 0;
					Inner_Val_DS18B20.temp[i] = DS_OUT[i].temp;
				}
				chSysUnlock();

			}
			else
			{
//				DS_OUT[0].temp = 0xFFFF;
				global_errors++;
				cont_errors++;
			}
		}

		chSysLock();

		Inner_Val_DS18B20.cont_errors = MAX(Inner_Val_DS18B20.cont_errors, cont_errors);
		Inner_Val_DS18B20.global_errors_32 = global_errors;
//		Inner_Val_DS18B20.critical_errors_32 = critical_errors;


		if (evt == EVENTMASK_REREAD)
		{
			_core_wakeup_i(&Update_Thread, MSG_OK);
		}

		chSysUnlock();

	}
}

void DS18B20_Start()
{
#if DS18B20_PRESENT
	DS18B20_Init();
	thread_t* thd = chThdCreateStatic(waDS18B20, sizeof(waDS18B20), HIGHPRIO, DS18B20, NULL);
	Core_Register_Thread(Temp, thd, &Update_Thread);
	chThdYield();
#endif
}

#endif
