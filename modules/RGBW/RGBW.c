#include "ch.h"
#include "hal.h"
#include "RGBW.h"
#include "core.h"

#define EVENTMASK_UPDATE 0x01
static thread_t *RGBW_Thread = NULL;

static PWMConfig pwmcfg =
{ 12000000, /* 12Mhz PWM clock frequency */
10000, /* PWM frequency 1.2 kHz*/
NULL, /* No callback */
/* Only channel 4 enabled */
{
{ PWM_OUTPUT_ACTIVE_HIGH, NULL },
{ PWM_OUTPUT_ACTIVE_HIGH, NULL },
{ PWM_OUTPUT_DISABLED, NULL },
{ PWM_OUTPUT_ACTIVE_HIGH, NULL }, }, 0 };

typedef struct timer_str
{
	volatile uint16_t * curr_power;
	volatile systime_t rise_time;
	volatile uint16_t max_power;
	volatile int8_t inc;
	virtual_timer_t* vt;
} timer_str_t;

static virtual_timer_t vt_r, vt_b;

/*
void static PWM_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

#ifdef STM32F100C8

#else
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(((GPIO_TypeDef *) GPIOA_BASE), &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(((GPIO_TypeDef *) GPIOB_BASE), &GPIO_InitStruct);

	GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource6, GPIO_AF_1);
	GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource7, GPIO_AF_1);
	GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOB_BASE), GPIO_PinSource1, GPIO_AF_1);
#endif

	pwmStart(&PWMD3, &pwmcfg);
}*/

static void timer_handler(void *arg)
{
	timer_str_t* timer_s = (timer_str_t*) arg;
	*timer_s->curr_power += timer_s->inc;
	if ((*timer_s->curr_power >= timer_s->max_power) || (*timer_s->curr_power <= 0))
	{
		timer_s->inc = 0;
	}
	else
	{
		chVTSetI(timer_s->vt, timer_s->rise_time, timer_handler, (void*) arg);
	}
	chEvtSignalI(RGBW_Thread, (eventmask_t) EVENTMASK_UPDATE);
}

volatile static core_base_struct_t Core_RGBW;
volatile static RGBW_Inner_Val Inner_Val_RGBW;

void RGBW_Init(void *arg)
{
	Core_RGBW.id = (uint32_t) arg;
	Core_RGBW.type = RGBW;
	Core_RGBW.thread = chThdGetSelfX();
	Core_RGBW.direction = RW;
	Core_RGBW.next = NULL;
	Core_RGBW.description = "RGBW Controller";
	Core_RGBW.current_value = 0xffff;
	Core_RGBW.set_value = 0x68;     //Initial Floor Temp value 0x68 = 26 deg. Celsius
	Core_RGBW.inner_values = &Inner_Val_RGBW;
	Core_RGBW.ival_size = sizeof(Inner_Val_RGBW);

	Core_Module_Register(&Core_RGBW);
}

systime_t GetRiseTicksPeriod(uint8_t duration_min, uint16_t max_pow)
{
	return (MS2ST(duration_min * 60 * 1000 / max_pow));
}

THD_WORKING_AREA(waRGBW_Controller, 256);
//__attribute__((noreturn))
THD_FUNCTION(RGBW_Controller,arg)
{
	(void) arg;
	//	chRegSetThreadName("DS18B20");

	RGBW_Init(arg);

//	PWM_Init();

	palSetPadMode(GPIOA, GPIOA_PIN6,
			PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN7,
			PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN1,
			PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	pwmStart(&PWMD3, &pwmcfg);

	RGBW_Thread = chThdGetSelfX();

	timer_str_t R_Tim, B_Tim;

//	systime_t time_day = chVTGetSystemTime();
	systime_t time_night = chVTGetSystemTime();

	Inner_Val_RGBW.Blue = 0;
	Inner_Val_RGBW.Red = 0;
	Inner_Val_RGBW.Green = 0;

	uint32_t Timeval_Current;

	const uint8_t Sunrise_Duration_min = 60;
	const uint8_t Sunset_Duration_min = 60;
	const uint8_t Daylight_Duration_hours = 12;

	bool Sunrise = TRUE;

	R_Tim.curr_power = &Inner_Val_RGBW.Red;
	R_Tim.inc = 1;
	R_Tim.max_power = 10000;
	R_Tim.rise_time = GetRiseTicksPeriod(Sunrise_Duration_min, R_Tim.max_power);
	R_Tim.vt = &vt_r;
	/*	G_Tim.curr_power = &Inner_Val_RGBW.Green;
	 G_Tim.inc = 0;
	 G_Tim.max_power = 0;
	 G_Tim.rise_time = 0;
	 G_Tim.vt = &vt_g;*/
	B_Tim.curr_power = &Inner_Val_RGBW.Blue;
	B_Tim.inc = 1;
	B_Tim.max_power = 5000;
	B_Tim.rise_time = GetRiseTicksPeriod(Sunrise_Duration_min, B_Tim.max_power);
	B_Tim.vt = &vt_b;

	while (TRUE)
	{

		pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Red));     // 10% duty cycle
		pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Blue));     // 10% duty cycle
		pwmEnableChannel(&PWMD3, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Green));     // 10% duty cycle

		if ((R_Tim.inc == 0) && (B_Tim.inc == 0))
		{
			if (Sunrise)
			{
				Timeval_Current = Daylight_Duration_hours * 3600;
				chThdSleepSeconds(Timeval_Current);
				Sunrise = FALSE;
				R_Tim.rise_time = GetRiseTicksPeriod(Sunset_Duration_min, R_Tim.max_power);
				B_Tim.rise_time = GetRiseTicksPeriod(Sunset_Duration_min, B_Tim.max_power);
				R_Tim.inc = -1;
				B_Tim.inc = -1;
			}
			else
			{
				Timeval_Current = 24 * 3600;
				sleepUntil(&time_night, S2ST(Timeval_Current));
				Sunrise = TRUE;
				R_Tim.rise_time = GetRiseTicksPeriod(Sunrise_Duration_min, R_Tim.max_power);
				B_Tim.rise_time = GetRiseTicksPeriod(Sunrise_Duration_min, B_Tim.max_power);
				R_Tim.inc = 1;
				B_Tim.inc = 1;
			}
		}
		else
		{
			chSysLock();
			if (!chVTIsArmedI(R_Tim.vt))
			{
				chVTSetI(R_Tim.vt, R_Tim.rise_time, timer_handler, &R_Tim);
			}
			if (!chVTIsArmedI(B_Tim.vt))
			{
				chVTSetI(B_Tim.vt, B_Tim.rise_time, timer_handler, &B_Tim);
			}
			chSysUnlock();
			chEvtWaitAll((eventmask_t) EVENTMASK_UPDATE);
		}
	}
}

void RGBW_Start(uint8_t id)
{
#if RGBW_PRESENT
	chThdCreateStatic(waRGBW_Controller, sizeof(waRGBW_Controller), NORMALPRIO, RGBW_Controller, (void*) (uint32_t) id);
#endif
}
