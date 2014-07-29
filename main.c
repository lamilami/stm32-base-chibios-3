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
#include <nRF24.h>
#include "radio.h"
#include "core.h"
#include "halconf.h"
#include "DS18B20.h"
#include "FloorHeater.h"
#include "WatchDog.h"
#include "RGBW.h"
#include "rtc.h"

//#include "test.h"

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
//  Init_GPIOs();
	chSysInit();

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
	RTCTime timespec;
	RTC_TimeTypeDef RTC_TimeStructure;

	/* Set Time hh:mm:ss */
	RTC_TimeStructure.RTC_H12 = 0x00;
	RTC_TimeStructure.RTC_Hours = 0x06;
	RTC_TimeStructure.RTC_Minutes = 0x30;
	RTC_TimeStructure.RTC_Seconds = 0x00;
//	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

	timespec.tv_time = (uint32_t)(
			((uint32_t) RTC_ByteToBcd2(RTC_TimeStructure.RTC_Hours) << 16)
					| ((uint32_t) RTC_ByteToBcd2(RTC_TimeStructure.RTC_Minutes)
							<< 8)
					| ((uint32_t) RTC_ByteToBcd2(RTC_TimeStructure.RTC_Seconds))
					| (((uint32_t) RTC_TimeStructure.RTC_H12) << 16));

	rtcSetTime(&RTCD1, &timespec);

	rtcGetTime(&RTCD1, &timespec);

	/* Fill the structure fields with the read parameters */
	RTC_TimeStructure.RTC_Hours = (uint8_t)(
			(timespec & (RTC_TR_HT | RTC_TR_HU)) >> 16);
	RTC_TimeStructure.RTC_Minutes = (uint8_t)(
			(timespec & (RTC_TR_MNT | RTC_TR_MNU)) >> 8);
	RTC_TimeStructure.RTC_Seconds = (uint8_t)(timespec & (RTC_TR_ST | RTC_TR_SU));
	RTC_TimeStructure.RTC_H12 = (uint8_t)((timespec & (RTC_TR_PM)) >> 16);

	/* Check the input parameters format */
	if (0)
	{
		/* Convert the structure parameters to Binary format */
		RTC_TimeStructure.RTC_Hours = (uint8_t) RTC_Bcd2ToByte(
				RTC_TimeStruct->RTC_Hours);
		RTC_TimeStructure.RTC_Minutes = (uint8_t) RTC_Bcd2ToByte(
				RTC_TimeStruct->RTC_Minutes);
		RTC_TimeStructure.RTC_Seconds = (uint8_t) RTC_Bcd2ToByte(
				RTC_TimeStruct->RTC_Seconds);
	}

//	RTC_GetTimeStamp()

	WatchDog_Start(-1);
	Core_Start(0);
	Radio_Start(1);
	DS18B20_Start(2);
	FloorHeater_Start(3);
	RGBW_Start(4);

//	uint8_t data[RF_MAX_PAYLOAD_LENGTH-1];

#if LCD1602_PRESENT
	InitializeLCD(); //Инициализация дисплея
	ClearLCDScreen();//Очистка дисплея от мусора
#endif

//    LCD_PutSignedInt(istr);
//    PrintStr("CXEM.NET");

	while (TRUE)
	{
#if LCD1602_PRESENT
		data[0]=2;
		int16_t tmp;
		Radio_Send_Command(10, RF_GET, 1, data);
		data[0]=3;
		Radio_Send_Command(10, RF_GET, 1, data);
		chThdSleepSeconds(3);
		Cursor(0,0); //Установка курсора
		PrintStr("T=");//Написание текста
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
		PrintStr("PWR="); //Написание текста
		tmp = PutData[3][3]*256+PutData[3][4];
//	    LCD_PutSignedInt(tmp>>2);
//	    PrintStr(".");
		LCD_PutUnsignedInt(tmp);
		PrintStr("              ");
#endif
		chThdSleepSeconds(180);
	}
}
