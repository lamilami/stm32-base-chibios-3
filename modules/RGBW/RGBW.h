#ifndef RGBW_H_
#define RGBW_H_
#include "core.h"

void RGBW_Start(uint8_t id);

typedef struct
{
	volatile uint16_t Red, Green, Blue, White;
} RGBW_Inner_Val;;

#endif
