#include "ch.h"
#include "hal.h"
#include "FloorHeater.h"
#include "core.h"

#if FloorHeater_PRESENT

static thread_reference_t Update_Thread;
static thread_reference_t FH_Thread;

typedef struct {
  uint8_t id;
  volatile FloorHeater_Inner_Val Inner_Val_FH;
  virtual_timer_t timer;
  PWMDriver driver;
  pwmchannel_t channel;
} FH_Working_Struct_t;

// pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, ipwr * 100)); // 10% duty cycle
// void pwmEnableChannel(PWMDriver *pwmp, pwmchannel_t channel, pwmcnt_t width);

//#define EVENTMASK_UPDATE_1 0x100

static core_base_struct_t Core_FloorHeater;
//volatile static FloorHeater_Inner_Val Inner_Val_FH[FH_QUANTITY] = {};
//volatile static virtual_timer_t timer[FH_QUANTITY];

static FH_Working_Struct_t FH[FH_QUANTITY];

int32_t UpdatePID_S(volatile FloorHeater_Inner_Val * pid, int32_t error) //, double position)
{
//	int32_t pTerm, /*dTerm,*/iTerm;

  pid->pPower = error << pid->RW.pGain; // calculate the proportional term
  pid->RW.iState += error; // calculate the integral state with appropriate limiting

  if (pid->RW.iState > pid->RW.iMax)
    pid->RW.iState = pid->RW.iMax;
  else if (pid->RW.iState < pid->RW.iMin)
    pid->RW.iState = pid->RW.iMin;
  pid->iPower = pid->RW.iState << pid->RW.iGain; // calculate the integral term
//  dTerm = pid->dGain * (position - pid->dState);
//  pid->dState = position;

//	chSysLock();
//	Inner_Val_FH.pPower = pTerm;
//	Inner_Val_FH.iPower = iTerm;
//	chSysUnlock();

  return (pid->pPower + pid->iPower); // - dTerm);
}

static PWMConfig pwmcfg = {1000, /* 1Khz PWM clock frequency */
                           2000, /* PWM period 2 seconds */
                           NULL, /* No callback */
                           /* Only channel 4 enabled */
                           { {PWM_OUTPUT_DISABLED, NULL}, {PWM_OUTPUT_DISABLED, NULL}, {PWM_OUTPUT_ACTIVE_HIGH, NULL}, {
                               PWM_OUTPUT_DISABLED, NULL}, },
                           0};

void FloorHeater_Init() {

  Core_FloorHeater.type = Heater;
//  Core_FloorHeater.custom.uint32 = 180;
//  Core_FloorHeater.Auto_Update_Sec = 180;
//	Core_FloorHeater.next = NULL;
  Core_FloorHeater.description = "FloorHeater, 330 Watt PWM (PI)";
  Core_FloorHeater.inner_values[0] = &FH[0].Inner_Val_FH;
  Core_FloorHeater.ival_size = sizeof(FloorHeater_Inner_Val);
  Core_FloorHeater.ival_rw_size = sizeof(FloorHeater_Inner_Val_RW);

  FH[0].Inner_Val_FH.RW.Auto_Update_Sec = 10;
  FH[0].Inner_Val_FH.RW.iGain = 2;
  FH[0].Inner_Val_FH.RW.pGain = 4;
  FH[0].Inner_Val_FH.RW.iState = 0;
  FH[0].Inner_Val_FH.RW.iMax = 25;
  FH[0].Inner_Val_FH.RW.iMin = -2;
  FH[0].Inner_Val_FH.RW.Desired_Temp = 0;
  FH[0].Inner_Val_FH.RW.Get_Temp_Callback = NULL;
  FH[0].Inner_Val_FH.RW.MaxPower = 100;

  FH[0].driver = PWMD1;
  FH[0].channel = 2;

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

void fh_timer_handler(void *p) {
  FH_Working_Struct_t *fh_struct = (FH_Working_Struct_t *)p;
  osalSysLockFromISR();
  chEvtSignalI(FH_Thread, (eventmask_t)(EVENT_MASK(fh_struct->id) << EVENTMASK_MOD_PRIVATE_SHIFT));
//  chEvtSignalI(FH_Thread, (eventmask_t)(256));
  chVTSetI(&fh_struct->timer, S2ST(fh_struct->Inner_Val_FH.RW.Auto_Update_Sec), fh_timer_handler, fh_struct);
  osalSysUnlockFromISR();
}


THD_WORKING_AREA(waFloorHeater, 128);
//__attribute__((noreturn))
THD_FUNCTION(FloorHeater,arg) {
  (void)arg;
//	chRegSetThreadName("FloorHeater");

//	PWM_Init();
  FH_Thread = chThdGetSelfX();

  RCC_TIMERS_ENABLE_CMD();
  PAL_SET_MODE_CMD();
  PWM_START_CMD();

  uint8_t x;

  osalSysLock();
  for (x = 0; x < FH_QUANTITY; x++) {
    chVTSetI(&FH[x].timer, S2ST(FH[x].Inner_Val_FH.RW.Auto_Update_Sec), fh_timer_handler, &FH[x]);
  }
  osalSysUnlock();

  while (TRUE) {

    eventmask_t evt = chEvtWaitOne(ALL_EVENTS);
    uint8_t sensor_id = 0xFF;

    if (evt == EVENTMASK_REREAD) {
      osalSysLock();
//			Copy_PID_Vals_S();
      _core_wakeup_i(&Update_Thread, MSG_OK);
      osalSysUnlock();
    }
    else {
      switch (evt) {
      case (EVENT_MASK(0) << EVENTMASK_MOD_PRIVATE_SHIFT):
        sensor_id = 0;
        break;
#if FH_QUANTITY >1
      case (EVENT_MASK(1) << EVENTMASK_MOD_PRIVATE_SHIFT):
        sensor_id = 1;
        break;
#endif
#if FH_QUANTITY >2
        case (EVENT_MASK(2) << EVENTMASK_MOD_PRIVATE_SHIFT):
        sensor_id = 2;
        break;
#endif
#if FH_QUANTITY >3
        case (EVENT_MASK(3) << EVENTMASK_MOD_PRIVATE_SHIFT):
        sensor_id = 3;
        break;
#endif
      }
      if (sensor_id < FH_QUANTITY) {
        volatile int16_t ipwr, Current_Temp;
        static volatile FloorHeater_Inner_Val Current_Ival;
        //      msg = chMsgSend(DS18B20_Thread, msg);
        //      Desired_Temp = (*Tmp_Base).set_value;
        osalSysLock();
        Current_Ival = FH[sensor_id].Inner_Val_FH;
        osalSysUnlock();

        if (Current_Ival.RW.Get_Temp_Callback != NULL) {
          Current_Temp = Current_Ival.RW.Get_Temp_Callback();
          volatile int32_t pwr = UpdatePID_S(&Current_Ival, (Current_Ival.RW.Desired_Temp - Current_Temp));
          if (pwr > Current_Ival.RW.MaxPower)
            pwr = Current_Ival.RW.MaxPower;
          if (pwr < 0)
            pwr = 0;
          ipwr = (int16_t)pwr;
        }
        else {
          Current_Temp = 0xFFF0;
          ipwr = 0;
        };

        Current_Ival.Power = ipwr;

        osalSysLock();
        //    FH[0].Inner_Val_FH
        FH[sensor_id].Inner_Val_FH = Current_Ival;
        osalSysUnlock();

        pwmEnableChannel(&FH[sensor_id].driver, FH[sensor_id].channel, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, ipwr * 100)); // 10% duty cycle

      }
    }
  }
}

void FloorHeater_Start() {
  FloorHeater_Init();
  thread_t* thd = chThdCreateStatic(waFloorHeater, sizeof(waFloorHeater), HIGHPRIO, FloorHeater, NULL);
  Core_Register_Thread(Heater, thd, &Update_Thread);
  chThdYield();
}

#endif
