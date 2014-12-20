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
//#include "stm32f0xx.h"
#endif
#include "ch.h"
#include "hal.h"
#include "core.h"
#include "halconf.h"
#if SEMIHOSTING
#include "semihosting.h"
#endif
//#include "LM75_hal.h"
//extern void eeprom_cmd_test(BaseSequentialStream *chp, int argc, char *argv[]);

#if LCD1602_PRESENT
#include "lcd.h"
#endif

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

#if PIR_PRESENT

uint16_t PIR_cb() {
	LEDB1Swap();
	return 0;
}
#endif

#if FloorHeater_PRESENT

volatile uint16_t bt136_temp = 0;

uint16_t FloorHeater_cb() {

#if DS18B20_PRESENT

	static DS18B20_Inner_Val Temp_Vals;
	Core_Module_Update(Temp, NULL, 1000);
	Core_Module_Read(Temp, (char*) &Temp_Vals);

#if DS18B20_NUMBER_OF_SENSORS == 1
	return Temp_Vals.temp[0];
#else
	register uint16_t i, Cur_Temp;
	Cur_Temp = 0;
	for (i = 0; i < DS18B20_NUMBER_OF_SENSORS - 1; i++) {
		Cur_Temp += Temp_Vals.temp[i];
	}
	osalSysLock();
	bt136_temp = Temp_Vals.temp[4];
	osalSysUnlock();
#if DS18B20_NUMBER_OF_SENSORS == 3
	Cur_Temp = Cur_Temp >> 1;
#elif DS18B20_NUMBER_OF_SENSORS == 5
	Cur_Temp = Cur_Temp >> 2;
#else
	Cur_Temp = Cur_Temp / DS18B20_NUMBER_OF_SENSORS;
#endif
	return Cur_Temp;

#endif
#elif DHT11_PRESENT

	static DHT11_Inner_Val Temp_Vals;
	Core_Module_Update(DHT11, NULL, 3000);
	Core_Module_Read(DHT11, (char*) &Temp_Vals);

	return Temp_Vals.temp;

//	return 25;

#endif

}

#endif //FloorHeater_PRESENT
/*
 * Application entry point.
 */
int main(void) {

	Core_Start();

	LEDB1Swap();
	LEDSwap();

#if uGFX_PRESENT
	coord_t width, height;
	coord_t i, j;

	/*
	 * This initialization requires the OS already active because it uses delay
	 * APIs inside.
	 */
	// Initialize uGFX and the underlying system
	gfxInit();
	// Get the screen size
	width = gdispGetWidth();
	height = gdispGetHeight();

	gdispDrawBox(10, 10, width / 2, height / 2, Yellow);
	gdispFillArea(width / 2, height / 2, width / 2 - 10, height / 2 - 10, Blue);
	gdispDrawLine(5, 30, width - 50, height - 40, Red);

	for (i = 5, j = 0; i < width && j < height; i += 7, j += i / 20)
		gdispDrawPixel(i, j, Green);

	font_t font = gdispOpenFont("DejaVuSans24");
	gwinSetDefaultFont(font);

	gdispDrawString(16, 16, "STM32VLDiscovery", font, White);

	gdispDrawStringBox(0, 150, width, 40, "ChibiOS/RT + uGFX", font, White,
			justifyCenter);

	chThdSleepMilliseconds(5000);

	  /*
	   * Normal main() thread activity, in this demo it does nothing except
	   * sleeping in a loop and check the button state
	   */

	  gdispClear(Black);
	  gdispDrawStringBox(0, 0, width, 24, "ADC Results:", font, Green, justifyCenter);
	  gdispFillStringBox(0, (height-24), width/2, 24, "Seconds:", font, Yellow, Green, justifyRight);

	  chThdSleepMilliseconds(5000);

	  /*
	   *   Using GWin
	   */

	  gwinSetDefaultFont(gdispOpenFont("DejaVuSans16"));
//	  gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	  gdispClear(White);

#endif

	/*
	 #define NRF_CE_IRQ_Port		GPIOF
	 #define NRF_CE_Pin GPIOF_PIN0

	 palSetPadMode(GPIOF, NRF_CE_Pin,
	 PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); // | PAL_STM32_PUDR_FLOATING);
	 palSetPad(NRF_CE_IRQ_Port, NRF_CE_Pin);

	 chThdSleepSeconds(3);
	 */
//	eeprom_cmd_test(NULL, 0, NULL);
	chThdSleepSeconds(1);

#if MY_ADDR==10
//	Radio_Send_Command(11, RF_PING, 0, NULL);
#else
//	Radio_Send_Command(10, RF_PING, 0, NULL);
#endif

#if WATCHDOG_PRESENT
	WatchDog_Start(15);
#endif

#if LM75_PRESENT
	/* buffers depth */
#define TMP75_RX_DEPTH 2
#define TMP75_TX_DEPTH 2

	/* input buffer */
	static uint8_t tmp75_rx_data[TMP75_RX_DEPTH];

	/* temperature value */
	static int16_t temperature = 0;

	static i2cflags_t errors = 0;

#define tmp75_addr 0b1001000

	i2cStart(&I2CD2,&lm75_i2ccfg);

	/* tune ports for I2C1*/
	palSetPadMode(GPIOB, 10, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
	palSetPadMode(GPIOB, 11, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

	chThdSleepMilliseconds(100); /* Just to be safe. */

	int16_t t_int = 0, t_frac = 0;
	msg_t status = MSG_OK;
	systime_t tmo = MS2ST(4);

	i2cAcquireBus(&I2CD2);
	status = i2cMasterReceiveTimeout(&I2CD2, tmp75_addr, tmp75_rx_data, 2, tmo);
	i2cReleaseBus(&I2CD2);

	if (status != MSG_OK) {
		errors = i2cGetErrors(&I2CD2);
	}

	t_int = tmp75_rx_data[0] * 100;
	t_frac = (tmp75_rx_data[1] * 100) >> 8;
	temperature = t_int + t_frac;
#endif

	systime_t time_start = chVTGetSystemTime();

//	RGBW_IW;

#ifdef RGBW_Test

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

#if PIR_PRESENT
	PIR_Inner_Val_RW PIR_IV;
	PIR_IV.Delay_Seconds=5;
	PIR_IV.PIR_Callback=PIR_cb;
	Core_Module_Update(PIR, (void *) &PIR_IV,1000);
#endif

#if RGBW_PRESENT
	//	RGBW_Inner_Val* RGBW_IV = (RGBW_Inner_Val*) Core_GetIvalAddrByType(RGBW);
	RGBW_Inner_Val RGBW_Day = {}, RGBW_Night = {};
//	RGBW_Inner_Val RGBW_Current;

	RGBW_Day.RW.Red_Set = 10000;
	RGBW_Day.RW.Blue_Set = 10000;
	RGBW_Day.RW.Green_Set = 10000;
	RGBW_Day.RW.Rise_Time_Sec = 30*60;
	RGBW_Day.RW.Max_Delay_Sec = 600;

	RGBW_Night.RW.Red_Set = 0;
	RGBW_Night.RW.Blue_Set = 0;
	RGBW_Night.RW.Green_Set = 0;
	RGBW_Night.RW.Rise_Time_Sec = 30*60;
	RGBW_Night.RW.Max_Delay_Sec = 600;
#endif
//	Core_Module_Read(RGBW, (void *) &RGBW_Current);

#if FloorHeater_PRESENT

	FloorHeater_Inner_Val_RW FH_IV;
	FH_IV.Desired_Temp = 32 << 2;
	FH_IV.Auto_Update_Sec = 180;
	FH_IV.Get_Temp_Callback = FloorHeater_cb;
	FH_IV.iGain = 2;
	FH_IV.pGain = 4;
	FH_IV.iState = 0;
	FH_IV.iMax = 25;
	FH_IV.iMin = -2;
	FH_IV.MaxPower = 100;

	Core_Module_Update(Heater, (void *) &FH_IV, 1000);
	chThdSleepSeconds(1);

#endif

	/*
	 palSetPadMode(GPIOA, GPIOA_PIN1, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	 palSetPad(GPIOA, 1);
	 WatchDog_Start(10);
	 */

	while (TRUE) {

#if DS18B20_PRESENT && !FloorHeater_PRESENT

		static DS18B20_Inner_Val DS_Temp_Vals;
		msg_t msg;
		msg = Core_Module_Update(Temp, NULL, 1000);
		DS_Temp_Vals.temp[0] = -77 << 2;
		Core_Module_Read(localhost, Temp, (char*) &DS_Temp_Vals);
		if ((DS_Temp_Vals.cont_errors > 0) || (msg != MSG_OK))
		DS_Temp_Vals.temp[0] = -99 << 2;

#endif

//	osalThreadSleepSeconds(1);

#if DHT11_PRESENT
		volatile static uint32_t time_cnt = 0;

		static DHT11_Inner_Val DHT_Temp_Vals;
		Core_Module_Update(DHT11, NULL, 3000);
		chThdSleepSeconds(1);
		DHT_Temp_Vals.temp = 77 << 2;
		DHT_Temp_Vals.humidity = 0;
		Core_Module_Read(localhost, DHT11, (char*) &DHT_Temp_Vals);

//		printf("DHT11 Temp: %d, Hum: %d,  Cnt: %d \r\n", (int) DHT_Temp_Vals.temp/4, (int) DHT_Temp_Vals.humidity, time_cnt);
		time_cnt++;
//	return Temp_Vals.temp;

//	return 25;
#if WATCHDOG_PRESENT
		if (time_cnt < 2160)
		WatchDog_Reset();
#endif
#endif

#if SEMIHOSTING
#if DS18B20_PRESENT
		SH_PrintStr("DS Temp="); //Написание текста
		SH_PutSignedQ2(DS_Temp_Vals.temp[0]);
#endif

#if DHT11_PRESENT
		SH_PrintStr("; DHT Temp="); //Написание текста
		SH_PutSignedQ2(DHT_Temp_Vals.temp);
		SH_PrintStr(", Hum=");//Написание текста
		SH_PutUnsignedInt(DHT_Temp_Vals.humidity);
#endif
		SH_PrintStr("\n");
#endif

#if LCD1602_PRESENT

		/*		data[0]=2;
		 int16_t tmp;
		 Radio_Send_Command(10, RF_GET, 1, data);
		 data[0]=3;
		 Radio_Send_Command(10, RF_GET, 1, data);
		 chThdSleepSeconds(3);*/
		ClearLCDScreen();
		Cursor(0, 0); //Установка курсора
		PrintStr("Outer temp=");//Написание текста
		/*	    PutData[2][0]=1;
		 PutData[2][1]=2;
		 PutData[2][2]=3;
		 PutData[2][3]=4;
		 PutData[2][4]=5;*/
		/*		volatile int16_t tmp=0;
		 tmp = DS_Temp_Vals.temp[0];
		 LCD_PutSignedInt(tmp>>2);
		 PrintStr(".");
		 LCD_PutUnsignedInt((tmp&3)*25);*/
		LCD_PutSignedQ2(DS_Temp_Vals.temp[0]);
		PrintStr("              ");
		Cursor(1, 0);
		PrintStr(" Temp="); //Написание текста
//		tmp = DHT_Temp_Vals.temp;
		LCD_PutUnsignedInt(DHT_Temp_Vals.temp >> 2);
		PrintStr(" Hum=");//Написание текста
//		tmp = DHT_Temp_Vals.temp;
		LCD_PutUnsignedInt(DHT_Temp_Vals.humidity);
//	    PrintStr(".");
//		LCD_PutUnsignedInt(tmp);
		PrintStr("    ");
#endif

#if RGBW_PRESENT

		Core_Module_Update(RGBW, (void *) &RGBW_Day, 3000);

//		chThdSleepSeconds(14*60*60);
		/*		chThdSleepMilliseconds(500);
		 Core_Module_Read(RGBW, (void *) &RGBW_Current);
		 chThdSleepMilliseconds(1500);*/
		chThdSleepSeconds(15*60*60);

		Core_Module_Update(RGBW, (void *) &RGBW_Night, 3000);

//		Timeval_Current = 24 * 3600 - Inner_Val_RGBW.Correction_24H;
		time_start = chThdSleepUntilWindowed(time_start,time_start + S2ST(24 * 60*60 - 117));

		/*		eventmask_t evt = chEvtWaitOne(ALL_EVENTS);
		 switch (evt)
		 {
		 case (EVENT_MASK((uint8_t) PIR)):
		 LEDB1Swap();
		 break;
		 default:
		 break;
		 }*/
//		time_start = chThdSleepUntilWindowed(time_start, time_start + S2ST(30));
#endif

#if FloorHeater_PRESENT

		chThdSleepSeconds(180);
		osalSysLock();
		uint16_t tmp_temp = bt136_temp >> 2;
		osalSysUnlock();
#define Half_Load	50
#define Low_Load	1
#define Mid_Temp_Max	65
#define Mid_Temp_Min	55
#define High_Temp_Max	85
#define High_Temp_Min	75
		if (FH_IV.MaxPower >= 100) {
			if (tmp_temp >= High_Temp_Max) {
				FH_IV.MaxPower = Low_Load;
				Core_Module_Update(Heater, (void *) &FH_IV, 1000);
			} else if (tmp_temp >= Mid_Temp_Max) {
				FH_IV.MaxPower = Half_Load;
				Core_Module_Update(Heater, (void *) &FH_IV, 1000);
			}
		} else if (FH_IV.MaxPower >= Half_Load) {
			if (tmp_temp >= High_Temp_Max) {
				FH_IV.MaxPower = Low_Load;
				Core_Module_Update(Heater, (void *) &FH_IV, 1000);
			} else if (tmp_temp <= Mid_Temp_Min) {
				FH_IV.MaxPower = 100;
				Core_Module_Update(Heater, (void *) &FH_IV, 1000);
			}
		} else {
			if (tmp_temp <= High_Temp_Min) {
				FH_IV.MaxPower = Half_Load;
				Core_Module_Update(Heater, (void *) &FH_IV, 1000);
			}
		}
#endif

#ifdef STM32F100C8_old

		//		GPIOD->BRR = 0x2;
		//		chThdSleepMilliseconds(100);
		//		GPIOD->BSRR = 0x2;
//		GPIOC->BRR = 0x2000;
//		 GPIOD->BRR = 0x2;
		palClearPad(GPIOC, GPIOC_PIN13);
//		palSetPad(GPIOD, GPIOD_PIN1);
		/* delay */
//		int i;
//        for(i=0;i<0x50000;i++);
		chThdSleepSeconds(1);
		palSetPad(GPIOC, GPIOC_PIN13);
//		palClearPad(GPIOD, GPIOD_PIN1);
//		 GPIOD->BSRR = 0x2;
//		 GPIOC->BSRR = 0x2000;
		//		WatchDog_Reset();
		/*		chThdSleepMilliseconds(800);
		 palClearPad(GPIOA, 1);
		 chThdSleepMilliseconds(100);
		 palSetPad(GPIOA, 1);
		 chThdSleepMilliseconds(100);
		 palClearPad(GPIOA, 1);*/

		chThdSleepSeconds(1);

//		WatchDog_Reset();

#endif

#if !DS18B20_PRESENT
		static DS18B20_Inner_Val DS_Temp_Vals;
//		msg_t msg;
//		msg = Core_Module_Update(Temp, NULL, 1000);
		DS_Temp_Vals.temp[0] = -77 << 2;
		Core_Module_Read(10, Temp, (char*) &DS_Temp_Vals);
		if ((DS_Temp_Vals.cont_errors > 0)) // || (msg != MSG_OK))
			DS_Temp_Vals.temp[0] = -99 << 2;
#endif

#if !FloorHeater_PRESENT && !RGBW_PRESENT
//		Radio_Send_Command(11, RF_PING, 0, NULL);
		time_start = chThdSleepUntilWindowed(time_start, time_start + S2ST(3));
//		LEDB1Swap();
#endif

	}
}
