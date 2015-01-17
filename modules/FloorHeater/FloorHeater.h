#ifndef FloorHeater_H_
#define FloorHeater_H_

#define FH_QUANTITY 2

#if FloorH_1
PWMConfig pwmcfg_fh = {1000, /* 1Khz PWM clock frequency */
                           2000, /* PWM period 2 seconds */
                           NULL, /* No callback */
                           /* Only channel 3 enabled */
                           { {PWM_OUTPUT_DISABLED, NULL}, {PWM_OUTPUT_DISABLED, NULL}, {PWM_OUTPUT_ACTIVE_HIGH, NULL}, {
                               PWM_OUTPUT_DISABLED, NULL}, },
                           0};
#else

#endif
//#define RCC_TIMERS_ENABLE_CMD   rccEnableTIM1(TRUE);
#define RCC_TIMERS_ENABLE_CMD() {                           \
    rccEnableTIM1(TRUE);                                    \
}
/*
 #define PAL_SET_MODE_CMD() {                                \
    palSetPadMode(GPIOA, GPIOA_PIN10,                       \
                  PAL_MODE_ALTERNATE(2) | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);                                   \
}
 */
#define PAL_SET_MODE_CMD() {                                \
    palSetPadMode(GPIOA, GPIOA_PIN9,                       \
                  PAL_MODE_ALTERNATE(2) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLDOWN | PAL_STM32_OTYPE_PUSHPULL);       \
    palSetPadMode(GPIOA, GPIOA_PIN10,    \
    			  PAL_MODE_ALTERNATE(2) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLDOWN | PAL_STM32_OTYPE_PUSHPULL);   \
}
#define PWM_START_CMD() {                                   \
      pwmStart(&PWMD1, &pwmcfg_fh);                            \
}

//#include "DS18B20.h"

// �������� ����������� � �������� �������
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
	int16_t iMax, iMin;
// Maximum and minimum allowable integrator state
	uint8_t iGain,     // integral gain
			pGain;     // proportional gain
	uint8_t MaxPower;

} FloorHeater_Inner_Val_RW;

typedef struct
{

	FloorHeater_Inner_Val_RW RW;

	volatile int16_t Power;
	volatile int16_t pPower;
	volatile int16_t iPower;

} FloorHeater_Inner_Val;

#endif
