#include "ch.h"
#include "hal.h"
#include "RGBW.h"
#include "core.h"

static PWMConfig pwmcfg =
{ 3000000, /* 3Mhz PWM clock frequency */
1000, /* PWM frequency 30 kHz*/
NULL, /* No callback */
/* Only channel 4 enabled */
{
{ PWM_OUTPUT_ACTIVE_HIGH, NULL },
{ PWM_OUTPUT_ACTIVE_HIGH, NULL },
{ PWM_OUTPUT_DISABLED, NULL },
{ PWM_OUTPUT_ACTIVE_HIGH, NULL }, }, 0 };

void static PWM_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

#ifdef STM32F100C8

#else
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);
#endif

	pwmStart(&PWMD3, &pwmcfg);
}

volatile static core_base_struct_t Core_RGBW;
volatile static struct
{
	volatile uint16_t Red, Green, Blue, White;
} Inner_Val_RGBW;

void RGBW_Init(void *arg)
{
	Core_RGBW.id = (uint32_t) arg;
	Core_RGBW.type = RGBW;
	Core_RGBW.thread = chThdGetSelfX();
	Core_RGBW.direction = RW;
	Core_RGBW.next = NULL;
	Core_RGBW.description = "4 Floor Temp Sensors DS18B20";
	Core_RGBW.current_value = 0xffff;
	Core_RGBW.set_value = 0x68; //Initial Floor Temp value 0x68 = 26 deg. Celsius
	Core_RGBW.inner_values = &Inner_Val_RGBW;
	Core_RGBW.ival_size = sizeof(Inner_Val_RGBW);
	/*	chSysLock();
	 Core_Base.next = &Core_DS18B20;
	 chSysUnlock();*/

	Core_Module_Register(&Core_RGBW);
}

THD_WORKING_AREA(waRGBW_Controller, 256);
//__attribute__((noreturn))
THD_FUNCTION(RGBW_Controller,arg)
{
	(void) arg;
//	thread_t *answer_thread;
	//	chRegSetThreadName("DS18B20");

	RGBW_Init(arg);

	PWM_Init();

	systime_t time = chVTGetSystemTime();
	systime_t time_day = time;
	systime_t time_night = time;

	Inner_Val_RGBW.Blue = 0;
	Inner_Val_RGBW.Red = 0;
	Inner_Val_RGBW.Green = 0;

	int8_t R_inc, B_inc;
	B_inc = 1;
	R_inc = 1;

	uint32_t Timeval_Current, R_set, B_set;

	R_set = 1000;
	B_set = 500;

	const uint8_t Sunrise_Duration_min = 30;
	const uint8_t Sunset_Duration_min = 30;
	const uint8_t Daylight_Duration_hours = 12;

	bool_t Sunrise = TRUE;

//	Sunrise_Timeval = Sunrise_Duration_min * 60 * 1000 / ((R_set + B_set) / 2);

	Timeval_Current = Sunrise_Duration_min * 60 * 1000 / MAX(R_set, B_set);

	while (TRUE)
	{

		pwmEnableChannel(&PWMD3, 0,
				PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Red * 10)); // 10% duty cycle
		pwmEnableChannel(&PWMD3, 1,
				PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Blue * 10)); // 10% duty cycle
		pwmEnableChannel(&PWMD3, 3,
				PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Green * 100)); // 10% duty cycle

		if ((B_inc = 0) && (R_inc = 0))
		{
			if (Sunrise)
			{
				time_day = chVTGetSystemTime();
				Timeval_Current = Daylight_Duration_hours * 3600;
				sleepUntil(&time_day, S2ST(Timeval_Current));
				Sunrise = FALSE;
				Timeval_Current = Sunset_Duration_min * 60 * 1000
						/ MAX(R_set, B_set);
				B_inc = -1;
				R_inc = -1;
			}
			else
			{
				Timeval_Current = 24 * 3600;
				sleepUntil(&time_night, S2ST(Timeval_Current));
				Sunrise = TRUE;
				Timeval_Current = Sunrise_Duration_min * 60 * 1000
						/ MAX(R_set, B_set);
				B_inc = 1;
				R_inc = 1;
			}
			time = chVTGetSystemTime();
		}
		else
		{
			sleepUntil(&time, MS2ST(Timeval_Current));
			Inner_Val_RGBW.Blue += B_inc;
			Inner_Val_RGBW.Red += R_inc;
			if ((Inner_Val_RGBW.Blue >= B_set) || (Inner_Val_RGBW.Blue <= 0))
			{
				B_inc = 0;
			}

			if ((Inner_Val_RGBW.Red >= R_set) || (Inner_Val_RGBW.Red <= 0))
			{
				R_inc = 0;
			}
		}
	}
}

void RGBW_Start(uint8_t id)
{
#if RGBW_PRESENT
	chThdCreateStatic(waRGBW_Controller, sizeof(waRGBW_Controller), NORMALPRIO,
			RGBW_Controller, (void*) (uint32_t) id);
#endif
}
