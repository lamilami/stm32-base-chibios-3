#ifndef _DHT11_H_
#define _DHT11_H_
/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 */
typedef enum
{
	DHT11_UNINIT = 0,
	DHT11_IDLE,
	DHT11_READ_REQUEST,
	DHT11_WAIT_RESPONSE,
	DHT11_READ_DATA,
	DHT11_READ_CRC,
	DHT11_READ_OK,
	DHT11_BUSY,
	DHT11_ERROR,
} dht11_state_t;

typedef struct
{
	// ...
	char *name;
	bool flag;
} lld_lock_t;

/*
 * @brief   ...
 * @details ...
 */
typedef void * varg_t;

/*
 * @brief   ...
 */
typedef struct
{
	char *desc;
	EXTDriver *ext_drv;
	uint32_t ext_mode;
	ioportid_t ext_port;
	uint16_t ext_pin;
	EXTChannelConfig ext_cfg;
	//
	dht11_state_t state;
//	systime_t refresh_time;
//	uint16_t refresh_period;
	uint8_t temp;
	uint8_t humidity;
	uint8_t bit_count;
	uint32_t data;
//	uint8_t crc;
	lld_lock_t lock;
	virtual_timer_t timer;
	gptcnt_t time_measurment;
	thread_t* updater_thread;
} dht11_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif
dht11_state_t dht11Init(dht11_t *sensor);
dht11_state_t dht11Update(dht11_t *sensor, uint8_t retry);
dht11_state_t dht11StartUpdate(dht11_t *sensor);
bool dht11GetTemperature(dht11_t *sensor, int8_t *temp);
bool dht11GetHumidity(dht11_t *sensor, int8_t *humidity);
#ifdef __cplusplus
}
#endif

extern dht11_t DHTD1;

#endif /* _DHT11_H_ */
