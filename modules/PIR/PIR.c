#include "ch.h"
#include "hal.h"
#include "PIR.h"
#include "core.h"

#include <stdlib.h>

//volatile uint16_t out_temp[4];
static core_base_struct_t Core_PIR;
volatile static PIR_Inner_Val Inner_Val;

static virtual_timer_t timer;

static void pir_timer_handler(void *p)
{
	chSysLockFromISR();
	Inner_Val.PIR_activated = FALSE;
	chEvtBroadcastI(&Core_PIR.event_source);
	chSysUnlockFromISR();
}

static void pir_ext_handler(EXTDriver *extp, expchannel_t channel)
{
	//
	(void) extp;
	chSysLockFromISR();
	Inner_Val.PIR_activated = TRUE;
	chVTSetI(&timer, S2ST(Inner_Val.Delay_Seconds), pir_timer_handler, NULL);
	chEvtBroadcastI(&Core_PIR.event_source);
	chSysUnlockFromISR();
}

void PIR_Init(void *arg)
{
	Core_PIR.id = (uint32_t) arg;
	Core_PIR.type = PIR;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
	Core_PIR.thread = chThdGetSelfX();
	Core_PIR.direction = RW;
	Core_PIR.next = NULL;
	Core_PIR.description = "PIR module";
	Core_PIR.current_value = 0xffff;
	Core_PIR.set_value = 0x68;     //Initial Floor Temp value 0x68 = 26 deg. Celsius
	Core_PIR.inner_values = &Inner_Val;
	Core_PIR.ival_size = sizeof(Inner_Val);
	/*	chSysLock();
	 Core_Base.next = &Core_DS18B20;
	 chSysUnlock();*/
	Inner_Val.Delay_Seconds = 180;

	chEvtObjectInit(&Core_PIR.event_source);

	Core_Module_Register(&Core_PIR);
}

void PIR_Start(uint8_t id)
{
#if PIR_PRESENT
//	chThdCreateStatic(waPIR_thread, sizeof(waPIR_thread), NORMALPRIO, PIR_thread, (void*) (uint32_t) id);
	PIR_Init((void*) (uint32_t) id);



	palSetPad(GPIOF, GPIOF_PIN1);
	palSetPadMode(GPIOF, GPIOF_PIN1, PAL_MODE_INPUT_PULLUP);

	EXTChannelConfig extcfg;
	extcfg.mode = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF;
	extcfg.cb = pir_ext_handler;

	chSysLock();
	extSetChannelModeI(&EXTD1, GPIOF_PIN1, &extcfg);
//	extChannelDisableI(&EXTD1, GPIOF_PIN1);
//	chVTSetI(&timer, MS2ST(5), pir_timer_handler, NULL);
	extChannelEnableI(&EXTD1, GPIOF_PIN1);
	chSysUnlock();

//	chEvtBroadcast();
#endif
}
