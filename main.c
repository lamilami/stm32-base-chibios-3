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
#include "onewire.h"

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
 * Blue LED blinker thread, times are in milliseconds.
 */
/*
 static WORKING_AREA(waThread1, 128);
 __attribute__((noreturn))
 static msg_t Thread1(void *arg) {

 (void)arg;
 chRegSetThreadName("blinker1");
 while (TRUE) {
 //    palClearPad(GPIOC, GPIOC_LED4);
 LEDB1Swap();
 chThdSleepMilliseconds(500);
 }
 }*/

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
//    palClearPad(GPIOC, GPIOC_LED3);
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
	 * Activates the serial driver 1 using the driver default configuration.
	 * PA9 and PA10 are routed to USART1.
	 */
//  sdStart(&SD1, NULL);
//  palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(1));       /* USART1 TX.       */
//  palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(1));      /* USART1 RX.       */
	/*
	 * Creates the blinker threads.
	 */
//  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
/*	chThdCreateStatic(waTransmit, sizeof(waTransmit), NORMALPRIO, Transmit,
			NULL);
*/
#ifdef DEBUG_Discovery
	chThdCreateStatic(waLedBlinker, sizeof(waLedBlinker), NORMALPRIO,
			LedBlinker, NULL);
#endif

/*	chThdCreateStatic(waRadio, sizeof(waRadio), NORMALPRIO, Radio, NULL);
	chThdCreateStatic(waRadio_Processor, sizeof(waRadio_Processor), NORMALPRIO,
			Radio_Processor, NULL);
*/

//  chThdSleepSeconds(2);
//  Radio_Send_Command (3, RF_PING);

//  spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
//  spiStart(&SPID1, &hs_spicfg);       /* Setup transfer parameters.       */
//  spiSelect(&SPID1);                  /* Slave Select assertion.          */
////  spiExchange(&SPID1, 512,
////              txbuf, rxbuf);          /* Atomic transfer operations.      */
//  uint16_t data = spiPolledExchange(&SPID1, 0xFF);
//  spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
//  spiReleaseBus(&SPID1);              /* Ownership release.               */

//  nRF24_hw_ce_low();
//  radio_init();
	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state, when the button is
	 * pressed the test procedure is launched with output on the serial
	 * driver 1.
	 */
	LEDB1Swap();
	OW_Init();
	while (TRUE)
	{
	OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ);
//    for (i=0; i<1000000; i++);

	chThdSleepMilliseconds(800);

    uint8_t buf[2];
    OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, buf,2, 2);


		msg_t msg;
//		msg=255;
//		chSysLock();
//		Blinker_Thread = chThdGetSelfX();
//		chSchGoSleepS(CH_STATE_SUSPENDED);
//		msg = chThdGetSelfX()->p_u.rdymsg; /* Retrieving the message, optional.*/
//		chSysUnlock();
//		uint16_t cnt = 0;
//		LEDB1Swap();
//		chThdSleepMilliseconds(100);
/*		for (cnt = 0; cnt < 65535; cnt++)
		{
			msg++;
//			chThdYield();
//			nop();
//			LEDSwap();
//			chThdSleepMilliseconds(100);
//			LEDSwap();
//			chThdSleepMilliseconds(100);
		}*/

		chThdSleepSeconds(5);
	}
}
