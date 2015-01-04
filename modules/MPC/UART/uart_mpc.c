#include "ch.h"
#include "hal.h"
#include "core.h"

#if MPC_UART_PRESENT

/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp) {

  (void)uartp;

//  palClearPad(IOPORT3, GPIOC_LED);
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp) {

  (void)uartp;

//  palSetPad(IOPORT3, GPIOC_LED);
//  chSysLockFromISR();
//  chVTResetI(&vt1);
//  chVTDoSetI(&vt1, MS2ST(5000), restart, NULL);
//  chSysUnlockFromISR();
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(UARTDriver *uartp, uartflags_t e) {

  (void)uartp;
  (void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c) {

  (void)uartp;
  (void)c;
  osalSysLockFromISR();
  chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_UART_IRQ);
  osalSysUnlockFromISR();
  /* Flashing the LED each time a character is received.*/
//  palClearPad(IOPORT3, GPIOC_LED);
//  chSysLockFromISR();
//  chVTResetI(&vt2);
//  chVTDoSetI(&vt2, MS2ST(200), ledoff, NULL);
//  chSysUnlockFromISR();
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxend(UARTDriver *uartp) {

  (void)uartp;
  osalSysLockFromISR();
  chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_UART_IRQ);
  osalSysUnlockFromISR();
}

//static SerialConfig sd_mpc_cfg = {38400, USART_CR1_M | USART_CR1_WAKE | USART_CR1_RWU, (USART_CR2_ADD & ((MY_ADDR >> 6) & 0x03)), USART_CR3_HDSEL};

static UARTConfig mpc_uart_cfg = {txend1, txend2, rxend, rxchar, rxerr, 38400, USART_CR1_M | USART_CR1_WAKE | USART_CR1_RWU, (USART_CR2_ADD & ((MY_ADDR >> 6) & 0x03)), USART_CR3_HDSEL};


void UART_MPC_init()
{

#ifdef STM32F100C8

  rccEnableUSART2(TRUE);

  palSetPadMode(GPIOA, GPIOA_PIN2, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
//  palSetPadMode(GPIOA, GPIOA_PIN3, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

  chThdSleepMilliseconds(1);

  uartStart(&UARTD2, &mpc_uart_cfg);

//  sdPut(&SD2, 0x3F0);
//  volatile uint16_t ch = 0;
//  ch = sdGet(&SD2);

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
