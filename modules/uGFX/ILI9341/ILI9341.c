#include "ch.h"
#include "hal.h"
#include "core.h"

#if ILI9341_PRESENT

#include "printf.h"

#define NUMBERS_WIDTH 35
#define NUMBERS_HEIGHT 24
#define HEIGHT_24   24
#define HEIGHT_32   32
#define COLON_WIDTH 4
#define TEXT_COLOR White
#define BG_COLOR Green
#define OT_BG_COLOR HTML2COLOR(0x660066)
#define TEMP_BG_COLOR   HTML2COLOR(0x820000)
#define HUM_BG_COLOR    HTML2COLOR(0x003264)
#define TEXT_1_COLOR    Red
#define TEXT_2_COLOR    Yellow
#define TEXT_3_COLOR    HTML2COLOR(0x00B050)

THD_WORKING_AREA(waILI9341, 2048);
//__attribute__((noreturn))
THD_FUNCTION(ILI9341,arg) {
  (void)arg;
//  thread_t *answer_thread;

  coord_t width, height;
//  coord_t i, j;

  /*
   * This initialization requires the OS already active because it uses delay
   * APIs inside.
   */

// Get the screen size
  width = gdispGetWidth();
  height = gdispGetHeight();

  /*
   gdispDrawBox(10, 10, width / 2, height / 2, Yellow);
   gdispFillArea(width / 2, height / 2, width / 2 - 10, height / 2 - 10, Blue);
   gdispDrawLine(5, 30, width - 50, height - 40, Red);

   for (i = 5, j = 0; i < width && j < height; i += 7, j += i / 20)
   gdispDrawPixel(i, j, Green);

   font_t font = gdispOpenFont("DejaVuSans24");
   gwinSetDefaultFont(font);

   gdispDrawString(16, 16, "STM32F100C by Michail", font, White);

   gdispDrawStringBox(0, 150, width, 40, "ChibiOS/RT + uGFX", font, White,
   justifyCenter);
   */
//  chThdSleepMilliseconds(5000);
  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state
   */

//  gdispClear(Black);
  /*
   gdispDrawStringBox(0, 0, width, 24, "ADC Results:", font, Green,
   justifyCenter);
   gdispFillStringBox(0, (height - 24), width / 2, 24, "Seconds:", font,
   Yellow, Green, justifyRight);
   */
//    chThdSleepMilliseconds(5000);
  static systime_t time_start;
  time_start = chVTGetSystemTime();
//  uint32_t seconds = 12, min = 49, hours = 11;

  struct tm DT_StructTM, DT_StructTM_old;

  DT_StructTM.tm_year = 114;
  DT_StructTM.tm_mon = 11;
  DT_StructTM.tm_mday = 22;
  DT_StructTM.tm_isdst = 0;
  DT_StructTM.tm_wday = 0;

  DT_StructTM.tm_hour = 20;
  DT_StructTM.tm_min = 17;
  DT_StructTM.tm_sec = 07;

  DT_StructTM_old = DT_StructTM;
  DT_StructTM_old.tm_sec--;
  DT_StructTM_old.tm_min--;
  DT_StructTM_old.tm_hour--;

  RTCDateTime DateTime;

  rtcConvertStructTmToDateTime(&DT_StructTM, 0, &DateTime);

//  rtcSetTime(&RTCD1, &DateTime);

//  chThdSleepSeconds(1000);

  font_t font24 = gdispOpenFont("DejaVuSans24");
  font_t font32 = gdispOpenFont("DejaVuSans32");
  gwinSetDefaultFont(font24);

  gdispClear(BG_COLOR);

  gdispFillStringBox(0, 0, width, HEIGHT_24, "GrowBox", font24, TEXT_COLOR, BG_COLOR, justifyCenter);
  gdispFillStringBox(0, HEIGHT_24, width, HEIGHT_32, "Out. Temp:", font32, TEXT_COLOR, OT_BG_COLOR, justifyLeft);

  gdispFillStringBox(0, HEIGHT_24 + HEIGHT_32, width / 2, HEIGHT_24, "Temp 1:", font24, TEXT_1_COLOR, TEMP_BG_COLOR,
                     justifyLeft);
  gdispFillStringBox(0, HEIGHT_24 * 2 + HEIGHT_32, width / 2, HEIGHT_24, "Temp 2:", font24, TEXT_2_COLOR, TEMP_BG_COLOR,
                     justifyLeft);
  gdispFillStringBox(0, HEIGHT_24 * 3 + HEIGHT_32, width / 2, HEIGHT_24, "Temp 3:", font24, TEXT_3_COLOR, TEMP_BG_COLOR,
                     justifyLeft);
  gdispFillArea(0, HEIGHT_24 * 4 + HEIGHT_32, width / 2, height - (HEIGHT_24 * 4 + HEIGHT_32 + NUMBERS_HEIGHT),
                TEMP_BG_COLOR);

  gdispFillStringBox(width / 2, HEIGHT_24 + HEIGHT_32, width / 2, HEIGHT_24, "Hum 1:", font24, TEXT_1_COLOR,
                     HUM_BG_COLOR, justifyLeft);
  gdispFillStringBox(width / 2, HEIGHT_24 * 2 + HEIGHT_32, width / 2, HEIGHT_24, "Hum 2:", font24, TEXT_2_COLOR,
                     HUM_BG_COLOR, justifyLeft);
  gdispFillStringBox(width / 2, HEIGHT_24 * 3 + HEIGHT_32, width / 2, HEIGHT_24, "Hum 3:", font24, TEXT_3_COLOR,
                     HUM_BG_COLOR, justifyLeft);
  gdispFillArea(width / 2, HEIGHT_24 * 4 + HEIGHT_32, width / 2, height - (HEIGHT_24 * 4 + HEIGHT_32 + NUMBERS_HEIGHT),
                HUM_BG_COLOR);

  /*
   *   Using GWin
   */

//    gwinSetDefaultFont(gdispOpenFont("DejaVuSans16"));
//    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
//    gdispClear(White);
  osalThreadSleepSeconds(1);

  while (TRUE) {

    char buf[5];
    rtcGetTime(&RTCD1, &DateTime);
    rtcConvertDateTimeToStructTm(&DateTime, &DT_StructTM);

    if (DT_StructTM.tm_hour != DT_StructTM_old.tm_hour) {
      sprintf_(buf, "%02u:", DT_StructTM.tm_hour);
      DT_StructTM_old.tm_hour = DT_StructTM.tm_hour;
      gdispFillStringBox(width - NUMBERS_WIDTH * 3 - COLON_WIDTH * 2, (height - NUMBERS_HEIGHT),
                         NUMBERS_WIDTH + COLON_WIDTH, NUMBERS_HEIGHT, buf, font24, TEXT_COLOR, BG_COLOR, justifyLeft);
    }
    if (DT_StructTM.tm_min != DT_StructTM_old.tm_min) {
      sprintf_(buf, "%02u:", DT_StructTM.tm_min);
      DT_StructTM_old.tm_min = DT_StructTM.tm_min;
      gdispFillStringBox(width - NUMBERS_WIDTH * 2 - COLON_WIDTH, (height - NUMBERS_HEIGHT),
                         NUMBERS_WIDTH + COLON_WIDTH, NUMBERS_HEIGHT, buf, font24, TEXT_COLOR, BG_COLOR, justifyLeft);
    }
    if (DT_StructTM.tm_sec != DT_StructTM_old.tm_sec) {
      sprintf_(buf, "%02u ", DT_StructTM.tm_sec);
      DT_StructTM_old.tm_sec = DT_StructTM.tm_sec;
      gdispFillStringBox(width - NUMBERS_WIDTH, (height - NUMBERS_HEIGHT), NUMBERS_WIDTH, NUMBERS_HEIGHT, buf, font24,
                         TEXT_COLOR, BG_COLOR, justifyLeft);
    }

    static DS18B20_Inner_Val DS_Temp_Vals;
    static int16_t temp_old = -999;
//		msg_t msg;
//		msg = Core_Module_Update(Temp, NULL, 1000);
    DS_Temp_Vals.temp[0] = -77 << 2;
    Core_Module_Update(Temp, NULL, 1000);
    Core_Module_Read(localhost, Temp, (char*)&DS_Temp_Vals);
    if ((DS_Temp_Vals.cont_errors > 0))     // || (msg != MSG_OK))
      DS_Temp_Vals.temp[0] = -99 << 2;

    if (temp_old != DS_Temp_Vals.temp[0]) {
      temp_old = DS_Temp_Vals.temp[0];
      sprintf_(buf, "%02f", (float)DS_Temp_Vals.temp[0]/4);
      gdispFillStringBox(width - 120, HEIGHT_24, 120, HEIGHT_32, buf, font32, TEXT_COLOR, OT_BG_COLOR, justifyRight);
    }

    time_start = chThdSleepUntilWindowed(time_start, time_start + S2ST(1));
  }
}

void ILI9341_Start() {
#if ILI9341_PRESENT
//  ILI9341_Init();
  chThdCreateStatic(waILI9341, sizeof(waILI9341), HIGHPRIO, ILI9341, NULL);
//  Core_Register_Thread(Temp, thd, &Update_Thread);
  chThdYield();
#endif
}

#endif
