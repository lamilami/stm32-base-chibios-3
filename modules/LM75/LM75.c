#include "ch.h"
#include "hal.h"
#include "core.h"

#include <stdlib.h>

#include "string.h"

#if LM75_PRESENT

static thread_reference_t Update_Thread;

static core_base_struct_t Core_LM75;
volatile static LM75_Inner_Val Inner_Val_LM75 = { };

//char sens_addr[LM75_NUMBER_OF_SENSORS][12] = LM75_SENSORS;

void LM75_Init() {
//	Core_LM75.id = (uint32_t) arg;
	Core_LM75.type = LM75;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
//	Core_LM75.thread = chThdGetSelfX();
//	Core_LM75.direction = RW;
	Core_LM75.next = NULL;
	Core_LM75.description = "LM75 Temp Sensors";
//	Core_LM75.current_value = 0xffff;
//	Core_LM75.set_value = 0x68;     //Initial Floor Temp value 0x68 = 26 deg. Celsius
	Core_LM75.inner_values = &Inner_Val_LM75;
	Core_LM75.ival_size = sizeof(LM75_Inner_Val);
	Core_LM75.ival_rw_size = sizeof(LM75_Inner_Val_RW);
	/*	chSysLock();
	 Core_Base.next = &Core_LM75;
	 chSysUnlock();*/
	Inner_Val_LM75.RW.Auto_Update_Sec = 180;
	register int i;
	for (i = 0; i < LM75_NUMBER_OF_SENSORS; i++) {
		Inner_Val_LM75.temp[i] = 0xFFFF;
	}

	Core_Module_Register(&Core_LM75);
}

THD_WORKING_AREA(waLM75, 256);
//__attribute__((noreturn))
THD_FUNCTION(thdLM75,arg) {
	(void) arg;

	static ucnt_t global_errors = 0;
	static uint16_t cont_errors = 0;

//	OW_Init();

/*
	if (sens_addr[0][0] == 0) {

		char buf[LM75_NUMBER_OF_SENSORS][8];
		OW_Scan((uint8_t *) buf, LM75_NUMBER_OF_SENSORS);

		register int i;
		for (i = 0; i < LM75_NUMBER_OF_SENSORS; i++) {
			sens_addr[i][0] = 0x55;
			memcpy(((uint8_t *) sens_addr[i]) + 1, (uint8_t *) buf, 8);
			sens_addr[i][9] = 0xbe;
			sens_addr[i][10] = 0xff;
			sens_addr[i][11] = 0xff;
		}
	}
*/
//	LM75_Init (arg,Core_LM75);
/*
	while (OW_Send(OW_SEND_RESET, (uint8_t *) "\xcc\x4e\x00\x00\x3f", 5, NULL,
			0, OW_NO_READ) != OW_OK) {
		//Waiting LM75 to initialize
		chThdSleepSeconds(1);
	}
*/
	while (TRUE) {

		eventmask_t evt = chEvtWaitOneTimeout(ALL_EVENTS,
				S2ST(Inner_Val_LM75.RW.Auto_Update_Sec));

/*
		while (OW_Send(OW_SEND_RESET, (uint8_t *) "\xcc\x44", 2, NULL, 0,
				OW_NO_READ) != OW_OK) {
			//Waiting LM75 to initialize
			chThdSleepMilliseconds(1);
			global_errors++;
		}
*/
		chThdSleepMilliseconds(200);

		union {
			uint8_t buf[2];
			int16_t temp;
		} DS_OUT[LM75_NUMBER_OF_SENSORS];

//		sens_addr[0] = "\x55";

		register int i;
		for (i = 0; i < LM75_NUMBER_OF_SENSORS; i++) {
/*
			if (OW_Send(OW_SEND_RESET, (uint8_t *) sens_addr[i], 12,
					DS_OUT[i].buf, 2, 10) == OW_OK) {
				//			cntr++;
				DS_OUT[i].temp = DS_OUT[i].temp >> 2;
				chSysLock();
				if ((Inner_Val_LM75.temp[i] != -1)
						&& (abs(DS_OUT[i].temp - Inner_Val_LM75.temp[i])
								> (10 << 2))) {
					global_errors++;
					cont_errors++;
//					Inner_Val_LM75.temp[i] = -99<<2;
				} else {
					cont_errors = 0;
					Inner_Val_LM75.temp[i] = DS_OUT[i].temp;
				}
				chSysUnlock();

			} else {
//				DS_OUT[0].temp = 0xFFFF;
				global_errors++;
				cont_errors++;
			}
			*/
		}

		chSysLock();

		Inner_Val_LM75.cont_errors =
				MAX(Inner_Val_LM75.cont_errors, cont_errors);
		Inner_Val_LM75.global_errors_32 = global_errors;
//		Inner_Val_LM75.critical_errors_32 = critical_errors;

		if (evt == EVENTMASK_REREAD) {
			_core_wakeup_i(&Update_Thread, MSG_OK);
		}

		chSysUnlock();

	}
}

void LM75_Start() {
#if LM75_PRESENT
	LM75_Init();
	thread_t* thd = chThdCreateStatic(waLM75, sizeof(waLM75), HIGHPRIO,
			thdLM75, NULL);
	Core_Register_Thread(Temp, thd, &Update_Thread);
	chThdYield();
#endif
}

#endif
