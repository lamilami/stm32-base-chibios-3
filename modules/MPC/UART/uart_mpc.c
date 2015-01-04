#include "ch.h"
#include "hal.h"
#include "core.h"

#if UART_MPC_PRESENT

static SerialConfig sd_mpc_cfg = {38400, USART_CR1_M | USART_CR1_WAKE | USART_CR1_RWU, (USART_CR2_ADD & ((MY_ADDR >> 6) & 0x03)), USART_CR3_HDSEL};

void UART_MPC_Start()
{

#ifdef STM32F100C8

  rccEnableUSART2(TRUE);

  palSetPadMode(GPIOA, GPIOA_PIN2, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
//  palSetPadMode(GPIOA, GPIOA_PIN3, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

  chThdSleepMilliseconds(1);

  sdStart(&SD2, &sd_mpc_cfg);

  sdPut(&SD2, 0x3F0);
  volatile uint16_t ch = 0;
  ch = sdGet(&SD2);

//  return OW_OK;

#else

//  rccEnableAHB(RCC_AHBENR_GPIOAEN, TRUE);
  rccEnableUSART1(TRUE);
//  rccEnableAPB2(RCC_APB2ENR_SYSCFGCOMPEN, TRUE);

  palSetPadMode(GPIOA, GPIOA_PIN9,
                PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_MID | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_OPENDRAIN);

  chThdSleepMilliseconds(1);

  sdStart(&SD1, &sd_cfg_high);

  return OW_OK;

#endif

/*
#if DHT11_PRESENT
    DHT11_Init();
    thread_t* thd = chThdCreateStatic(waDHT11_thread, sizeof(waDHT11_thread), HIGHPRIO, DHT11_thread, NULL);
    Core_Register_Thread(DHT11, thd, &Update_Thread);
    chThdYield();
#endif
*/
}


#endif
