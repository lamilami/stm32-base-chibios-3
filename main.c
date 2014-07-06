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

//#include "test.h"

#ifdef DEBUG_Discovery
static thread_t *Blinker_Thread = NULL;
/*
 * LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waLedBlinker, 128);
//__attribute__((noreturn))
static THD_FUNCTION(LedBlinker,arg)
{

	(void) arg;
	chRegSetThreadName("LedBlinker");
	while (TRUE)
	{
		msg_t msg;
//		msg=255;
		chSysLock();
		Blinker_Thread = chThdGetSelfX();
//		chSchGoSleepS(CH_STATE_SUSPENDED);
//		msg = chThdGetSelfX()->p_u.rdymsg; /* Retrieving the message, optional.*/
		chSysUnlock();
//		uint16_t cnt = 0;
//		chThdYield();
		chThdSleepMilliseconds(250);
		LEDSwap();
/*		for (cnt = 0; cnt < 65535; cnt++)
		{
			msg++;
//			nop();
//			LEDSwap();
//			chThdSleepMilliseconds(100);
//			LEDSwap();
//			chThdSleepMilliseconds(100);
		}*/
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

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waTransmit, 128);
//__attribute__((noreturn))
static THD_FUNCTION (Transmit,arg)
{

	(void) arg;
	chRegSetThreadName("Transmitter");
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

	DS18B20_Start();
	FloorHeater_Start();

	while (TRUE)
	{
		chThdSleepSeconds(5);
	}
}
