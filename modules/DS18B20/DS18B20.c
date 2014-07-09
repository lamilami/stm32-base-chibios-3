#include "ch.h"
#include "hal.h"
#include "onewire.h"
#include "DS18B20.h"
#include "core.h"

//volatile uint16_t out_temp[4];

volatile core_base_struct_t Core_DS18B20;
volatile static struct
{
	uint16_t	temp[4];
	uint16_t	cont_errors;
	union
	{
		uint16_t	global_errors[2];
		uint32_t	global_errors_32;
	};
	union
	{
		uint16_t	critical_errors[2];
		uint32_t	critical_errors_32;
	};
} Inner_Val;

void DS18B20_Init (void *arg)
{

	Core_DS18B20.id = (uint8_t) arg;
	Core_DS18B20.type = Temp;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
	Core_DS18B20.thread = chThdGetSelfX();
	Core_DS18B20.direction = RW;
	Core_DS18B20.next = NULL;
	Core_DS18B20.description = "4 Floor Temp Sensors DS18B20";
	Core_DS18B20.current_value=0xffff;
	Core_DS18B20.inner_values=&Inner_Val;

/*	chSysLock();
		Core_Base.next = &Core_DS18B20;
	chSysUnlock();*/

	Module_Register (Core_DS18B20);
}

THD_WORKING_AREA(waDS18B20, 128);
//__attribute__((noreturn))
THD_FUNCTION(DS18B20,arg)
{
	(void) arg;
	thread_t *answer_thread;
	chRegSetThreadName("DS18B20");

	static ucnt_t global_errors=0;
	static ucnt_t critical_errors=0;
	static uint16_t cont_errors=0;
	static uint16_t old_temp=0xffff;

	DS18B20_Init (arg);

	OW_Init();
	uint8_t buf[32];
//	OW_Scan(buf,4);

	while (OW_Send(OW_SEND_RESET, "\xcc\x4e\x00\x00\x3f", 5, NULL, NULL, OW_NO_READ)==OW_ERROR)
	{
		//Waiting DS18B20 to initialize
		chThdSleepSeconds(1);
	}

	while (TRUE)
	{
//		answer_thread = chMsgWait();
//		chMsgGet(answer_thread);

		while (OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ)==OW_ERROR)
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

		volatile uint16_t temp_fract;

//		OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, DS_OUT.buf, 2, 2);

		uint8_t cntr;
		cntr = 0;

		if (OW_Send(OW_SEND_RESET, "\x55\x28\x08\x38\xbf\x03\x00\x00\xea\xbe\xff\xff", 12, DS_OUT[0].buf,2, 10)==OW_OK)
		{
			cntr++;
			DS_OUT[0].temp=DS_OUT[0].temp>>2;
		} else {
			DS_OUT[0].temp=0;
		}


		if (OW_Send(OW_SEND_RESET, "\x55\x28\xd8\x2a\xbf\x03\x00\x00\xd1\xbe\xff\xff", 12, DS_OUT[1].buf,2, 10)==OW_OK)
		{
			cntr++;
			DS_OUT[1].temp=DS_OUT[1].temp>>2;
		} else {
			DS_OUT[1].temp=0;
		}

		if (OW_Send(OW_SEND_RESET, "\x55\x28\x0b\x51\xbf\x03\x00\x00\x51\xbe\xff\xff", 12, DS_OUT[2].buf,2, 10)==OW_OK)
		{
			cntr++;
			DS_OUT[2].temp=DS_OUT[2].temp>>2;
		} else {
			DS_OUT[2].temp=0;
		}

		if (OW_Send(OW_SEND_RESET, "\x55\x28\x97\xbb\xd5\x03\x00\x00\x71\xbe\xff\xff", 12, DS_OUT[3].buf,2, 10)==OW_OK)
		{
			cntr++;
			DS_OUT[3].temp=DS_OUT[3].temp>>2;
		} else {
			DS_OUT[3].temp=0;
		}

		global_errors+=4-cntr;

		chSysLock();
		Inner_Val.temp[0]=DS_OUT[0].temp;
		Inner_Val.temp[1]=DS_OUT[1].temp;
		Inner_Val.temp[2]=DS_OUT[2].temp;
		Inner_Val.temp[3]=DS_OUT[3].temp;
		chSysUnlock();

		old_temp = Core_DS18B20.current_value;

		if (cntr>2)
		{
			chSysLock();
			Core_DS18B20.current_value=(DS_OUT[0].temp+DS_OUT[1].temp+DS_OUT[2].temp+DS_OUT[3].temp)/cntr;
			chSysUnlock();
			if ((old_temp!=0xffff) && (abs(Core_DS18B20.current_value-old_temp)<=(10<<2)))
			{
				chSysLock();
				Core_DS18B20.current_value=old_temp;
				chSysUnlock();
				critical_errors++;
				cont_errors++;
			} else {
				cont_errors=0;
			}
		} else {
			critical_errors++;
			cont_errors++;
		}

		chSysLock();
		Inner_Val.cont_errors =MAX(Inner_Val.cont_errors,cont_errors);
		Inner_Val.global_errors_32=global_errors;
		Inner_Val.critical_errors_32=critical_errors;
		chSysUnlock();

//    	msg_t OutTemp =

		LEDSwap();
      	chThdSleepSeconds(3);
//    	chMsgRelease (answer_thread, (msg_t) DS_OUT.temp);
	}
}

void DS18B20_Start(void *arg)
{
#if DS18B20_PRESENT
	chThdCreateStatic(waDS18B20, sizeof(waDS18B20), NORMALPRIO,
			DS18B20, arg);
#endif
}

