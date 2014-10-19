#include "ch.h"
#include "hal.h"
#include "core.h"

#include "string.h"

static core_array_t Modules_Array[Other];

static core_base_struct_t Core_Base;
volatile core_base_struct_t* Core_BasePtr = NULL;
//static msg_t core_msg_b;

void Core_Module_Register(core_base_struct_t* Base_Struct)
{
	chSysLock();
	if (Core_BasePtr != NULL)
	{
		volatile static core_base_struct_t *current;
		current = Core_BasePtr;
		while ((*current).next != NULL)
		{
			current = (*current).next;
		}

		(*current).next = Base_Struct;
	}
	else
	{
		Core_BasePtr = Base_Struct;
	}
	(*Base_Struct).next = NULL;
	chSysUnlock();

	Modules_Array[(*Base_Struct).type].Base_Struct = Base_Struct;

//	chEvtObjectInit(&(*Base_Struct).event_source);
//	chEvtRegisterMask(&(*Base_Struct).event_source, &(*Base_Struct).event_listener, EVENT_MASK((uint8_t)(*Base_Struct).type));
}
/*
 uint8_t Core_GetDataById(const uint8_t id, uint16_t* data)
 {
 volatile static core_base_struct_t *current;
 current = Core_BasePtr;
 while (((*current).id != id) && ((*current).next != NULL))
 {
 current = (*current).next;
 }

 if ((*current).id != id)
 {
 return 0;
 }
 else
 {
 data[0] = (*current).set_value;
 data[1] = (*current).current_value;
 ByteArrayCopy((uint8_t*) (*current).inner_values, (uint8_t*) (data + 4), (*current).ival_size);
 }

 return ((*current).ival_size + 4);
 }
 */
core_base_struct_t* Core_GetStructAddrByType(const core_types_t type)
{
	/*	volatile static core_base_struct_t *current;
	 current = Core_BasePtr;
	 while (((*current).type != type) && ((*current).next != NULL))
	 {
	 current = (*current).next;
	 }

	 if ((*current).type != type)
	 {
	 return NULL;
	 }

	 return (core_base_struct_t*) current;*/

	return Modules_Array[type].Base_Struct;
}

void* Core_GetIvalAddrByType(const core_types_t type)
{
//	core_base_struct_t *current;
//	current = Core_GetStructAddrByType(type)->inner_values;

//	return (void *) Core_GetStructAddrByType(type)->inner_values;
	return (void *) Modules_Array[type].Base_Struct->inner_values;
}

/*
 uint16_t Core_SetDataById(const uint8_t id, uint16_t value)
 {
 volatile static core_base_struct_t *current;
 current = Core_BasePtr;
 while (((*current).id != id) && ((*current).next != NULL))
 {
 current = (*current).next;
 }

 if ((*current).id != id)
 {
 return 0xffff;
 }
 else
 {
 chSysLock();
 (*current).set_value = value;
 chSysUnlock();
 }

 return ((*current).set_value);
 }
 */

void Core_Init()
{
	Core_Base.type = Base;
	Core_Base.ival_size = 0;
	Core_Base.next = NULL;
	Core_Base.description = "Test Board 1\0";

	Core_Module_Register(&Core_Base);

	EXTConfig extcfg =
	{
	{
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
#ifndef STM32F10X_MD_VL
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
	{ EXT_CH_MODE_DISABLED, NULL },
#endif
	{ EXT_CH_MODE_DISABLED, NULL } } };

	/*
	 * Activates the EXT driver 1.
	 */
	extStart(&EXTD1, &extcfg);
}

#ifdef Core_Thread
THD_WORKING_AREA(waCore, 256);
//__attribute__((noreturn))
THD_FUNCTION(Core,arg)
{
	(void) arg;
	thread_t *answer_thread;
	msg_t message;
//	chRegSetThreadName("Core");

	EXTConfig extcfg =
	{
		{
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL},
			{	EXT_CH_MODE_DISABLED, NULL}}};

	/*
	 * Activates the EXT driver 1.
	 */
	extStart(&EXTD1, &extcfg);

//	core_base_struct_t *current;
//	current = &Core_Base;
//	chMBObjectInit(&core_mb, core_mb_b, 1);

	while (TRUE)
	{
//		chMBFetch(&core_mb, (msg_t *) &tx_buffer, TIME_INFINITE);
		answer_thread = chMsgWait();
		message = chMsgGet(answer_thread);
		chMsgRelease(answer_thread, (msg_t) message);
		chThdSleepSeconds(50);
	}
}
#endif

inline void Core_Register_Thread(core_types_t type, thread_t* thd, thread_reference_t* upd_thd)
{
	Modules_Array[type].Base_Thread = thd;
	Modules_Array[type].Base_Thread_Updater = upd_thd;
	*Modules_Array[type].Base_Thread_Updater = NULL;
}

static void Start_Modules(void)
{
#if WATCHDOG_PRESENT
	WatchDog_Start();
#endif
#if CLI_PRESENT
	CLI_Start();
#endif
#if RADIO_PRESENT
	Radio_Start();
#endif
#if DS18B20_PRESENT
	DS18B20_Start();
#endif
#if DHT11_PRESENT
	DHT11_Start();
#endif
#if FloorHeater_PRESENT
	FloorHeater_Start();
#endif
#if RGBW_PRESENT
	RGBW_Start();
#endif
#if PIR_PRESENT
	PIR_Start();
#endif
}

void Core_Start()
{
	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	chThdSetPriority(LOWPRIO);
	register int i;
	for (i = 0; i <= Other; i++)
	{
		Modules_Array[i].Base_Struct = NULL;
		Modules_Array[i].Base_Thread = NULL;
//		*Modules_Array[i].Base_Thread_Updater = NULL;
	}

//	Core_Init((void*) (uint32_t) id);
	Core_Init();
//	Modules_Array[Base].Base_Thread = chThdCreateStatic(waCore, sizeof(waCore), LOWPRIO, Core, NULL);

	while (Core_BasePtr == NULL)
	{
		chThdYield();
	}
	Start_Modules();
}

/*void sleepUntil(systime_t *previous, systime_t period)
 {
 systime_t future = *previous + period;
 chSysLock();
 systime_t now = chVTGetSystemTime();
 int mustDelay = now < *previous ? (now < future && future < *previous) : (now < future || future < *previous);
 if (mustDelay)
 chThdSleepS(future - now);
 chSysUnlock();
 *previous = future;
 }*/
/*
 void ByteArrayCopy(const volatile char* __restrict src, volatile char* __restrict dst, const uint8_t cnt)
 {
 int i;
 for (i = 0; i < cnt; i++)
 {
 dst[i] = src[i];
 }
 }*/

msg_t Core_Module_Update(const core_types_t type, const char * inval, const systime_t timeout_milliseconds)
{
	if (inval != NULL)
	{
		core_base_struct_t* base_struct = Core_GetStructAddrByType(type);
		chSysLock();
		memcpy((void *) base_struct->inner_values, inval, base_struct->ival_rw_size);
//	ByteArrayCopy(inval, (char*) base_struct->inner_values, base_struct->ival_rw_size);
		chSysUnlock();
	}
	if (Modules_Array[type].Base_Thread != 0)
	{
		chSysLock();
		while (*Modules_Array[type].Base_Thread_Updater != NULL)
		{
			  chSysUnlock();
			  chSysLock();
			  chSchDoYieldS();
		}
		chEvtSignalI(Modules_Array[type].Base_Thread, EVENTMASK_REREAD);
		msg_t msg = _core_wait_s(Modules_Array[type].Base_Thread_Updater, timeout_milliseconds);
		*Modules_Array[type].Base_Thread_Updater = NULL;
		chSysUnlock();
		return msg;
	}
	return MSG_ERROR;
}

uint8_t Core_Module_Read(const core_types_t type, char * inval)
{
	core_base_struct_t* base_struct = Core_GetStructAddrByType(type);
	if (base_struct == NULL)
		return 0;
	chSysLock();
	memcpy(inval, (void *) base_struct->inner_values, base_struct->ival_size);
//	ByteArrayCopy((char*) base_struct->inner_values, inval, base_struct->ival_size);
	chSysUnlock();
	return base_struct->ival_size;
}

/*
 bool Core_Events_Register(const core_types_t type)
 {
 core_base_struct_t* current_Core;
 do {
 current_Core = Core_GetStructAddrByType(type);
 } while (current_Core == NULL);
 chEvtObjectInit(&current_Core->event_source);
 chEvtRegisterMask(&current_Core->event_source, &Core_EvtListener, EVENT_MASK((uint8_t) type));
 return TRUE;
 }*/
