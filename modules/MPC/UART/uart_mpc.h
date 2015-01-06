#ifndef UART_MPC_H_
#define UART_MPC_H_

#if MPC_UART_PRESENT

#ifdef STM32F100C8
#define UARTD   UARTD2
#else
#define UARTD   UARTD1
#endif

void UART_MPC_init(void);
void inline _uart_send(payload_t * tx_buffer);
bool inline _uart_rcv_irq(payload_t * rx_buffer);

#endif

#endif
