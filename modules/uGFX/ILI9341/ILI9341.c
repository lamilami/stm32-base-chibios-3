#include "ch.h"
#include "hal.h"
#include "core.h"

//#if ILI9341_PRESENT
#if 1

#include "printf.h"

static thread_t *tp;
volatile static coord_t width, height;
#define ALARM_PERIOD_MS 1000
#define NUMBERS_WIDTH 35
#define GRAPH_NUMBERS_WIDTH 30
#define GRAPH_HEIGHT    60
#define GRAPH_COLOR     Grey
#define NUMBERS_HEIGHT  24
#define HEIGHT_24       24
#define HEIGHT_32       32
#define COLON_WIDTH     4
#define TEXT_COLOR      White
#define BG_COLOR        Green
#define OT_BG_COLOR     HTML2COLOR(0x660066)
#define TEMP_BG_COLOR   HTML2COLOR(0x820000)
#define HUM_BG_COLOR    HTML2COLOR(0x003264)
#define TEXT_1_COLOR    HTML2COLOR(0x00FFFF)
#define TEXT_2_COLOR    Yellow
#define TEXT_3_COLOR    HTML2COLOR(0x00B050)
COLOR_TYPE Text_Color[3] = {TEXT_1_COLOR, TEXT_2_COLOR, TEXT_3_COLOR};
COLOR_TYPE BG_Color[2] = {TEMP_BG_COLOR, HUM_BG_COLOR};

static void rtc_cb(RTCDriver *rtcp, rtcevent_t event) {

  (void)rtcp;

  switch (event) {
  case RTC_EVENT_OVERFLOW:
    break;
  case RTC_EVENT_SECOND:
    /* Wakes up the thread.*/
    chSysLockFromISR();
    chEvtSignalI(tp, (eventmask_t)1);
    chSysUnlockFromISR();
    break;
  case RTC_EVENT_ALARM:
    /* Wakes up the thread.*/
    chSysLockFromISR();
    chEvtSignalI(tp, (eventmask_t)2);
    chSysUnlockFromISR();
    break;
  }
}

void Redraw_Graph(coord_t x_start, coord_t graph_width, int16_t old_min[2], int16_t min[2], float old_range[2],
                  float range[2], graph_elem_t* Graph_Elem, graph_elem_t* Vals_Cur, bool shift) {
  static graph_elem_t Vals_prev[2];
  register uint8_t x, num, val;
  graph_elem_t* Curr_Elem;

//  Curr_Elem = Graph_Elem;

  for (val = 0; val < 2; val++) {
    for (num = 0; num < 3; num++) {
      Vals_prev[0].val[val][num] = Vals_prev[1].val[val][num] = -99;
      for (x = 0; x < graph_width; x++) {
        Curr_Elem = Graph_Elem + x;
        if ((shift) || (old_range[val] != range[val])) {
          if (Curr_Elem->val[val][num] > -50) {
            if (Vals_prev[0].val[val][num] > -50) {
              gdispDrawLine(x + val * (width / 2) + x_start,
                            height - NUMBERS_HEIGHT - 17 - (Vals_prev[0].val[val][num] - old_min[val]) * old_range[val],
                            x + 1 + val * (width / 2) + x_start,
                            height - NUMBERS_HEIGHT - 17 - (Curr_Elem->val[val][num] - old_min[val]) * old_range[val],
                            BG_Color[val]);
            }
            Vals_prev[0].val[val][num] = Curr_Elem->val[val][num];
          }
        }
        if (shift) {
          if (x < graph_width - 1) {
            Curr_Elem->val[val][num] = (Curr_Elem + 1)->val[val][num];
          }
          else {
            if ((ABS(Vals_Cur->val[0][num] - Vals_prev[1].val[0][num]) < 10) || (Vals_prev[1].val[val][num] == -99))
              Curr_Elem->val[val][num] = Vals_Cur->val[val][num];
            else
              Curr_Elem->val[val][num] = -99;
          }
        }
        if (Curr_Elem->val[val][num] > -50) {
          if (Vals_prev[1].val[val][num] > -50) {
            gdispDrawLine(x + val * (width / 2) + x_start,
                          height - NUMBERS_HEIGHT - 17 - (Vals_prev[1].val[val][num] - min[val]) * range[val],
                          x + 1 + val * (width / 2) + x_start,
                          height - NUMBERS_HEIGHT - 17 - (Curr_Elem->val[val][num] - min[val]) * range[val],
                          Text_Color[num]);
          }
          Vals_prev[1].val[val][num] = Curr_Elem->val[val][num];
        }
      }
    }
  }
}

graph_elem_t Get_Mean(graph_elem_t* Graph_Elem, uint8_t qty) {
  graph_elem_t Mean;
  uint8_t counter[2][3] = {};
  register uint8_t x, num, val;
  uint16_t sum[2][3] = {};
  graph_elem_t* Curr_Elem;
  for (val = 0; val < 2; val++) {
    for (num = 0; num < 3; num++) {
      for (x = 0; x < qty; x++) {
        Curr_Elem = Graph_Elem + x;
        if (Curr_Elem->val[val][num] > -50) {
          sum[val][num] += Curr_Elem->val[val][num];
          counter[val][num]++;
        }
      }
      if (counter[val][num] > 0) {
        Mean.val[val][num] = (int8_t)((float)sum[val][num] / counter[val][num] + 0.5);
      }
      else {
        Mean.val[val][num] = -99;
      }
    }
  }
  return Mean;
}

THD_WORKING_AREA(waILI9341, 2048);
//__attribute__((noreturn))
THD_FUNCTION(ILI9341,arg) {
  (void)arg;
//  thread_t *answer_thread;

  tp = chThdGetSelfX();

//  coord_t width, height;
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
//  static systime_t time_start;
//  time_start = chVTGetSystemTime();
//  uint32_t seconds = 12, min = 49, hours = 11;
  struct tm DT_StructTM, DT_StructTM_old;
  /*
   DT_StructTM.tm_year = 114;
   DT_StructTM.tm_mon = 11;
   DT_StructTM.tm_mday = 22;
   DT_StructTM.tm_isdst = 0;
   DT_StructTM.tm_wday = 0;

   DT_StructTM.tm_hour = 20;
   DT_StructTM.tm_min = 17;
   DT_StructTM.tm_sec = 07;

   DT_StructTM_old = DT_StructTM;
   */

  DT_StructTM_old.tm_sec = -1;
  DT_StructTM_old.tm_min = -1;
  DT_StructTM_old.tm_hour = -1;

  RTCDateTime DateTime;
//  RTCAlarm AlarmTime;

  rtcConvertStructTmToDateTime(&DT_StructTM, 0, &DateTime);

//  rtcSetTime(&RTCD1, &DateTime);

//  chThdSleepSeconds(1000);

  font_t font12 = gdispOpenFont("DejaVuSans12");
  font_t font24 = gdispOpenFont("DejaVuSans24");
  font_t font32 = gdispOpenFont("DejaVuSans32");
  gwinSetDefaultFont(font24);

  gdispClear(BG_COLOR);

  gdispFillStringBox(0, 0, width, HEIGHT_24, "GrowBox", font24, Red, BG_COLOR, justifyCenter);
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

//  gdispDrawBox(5, height - NUMBERS_HEIGHT - GRAPH_HEIGHT - 25, width / 2 - 10, GRAPH_HEIGHT + 20, GRAPH_COLOR);
//  gdispDrawBox(width / 2 + 5, height - NUMBERS_HEIGHT - GRAPH_HEIGHT - 25, width / 2 - 10, GRAPH_HEIGHT + 20, GRAPH_COLOR);

  static graph_elem_t Vals_Cur_s10, Vals_Cur_m5, Vals_Cur_h1, Vals_Cur_d1, Vals_old_s10;
  graph_t Graph_Data;
  uint8_t x, num, val;

  for (val = 0; val < 2; val++) {
    gdispDrawLine((width / 2) * val + 5, height - NUMBERS_HEIGHT - 5, (width / 2) * (val + 1) - 3,
                  height - NUMBERS_HEIGHT - 5, GRAPH_COLOR);
//    gdispDrawLine(width / 2 + 5, height - NUMBERS_HEIGHT - 5, width - 3, height - NUMBERS_HEIGHT - 5, GRAPH_COLOR);

    gdispDrawLine((width / 2) * (val + 1) - 3, height - NUMBERS_HEIGHT - 5, (width / 2) * (val + 1) - 3,
                  height - NUMBERS_HEIGHT - GRAPH_HEIGHT - 30, GRAPH_COLOR);
//    gdispDrawLine(width - 3, height - NUMBERS_HEIGHT - 5, width - 3, height - NUMBERS_HEIGHT - GRAPH_HEIGHT - 30,
//                  GRAPH_COLOR);

    gdispDrawLine((width / 2) * val + 10 + D1_VALS + H1_VALS + M5_VALS + 3, height - NUMBERS_HEIGHT - 5,
                  (width / 2) * val + 10 + D1_VALS + H1_VALS + M5_VALS + 3, height - NUMBERS_HEIGHT - GRAPH_HEIGHT - 25,
                  GRAPH_COLOR);
//    gdispDrawLine(width / 2 + 10 + D1_VALS + H1_VALS + M5_VALS + 3, height - NUMBERS_HEIGHT - 5,
//                  width / 2 + 10 + D1_VALS + H1_VALS + M5_VALS + 3, height - NUMBERS_HEIGHT - GRAPH_HEIGHT - 25,
//                  GRAPH_COLOR);

    gdispDrawLine((width / 2) * val + 10 + D1_VALS + H1_VALS + 2, height - NUMBERS_HEIGHT - 5,
                  (width / 2) * val + 10 + D1_VALS + H1_VALS + 2, height - NUMBERS_HEIGHT - GRAPH_HEIGHT - 25,
                  GRAPH_COLOR);

    gdispDrawLine((width / 2) * val + 10 + D1_VALS + 1, height - NUMBERS_HEIGHT - 5,
                  (width / 2) * val + 10 + D1_VALS + 1, height - NUMBERS_HEIGHT - GRAPH_HEIGHT - 25, GRAPH_COLOR);
//  gdispDrawLine(val * (width / 2) + x_start, height - NUMBERS_HEIGHT - 5, val * (width / 2) + x_start,
//               height - NUMBERS_HEIGHT - 17 - (Curr_Elem->val[val][num] - old_min[val]) * old_range[val], GRAPH_COLOR);
  }

  /*
   *   Using GWin
   */

//    gwinSetDefaultFont(gdispOpenFont("DejaVuSans16"));
//    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
//    gdispClear(White);
  for (val = 0; val < 2; val++) {
    for (num = 0; num < 3; num++) {
      for (x = 0; x < S10_VALS + M5_VALS + H1_VALS + D1_VALS; x++) {
        Graph_Data.through[x].val[val][num] = -99;
      }
      Vals_old_s10.val[val][num] = Vals_Cur_s10.val[val][num] = -99;
    }
  }

  osalThreadSleepSeconds(1);

  rtcSetCallback(&RTCD1, rtc_cb);

  while (TRUE) {

    char buf[5];
    rtcGetTime(&RTCD1, &DateTime);
    /*
     AlarmTime = DateTime;
     AlarmTime.millisecond = AlarmTime.millisecond - (AlarmTime.millisecond%ALARM_PERIOD_MS) + ALARM_PERIOD_MS;
     rtcSetAlarm(&RTCD1, 0, &AlarmTime);
     */
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
    static int16_t temp_old = -99;
//		msg_t msg;
//		msg = Core_Module_Update(Temp, NULL, 1000);
    DS_Temp_Vals.temp[0] = -77 << 2;
    Core_Module_Update(Temp, NULL, 1000);
    Core_Module_Read(localhost, Temp, (char*)&DS_Temp_Vals);
    if ((DS_Temp_Vals.cont_errors > 0))     // || (msg != MSG_OK))
      DS_Temp_Vals.temp[0] = -99 << 2;

    if (temp_old != DS_Temp_Vals.temp[0]) {
      temp_old = DS_Temp_Vals.temp[0];
      sprintf_(buf, "%02f", (float)DS_Temp_Vals.temp[0] / 4);
      gdispFillStringBox(width - 120, HEIGHT_24, 120, HEIGHT_32, buf, font32, TEXT_COLOR, OT_BG_COLOR, justifyRight);
    }

//    static graph_elem_t Vals_Cur = {}, Vals_old = {}, Vals_prev[2];

    Vals_Cur_s10.temp[2] = DS_Temp_Vals.temp[0] / 4;

    DS_Temp_Vals.temp[0] = -77 << 2;
//    Core_Module_Update(Temp, NULL, 1000);
    Core_Module_Read(10, Temp, (char*)&DS_Temp_Vals);
    if ((DS_Temp_Vals.cont_errors > 0))     // || (msg != MSG_OK))
      DS_Temp_Vals.temp[0] = -99 << 2;

    //   Vals[0].temp[0] = DS_Temp_Vals.temp[0];
    //   Vals[0].hum[1] = DS_Temp_Vals.temp[0];

//    Vals_Cur_s10.hum[1] = DS_Temp_Vals.temp[0] / 4;
    Vals_Cur_s10.temp[1] = DS_Temp_Vals.temp[0] / 4;
    Vals_Cur_s10.hum[1] = DS_Temp_Vals.temp[0] / 2;

    static DHT11_Inner_Val DHT_Temp_Vals;
//    Core_Module_Update(DHT11, NULL, 3000);
//    chThdSleepSeconds(1);
    DHT_Temp_Vals.temp = -77 << 2;
    DHT_Temp_Vals.humidity = -99;
    Core_Module_Read(10, DHT11, (char*)&DHT_Temp_Vals);

    Vals_Cur_s10.hum[0] = DHT_Temp_Vals.humidity;
    Vals_Cur_s10.temp[0] = DHT_Temp_Vals.temp / 4;

//    uint8_t x, num, val;

    for (val = 0; val < 2; val++) {
      for (num = 0; num < 3; num++) {
        if (Vals_old_s10.val[val][num] != Vals_Cur_s10.val[val][num]) {
          Vals_old_s10.val[val][num] = Vals_Cur_s10.val[val][num];
          sprintf_(buf, "%02i ", Vals_Cur_s10.val[val][num]);
//          gdispFillStringBox(width - 120, HEIGHT_24, 120, HEIGHT_32, buf, font32, TEXT_COLOR, OT_BG_COLOR, justifyRight);
          gdispFillStringBox((width / 2) * (val + 1) - 36 - 5, HEIGHT_24 * (num + 1) + HEIGHT_32, 36, HEIGHT_24, buf,
                             font24, Text_Color[num], BG_Color[val], justifyRight);
//          gdispFillStringBox(width / 2, HEIGHT_24 * 2 + HEIGHT_32, width / 2, HEIGHT_24, "Hum 2:", font24, TEXT_2_COLOR,
//                             HUM_BG_COLOR, justifyLeft);
//          gdispFillStringBox(width / 2, HEIGHT_24 * 3 + HEIGHT_32, width / 2, HEIGHT_24, "Hum 3:", font24, TEXT_3_COLOR,
//                             HUM_BG_COLOR, justifyLeft);
        }
      }
    }

    if ((DateTime.millisecond - (DateTime.millisecond % 1000)) % ALARM_PERIOD_MS == 0) {
//      bool redraw;
//        redraw = FALSE;
      int16_t max[2] = {-99, -99}, min[2] = {99, 99};
      static int16_t old_min[2] = {99, 99}, old_max[2] = {99, 99};
      static uint8_t cnt_s10 = 0, cnt_m5 = 0, cnt_h1 = 0;

      if (cnt_h1 >= D1_PERIOD) {
        Vals_Cur_d1 = Get_Mean(Graph_Data.h1, D1_PERIOD);
      }

      if (cnt_m5 >= H1_PERIOD) {
        Vals_Cur_h1 = Get_Mean(Graph_Data.m5, H1_PERIOD);
      }

      if (cnt_s10 >= M5_PERIOD) {
        Vals_Cur_m5 = Get_Mean(Graph_Data.s10, M5_PERIOD);
      }

      static float old_range[2] = {GRAPH_HEIGHT, GRAPH_HEIGHT}, range[2];

      for (val = 0; val < 2; val++) {
        for (num = 0; num < 3; num++) {
          for (x = 0; x < S10_VALS + M5_VALS + H1_VALS + D1_VALS; x++) {
            if (Graph_Data.through[x].val[val][num] > -50) {
              max[val] = MAX(max[val], Graph_Data.through[x].val[val][num]);
              min[val] = MIN(min[val], Graph_Data.through[x].val[val][num]);
            }
          }
          if (Vals_Cur_s10.val[val][num] > -50) {
            max[val] = MAX(max[val], Vals_Cur_s10.val[val][num]);
            min[val] = MIN(min[val], Vals_Cur_s10.val[val][num]);
          }
        }
        if (max[val] != min[val])
          range[val] = GRAPH_HEIGHT / (float)(max[val] - min[val]);
        else
          range[val] = 0;

        if ((old_min[val] != min[val]) || (old_max[val] != max[val])) {
          sprintf_(buf, "%02i", min[val]);
          gdispFillStringBox((width / 2) * (val + 1) - 21, height - NUMBERS_HEIGHT - 22, 15, 10, buf, font12,
                             GRAPH_COLOR, BG_Color[val], justifyRight);
          if (min[val] != max[val])
            sprintf_(buf, "%02i", max[val]);
          else
            sprintf_(buf, "    ", max[val]);
          gdispFillStringBox((width / 2) * (val + 1) - 21, height - NUMBERS_HEIGHT - 22 - GRAPH_HEIGHT, 15, 10, buf,
                             font12, GRAPH_COLOR, BG_Color[val], justifyRight);
        }
      }

      if (cnt_h1 >= D1_PERIOD) {
        Redraw_Graph(10, D1_VALS, &old_min[0], &min[0], &old_range[0], &range[0], &Graph_Data.d1[0], &Vals_Cur_d1,
                     TRUE);
        cnt_h1 = 0;
      }
      if (cnt_m5 >= H1_PERIOD) {
        Redraw_Graph(10 + D1_VALS + 1, H1_VALS, &old_min[0], &min[0], &old_range[0], &range[0], &Graph_Data.h1[0],
                     &Vals_Cur_h1, TRUE);
        cnt_m5 = 0;
        cnt_h1++;
      }
      if (cnt_s10 >= M5_PERIOD) {
        Redraw_Graph(10 + D1_VALS + H1_VALS + 2, M5_VALS, &old_min[0], &min[0], &old_range[0], &range[0],
                     &Graph_Data.m5[0], &Vals_Cur_m5, TRUE);
        cnt_s10 = 0;
        cnt_m5++;
      }
//      else if ((old_range[0] != range[0]) || (old_range[1] != range[1]) || (old_min[0] != min[0])
//          || (old_min[1] != min[1])) {
//        Redraw_Graph(10 + D1_VALS + H1_VALS + 2, M5_VALS, &old_min[0], &min[0], &old_range[0], &range[0],
//                     &Graph_Data.m5[0], &Vals_Cur_m5, FALSE);
//      }
      Redraw_Graph(10 + D1_VALS + H1_VALS + M5_VALS + 3, S10_VALS, &old_min[0], &min[0], &old_range[0], &range[0],
                   &Graph_Data.s10[0], &Vals_Cur_s10, TRUE);

      if ((old_max[0] != max[0]) || (old_max[1] != max[1]) || (old_min[0] != min[0]) || (old_min[1] != min[1])) {
        old_min[0] = min[0];
        old_min[1] = min[1];
        old_max[0] = max[0];
        old_max[1] = max[1];
        old_range[0] = range[0];
        old_range[1] = range[1];

        gdispFillArea(10 + 1, height - NUMBERS_HEIGHT - 17 - GRAPH_HEIGHT, D1_VALS, GRAPH_HEIGHT + 1, TEMP_BG_COLOR);
        gdispFillArea(10 + 1 + (width / 2), height - NUMBERS_HEIGHT - 17 - GRAPH_HEIGHT, D1_VALS, GRAPH_HEIGHT + 1,
                      HUM_BG_COLOR);
        Redraw_Graph(10, D1_VALS, &old_min[0], &min[0], &old_range[0], &range[0], &Graph_Data.d1[0], &Vals_Cur_d1,
                     FALSE);

        gdispFillArea(10 + D1_VALS + 2, height - NUMBERS_HEIGHT - 17 - GRAPH_HEIGHT, H1_VALS, GRAPH_HEIGHT + 1,
                      TEMP_BG_COLOR);
        gdispFillArea(10 + D1_VALS + 2 + (width / 2), height - NUMBERS_HEIGHT - 17 - GRAPH_HEIGHT, H1_VALS,
                      GRAPH_HEIGHT + 1, HUM_BG_COLOR);
        Redraw_Graph(10 + D1_VALS + 1, H1_VALS, &old_min[0], &min[0], &old_range[0], &range[0], &Graph_Data.h1[0],
                     &Vals_Cur_h1, FALSE);
        gdispFillArea(10 + D1_VALS + H1_VALS + 3, height - NUMBERS_HEIGHT - 17 - GRAPH_HEIGHT, M5_VALS,
                      GRAPH_HEIGHT + 1, TEMP_BG_COLOR);
        gdispFillArea(10 + D1_VALS + H1_VALS + 3 + (width / 2), height - NUMBERS_HEIGHT - 17 - GRAPH_HEIGHT, M5_VALS,
                      GRAPH_HEIGHT + 1, HUM_BG_COLOR);
        Redraw_Graph(10 + D1_VALS + H1_VALS + 2, M5_VALS, &old_min[0], &min[0], &old_range[0], &range[0],
                     &Graph_Data.m5[0], &Vals_Cur_m5, FALSE);
        Redraw_Graph(10 + D1_VALS + H1_VALS + M5_VALS + 3, S10_VALS, &old_min[0], &min[0], &old_range[0], &range[0],
                     &Graph_Data.s10[0], &Vals_Cur_s10, FALSE);
      }

      cnt_s10++;
    }
    //    time_start = chThdSleepUntilWindowed(time_start, time_start + S2ST(1));
    chEvtWaitOne((eventmask_t)1);

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
