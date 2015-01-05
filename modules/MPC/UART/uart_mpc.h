#ifndef UART_MPC_H_
#define UART_MPC_H_

#if MPC_UART_PRESENT

void UART_MPC_init(void);
void inline _uart_send(payload_t * tx_buffer);

#endif

#endif
