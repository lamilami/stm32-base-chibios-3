#include "ch.h"
#include "hal.h"
#include "RGBW.h"
#include "core.h"

#if RGBW_PRESENT

#define EVENTMASK_UPDATE 0x80

static thread_t *RGBW_Thread = NULL;
static thread_reference_t Update_Thread;

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
	volatile int16_t * curr_power;
	volatile systime_t rise_time;
	volatile int16_t max_power;
	volatile int8_t inc;
	virtual_timer_t* vt;
} timer_str_t;

static virtual_timer_t vt_r, vt_b, vt_g;

static void timer_handler(void *arg)
{
	chSysLockFromISR();
	timer_str_t* timer_s = (timer_str_t*) arg;
	*timer_s->curr_power += timer_s->inc;
	if (!chVTIsArmedI(timer_s->vt))
		chVTDoSetI(timer_s->vt, timer_s->rise_time, timer_handler, (void*) arg);
	chEvtSignalI(RGBW_Thread, (eventmask_t) EVENTMASK_UPDATE);
	chSysUnlockFromISR();
}

static core_base_struct_t Core_RGBW;
volatile static RGBW_Inner_Val Inner_Val_RGBW;

void RGBW_Init()
{
	Core_RGBW.type = RGBW;
//	Core_RGBW.next = NULL;
//	Core_RGBW.description = "RGBW Controller";
	Core_RGBW.inner_values[0] = &Inner_Val_RGBW;
	Core_RGBW.ival_size = sizeof(RGBW_Inner_Val);
	Core_RGBW.ival_rw_size = sizeof(RGBW_Inner_Val_RW);

	Inner_Val_RGBW.RW.Max_Delay_Sec = 100;
	Inner_Val_RGBW.RW.Red_Set = 0;
	Inner_Val_RGBW.RW.Blue_Set = 0;
	Inner_Val_RGBW.RW.Green_Set = 0;

	Core_Module_Register(&Core_RGBW);
}

inline systime_t GetRiseTicksPeriod(uint16_t duration_sec, uint16_t max_pow)
{
	return (S2ST((uint32_t)duration_sec) / max_pow);
}

void ComputeRiseVals_S(const uint16_t current_val, const uint16_t set_val, timer_str_t *tim_struct)
{
//	chSysLock();
	chVTResetI((*tim_struct).vt);
	if (current_val != set_val)
	{
		(*tim_struct).rise_time = GetRiseTicksPeriod(Inner_Val_RGBW.RW.Rise_Time_Sec, ABS((int16_t)(set_val - current_val)));
		uint8_t coeff = (MS2ST(1) / (*tim_struct).rise_time) + 1;
		(*tim_struct).rise_time *= coeff;
		if (set_val > current_val)
		{
			(*tim_struct).inc = coeff;
		}
		else
		{
			(*tim_struct).inc = 0 - coeff;
		}
		(*tim_struct).max_power = set_val;
		chVTSetI((*tim_struct).vt, (*tim_struct).rise_time, timer_handler, tim_struct);
	}
	else
	{
		(*tim_struct).rise_time = 0;
		(*tim_struct).inc = 0;
	}
//	chSysUnlock();
}

void CheckVT_S(timer_str_t *tim_struct)
{
	if ((*tim_struct).inc != 0)
	{
		if (((*(*tim_struct).curr_power >= (*tim_struct).max_power) && ((*tim_struct).inc > 0))
				|| ((*(*tim_struct).curr_power <= (*tim_struct).max_power) && ((*tim_struct).inc < 0)))
		{
			(*tim_struct).inc = 0;
			*(*tim_struct).curr_power = (*tim_struct).max_power;
			chVTResetI((*tim_struct).vt);
		}
	}
}

THD_WORKING_AREA(waRGBW_Controller, 128);
//__attribute__((noreturn))
THD_FUNCTION(RGBW_Controller,arg)
{
	(void) arg;

	palSetPadMode(GPIOA, GPIOA_PIN6, PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN7, PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	pwmStart(&PWMD3, &pwmcfg);

	RGBW_Thread = chThdGetSelfX();

	timer_str_t R_Tim, B_Tim, G_Tim;

	Inner_Val_RGBW.Blue_Power = 10;
	Inner_Val_RGBW.Red_Power = 10;
	Inner_Val_RGBW.Green_Power = 10;

	R_Tim.curr_power = &Inner_Val_RGBW.Red_Power;
	R_Tim.inc = 0;
	R_Tim.rise_time = 0;
	R_Tim.max_power = *R_Tim.curr_power;
	R_Tim.vt = &vt_r;

	G_Tim.curr_power = &Inner_Val_RGBW.Green_Power;
	G_Tim.inc = 0;
	G_Tim.rise_time = 0;
	G_Tim.max_power = *G_Tim.curr_power;
	G_Tim.vt = &vt_g;

	B_Tim.curr_power = &Inner_Val_RGBW.Blue_Power;
	B_Tim.inc = 0;
	B_Tim.rise_time = 0;
	B_Tim.max_power = *B_Tim.curr_power;
	B_Tim.vt = &vt_b;

	while (TRUE)
	{

		pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Red_Power));     // 10% duty cycle
		pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Blue_Power));     // 10% duty cycle
		pwmEnableChannel(&PWMD3, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, Inner_Val_RGBW.Green_Power));     // 10% duty cycle

		eventmask_t evt = chEvtWaitOneTimeout(ALL_EVENTS, S2ST(Inner_Val_RGBW.RW.Max_Delay_Sec));

		chSysLock();

		switch (evt)
		{
		case (EVENTMASK_REREAD):

			ComputeRiseVals_S(Inner_Val_RGBW.Red_Power, Inner_Val_RGBW.RW.Red_Set, &R_Tim);
			ComputeRiseVals_S(Inner_Val_RGBW.Blue_Power, Inner_Val_RGBW.RW.Blue_Set, &B_Tim);
			ComputeRiseVals_S(Inner_Val_RGBW.Green_Power, Inner_Val_RGBW.RW.Green_Set, &G_Tim);

			_core_wakeup_i(&Update_Thread, MSG_OK);

//			break;
		case (EVENTMASK_UPDATE):
		case ((eventmask_t) NULL):

			CheckVT_S(&R_Tim);
			CheckVT_S(&G_Tim);
			CheckVT_S(&B_Tim);

			break;
		default:
			break;
		}

		chSysUnlock();
	}
}

void RGBW_Start()
{
#if RGBW_PRESENT
	RGBW_Init();
	thread_t* thd = chThdCreateStatic(waRGBW_Controller, sizeof(waRGBW_Controller), NORMALPRIO, RGBW_Controller, NULL);
	Core_Register_Thread(RGBW, thd, &Update_Thread);
	chThdYield();
#endif
}

#endif
