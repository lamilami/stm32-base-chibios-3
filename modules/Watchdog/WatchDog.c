#include "ch.h"
#include "hal.h"
#include "core.h"

#if WATCHDOG_PRESENT

#include "WatchDog.h"
#include "iwdg.h"

static thread_t *wtdg;
static eventmask_t eventmask = 0;
//static event_source_t es[Core_Type_MAX];
//static event_listener_t el[Core_Type_MAX];

inline void WatchDog_Start(thread_t *tp, uint16_t seconds) {
  wtdg = tp;
  iwdgInit();
  IWDGConfig watchdog_cfg;
  watchdog_cfg.div = IWDG_DIV_256;
//	watchdog_cfg.counter = 0x0C35;
  if (seconds == 0)
    seconds = 10;
  seconds = (STM32_LSICLK * seconds) / 256;
  chDbgCheck(seconds <= 0xFFF);
  watchdog_cfg.counter = seconds;

  iwdgStart(&IWDGD, &watchdog_cfg);
}

inline void WatchDog_Reset() {
  chEvtWaitAll(eventmask);
  iwdgReset(&IWDGD);
}

void WatchDog_Notify(core_types_t tid) {

  chEvtSignal(wtdg, EVENT_MASK(tid));
}

void WatchDog_Register(core_types_t tid) {

  if (tid < Core_Type_MAX) {
//    chEvtInit(&es[tid]);

//    chEvtRegisterMask(&es[tid], &el[tid], EVENT_MASK( tid ));

    eventmask |= EVENT_MASK( tid );
  }
}

#endif
