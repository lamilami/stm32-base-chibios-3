#include "ch.h"
#include "hal.h"
#include "ext.h"
#include "chconf.h"
#include "core.h"

#if DHT11_PRESENT

#include <DHT11_hal.h>

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

#define EVENTMASK_OK    EVENT_MASK(28)
#define EVENTMASK_ERROR EVENT_MASK(29)

#define _CONCAT(a,b) a ## b
#define CONCAT(a,b) _CONCAT(a,b)
#define GPT_DRV CONCAT(GPTD,DHT_USE_TIMER)

#if !CONCAT(STM32_GPT_USE_TIM,DHT_USE_TIMER)
GPTDriver GPT_DRV;
#endif

//#define CHECK_APB2 CONCAT(CONCAT(RCC_APB1ENR_TIM,17),EN)
#define RCC_BUS CONCAT(RCC_,DHT_TIMER_USE_BUS)
#define RCC_BUS_EN CONCAT(RCC_BUS,ENR_TIM17EN)

//GPTDriver GPTD17;
//dht11_t DHTD1;

#define Timer_Clock 100000

#define TIMx2US(n) ((((n) - 1UL) * (1000000UL / Timer_Clock)) + 1UL)

/*
 * GPT2 configuration.
 */
static const GPTConfig gptxcfg = {Timer_Clock, /* 100kHz timer clock.*/
                                   NULL, /* Timer callback.*/
                                   0, 0};

/**
 * @brief   Enables the TIMx peripheral clock.
 * @note    The @p lp parameter is ignored in this family.
 *
 * @param[in] lp        low power enable flag
 *
 * @api
 */
#define rccEnableTIMx(lp) RCC->CONCAT(DHT_TIMER_USE_BUS,ENR) |= (RCC_BUS_EN);

//rccEnableAPB2(RCC_APB2ENR_TIM17EN, lp)

/**
 * @brief   Disables the TIM3 peripheral clock.
 * @note    The @p lp parameter is ignored in this family.
 *
 * @param[in] lp        low power enable flag
 *
 * @api
 */
#define rccDisableTIMx(lp) RCC->CONCAT(DHT_TIMER_USE_BUS,ENR) &= ~(RCC_BUS_EN);

/**
 * @brief   Resets the TIM3 peripheral.
 *
 * @api
 */

//rccResetAPB2(RCC_APB2RSTR_TIM17RST)
/** @} */

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 */
dht11_t *sensor_handlers[EXT_MAX_CHANNELS];

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 inline void NanoSleep(uint16_t cnt)
 {
 volatile uint16_t i;
 if (cnt == 0) cnt = 30000;
 for(i=0;i<cnt;i++);
 }
 */

/*
 * @brief
 */
inline bool lldLock(lld_lock_t *lock) {
  bool locked = false;
  //
//	NanoSleep(0);
  chSysLock();
  if (lock->flag == false) {
    lock->flag = true;
    locked = true;
  }
//	NanoSleep(0);
  chSysUnlock();
  return locked;
}

/*
 * @brief
 */
inline void lldUnlock(lld_lock_t *lock) {
  //
//	NanoSleep(0);
  chSysLock();
  if (lock->flag == true) {
    lock->flag = false;
  }
  chSysUnlock();
//	NanoSleep(0);
}

/*
 * @brief
 */
inline bool lldLockISR(lld_lock_t *lock) {
  bool locked = false;
  //
  chSysLockFromISR();
  if (lock->flag == false) {
    lock->flag = true;
    locked = true;
  }
  chSysUnlockFromISR();
  return locked;
}

/*
 * @brief
 */
inline void lldUnlockISR(lld_lock_t *lock) {
  //
  chSysLockFromISR();
  if (lock->flag == true) {
    lock->flag = false;
  }
  chSysUnlockFromISR();
}

/*
 * @brief   ...
 * @details ...
 */
static void dht11_lld_ext_handler(EXTDriver *extp, expchannel_t channel) {
  //
  (void)extp;
  dht11_t *sensor = sensor_handlers[channel];
  if (sensor == NULL) {
    return;
  }
  //
  if (lldLockISR(&sensor->lock) == true) {
    switch (sensor->state) {
    case DHT11_WAIT_RESPONSE:
      sensor->bit_count++;
      if (sensor->bit_count == 3) {
        sensor->bit_count = 0;
        sensor->data = 0;
        sensor->state = DHT11_READ_DATA;
      }
      break;
    case DHT11_READ_DATA:
      sensor->bit_count++;
      if (sensor->bit_count % 2 == 1) {
        sensor->time_measurment = gptGetCounterX(&GPT_DRV);
        sensor->data <<= 1;
      }
      else {
        systime_t tmp_time = gptGetCounterX(&GPT_DRV);
        tmp_time = (uint16_t)(tmp_time - sensor->time_measurment);
        if (TIMx2US(tmp_time) > 50) {
          sensor->data += 1;
        }
      }
      if (sensor->bit_count == 64) {
        sensor->bit_count = 0;
#if DHT_USE_CRC_CHECKSUM
        sensor->crc = 0;
#endif
        sensor->state = DHT11_READ_CRC;
      }
      break;
    case DHT11_READ_CRC:
      sensor->bit_count++;
#if DHT_USE_CRC_CHECKSUM
      if (sensor->bit_count % 2 == 1) {
        sensor->time_measurment = gptGetCounterX(&GPT_DRV);
        sensor->crc <<= 1;
      }
      else {
        systime_t tmp_time = gptGetCounterX(&GPT_DRV);
        tmp_time = (uint16_t)(tmp_time - sensor->time_measurment);
        if (TIMx2US(tmp_time) > 40) {
          sensor->crc += 1;
        }
      }
#endif
      if (sensor->bit_count == 16) {
        chSysLockFromISR();
        extChannelDisableI(sensor->ext_drv, sensor->ext_pin);
        chVTResetI(&sensor->timer);
#if DHT_USE_DHT11
#if DHT_USE_DHT22
        if (sensor->dht_type == DHT_DHT11) {
#endif
        sensor->temp = (sensor->data & 0xFF00) >> 8;
        sensor->humidity = (sensor->data & 0xFF000000) >> 24;
#if DHT_USE_DHT22
      }
      else {
#endif
#endif
#if DHT_USE_DHT22
        sensor->temp = (sensor->data & 0xFFFF);
        sensor->humidity = (sensor->data & 0xFFFF0000) >> 16;
#if DHT_USE_DHT11
      }
#endif
#endif
#if DHT_USE_CRC_CHECKSUM
        uint8_t checksum = (uint8_t)(sensor->data & 0xFF) + (uint8_t)((sensor->data & 0xFF00) >> 8)
            + (uint8_t)((sensor->data & 0xFF0000) >> 16) + (uint8_t)((sensor->data & 0xFF000000) >> 24);
        if (sensor->crc == checksum) {
#endif
          sensor->state = DHT11_READ_OK;
          if (sensor->updater_thread != NULL)
            chEvtSignalI(sensor->updater_thread, (eventmask_t)(EVENTMASK_OK));
#if DHT_USE_CRC_CHECKSUM
        } else {
          sensor->state = DHT11_CRC_ERROR;
          if (sensor->updater_thread != NULL)
            chEvtSignalI(sensor->updater_thread, (eventmask_t)(EVENTMASK_ERROR));
        }
#endif
        osalSysUnlockFromISR();
      }
      break;
    case DHT11_UNINIT:
    case DHT11_IDLE:
    case DHT11_READ_REQUEST:
    case DHT11_READ_OK:
    case DHT11_BUSY:
    case DHT11_ERROR:
    case DHT11_CRC_ERROR:
      break;
    }
    lldUnlockISR(&sensor->lock);
  }
}

/*
 * @brief   ...
 * @details ...
 */
void dht11_timer_handler(void *p) {
  dht11_t *sensor = (dht11_t *)p;
  //
  if (lldLockISR(&sensor->lock) == true) {
    switch (sensor->state) {
    case DHT11_READ_REQUEST:
      chSysLockFromISR();
      chVTSetI(&sensor->timer, MS2ST(5), dht11_timer_handler, sensor);
      palSetPad(sensor->ext_port, sensor->ext_pin);
      palSetPadMode(sensor->ext_port, sensor->ext_pin, PAL_MODE_INPUT_PULLUP);
      extChannelEnableI(sensor->ext_drv, sensor->ext_pin);
      chSysUnlockFromISR();
      sensor->bit_count = 0;
      sensor->state = DHT11_WAIT_RESPONSE;
      break;
    case DHT11_UNINIT:
    case DHT11_IDLE:
    case DHT11_WAIT_RESPONSE:
    case DHT11_READ_DATA:
    case DHT11_READ_CRC:
    case DHT11_READ_OK:
    case DHT11_BUSY:
    case DHT11_ERROR:
    case DHT11_CRC_ERROR:
      chSysLockFromISR();
      extChannelDisableI(sensor->ext_drv, sensor->ext_pin);
      palSetPadMode(sensor->ext_port, sensor->ext_pin, PAL_MODE_OUTPUT_PUSHPULL);
      palSetPad(sensor->ext_port, sensor->ext_pin);
      chVTResetI(&sensor->timer);
      sensor->state = DHT11_ERROR;
      if (sensor->updater_thread != NULL)
        chEvtSignalI(sensor->updater_thread, (eventmask_t)(EVENTMASK_ERROR));
      osalSysUnlockFromISR();
      break;
    }
    lldUnlockISR(&sensor->lock);
  }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 */
dht11_state_t dht11Init(dht11_t *sensor) {
  GPT_DRV.tim = STM32_TIM17;
  gptObjectInit(&GPT_DRV);

  rccEnableTIMx(FALSE);
  GPT_DRV.clock = STM32_TIMCLK1;

  gptStart(&GPT_DRV, &gptxcfg);

  dht11_state_t state;
  if (lldLock(&sensor->lock) == true) {
    sensor->updater_thread = NULL;
    sensor->temp = 0;
    sensor->humidity = 0;
    sensor_handlers[sensor->ext_pin] = sensor;

    palSetPadMode(sensor->ext_port, sensor->ext_pin, PAL_MODE_OUTPUT_PUSHPULL);
    sensor->ext_cfg.mode = EXT_CH_MODE_BOTH_EDGES | sensor->ext_mode;
    sensor->ext_cfg.cb = dht11_lld_ext_handler;
    osalSysLock();
    extSetChannelModeI(sensor->ext_drv, sensor->ext_pin, &sensor->ext_cfg);
    extChannelDisableI(sensor->ext_drv, sensor->ext_pin);
    gptStopTimerI(&GPT_DRV);
    gptStartContinuousI(&GPT_DRV, 65000);
    osalSysUnlock();

    state = sensor->state = DHT11_IDLE;
    lldUnlock(&sensor->lock);
    return state;
  }
  else {
    return DHT11_UNINIT;
  }
}

/*
 * @brief   ...
 */
dht11_state_t dht11Update(dht11_t *sensor, uint8_t retry) {
  //
  dht11_state_t state;
  uint8_t cnt = 0;
  eventmask_t evt = EVENTMASK_ERROR;

  while (sensor->updater_thread != NULL) {
    osalThreadSleepMilliseconds(10);
  }

  sensor->updater_thread = chThdGetSelfX();

  do {
    if (dht11StartUpdate(sensor) != DHT11_BUSY) {
      evt = chEvtWaitOne(EVENTMASK_OK | EVENTMASK_ERROR);
      if (evt != EVENTMASK_OK) {
        osalThreadSleepMilliseconds(10);
      }
      cnt++;
    }
  } while ((evt != EVENTMASK_OK) && (cnt < retry));

  state = sensor->state;

  sensor->updater_thread = NULL;
  return state;
}

dht11_state_t dht11StartUpdate(dht11_t *sensor) {
  dht11_state_t state;

  if (lldLock(&sensor->lock) == true) {

    // low pulse
    sensor->bit_count = 0;
//    sensor->updater_thread = NULL;
    state = sensor->state = DHT11_READ_REQUEST;
    palSetPadMode(sensor->ext_port, sensor->ext_pin, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(sensor->ext_port, sensor->ext_pin);

    osalSysLock();

    chVTSetI(&sensor->timer, MS2ST(18), dht11_timer_handler, sensor);

    osalSysUnlock();

    lldUnlock(&sensor->lock);
    //
    return state;
  }
  else {
    return DHT11_BUSY;
  }

}

/*
 * @brief   ...
 */
bool dht11GetTemperature(dht11_t *sensor, int16_t *temp) {
//
  *temp = sensor->temp;
  return true;
}

/*
 * @brief   ...
 */
bool dht11GetHumidity(dht11_t *sensor, int16_t *humidity) {
//
  *humidity = sensor->humidity;
  return true;
}

#endif
