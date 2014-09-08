#include "ch.h"
#include "hal.h"
#include "FloorHeater.h"
#include "core.h"

#if FloorHeater_PRESENT

static core_base_struct_t Core_FloorHeater;
volatile static struct
{
	volatile uint16_t Power;
	volatile uint16_t pPower;
	volatile uint16_t iPower;
} Inner_Val;

typedef struct
{
//  double dState;                  // Last position input
	int32_t iState;     // Integrator state
	int32_t iMax, iMin;
	// Maximum and minimum allowable integrator state
	int32_t iGain,     // integral gain
			pGain;     // proportional gain
//             dGain;         // derivative gain
} SPid;



int32_t UpdatePID(SPid * pid, int32_t error)     //, double position)
{
	float pTerm, /*dTerm,*/iTerm;

	pTerm = error << pid->pGain;     // calculate the proportional term
	pid->iState += error;     // calculate the integral state with appropriate limiting

	if (pid->iState > pid->iMax)
		pid->iState = pid->iMax;
	else if (pid->iState < pid->iMin)
		pid->iState = pid->iMin;
	iTerm = pid->iState << pid->iGain;     // calculate the integral term
//  dTerm = pid->dGain * (position - pid->dState);
//  pid->dState = position;

	chSysLock();
	Inner_Val.pPower = pTerm;
	Inner_Val.iPower = iTerm;
	chSysUnlock();

	return (pTerm + iTerm);     // - dTerm);
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
	Core_FloorHeater.direction = RO;
	Core_FloorHeater.next = NULL;
	Core_FloorHeater.description = "FloorHeater, 330 Watt PWM (PI)";
	Core_FloorHeater.current_value = 0xffff;
	Core_FloorHeater.inner_values = &Inner_Val;
	Core_FloorHeater.ival_size = sizeof(Inner_Val);

	/*	chSysLock();
	 Core_Base.next = &Core_DS18B20;
	 chSysUnlock();*/

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

	volatile core_base_struct_t *Tmp_Base;
	Tmp_Base = Core_BasePtr;
	while ((*Tmp_Base).type != Temp)
	{
		Tmp_Base = (*Tmp_Base).next;
	}

	thread_t *DS18B20_Thread = (thread_t *) (*Tmp_Base).thread;
	volatile uint16_t msg;

	SPid Floor_PID;
	Floor_PID.pGain = 4;
	Floor_PID.iGain = 2;
	Floor_PID.iState = 0;
	Floor_PID.iMax = 25;
	Floor_PID.iMin = -2;

	uint16_t Desired_Temp;     // = Set_TEMP << 2;

	systime_t time = chVTGetSystemTime();
//	pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 9000));   // 10% duty cycle
	while (TRUE)
	{

		volatile int16_t ipwr;
		msg = chMsgSend(DS18B20_Thread, msg);
		Desired_Temp = (*Tmp_Base).set_value;
		volatile int32_t pwr = UpdatePID(&Floor_PID, (float) (Desired_Temp - msg));
		if (pwr > 100)
			pwr = 100;
		if (pwr < 0)
			pwr = 0;
		ipwr = (int16_t) pwr;

		/*		if (msg>Desired_Temp)
		 {
		 ipwr = 10;
		 }
		 else ipwr = 90;*/

		chSysLock();
		Inner_Val.Power = ipwr;
		chSysUnlock();

		pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, ipwr * 100));     // 10% duty cycle

//		time += S2ST(120);
		sleepUntil(&time, S2ST(60));
	}
}

void FloorHeater_Start()
{
	FloorHeater_Init();
	chThdCreateStatic(waFloorHeater, sizeof(waFloorHeater), HIGHPRIO, FloorHeater, NULL);
	chThdYield();
}

#endif
