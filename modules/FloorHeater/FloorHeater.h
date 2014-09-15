#ifndef FloorHeater_H_
#define FloorHeater_H_

#include "DS18B20.h"

// Желаемая температура в градусах Цельсия
//#define Set_TEMP	27

void FloorHeater_Start();

//Function pointer called CallbackType that takes a float
//and returns an int
typedef uint16_t (*FloorHeater_Callback_Type)();

typedef struct
{

	volatile systime_t Auto_Update_Sec;
	volatile uint16_t Desired_Temp;
	FloorHeater_Callback_Type Get_Temp_Callback;

	int32_t iState;     // Integrator state
	int32_t iMax, iMin;
	// Maximum and minimum allowable integrator state
	int32_t iGain,     // integral gain
			pGain;     // proportional gain

} FloorHeater_Inner_Val_RW;

typedef struct
{

	FloorHeater_Inner_Val_RW RW;

	volatile int16_t Power;
	volatile int16_t pPower;
	volatile int16_t iPower;

} FloorHeater_Inner_Val;

#endif
