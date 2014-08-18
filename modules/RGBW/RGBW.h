#ifndef RGBW_H_
#define RGBW_H_
#include "core.h"

void RGBW_Start();

typedef struct
{
	volatile uint16_t Red, Green, Blue, White;
	volatile systime_t Correction_24H;
} RGBW_Inner_Val;;

#endif
