#ifndef FloorHeater_H_
#define FloorHeater_H_

#include "DS18B20.h"

// Желаемая температура в градусах Цельсия
#define Set_TEMP	27

void FloorHeater_Start();

volatile core_base_struct_t Core_FloorHeater;

#endif
