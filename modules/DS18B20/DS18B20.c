#include "ch.h"
#include "hal.h"
#include "onewire.h"
#include "DS18B20.h"
#include "core.h"

#include <stdlib.h>

//volatile uint16_t out_temp[4];
static core_base_struct_t Core_DS18B20;
volatile static struct
{
	volatile uint16_t temp[4];
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

void DS18B20_Init(void *arg)
{
	Core_DS18B20.id = (uint32_t) arg;
	Core_DS18B20.type = Temp;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
	Core_DS18B20.thread = chThdGetSelfX();
	Core_DS18B20.direction = RW;
	Core_DS18B20.next = NULL;
	Core_DS18B20.description = "4 Floor Temp Sensors DS18B20";
	Core_DS18B20.current_value = 0xffff;
	Core_DS18B20.set_value = 0x68;     //Initial Floor Temp value 0x68 = 26 deg. Celsius
	Core_DS18B20.inner_values = &Inner_Val;
	Core_DS18B20.ival_size = sizeof(Inner_Val);
	/*	chSysLock();
	 Core_Base.next = &Core_DS18B20;
	 chSysUnlock();*/

	Core_Module_Register(&Core_DS18B20);
}

THD_WORKING_AREA(waDS18B20, 256);
//__attribute__((noreturn))
THD_FUNCTION(DS18B20,arg)
{
	(void) arg;
	thread_t *answer_thread;
	//	chRegSetThreadName("DS18B20");

	DS18B20_Init(arg);

	/*	volatile core_base_struct_t Core_DS18B20 =
	 { 2,Temp,chThdGetSelfX(),RW,0,0,&Inner_Val,sizeof(Inner_Val),"4 Floor Temp Sensors DS18B20",NULL
	 };*/

//	Core_Module_Register (&Core_DS18B20);
	static ucnt_t global_errors = 0;
	static ucnt_t critical_errors = 0;
	static uint16_t cont_errors = 0;
	static uint16_t old_temp = 0xffff;

	OW_Init();
//	uint8_t buf[32];
//	OW_Scan(buf,4);

//	DS18B20_Init (arg,Core_DS18B20);

	while (OW_Send(OW_SEND_RESET, (uint8_t *) "\xcc\x4e\x00\x00\x3f", 5, NULL, 0, OW_NO_READ) == OW_ERROR)
	{
		//Waiting DS18B20 to initialize
		chThdSleepSeconds(1);
	}

	while (TRUE)
	{
		answer_thread = chMsgWait();
		chMsgGet(answer_thread);

		while (OW_Send(OW_SEND_RESET, (uint8_t *) "\xcc\x44", 2, NULL, 0, OW_NO_READ) == OW_ERROR)
		{
			//Waiting DS18B20 to initialize
			chThdSleepMilliseconds(1);
			global_errors++;
		}
//    for (i=0; i<1000000; i++);

		chThdSleepMilliseconds(200);

		union
		{
			uint8_t buf[2];
			uint16_t temp;
		} DS_OUT[4];

//		volatile uint16_t temp_fract;

//		OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, DS_OUT.buf, 2, 2);

		uint8_t cntr;
		cntr = 0;

		if (OW_Send(OW_SEND_RESET, (uint8_t *) "\x55\x28\x08\x38\xbf\x03\x00\x00\xea\xbe\xff\xff", 12, DS_OUT[0].buf, 2, 10) == OW_OK)
		{
			cntr++;
			DS_OUT[0].temp = DS_OUT[0].temp >> 2;
		}
		else
		{
			DS_OUT[0].temp = 0;
		}

		if (OW_Send(OW_SEND_RESET, (uint8_t *) "\x55\x28\xd8\x2a\xbf\x03\x00\x00\xd1\xbe\xff\xff", 12, DS_OUT[1].buf, 2, 10) == OW_OK)
		{
			cntr++;
			DS_OUT[1].temp = DS_OUT[1].temp >> 2;
		}
		else
		{
			DS_OUT[1].temp = 0;
		}

		if (OW_Send(OW_SEND_RESET, (uint8_t *) "\x55\x28\x0b\x51\xbf\x03\x00\x00\x51\xbe\xff\xff", 12, DS_OUT[2].buf, 2, 10) == OW_OK)
		{
			cntr++;
			DS_OUT[2].temp = DS_OUT[2].temp >> 2;
		}
		else
		{
			DS_OUT[2].temp = 0;
		}

		if (OW_Send(OW_SEND_RESET, (uint8_t *) "\x55\x28\x97\xbb\xd5\x03\x00\x00\x71\xbe\xff\xff", 12, DS_OUT[3].buf, 2, 10) == OW_OK)
		{
			cntr++;
			DS_OUT[3].temp = DS_OUT[3].temp >> 2;
		}
		else
		{
			DS_OUT[3].temp = 0;
		}

		global_errors += 4 - cntr;

		chSysLock();
		Inner_Val.temp[0] = DS_OUT[0].temp;
		Inner_Val.temp[1] = DS_OUT[1].temp;
		Inner_Val.temp[2] = DS_OUT[2].temp;
		Inner_Val.temp[3] = DS_OUT[3].temp;
		chSysUnlock();

		old_temp = Core_DS18B20.current_value;

		if (cntr > 2)
		{
			chSysLock();
			Core_DS18B20.current_value = (DS_OUT[0].temp + DS_OUT[1].temp + DS_OUT[2].temp + DS_OUT[3].temp) / cntr;
			chSysUnlock();
			if ((old_temp != 0xffff) && (abs(Core_DS18B20.current_value - old_temp) > (10 << 2)))
			{
				chSysLock();
				Core_DS18B20.current_value = old_temp;
				chSysUnlock();
				critical_errors++;
				cont_errors++;
			}
			else
			{
				cont_errors = 0;
			}
		}
		else
		{
			critical_errors++;
			cont_errors++;
		}

		chSysLock();
		Inner_Val.cont_errors = MAX(Inner_Val.cont_errors,cont_errors);
		Inner_Val.global_errors_32 = global_errors;
		Inner_Val.critical_errors_32 = critical_errors;
		chSysUnlock();

//    	msg_t OutTemp =

		LEDSwap();
//      	chThdSleepSeconds(3);
		chMsgRelease(answer_thread, (msg_t) Core_DS18B20.current_value);
	}
}

void DS18B20_Start(uint8_t id)
{
#if DS18B20_PRESENT
	chThdCreateStatic(waDS18B20, sizeof(waDS18B20), HIGHPRIO, DS18B20, (void*) (uint32_t) id);
	chThdYield();
#endif
}

