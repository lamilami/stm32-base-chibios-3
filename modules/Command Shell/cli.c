#include "ch.h"
#include "hal.h"
#include "core.h"


#if CLI_PRESENT
#include "shell.h"
#include "chprintf.h"
/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

//#define SHELL_WA_SIZE   THD_WA_SIZE(512)
THD_WORKING_AREA(waShell, 512);

//static core_base_struct_t* core_struct;
static char nullchar;

#if DHT11_PRESENT
static void cmd_dht11(BaseSequentialStream *chp, int argc, char *argv[])
{
//  size_t n, size;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: dht11\r\n");
		return;
	}

	core_struct = Core_GetStructAddrByType(DHT11);
	DHT11_Inner_Val* dht11_ival = (DHT11_Inner_Val*) core_struct->inner_values;

	while (TRUE)
	{
		chprintf(chp, "Temp: %u deg.C, Humid: %u%%", dht11_ival->temp >> 2, dht11_ival->humidity);
		if (shellGetLine(chp, &nullchar, 1))
		{
			chprintf(chp, "\r\n");
			break;
		}
	}
}
#endif

#if RGBW_PRESENT
static void cmd_rgbw(BaseSequentialStream *chp, int argc, char *argv[])
{
//  size_t n, size;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: rgbw\r\n");
		return;
	}

//	core_struct = Core_GetStructAddrByType(RGBW);
//	RGBW_Inner_Val* rgbw_ival = (RGBW_Inner_Val*) core_struct->inner_values;

	RGBW_Inner_Val rgbw_ival;

	while (TRUE)
	{
		if (Core_Module_Read(RGBW, (void *) &rgbw_ival) != 0)
		{
			chprintf(chp, "R:%3u.%02u%%, ", rgbw_ival.Red / 100, rgbw_ival.Red % 100);
			chprintf(chp, "G:%3u.%02u%%, ", rgbw_ival.Green / 100, rgbw_ival.Green % 100);
			chprintf(chp, "B:%3u.%02u%%, ", rgbw_ival.Blue / 100, rgbw_ival.Blue % 100);
			chprintf(chp, "W:%3u%%", rgbw_ival.White);
			if (shellGetLine(chp, &nullchar, 1))
			{
				chprintf(chp, "\r\n");
				break;
			}
		}
	}
}
#endif

#ifdef CMD_THREADS
static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
//  static const char *states[] = {THD_STATE_NAMES};
//  thread_t *tp;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "    addr    stack prio refs     state time\r\n");
	/*  tp = chRegFirstThread();
	 do {
	 chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n",
	 (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
	 (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
	 states[tp->p_state], (uint32_t)tp->p_time);
	 tp = chRegNextThread(tp);
	 } while (tp != NULL); */
}
#endif

static const ShellCommand commands[] =
{
#if RGBW_PRESENT
		{ "rgbw", cmd_rgbw },
#endif
#if DHT11_PRESENT
		{	"dht11", cmd_dht11},
#endif
//		{ "threads", cmd_threads },
		{ NULL, NULL } };

static const ShellConfig shell_cfg1 =
{ (BaseSequentialStream *) &SD1, commands };

THD_WORKING_AREA(waCLI, 128);
//__attribute__((noreturn))
THD_FUNCTION(CLI,arg)
{
	(void) arg;

	static event_listener_t EvtListenerShell;

//	CLI_GPIO_Init();

	palSetPadMode(GPIOA, GPIOA_PIN9, PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN10, PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_PUSHPULL);

	sdStart(&SD1, NULL);

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	chEvtRegisterMask(&shell_terminated, &EvtListenerShell, EVENT_MASK(0));

	while (TRUE)
	{
		shellCreateStatic(&shell_cfg1, waShell, sizeof(waShell), LOWPRIO);
		chEvtWaitAny(EVENT_MASK(0));
	}
}

void CLI_Start()
{
#if CLI_PRESENT
	chThdCreateStatic(waCLI, sizeof(waCLI), LOWPRIO, CLI, NULL);
#endif
}
#endif
