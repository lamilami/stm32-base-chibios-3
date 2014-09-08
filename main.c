/*
 ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
#ifdef STM32F100C8
#include "stm32f10x.h"
#else
#include "stm32f0xx.h"
#endif
#include "ch.h"
#include "hal.h"
#include "core.h"
#include "halconf.h"

#ifdef DEBUG_Discovery

#ifdef comment

static thread_t *Blinker_Thread = NULL;
/*
 * LED blinker thread, times are in milliseconds.
 */

static THD_WORKING_AREA(waLedBlinker, 128);
__attribute__((noreturn))
static THD_FUNCTION(LedBlinker,arg)
{

	(void) arg;
//	chRegSetThreadName("LedBlinker");
	while (TRUE)
	{
//		msg_t msg;
//		msg=255;
//		chSysLock();
//		Blinker_Thread = chThdGetSelfX();
//		chSchGoSleepS(CH_STATE_SUSPENDED);
//		msg = chThdGetSelfX()->p_u.rdymsg; /* Retrieving the message, optional.*/
//		chSysUnlock();
//		uint16_t cnt = 0;
//		chThdYield();
		chThdSleepMilliseconds(250);
		LEDSwap();
//				for (cnt = 0; cnt < 65535; cnt++)
//		 {
//		 msg++;
		//			nop();
		//			LEDSwap();
		//			chThdSleepMilliseconds(100);
		//			LEDSwap();
		//			chThdSleepMilliseconds(100);
//		 }
	}
}

void ll_ledblink(uint8_t cnt)
{
	if (Blinker_Thread != NULL)
	{
		Blinker_Thread->p_u.rdymsg = (msg_t) cnt; /* Sending the message, optional.*/
		chSchReadyI(Blinker_Thread);
		Blinker_Thread = NULL;
	}
}

void LEDBlinkS(uint8_t cnt)
{
	chSysLock();
	ll_ledblink(cnt);
	chSysUnlock();
}

void LEDBlinkI(uint8_t cnt)
{
	osalSysLockFromISR();
	ll_ledblink(cnt);
	osalSysUnlockFromISR();
}

#endif
#endif
/*
 * Green LED blinker thread, times are in milliseconds.
 */
/*
 static THD_WORKING_AREA(waTransmit, 128);
 //__attribute__((noreturn))
 static THD_FUNCTION (Transmit,arg)
 {

 (void) arg;
 //	chRegSetThreadName("Transmitter");
 rf_sended_debug = FALSE;
 chThdSleepSeconds(3);
 while (TRUE)
 {
 //	LEDSwap();
 if (!rf_sended_debug)
 {
 while (!rf_sended_debug)
 {
 //				Radio_Send_Command(3, RF_PING);
 chThdSleepSeconds(5);
 }
 }
 rf_sended_debug = FALSE;
 chThdSleepSeconds(15);
 }
 }
 */

uint16_t FloorHeater_cb()
{
	static DS18B20_Inner_Val Temp_Vals;
	Core_Module_Update(Temp, NULL, 1000);
	Core_Module_Read(Temp,(char*) &Temp_Vals);
	return Temp_Vals.temp[0];
}


/*
 * Application entry point.
 */
int main(void)
{
	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();
	chThdSetPriority(LOWPRIO);
	/*
	 * Creates the blinker threads.
	 */
//  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
	/*	chThdCreateStatic(waTransmit, sizeof(waTransmit), NORMALPRIO, Transmit,
	 NULL);
	 */
	/*
	 #ifdef DEBUG_Discovery
	 chThdCreateStatic(waLedBlinker, sizeof(waLedBlinker), NORMALPRIO,
	 LedBlinker, NULL);
	 #endif
	 */
	/*	chThdCreateStatic(waRadio, sizeof(waRadio), NORMALPRIO, Radio, NULL);
	 chThdCreateStatic(waRadio_Processor, sizeof(waRadio_Processor), NORMALPRIO,
	 Radio_Processor, NULL);
	 */

//  chThdSleepSeconds(2);
//  Radio_Send_Command (3, RF_PING);
//  nRF24_hw_ce_low();
//  radio_init();
	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state, when the button is
	 * pressed the test procedure is launched with output on the serial
	 * driver 1.
	 */
//	LEDB1Swap();
	Core_Start();

//	uint8_t data[RF_MAX_PAYLOAD_LENGTH-1];
#if LCD1602_PRESENT
	InitializeLCD();     //������������� �������
	ClearLCDScreen();//������� ������� �� ������
#endif

//	RGBW_IW;

#ifdef RGBW_Test
	systime_t time_start = chVTGetSystemTime();

	RTCDateTime DateTime;

	DateTime.year = 33;
	DateTime.month = 8;
	DateTime.dstflag = 0;
	DateTime.dayofweek = 4;
	DateTime.day = 28;
	DateTime.millisecond = 0;

	rtcSetTime(&RTCD1, &DateTime);

	chThdSleepSeconds(1000);

	rtcGetTime(&RTCD1, &DateTime);

#ifdef tralala
	/* Disable write protection. */
	RTCD1.rtc->WPR = 0xCA;
	RTCD1.rtc->WPR = 0x53;

//if (!(RTCD1.rtc->ISR & RTC_ISR_INITS))
	{
#endif

//		rtc_lld_enter_init();
//		RTCD1.rtc->CR = 0;
//		RTCD1.rtc->ISR = 0;
//		RTCD1.rtc->PRER = RTC_PRER(125, 320);
//		RTCD1.rtc->PRER = RTC_PRER(125, 320);
//		RTCD1.rtc->CAL |= RTC_CAL_CALP|RTC_CAL_CALM_1|RTC_CAL_CALM_3|RTC_CAL_CALM_5|RTC_CAL_CALM_6|RTC_CAL_CALM_7;
//		rtc_lld_exit_init();
//	}
//	RTCD1.rtc->WPR = 0xFF;
//#endif

		DateTime.year = 33;
		DateTime.month = 8;
		DateTime.dstflag = 0;
		DateTime.dayofweek = 4;
		DateTime.day = 28;
		DateTime.millisecond = 0;

		rtcSetTime(&RTCD1, &DateTime);

		chThdSleepSeconds(1000);

		rtcGetTime(&RTCD1, &DateTime);

#endif

#if RGBW_PRESENT
//	RGBW_Inner_Val* RGBW_IV = (RGBW_Inner_Val*) Core_GetIvalAddrByType(RGBW);
	RGBW_Inner_Val RGBW_Day = {}, RGBW_Night = {};
//	RGBW_Inner_Val RGBW_Current;

	RGBW_Day.RW.Red_Set = 10000;
	RGBW_Day.RW.Blue_Set = 10000;
	RGBW_Day.RW.Green_Set = 10000;
	RGBW_Day.RW.Rise_Time_Sec = 1;
	RGBW_Day.RW.Max_Delay_Sec = 600;

	RGBW_Night.RW.Red_Set = 0;
	RGBW_Night.RW.Blue_Set = 0;
	RGBW_Night.RW.Green_Set = 0;
	RGBW_Night.RW.Rise_Time_Sec = 1;
	RGBW_Night.RW.Max_Delay_Sec = 600;
#endif
//	Core_Module_Read(RGBW, (void *) &RGBW_Current);

#if FloorHeater_PRESENT

	FloorHeater_Inner_Val_RW FH_IV;
	FH_IV.Desired_Temp = 26;
	FH_IV.Auto_Update_Sec = 60;
	FH_IV.Get_Temp_Callback = FloorHeater_cb;
	FH_IV.iGain = 2;
	FH_IV.pGain = 4;
	FH_IV.iState = 0;
	FH_IV.iMax = 25;
	FH_IV.iMin = -2;

	Core_Module_Update(Heater,(void *) &FH_IV, 1000);

#endif

	while (TRUE)
	{
#if LCD1602_PRESENT
		data[0]=2;
		int16_t tmp;
		Radio_Send_Command(10, RF_GET, 1, data);
		data[0]=3;
		Radio_Send_Command(10, RF_GET, 1, data);
		chThdSleepSeconds(3);
		Cursor(0,0);     //��������� �������
		PrintStr("T=");//��������� ������
		/*	    PutData[2][0]=1;
		 PutData[2][1]=2;
		 PutData[2][2]=3;
		 PutData[2][3]=4;
		 PutData[2][4]=5;*/
		tmp = PutData[2][3]*256+PutData[2][4];
		LCD_PutSignedInt(tmp>>2);
		PrintStr(".");
		LCD_PutUnsignedInt((tmp&3)*25);
		PrintStr("              ");
		Cursor(1,0);
		PrintStr("PWR=");     //��������� ������
		tmp = PutData[3][3]*256+PutData[3][4];
//	    LCD_PutSignedInt(tmp>>2);
//	    PrintStr(".");
		LCD_PutUnsignedInt(tmp);
		PrintStr("              ");
#endif

#if RGBW_PRESENT

		Core_Module_Update(RGBW, (void *) &RGBW_Day, 3000);

//		chThdSleepSeconds(14*60*60);
/*		chThdSleepMilliseconds(500);
		Core_Module_Read(RGBW, (void *) &RGBW_Current);
		chThdSleepMilliseconds(1500);*/
		chThdSleepMilliseconds(500);

		Core_Module_Update(RGBW, (void *) &RGBW_Night, 3000);

//		Timeval_Current = 24 * 3600 - Inner_Val_RGBW.Correction_24H;
//		time_start = chThdSleepUntilWindowed(time_start,time_start + S2ST(24 * 60*60 - 117));

		time_start = chThdSleepUntilWindowed(time_start, time_start + S2ST(4));
#endif

#ifdef WaitEvents
		eventmask_t evt = chEvtWaitOne(ALL_EVENTS);
		switch (evt)
		{
			case (EVENT_MASK((uint8_t) PIR)):
			LEDB1Swap();
			break;
			default:
			break;
		}
#endif
		chThdSleepSeconds(300);
	}
}
