#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"
#include "core.h"

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

//#define SHELL_WA_SIZE   THD_WA_SIZE(512)
THD_WORKING_AREA(waShell, 256);

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
//  size_t n, size;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: mem\r\n");
		return;
	}
//  n = chHeapStatus(NULL, &size);
//  chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
//  chprintf(chp, "heap fragments   : %u\r\n", n);
//  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

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

static const ShellCommand commands[] =
{
{ "mem", cmd_mem },
{ "threads", cmd_threads },
{ NULL, NULL } };

static const ShellConfig shell_cfg1 =
{ (BaseSequentialStream *) &SD1, commands };

/*static void CLI_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(((GPIO_TypeDef *) GPIOA_BASE), &GPIO_InitStruct);

	GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource9, GPIO_AF_1);
	GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource10, GPIO_AF_1);

	palSetPadMode(GPIOA, GPIOA_PIN9,
			PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN10,
			PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_PUSHPULL);
}*/

THD_WORKING_AREA(waCLI, 128);
//__attribute__((noreturn))
THD_FUNCTION(CLI,arg)
{
	(void) arg;

	static event_listener_t EvtListenerShell;

//	CLI_GPIO_Init();

	palSetPadMode(GPIOA, GPIOA_PIN9,
			PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN10,
			PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_PUSHPULL);

	sdStart(&SD1, NULL);

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	chEvtRegisterMask(&shell_terminated, &EvtListenerShell, EVENT_MASK(0));

	while (TRUE)
	{
		shellCreateStatic(&shell_cfg1, waShell, sizeof(waShell), NORMALPRIO);
		chEvtWaitAny(EVENT_MASK(0));
	}
}

void CLI_Start(uint8_t id)
{
#if CLI_PRESENT
	chThdCreateStatic(waCLI, sizeof(waCLI), LOWPRIO, CLI, (void*) (uint32_t) id);
#endif
}
