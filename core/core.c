#include "ch.h"
#include "hal.h"
#include "core.h"

static msg_t core_msg_b;

void Core_Module_Register (core_base_struct_t Base_Struct)
{
	core_base_struct_t *current;
	current = &Core_Base;
	chSysLock();
	while ((*current).next!=NULL)
	{
		current = (*current).next;
	}

	(*current).next = &Base_Struct;
	Base_Struct.next = NULL;
	chSysUnlock();
}

uint8_t Core_GetDataById(const uint8_t id, const uint8_t val, uint16_t* data)
{
	core_base_struct_t *current;
	current = &Core_Base;
	while (((*current).id!=id) && ((*current).next!=NULL))
	{
		current = (*current).next;
	}

	if ((*current).id!=id)
	{
		return 0;
	} else {
		data[0]=(*current).set_value;
		data[1]=(*current).current_value;
		ByteArrayCopy((*current).inner_values, data+4, (*current).ival_size);
	}

	return ((*current).ival_size+4);
}

//HAL_FAILED

void Core_Init()
{
	Core_Base.id = 0;
	Core_Base.type = Base;
//	Core_Base.addr = MY_ADDR;
//	Core_Base.mbox = &core_mb;
	Core_Base.thread = chThdGetSelfX();
	Core_Base.direction = None;
	Core_Base.ival_size = 0;
	Core_Base.next = NULL;
	Core_Base.description = "Test Board 1\0";
}

THD_WORKING_AREA(waCore, 128);
//__attribute__((noreturn))
THD_FUNCTION(Core,arg)
{
	(void) arg;
	thread_t 	*answer_thread;
	msg_t		message;
//	chRegSetThreadName("Core");

	Core_Init();
	core_base_struct_t *current;
	current = &Core_Base;
//	chMBObjectInit(&core_mb, core_mb_b, 1);

	while (TRUE)
	{
//		chMBFetch(&core_mb, (msg_t *) &tx_buffer, TIME_INFINITE);
		answer_thread = chMsgWait();
		message = chMsgGet(answer_thread);
		chMsgRelease (answer_thread, (msg_t) message);

	}
}

void Core_Start()
{
	chThdCreateStatic(waCore, sizeof(waCore), NORMALPRIO,
			Core, NULL);
}

void sleepUntil(systime_t *previous, systime_t period)
{
  systime_t future = *previous + period;
  chSysLock();
  systime_t now = chVTGetSystemTime();
  int mustDelay = now < *previous ?
    (now < future && future < *previous) :
    (now < future || future < *previous);
  if (mustDelay)
    chThdSleepS(future - now);
  chSysUnlock();
  *previous = future;
}

void ByteArrayCopy(uint8_t* src, uint8_t* dst, uint8_t cnt)
{
	int i;
	for (i=0;i<cnt;i++)
	{
		dst[i]=src[i];
	}
}
