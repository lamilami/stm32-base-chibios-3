#ifndef RGBW_H_
#define RGBW_H_
#include "core.h"

void RGBW_Start();

typedef struct
{
	volatile uint32_t Red_Set, Green_Set, Blue_Set, White_Set;
	volatile uint16_t Rise_Time_Sec;
	volatile uint8_t Max_Delay_Sec;
} RGBW_Inner_Val_RW;

typedef struct
{

	RGBW_Inner_Val_RW RW;

	volatile int32_t Red, Green, Blue, White;

//	volatile systime_t 	Correction_24H_Sec;
} RGBW_Inner_Val;

#endif
