#include "ch.h"
#include "hal.h"
#include "ext.h"
#include "chconf.h"
#include <DHT11_hal.h>

//#define ST2US_mod(n) ((((n) - 1UL) * (1000000UL / CH_CFG_ST_FREQUENCY)) + 1UL)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

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
 * @brief
 */
inline bool lldLock(lld_lock_t *lock)
{
	bool locked = false;
	//
	chSysLock();
	if (lock->flag == false)
	{
		lock->flag = true;
		locked = true;
	}
	chSysUnlock();
	return locked;
}

/*
 * @brief
 */
inline void lldUnlock(lld_lock_t *lock)
{
	//
	chSysLock();
	if (lock->flag == true)
	{
		lock->flag = false;
	}
	chSysUnlock();
}

/*
 * @brief
 */
inline bool lldLockISR(lld_lock_t *lock)
{
	bool locked = false;
	//
	chSysLockFromISR();
	if (lock->flag == false)
	{
		lock->flag = true;
		locked = true;
	}
	chSysUnlockFromISR();
	return locked;
}

/*
 * @brief
 */
inline void lldUnlockISR(lld_lock_t *lock)
{
	//
	chSysLockFromISR();
	if (lock->flag == true)
	{
		lock->flag = false;
	}
	chSysUnlockFromISR();
}

/*
 * @brief   ...
 * @details ...
 */
static void dht11_lld_ext_handler(EXTDriver *extp, expchannel_t channel)
{
	//
	(void) extp;
	dht11_t *sensor = sensor_handlers[channel];
	if (sensor == NULL)
	{
		return;
	}
	//
	if (lldLockISR(&sensor->lock) == true)
	{
		switch (sensor->state)
		{
		case DHT11_WAIT_RESPONSE:
			sensor->bit_count++;
			if (sensor->bit_count == 3)
			{
				sensor->bit_count = 0;
				sensor->data = 0;
				sensor->state = DHT11_READ_DATA;
			}
			break;
		case DHT11_READ_DATA:
			sensor->bit_count++;
			if (sensor->bit_count % 2 == 1)
			{
				sensor->time_measurment = chVTGetSystemTime();
				sensor->data <<= 1;
			}
			else
			{
				volatile systime_t tmp_time = chVTGetSystemTime();
				sensor->time_measurment = tmp_time - sensor->time_measurment;
				if (ST2US(sensor->time_measurment) > 50)
				{
					sensor->data += 1;
				}
			}
			if (sensor->bit_count == 64)
			{
				sensor->bit_count = 0;
				sensor->crc = 0;
				sensor->state = DHT11_READ_CRC;
			}
			break;
		case DHT11_READ_CRC:
			sensor->bit_count++;
			//sensor->crc = sensor->crc << 1;
			if (sensor->bit_count % 2 == 1)
			{
				sensor->time_measurment = chVTGetSystemTime();
				sensor->crc <<= 1;
			}
			else
			{
				sensor->time_measurment -= chVTGetSystemTime();
				if (ST2US(sensor->time_measurment) > 40)
				{
					sensor->crc += 1;
				}
			}
			if (sensor->bit_count == 16)
			{
				chSysLockFromISR();
				extChannelDisableI(sensor->ext_drv, sensor->ext_pin);
				if (chVTIsArmedI(&sensor->timer) == true)
				{
					chVTResetI(&sensor->timer);
				}
				chSysUnlockFromISR();
				sensor->temp = (sensor->data & 0xFF00) >> 8;
				sensor->humidity = (sensor->data & 0xFF000000) >> 24;
				sensor->state = DHT11_READ_OK;
			}
			break;
		case DHT11_UNINIT:
		case DHT11_IDLE:
		case DHT11_READ_REQUEST:
		case DHT11_READ_OK:
		case DHT11_BUSY:
		case DHT11_ERROR:
			break;
		}
		lldUnlockISR(&sensor->lock);
	}
}

/*
 * @brief   ...
 * @details ...
 */
void dht11_timer_handler(void *p)
{
	dht11_t *sensor = (dht11_t *) p;
	//
	if (lldLockISR(&sensor->lock) == true)
	{
		switch (sensor->state)
		{
		case DHT11_READ_REQUEST:
			chSysLockFromISR();
			chVTSetI(&sensor->timer, MS2ST(5), dht11_timer_handler, sensor);
			extChannelEnableI(sensor->ext_drv, sensor->ext_pin);
			chSysUnlockFromISR();
			palSetPad(sensor->ext_port, sensor->ext_pin);
			palSetPadMode(sensor->ext_port, sensor->ext_pin,
					PAL_MODE_INPUT_PULLUP);
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
			chSysLockFromISR();
			extChannelDisableI(sensor->ext_drv, sensor->ext_pin);
			palSetPadMode(sensor->ext_port, sensor->ext_pin,
					PAL_MODE_OUTPUT_PUSHPULL);
			palSetPad(sensor->ext_port, sensor->ext_pin);
			if (chVTIsArmedI(&sensor->timer) == true)
			{
				chVTResetI(&sensor->timer);
			}
			chSysUnlockFromISR();
			sensor->state = DHT11_ERROR;
//                SerialConsole::debug("dht11Update timer error\r\n");
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
dht11_state_t dht11Init(dht11_t *sensor)
{
	dht11_state_t state;
	if (lldLock(&sensor->lock) == true)
	{
		sensor->refresh_time = 0;
		sensor->temp = 0;
		sensor->humidity = 0;
		sensor_handlers[sensor->ext_pin] = sensor;
//		tmObjectInit(&sensor->time_measurment);
		// configure ext channel used by radio
		palSetPadMode(sensor->ext_port, sensor->ext_pin,
				PAL_MODE_OUTPUT_PUSHPULL);
		sensor->ext_cfg.mode = EXT_CH_MODE_BOTH_EDGES | sensor->ext_mode;
		sensor->ext_cfg.cb = dht11_lld_ext_handler;
		chSysLock();
		extSetChannelModeI(sensor->ext_drv, sensor->ext_pin, &sensor->ext_cfg);
		extChannelDisableI(sensor->ext_drv, sensor->ext_pin);
		chSysUnlock();
		//
		state = sensor->state = DHT11_IDLE;
		lldUnlock(&sensor->lock);
		return state;
	}
	else
	{
		return DHT11_UNINIT;
	}
}

/*
 * @brief   ...
 */
bool dht11Update(dht11_t *sensor, varg_t unused)
{
	//
	(void) unused;
	dht11_state_t state;
	//
	//SerialConsole::debug("dht11Update start\r\n");
	if (lldLock(&sensor->lock) == true)
	{
		if (sensor->refresh_time < chVTGetSystemTime())
		{

			sensor->refresh_time = chVTGetSystemTime() + MS2ST(sensor->refresh_period);
			// low pulse
			sensor->bit_count = 0;
			state = sensor->state = DHT11_READ_REQUEST;
			palSetPadMode(sensor->ext_port, sensor->ext_pin,
					PAL_MODE_OUTPUT_PUSHPULL);
			palClearPad(sensor->ext_port, sensor->ext_pin);
			// timer callback started
			chVTSet(&sensor->timer, MS2ST(25), dht11_timer_handler, sensor);
			lldUnlock(&sensor->lock);
			//
			return state;
		}
		else
		{
			lldUnlock(&sensor->lock);
			//SerialConsole::debug("dht11Update idle\r\n");
			return DHT11_IDLE;
		}
	}
	else
	{
		//SerialConsole::debug("dht11Update lock failed\r\n");
		return DHT11_BUSY;
	}
}

/*
 * @brief   ...
 */
bool dht11GetTemperature(dht11_t *sensor, int8_t *temp)
{
	//
	*temp = sensor->temp;
	return true;
}

/*
 * @brief   ...
 */
bool dht11GetHumidity(dht11_t *sensor, int8_t *humidity)
{
	//
	*humidity = sensor->humidity;
	return true;
}
