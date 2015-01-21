#include "ch.h"
#include "hal.h"
#include "core.h"

#if WATCHDOG_PRESENT

#include "WatchDog.h"
#include "iwdg.h"

inline void WatchDog_Start(uint16_t seconds)
{
	iwdgInit();
	IWDGConfig watchdog_cfg;
	watchdog_cfg.div = IWDG_DIV_256;
//	watchdog_cfg.counter = 0x0C35;
	if (seconds == 0) seconds = 10;
	seconds = (STM32_LSICLK*seconds)/256;
	chDbgCheck(seconds <= 0xFFF);
	watchdog_cfg.counter = seconds;

	iwdgStart(&IWDGD, &watchdog_cfg);
}

inline void WatchDog_Reset()
{
	iwdgReset(&IWDGD);
}

#endif
