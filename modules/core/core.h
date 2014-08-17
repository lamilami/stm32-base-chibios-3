#ifndef _CORE_H_
#define _CORE_H_

#define MY_ADDR				0x10
#define RADIO_PRESENT		FALSE
#define DS18B20_PRESENT		FALSE
#define DHT11_PRESENT		FALSE
#define FloorHeater_PRESENT FALSE
#define LCD1602_PRESENT 	FALSE
#define WATCHDOG_PRESENT	FALSE
#define RGBW_PRESENT		TRUE
#define CLI_PRESENT			FALSE
#define PIR_PRESENT			TRUE

//#define StM() WatchDog_Start();

#include "radio.h"
#include "DS18B20.h"
#include "FloorHeater.h"
#include "WatchDog.h"
#include "DHT11.h"
#include "RGBW.h"
#include "cli.h"
#include "PIR.h"

#if (!DS18B20_PRESENT && FloorHeater_PRESENT)
#error "Can't use FloorHeater without DS18B20 temp sensor!"
#endif

#if (LCD1602_PRESENT && (DS18B20_PRESENT || FloorHeater_PRESENT))
#error "LCD1602 conflicts with DS18B20 and FloorHeater!"
#endif

typedef enum
{
	Base,     		//**< Radio is idle */
	Temp,     		//**< Maximum number of retries have occured */
	Humidity,     	//**< Data is sent */
	Light,     		//**< Data recieved */
	Vent,     		//**< Ack payload recieved */
	Heater,
	RGBW,
	DHT11,
	PIR,
	Other     		//**< Radio is busy */
} core_types_t;

typedef struct core_base_struct core_base_struct_t;

struct core_base_struct
{
	core_types_t type;
	event_source_t event_source;
	volatile void* inner_values;
	volatile uint8_t ival_size;
	volatile const char* description;
	core_base_struct_t *next;
};

volatile extern core_base_struct_t* Core_BasePtr;

void Core_Module_Register(core_base_struct_t* Base_Struct);
//uint8_t Core_GetDataById(const uint8_t id, uint16_t* data);
//uint16_t Core_SetDataById(const uint8_t id, uint16_t value);

core_base_struct_t* Core_GetStructAddrByType(const core_types_t type);
bool Core_Events_Register(const core_types_t type);

void sleepUntil(systime_t *previous, systime_t period);
void ByteArrayCopy(uint8_t* src, uint8_t* dst, uint8_t cnt);
//#define ABS(a) (((a) < 0) ? -(a) : (a))

void Core_Start();

EXTConfig extcfg;
event_listener_t Core_EvtListener;

#endif
