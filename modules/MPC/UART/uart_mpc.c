#include "ch.h"
#include "hal.h"
#include "core.h"

#if MPC_UART_PRESENT
//#if 1
#include "uart_mpc.h"

static virtual_timer_t vt1;
/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp) {

  (void)uartp;
 // osalSysLockFromISR();
//  chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_UART_TX_IRQ);
 // osalSysUnlockFromISR();
//  palClearPad(IOPORT3, GPIOC_LED);
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp) {

  (void)uartp;
  osalSysLockFromISR();
  chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_UART_TX_IRQ);
  osalSysUnlockFromISR();
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

static uint16_t rxb[MPC_MAX_PAYLOAD_LENGTH + 2];
static uint16_t txb[MPC_MAX_PAYLOAD_LENGTH + 2];
//volatile uint8_t z = 0;

static void _RX_ERR(void *p) {
  osalSysLockFromISR();
  uartStopReceiveI(&UARTD);
  chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_UART_ERROR);
  osalSysUnlockFromISR();
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c) {

  (void)uartp;
  (void)c;
  osalSysLockFromISR();
  if (c > 255) {
  }
  else {
    if (c > 0) {
      rxb[0] = c;
      uartStartReceiveI(&UARTD, c, &rxb[1]);
      chVTResetI(&vt1);
      chVTDoSetI(&vt1, MS2ST(1000), _RX_ERR, NULL);
    }
  }
//  if (z>rxb[0])
//    chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_UART_IRQ);
//  chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_UART_IRQ);
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
  chVTResetI(&vt1);
  chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_UART_IRQ);
//  UARTD.usart->RQR |= USART_RQR_MMRQ;
  osalSysUnlockFromISR();
}

//static SerialConfig sd_mpc_cfg = {38400, USART_CR1_M | USART_CR1_WAKE | USART_CR1_RWU, (USART_CR2_ADD & ((MY_ADDR >> 6) & 0x03)), USART_CR3_HDSEL};

//static UARTConfig mpc_uart_cfg = {txend1, txend2, rxend, rxchar, rxerr, 38400, USART_CR1_M, 0, USART_CR3_HDSEL};

void UART_MPC_init() {

#ifdef STM32F100C8

  static UARTConfig mpc_uart_cfg = {txend1, txend2, rxend, rxchar, rxerr, 38400, (USART_CR1_M | USART_CR1_WAKE),
    (USART_CR2_ADD & ((MY_ADDR >> 6) & 0x03)), USART_CR3_HDSEL};

  rccEnableUSART2(TRUE);

  palSetPadMode(GPIOA, GPIOA_PIN2, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
//  palSetPadMode(GPIOA, GPIOA_PIN3, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

  chThdSleepMilliseconds(1);

  uartStart(&UARTD, &mpc_uart_cfg);
//  USART_TypeDef *u = UARTD2.usart;
  UARTD.usart->CR1 |= USART_CR1_RWU;

//  sdPut(&SD2, 0x3F0);
//  volatile uint16_t ch = 0;
//  ch = sdGet(&SD2);

//  return OW_OK;
/*
  chThdSleepMilliseconds(1);

  char q[12];
  q[0] = 0x5A;
  q[1] = 0x00;//Address bit
  q[2] = 0x01;
  q[3] = 0x01;
  q[4] = 0xA5;
  q[5] = 0x00;
  q[6] = 0x5A;
  q[7] = 0x00;//Address bit
  q[8] = 0x02;
  q[9] = 0x01;
  q[10] = 0xA5;
  q[11] = 0x00;
*/
//  uartStartSend(&UARTD, 6, &q);

  chThdSleepSeconds(1);
//  __asm("BKPT #0\n");
 // z++;

#else

  static UARTConfig mpc_uart_cfg = {txend1, txend2, rxend, rxchar, rxerr, 38400, (USART_CR1_M | USART_CR1_WAKE),
                                    (USART_CR2_ADD & (MY_ADDR << 18)), USART_CR3_HDSEL};

  rccEnableUSART1(TRUE);

//  palSetPadMode(GPIOA, GPIOA_PIN2,
//                PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_MID | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_OPENDRAIN);
  palSetPadMode(GPIOA, GPIOA_PIN2,
                PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_OPENDRAIN);

  //  palSetPadMode(GPIOA, GPIOA_PIN3, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

  chThdSleepMilliseconds(1);

  uartStart(&UARTD, &mpc_uart_cfg);
//  USART_TypeDef *u = UARTD2.usart;
  UARTD.usart->CR1 |= USART_CR1_MME;
  UARTD.usart->RQR |= USART_RQR_MMRQ;

//  sdPut(&SD2, 0x3F0);
//  volatile uint16_t ch = 0;
//  ch = sdGet(&SD2);

//  return OW_OK;
  /*
   chThdSleepMilliseconds(1);

   uint16_t q[24];
   q[0] = 0x0055;
   q[1] = 0x0102;
   q[2] = 0x00AA;
   q[3] = 0x00BB;
   q[4] = 0x0101;
   q[5] = 0x0004;
   q[6] = 0x00CC;
   q[7] = 0x0066;
   q[8] = 0x0039;
   q[9] = 0x0102;
   q[10] = 0x0077;
   q[11] = 0x0088;
   q[12] = 0x0100;
   q[13] = 0x0099;
   q[14] = 0x0099;

   uartStartSend(&UARTD, 28, &q);

   volatile eventmask_t t = chEvtWaitOneTimeout((eventmask_t)EVENTMASK_UART_IRQ | EVENTMASK_UART_ERROR, S2ST(3));
   */
//  chThdSleepSeconds(5);
//  __asm("BKPT #0\n");
//  z++;
#endif

}

void inline _uart_send(payload_t * tx_buffer) {
  uint8_t x = 0;
  txb[0] = (((*tx_buffer).dst_addr & 0xC0) >> 6) | 0x0100;
  txb[1] = (*tx_buffer).size;
  for (x = 0; x < (*tx_buffer).size; x++) {
    txb[x + 2] = (*tx_buffer).pload[x];
  }
  uartStartSend(&UARTD, ((*tx_buffer).size + 2), &txb);
  chEvtWaitOne((eventmask_t)EVENTMASK_UART_TX_IRQ);
}

bool inline _uart_rcv_irq(payload_t * rx_buffer) {
  /* Perform processing here.*/
  (*rx_buffer).size = rxb[0];
  uint8_t x;
  for (x = 0; x < (*rx_buffer).size; x++) {
    (*rx_buffer).pload[x] = rxb[x + 1];
  }
  if ((*rx_buffer).dst_addr == MY_ADDR) {
    (*rx_buffer).pipenum = 1;
    return TRUE;
  }
  return FALSE;
}

#endif
