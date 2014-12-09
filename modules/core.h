#ifndef _CORE_H_
#define _CORE_H_

#define MY_ADDR				0x10
#define RADIO_PRESENT		FALSE
#define DS18B20_PRESENT		TRUE
#define DHT11_PRESENT		TRUE
#define FloorHeater_PRESENT FALSE
#define LCD1602_PRESENT 	FALSE
#define WATCHDOG_PRESENT	FALSE
#define RGBW_PRESENT		FALSE
#define CLI_PRESENT			FALSE
#define PIR_PRESENT			FALSE
#define LM75_PRESENT		FALSE

#define SEMIHOSTING			FALSE

//#define StM() WatchDog_Start();

#include "radio.h"
#include "DS18B20.h"
#include "FloorHeater.h"
#include "WatchDog.h"
#include "DHT11.h"
#include "RGBW.h"
#include "cli.h"
#include "PIR.h"
#include "lcd.h"
//#include "LM75.h"

/*
#if (!DS18B20_PRESENT && FloorHeater_PRESENT)
#error "Can't use FloorHeater without DS18B20 temp sensor!"
#endif
*/
/*
#if (LCD1602_PRESENT && (DS18B20_PRESENT || FloorHeater_PRESENT))
#error "LCD1602 conflicts with DS18B20 and FloorHeater!"
#endif
*/
#define EVENTMASK_REREAD 0x04

#define MSG_ERROR	-3

typedef enum
{
	Base,
	Temp,
	Humidity,
	Light,
	Vent,
	Heater,
	RGBW,
	DHT11,
	PIR,
	LM75,
	Other
} core_types_t;

typedef msg_t (*mod_update_timeout_t)(msg_t msg, systime_t timeout);

typedef struct core_base_struct core_base_struct_t;

struct core_base_struct
{
	core_types_t type;
//	mod_update_timeout_t Mod_Update_Timeout;
//	event_source_t event_source;
//	event_listener_t event_listener;
	volatile void* inner_values;
	volatile uint8_t ival_rw_size;
	volatile uint8_t ival_size;
	volatile const char* description;
	core_base_struct_t *next;
};

typedef struct
{
	core_base_struct_t* Base_Struct;
	thread_t* Base_Thread;
	thread_reference_t* Base_Thread_Updater;
} core_array_t;

//volatile extern core_base_struct_t* Core_BasePtr;

void Core_Module_Register(core_base_struct_t* Base_Struct);
//uint8_t Core_GetDataById(const uint8_t id, uint16_t* data);
//uint16_t Core_SetDataById(const uint8_t id, uint16_t value);

//core_base_struct_t* Core_GetStructAddrByType(const core_types_t type);
//void* Core_GetIvalAddrByType(const core_types_t type);
//msg_t Core_Module_Update(const core_types_t type, systime_t timeout_milliseconds);
msg_t Core_Module_Update(const core_types_t type, const char * inval, const systime_t timeout_milliseconds);
uint8_t Core_Module_Read(const core_types_t type, char * inval);
bool Core_Events_Register(const core_types_t type);
inline void Core_Register_Thread(const core_types_t type, thread_t* thd, thread_reference_t* upd_thd);
//void ByteArrayCopy(const volatile char* src, volatile char* dst, const uint8_t cnt);
#define ABS(a) (((a) < 0) ? -(a) : (a))
#define MAX(x,y) (((x)>(y))?(x):(y)) // максимум двух чисел

void Core_Start();

EXTConfig extcfg;

event_listener_t Core_EvtListener;

#define _core_wait_s(thd, milliseconds) osalThreadSuspendTimeoutS(thd, MS2ST(milliseconds));

#define _core_wakeup_i(thd, msg) if (*thd->p_state == CH_STATE_SUSPENDED) osalThreadResumeI(thd, msg);

#endif
