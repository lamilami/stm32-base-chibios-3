#ifndef _CORE_H_
#define _CORE_H_

#define NET_ADDR            30      //5-bit nRF24 addr
#define SUB_ADDR            0       //3-bit USART addr

#define MY_ADDR				((NET_ADDR & 0x3F) | ((SUB_ADDR << 6) & 0xC0))
#define SUBMOD_ADDR(addr)   ((NET_ADDR & 0x3F) | (((addr) << 6) & 0xC0))

#define MPC_PRESENT         TRUE

#if MPC_PRESENT
#define MPC_RADIO_PRESENT	TRUE
#define MPC_UART_PRESENT    TRUE
#endif

#define DS18B20_PRESENT		TRUE
#define DHT11_PRESENT		FALSE
#define FloorHeater_PRESENT FALSE
#define LCD1602_PRESENT 	FALSE
#define WATCHDOG_PRESENT	FALSE
#define RGBW_PRESENT		FALSE
#define CLI_PRESENT			FALSE
#define PIR_PRESENT			FALSE
#define LM75_PRESENT		FALSE

#define uGFX_PRESENT		TRUE

#if uGFX_PRESENT
#define ILI9341_PRESENT     TRUE

#endif

#define SEMIHOSTING			FALSE

#define localhost			0

#endif

#ifdef _HALCONF_H_
#ifndef _CORE_H_2_
#define _CORE_H_2_

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

#if uGFX_PRESENT
#include "gfx.h"
#if ILI9341_PRESENT
#include "board_ILI9341_spi.h"
#include "ILI9341.h"
#endif
#endif

#if MPC_PRESENT
#include "MPC.h"
#endif

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
#define EVENTMASK_MOD_PRIVATE_SHIFT 8

#define MAX_NUMBER_OF_SENSORS 2

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

//typedef msg_t (*mod_update_timeout_t)(msg_t msg, systime_t timeout);

typedef struct core_base_struct core_base_struct_t;

struct core_base_struct
{
	core_types_t type;
	uint8_t quantity;
    thread_t* Base_Thread;
    thread_reference_t* Base_Thread_Updater;
//	mod_update_timeout_t Mod_Update_Timeout;
//	event_source_t event_source;
//	event_listener_t event_listener;
	volatile void* inner_values[MAX_NUMBER_OF_SENSORS];
	volatile uint8_t ival_rw_size;
	volatile uint8_t ival_size;
//	volatile const char* description;
//	core_base_struct_t *next;
/*	union {
	    volatile char custom_field[4];
	    volatile uint8_t uint8[4];
	    volatile uint16_t uint16[2];
	    volatile uint32_t uint32;
	} custom;*/
};

typedef struct
{
	core_base_struct_t* Base_Struct;
//	thread_t* Base_Thread;
//	thread_reference_t* Base_Thread_Updater;
} core_array_t;

//volatile extern core_base_struct_t* Core_BasePtr;

void Core_Module_Register(core_base_struct_t* Base_Struct, thread_t* thd, thread_reference_t* upd_thd);
//uint8_t Core_GetDataById(const uint8_t id, uint16_t* data);
//uint16_t Core_SetDataById(const uint8_t id, uint16_t value);

//core_base_struct_t* Core_GetStructAddrByType(const core_types_t type);
//void* Core_GetIvalAddrByType(const core_types_t type);
//msg_t Core_Module_Update(const core_types_t type, systime_t timeout_milliseconds);
msg_t Core_Module_Update(const core_types_t type, const uint8_t number, const char * inval, const systime_t timeout_milliseconds);
msg_t Core_Module_Read(const uint8_t addr, const core_types_t type, const uint8_t number, char * inval);
bool Core_Events_Register(const core_types_t type);
//inline void Core_Register_Thread(const core_types_t type, thread_t* thd, thread_reference_t* upd_thd);
//void ByteArrayCopy(const volatile char* src, volatile char* dst, const uint8_t cnt);
#define ABS(a) (((a) < 0) ? -(a) : (a))
#define MAX(x,y) (((x)>(y))?(x):(y)) // максимум двух чисел
#define MIN(x,y) (((x)<(y))?(x):(y))

void Core_Start();

EXTConfig extcfg;

event_listener_t Core_EvtListener;

#define _core_wait_s(thd, milliseconds) osalThreadSuspendTimeoutS(thd, MS2ST(milliseconds));

#define _core_wakeup_i(thd, msg) if (*thd->p_state == CH_STATE_SUSPENDED) osalThreadResumeI(thd, msg);

#endif
#endif
