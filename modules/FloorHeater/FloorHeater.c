#include "ch.h"
#include "hal.h"
#include "FloorHeater.h"

static PWMConfig pwmcfg = {
  1000, /* 1Khz PWM clock frequency */
  60000, /* PWM period 1 minute */
  NULL,  /* No callback */
  /* Only channel 4 enabled */
  {
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
  },
  0
};

void PWM_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

#ifdef STM32F100C8

#else
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,	ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_2);
#endif

	  pwmStart(&PWMD1, &pwmcfg);
}

THD_WORKING_AREA(waFloorHeater, 128);
//__attribute__((noreturn))
THD_FUNCTION(FloorHeater,arg)
{
	(void) arg;
	chRegSetThreadName("FloorHeater");

	PWM_Init();
	pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));   // 10% duty cycle
	while (TRUE)
	{
		chThdSleepSeconds(5);
	}
}

void FloorHeater_Start()
{
#if FloorHeater_PRESENT
	chThdCreateStatic(waFloorHeater, sizeof(waFloorHeater), NORMALPRIO,
			FloorHeater, NULL);
#endif
}
