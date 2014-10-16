#ifndef PIR_H_
#define PIR_H_
#include "core.h"

void PIR_Start();

typedef uint16_t (*PIR_Callback_Type)();

typedef struct
{

	volatile uint16_t Delay_Seconds;
	PIR_Callback_Type PIR_Callback;

} PIR_Inner_Val_RW;

typedef struct
{

	PIR_Inner_Val_RW RW;

	volatile bool PIR_activated;

} PIR_Inner_Val;

#endif
