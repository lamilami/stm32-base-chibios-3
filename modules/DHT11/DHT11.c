#include "ch.h"
#include "hal.h"
#include "core.h"

#if DHT11_PRESENT

#include "DHT11_hal.h"
#include "DHT11.h"

#include <stdlib.h>

static thread_reference_t Update_Thread;
static thread_reference_t DHT11_Thread;

typedef struct {
  uint8_t id;
  volatile DHT11_Inner_Val Inner_Val_DHT11;
  virtual_timer_t timer;
  dht11_t DHTD;
} DHT_Working_Struct_t;

//volatile uint16_t out_temp[4];
static core_base_struct_t Core_DHT11;
//volatile DHT11_Inner_Val Inner_Val_DHT11[DHT11_QUANTITY];
static DHT_Working_Struct_t DHT[FH_QUANTITY];

void DHT11_Init() {
  Core_DHT11.type = DHT11;
//    Core_DHT11.Auto_Update_Sec = 180;
//	Core_DHT11.direction = RW;
//	Core_DHT11.next = NULL;
//	Core_DHT11.description = "DHT11 Hum&Temp Sensor";
  Core_DHT11.ival_size = sizeof(DHT11_Inner_Val);
  Core_DHT11.ival_rw_size = sizeof(DHT11_Inner_Val_RW);

  uint8_t x;

  for (x = 0; x < DHT11_QUANTITY; x++) {
    Core_DHT11.inner_values[x] = &DHT[x].Inner_Val_DHT11;
    DHT[x].Inner_Val_DHT11.RW.Auto_Update_Sec = 1;
    DHT[x].Inner_Val_DHT11.cont_errors = 0;
    DHT[x].Inner_Val_DHT11.global_errors = 0;
    DHT[x].Inner_Val_DHT11.humidity = -99;
    DHT[x].Inner_Val_DHT11.temp = -99 << 2;
    DHT[x].id = x;
  }

  DHT[0].DHTD.ext_pin = DHT11_0_PIN;
  DHT[0].DHTD.ext_port = DHT11_0_PORT;
  DHT[0].DHTD.ext_drv = &EXTD1;
  DHT[0].DHTD.ext_mode = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | DHT11_0_EXT_PORT;
  dht11Init(&DHT[0].DHTD);

#if (DHT11_QUANTITY>1)
  DHT[1].DHTD.ext_pin = DHT11_1_PIN;
  DHT[1].DHTD.ext_port = DHT11_1_PORT;
  DHT[1].DHTD.ext_drv = &EXTD1;
  DHT[1].DHTD.ext_mode = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | DHT11_1_EXT_PORT;
  dht11Init(&DHT[1].DHTD);
#endif
//	Core_Module_Register(&Core_DHT11);
}

void dht_timer_handler(void *p) {
  DHT_Working_Struct_t *DHT_struct = (DHT_Working_Struct_t *)p;
  osalSysLockFromISR();
  chEvtSignalI(DHT11_Thread, (eventmask_t)(EVENT_MASK(DHT_struct->id) << EVENTMASK_MOD_PRIVATE_SHIFT));
//  chEvtSignalI(FH_Thread, (eventmask_t)(256));
  chVTSetI(&DHT_struct->timer, S2ST(DHT_struct->Inner_Val_DHT11.RW.Auto_Update_Sec), dht_timer_handler, DHT_struct);
  osalSysUnlockFromISR();
}

THD_WORKING_AREA(waDHT11_thread, 64);
//__attribute__((noreturn))
THD_FUNCTION(DHT11_thread,arg) {
  (void)arg;

  DHT11_Thread = chThdGetSelfX();

  uint8_t x;
  osalSysLock();
  for (x = 0; x < DHT11_QUANTITY; x++) {
    chVTSetI(&DHT[x].timer, S2ST(DHT[x].Inner_Val_DHT11.RW.Auto_Update_Sec), dht_timer_handler, &DHT[x]);
  }
  osalSysUnlock();

  chEvtGetAndClearEvents(ALL_EVENTS);

  while (TRUE) {
    eventmask_t evt = 0;
    uint8_t sensor_id = 0xFF;

    evt = chEvtWaitOne(ALL_EVENTS);

    if (evt == EVENTMASK_REREAD) {
      osalSysLock();
      _core_wakeup_i(&Update_Thread, MSG_OK);
      osalSysUnlock();
    }
    else {
      switch (evt) {
      case (EVENT_MASK(0) << EVENTMASK_MOD_PRIVATE_SHIFT):
        sensor_id = 0;
        break;
#if DHT11_QUANTITY >1
      case (EVENT_MASK(1) << EVENTMASK_MOD_PRIVATE_SHIFT):
        sensor_id = 1;
        break;
#endif
#if DHT11_QUANTITY >2
        case (EVENT_MASK(2) << EVENTMASK_MOD_PRIVATE_SHIFT):
        sensor_id = 2;
        break;
#endif
#if DHT11_QUANTITY >3
        case (EVENT_MASK(3) << EVENTMASK_MOD_PRIVATE_SHIFT):
        sensor_id = 3;
        break;
#endif
      }

      if (sensor_id < DHT11_QUANTITY) {

        uint8_t error = 0;

        int8_t humidity, temperature;

        if (dht11Update(&DHT[sensor_id].DHTD, 3) != DHT11_READ_OK) {
          error++;
          humidity = -99;
          temperature = -99;
        }
        else {
//          error = 0;
          dht11GetHumidity(&DHT[sensor_id].DHTD, &humidity);
          dht11GetTemperature(&DHT[sensor_id].DHTD, &temperature);
        }

        osalSysLock();

        DHT[sensor_id].Inner_Val_DHT11.temp = temperature << 2;
        DHT[sensor_id].Inner_Val_DHT11.humidity = humidity;
        if (error == 0) {
          DHT[sensor_id].Inner_Val_DHT11.cont_errors = 0;
        }
        else {
          DHT[sensor_id].Inner_Val_DHT11.global_errors++;
          DHT[sensor_id].Inner_Val_DHT11.cont_errors++;
        }

        osalSysUnlock();
      }
    }
  }
}

void DHT11_Start() {
#if DHT11_PRESENT
  DHT11_Init();
  thread_t* thd = chThdCreateStatic(waDHT11_thread, sizeof(waDHT11_thread), HIGHPRIO, DHT11_thread, NULL);
  Core_Module_Register(&Core_DHT11, thd, &Update_Thread);
//	Core_Register_Thread(DHT11, thd, &Update_Thread);
  chThdYield();
#endif
}
#endif
