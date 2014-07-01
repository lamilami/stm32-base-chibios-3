#include "ch.h"
#include "hal.h"
#include "core.h"

void Core_Init()
{
	Core_Base.type = Base;
	Core_Base.addr = MY_ADDR;
	Core_Base.mbox = NULL;
	Core_Base.direction = None;
	Core_Base.next = NULL;
	Core_Base.description = "Test Board 1\0";
}
