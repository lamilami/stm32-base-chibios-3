#ifndef WatchDog_H_
#define WatchDog_H_
#include "core.h"

void WatchDog_Start(thread_t *tp, uint16_t seconds);
void WatchDog_Reset();
void WatchDog_Notify(core_types_t tid);
void WatchDog_Register(core_types_t tid);

#endif
