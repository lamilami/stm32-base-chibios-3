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
	volatile uint32_t ext_mode;
	volatile ioportid_t ext_port;
	volatile uint16_t ext_pin;
	EXTChannelConfig ext_cfg;
	//
	volatile dht11_state_t state;
	volatile systime_t refresh_time;
	volatile uint16_t refresh_period;
	volatile uint8_t temp;
	volatile uint8_t humidity;
	volatile uint8_t bit_count;
	volatile uint32_t data;
	volatile uint8_t crc;
	lld_lock_t lock;
	virtual_timer_t timer;
	volatile gptcnt_t time_measurment;
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
bool dht11Update(dht11_t *sensor, varg_t unused);
bool dht11GetTemperature(dht11_t *sensor, int8_t *temp);
bool dht11GetHumidity(dht11_t *sensor, int8_t *humidity);
#ifdef __cplusplus
}
#endif

#endif /* _DHT11_H_ */
