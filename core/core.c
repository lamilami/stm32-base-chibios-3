#include "ch.h"
#include "hal.h"
#include "core.h"

static THD_WORKING_AREA(waCoreInterconnect, 128);
//__attribute__((noreturn))
static THD_FUNCTION(CoreInterconnect,arg)
{
	(void) arg;
	chRegSetThreadName("CoreInterconnect");

	Core_Base.type = Base;
	Core_Base.addr = MY_ADDR;
	Core_Base.mbox = NULL;
	Core_Base.direction = None;
	Core_Base.next = NULL;
	Core_Base.description = "Test Board 1\0";

	while (TRUE)
	{

	}
}
