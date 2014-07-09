#ifndef FloorHeater_H_
#define FloorHeater_H_

#include "DS18B20.h"

#define FloorHeater_PRESENT TRUE

// Желаемая температура в градусах Цельсия
#define Set_TEMP	26

#if (!DS18B20_PRESENT && FloorHeater_PRESENT)
#error "Can't start FloorHeater without DS18B20 temp sensor!"
#endif

void FloorHeater_Start();

#endif
