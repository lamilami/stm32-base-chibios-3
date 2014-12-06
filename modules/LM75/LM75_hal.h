#ifndef LM75_HAL_H
#define LM75_HAL_H

#include <hal.h>

#ifdef STM32F100C8
static const I2CConfig lm75_i2ccfg =
{	OPMODE_I2C, 100000, STD_DUTY_CYCLE
};
#endif

#endif
