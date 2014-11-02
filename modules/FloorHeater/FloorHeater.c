#include "ch.h"
#include "hal.h"
#include "FloorHeater.h"
#include "core.h"

#if FloorHeater_PRESENT

static thread_reference_t Update_Thread;

static core_base_struct_t Core_FloorHeater;
volatile static FloorHeater_Inner_Val Inner_Val_FH = {};
//volatile static FloorHeater_Inner_Val_RW Floor_PID;

/*
typedef struct
{
//  double dState;                  // Last position input
	int32_t iState;     // Integrator state
	int32_t iMax, iMin;
	// Maximum and minimum allowable integrator state
	int32_t iGain,     // integral gain
			pGain;     // proportional gain
//             dGain;         // derivative gain
	uint16_t Desired_Temp;
} SPid;

volatile static SPid Floor_PID = { };*/

int32_t UpdatePID_S(volatile FloorHeater_Inner_Val * pid, int32_t error)     //, double position)
{
//	int32_t pTerm, /*dTerm,*/iTerm;

	pid->pPower = error << pid->RW.pGain;     // calculate the proportional term
	pid->RW.iState += error;     // calculate the integral state with appropriate limiting

	if (pid->RW.iState > pid->RW.iMax)
		pid->RW.iState = pid->RW.iMax;
	else if (pid->RW.iState < pid->RW.iMin)
		pid->RW.iState = pid->RW.iMin;
	pid->iPower = pid->RW.iState << pid->RW.iGain;     // calculate the integral term
//  dTerm = pid->dGain * (position - pid->dState);
//  pid->dState = position;

//	chSysLock();
//	Inner_Val_FH.pPower = pTerm;
//	Inner_Val_FH.iPower = iTerm;
//	chSysUnlock();

	return (pid->pPower + pid->iPower);     // - dTerm);
}

static PWMConfig pwmcfg =
{ 1000, /* 1Khz PWM clock frequency */
2000, /* PWM period 2 seconds */
NULL, /* No callback */
/* Only channel 4 enabled */
{
{ PWM_OUTPUT_DISABLED, NULL },
{ PWM_OUTPUT_DISABLED, NULL },
{ PWM_OUTPUT_ACTIVE_HIGH, NULL },
{ PWM_OUTPUT_DISABLED, NULL }, }, 0 };

void FloorHeater_Init()
{

//	Core_FloorHeater.id = (uint32_t) arg;
	Core_FloorHeater.type = Heater;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
//	Core_FloorHeater.thread = chThdGetSelfX();
//	Core_FloorHeater.direction = RO;
	Core_FloorHeater.next = NULL;
	Core_FloorHeater.description = "FloorHeater, 330 Watt PWM (PI)";
//	Core_FloorHeater.current_value = 0xffff;
	Core_FloorHeater.inner_values = &Inner_Val_FH;
	Core_FloorHeater.ival_size = sizeof(FloorHeater_Inner_Val);
	Core_FloorHeater.ival_rw_size = sizeof(FloorHeater_Inner_Val_RW);

	/*	chSysLock();
	 Core_Base.next = &Core_DS18B20;
	 chSysUnlock();*/

	Inner_Val_FH.RW.iGain = 2;
	Inner_Val_FH.RW.pGain = 4;
	Inner_Val_FH.RW.iState = 0;
	Inner_Val_FH.RW.iMax = 25;
	Inner_Val_FH.RW.iMin = -2;
	Inner_Val_FH.RW.Desired_Temp = 0;
	Inner_Val_FH.RW.Auto_Update_Sec = 180;
	Inner_Val_FH.RW.Get_Temp_Callback = NULL;
	Inner_Val_FH.RW.MaxPower = 100;

	Core_Module_Register(&Core_FloorHeater);
}

//void static PWM_Init()
//{
/*	GPIO_InitTypeDef GPIO_InitStruct;

 #ifdef STM32F100C8

 #else
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
 //	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(((GPIO_TypeDef *) GPIOA_BASE), &GPIO_InitStruct);

 GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource10, GPIO_AF_2);


 #endif*/

//}
/*
inline static void Copy_PID_Vals_S()
{
	Floor_PID = Inner_Val_FH.RW;
}*/

THD_WORKING_AREA(waFloorHeater, 256);
//__attribute__((noreturn))
THD_FUNCTION(FloorHeater,arg)
{
	(void) arg;
//	chRegSetThreadName("FloorHeater");

//	PWM_Init();

	rccEnableTIM1(TRUE);
	palSetPadMode(GPIOA, GPIOA_PIN10, PAL_MODE_ALTERNATE(2) | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);

	pwmStart(&PWMD1, &pwmcfg);

//	volatile core_base_struct_t *Tmp_Base;
//	Tmp_Base = Core_BasePtr;
	/*	while ((*Tmp_Base).type != Temp)
	 {
	 Tmp_Base = (*Tmp_Base).next;
	 }

	 thread_t *DS18B20_Thread = (thread_t *) (*Tmp_Base).thread;
	 volatile uint16_t msg;*/
/*
	osalSysLock();
	Copy_PID_Vals_S();
	osalSysUnlock();
*/
//	uint16_t Desired_Temp;     // = Set_TEMP << 2;

//	systime_t time = chVTGetSystemTime();
//	pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 9000));   // 10% duty cycle
	while (TRUE)
	{

		eventmask_t evt = chEvtWaitOneTimeout(ALL_EVENTS, S2ST(Inner_Val_FH.RW.Auto_Update_Sec));

		if (evt == EVENTMASK_REREAD)
		{
			osalSysLock();
//			Copy_PID_Vals_S();
			_core_wakeup_i(&Update_Thread, MSG_OK);
			osalSysUnlock();
		}

		volatile int16_t ipwr, Current_Temp;
//		msg = chMsgSend(DS18B20_Thread, msg);
//		Desired_Temp = (*Tmp_Base).set_value;
		osalSysLock();
		if (Inner_Val_FH.RW.Get_Temp_Callback!=NULL)
		{
			osalSysUnlock();
			Current_Temp = Inner_Val_FH.RW.Get_Temp_Callback();
			osalSysLock();
			volatile int32_t pwr = UpdatePID_S(&Inner_Val_FH, (Inner_Val_FH.RW.Desired_Temp - Current_Temp));
			if (pwr > Inner_Val_FH.RW.MaxPower)
				pwr = Inner_Val_FH.RW.MaxPower;
			if (pwr < 0)
				pwr = 0;
			ipwr = (int16_t) pwr;
		}
		else {
			Current_Temp = 0xFFF0;
			ipwr = 0;
		};
		osalSysUnlock();

		/*		if (msg>Desired_Temp)
		 {
		 ipwr = 10;
		 }
		 else ipwr = 90;*/

		chSysLock();
		Inner_Val_FH.Power = ipwr;
		chSysUnlock();

		pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, ipwr * 100));     // 10% duty cycle

//		time += S2ST(120);
//		sleepUntil(&time, S2ST(60));
	}
}

void FloorHeater_Start()
{
	FloorHeater_Init();
	thread_t* thd = chThdCreateStatic(waFloorHeater, sizeof(waFloorHeater), HIGHPRIO, FloorHeater, NULL);
	Core_Register_Thread(Heater, thd, &Update_Thread);
	chThdYield();
}

#endif
