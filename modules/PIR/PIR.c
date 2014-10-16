#include "ch.h"
#include "hal.h"
#include "PIR.h"
#include "core.h"

#if PIR_PRESENT

#include <stdlib.h>

static core_base_struct_t Core_PIR;
volatile static PIR_Inner_Val Inner_Val;

static virtual_timer_t timer;

static void pir_timer_handler(void *p)
{
	chSysLockFromISR();
	Inner_Val.PIR_activated = FALSE;
	if (Inner_Val.RW.PIR_Callback != NULL)
	{
		Inner_Val.RW.PIR_Callback();
	}
//	chEvtBroadcastI(&Core_PIR.event_source);
	chSysUnlockFromISR();
}

static void pir_ext_handler(EXTDriver *extp, expchannel_t channel)
{
	//
	(void) extp;
	chSysLockFromISR();
	if (!Inner_Val.PIR_activated)
	{
		Inner_Val.PIR_activated = TRUE;
		if (Inner_Val.RW.PIR_Callback != NULL)
		{
			Inner_Val.RW.PIR_Callback();
		}
//		chEvtBroadcastI(&Core_PIR.event_source);
	}
	chVTSetI(&timer, S2ST(Inner_Val.RW.Delay_Seconds), pir_timer_handler, NULL);
	chSysUnlockFromISR();
}

static void PIR_Init(void)
{
	Core_PIR.type = PIR;
	Core_PIR.next = NULL;
	Core_PIR.description = "PIR module";
	Core_PIR.inner_values = &Inner_Val;
	Core_PIR.ival_size = sizeof(PIR_Inner_Val);
	Core_PIR.ival_rw_size = sizeof(PIR_Inner_Val_RW);

	Inner_Val.RW.Delay_Seconds = 5;
	Inner_Val.RW.PIR_Callback = NULL;

	Core_Module_Register(&Core_PIR);
}

void PIR_Start()
{


	palSetPad(GPIOF, GPIOF_PIN1);
	palSetPadMode(GPIOF, GPIOF_PIN1, PAL_MODE_INPUT);

	EXTChannelConfig extcfg;
	extcfg.mode = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF;
	extcfg.cb = pir_ext_handler;

	chSysLock();
	extSetChannelModeI(&EXTD1, GPIOF_PIN1, &extcfg);
	extChannelEnableI(&EXTD1, GPIOF_PIN1);
	chSysUnlock();

	PIR_Init();

}
#endif
