#include "ch.h"
#include "hal.h"
#include "core.h"

static msg_t core_msg_b;

void Core_Init()
{
	Core_Base.id = 0;
	Core_Base.type = Base;
//	Core_Base.addr = MY_ADDR;
	Core_Base.mbox = &core_mb;
	Core_Base.direction = None;
	Core_Base.next = NULL;
	Core_Base.description = "Test Board 1\0";
}

THD_WORKING_AREA(waCore, 128);
//__attribute__((noreturn))
THD_FUNCTION(Core,arg)
{
	(void) arg;
	chRegSetThreadName("Core");

	Core_Init();
	core_base_struct_t *current;
	current = &Core_Base;
	chMBObjectInit(&core_mb, core_mb_b, 1);

	while (TRUE)
	{
//		chMBFetch(&core_mb, (msg_t *) &tx_buffer, TIME_INFINITE);
	}
}

void Core_Start()
{
	chThdCreateStatic(waCore, sizeof(waCore), NORMALPRIO,
			Core, NULL);
}
