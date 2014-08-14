#ifndef PIR_H_
#define PIR_H_
#include "core.h"

void PIR_Start(uint8_t id);

typedef struct
{
	volatile bool PIR_activated;
	volatile uint16_t Delay_Seconds;
} PIR_Inner_Val;;

#endif
