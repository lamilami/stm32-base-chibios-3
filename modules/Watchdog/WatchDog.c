#include "ch.h"
#include "hal.h"

#if WATCHDOG_PRESENT

#include "WatchDog.h"
#include "iwdg.h"

THD_WORKING_AREA(waWatchDog, 128);
//__attribute__((noreturn))
THD_FUNCTION(WatchDog,arg)
{
	(void) arg;

	IWDGConfig watchdog_cfg;
	watchdog_cfg.counter = 0x0C35;
	watchdog_cfg.div = IWDG_DIV_128;

	iwdgStart(&IWDGD, &watchdog_cfg);

	while (TRUE)
	{
		chThdSleepSeconds(5);
		iwdgReset(&IWDGD);
	}
}

void WatchDog_Start()
{
#if WATCHDOG_PRESENT
	chThdCreateStatic(waWatchDog, sizeof(waWatchDog), HIGHPRIO, WatchDog, NULL);
	chThdYield();
#endif
}

#endif
