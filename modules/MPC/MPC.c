#include "ch.h"
#include "hal.h"
#include "core.h"

#if MPC_PRESENT

#include "MPC.h"
#include "string.h"
#if MPC_RADIO_PRESENT
#include "radio.h"
#endif
#if MPC_UART_PRESENT
#include "uart_mpc.h"
#endif

#define MB_CNT 4
#define RF_MAX_IO_BUFFERS 4

static mailbox_t rf_mb[MB_CNT];
static msg_t rf_mb_b[MB_CNT][RF_MAX_IO_BUFFERS];

#define RF_MB_FREE  0
#define RF_MB_RX    1
#define RF_MB_TX    2
#define RF_MB_PS    3

THD_WORKING_AREA(waMPC_Processor, 256);
THD_FUNCTION(MPC_Processor,arg) {
  (void)arg;
//  chRegSetThreadName("Radio_Processor");
  payload_t * rx_buffer = 0;
  uint8_t cnt = 0;
  chThdSleepSeconds(1);
  while (TRUE) {
    chMBFetch(&rf_mb[RF_MB_RX], (msg_t *)&rx_buffer, TIME_INFINITE);
//      LEDBlinkS(1);
//      chThdSleepMilliseconds(100);

    switch ((*rx_buffer).cmd) {
    case RF_PING:
//          LEDB1Swap();
      MPC_Send_Command((*rx_buffer).src_addr, RF_PONG, 0, NULL); // load message into radio
      chMBPost(&rf_mb[RF_MB_FREE], (msg_t)rx_buffer, TIME_INFINITE);
      break;
    case RF_PONG:
//          LEDB1Swap();
//      MPC_Send_Command((*rx_buffer).src_addr, RF_PING, 0, NULL);
      chMBPost(&rf_mb[RF_MB_FREE], (msg_t)rx_buffer, TIME_INFINITE);
//          nRF24_hw_ce_high();
      break;
    case RF_GET:
//          cnt = Core_GetDataById((*rx_buffer).data[0], (uint16_t*) &((*rx_buffer).data[1]));
      cnt = Core_Module_Read(localhost, (*rx_buffer).data[0], (char *)(*rx_buffer).data);
      MPC_Send_Command((*rx_buffer).src_addr, RF_PUT, cnt, (*rx_buffer).data);
      chMBPost(&rf_mb[RF_MB_FREE], (msg_t)rx_buffer, TIME_INFINITE);
      break;
    case RF_PUT:
      chSysLock();
      if (chMBGetUsedCountI(&rf_mb[RF_MB_PS]) < 0) {
        chMBPostS(&rf_mb[RF_MB_PS], (msg_t)rx_buffer, TIME_INFINITE);
      }
      else {
        chMBPostS(&rf_mb[RF_MB_FREE], (msg_t)rx_buffer, TIME_INFINITE);
      }
      chSysUnlock();
      break;
      /*
       case RF_SET:
       Core_SetDataById((*rx_buffer).data[0],(uint16_t) ((*rx_buffer).data[1]+256*(*rx_buffer).data[2]));
       Radio_Send_Command((*rx_buffer).src_addr, RF_OK, 0, NULL); // load message into radio
       break;*/
    default:
      chMBPost(&rf_mb[RF_MB_FREE], (msg_t)rx_buffer, TIME_INFINITE);
      break;
    }
  }
}

THD_WORKING_AREA(waMPC, 256);
THD_FUNCTION(MPC,arg) {
  (void)arg;
//  chRegSetThreadName("Radio");

  MPC_Thread = chThdGetSelfX();

  static payload_t payload[RF_MAX_IO_BUFFERS];
  uint8_t x;

  for (x = 0; x < MB_CNT; x++) {
    chMBObjectInit(&rf_mb[x], rf_mb_b[x], RF_MAX_IO_BUFFERS);
  }

//  osalSysLock();
  for (x = 0; x < RF_MAX_IO_BUFFERS; x++) {
    chMBPost(&rf_mb[RF_MB_FREE], (msg_t)&payload[x], TIME_INFINITE);
  }
//  osalSysUnlock();

#if MPC_RADIO_PRESENT
  radio_init();
#endif

#if MPC_UART_PRESENT
  UART_MPC_init();
#endif

  chThdCreateStatic(waMPC_Processor, sizeof(waMPC_Processor), NORMALPRIO, MPC_Processor, NULL);

  while (TRUE) {
    /* Checks if an IRQ happened else wait.*/
    eventmask_t t = chEvtWaitOne((eventmask_t)EVENTMASK_RADIO_IRQ | EVENTMASK_UART_IRQ | EVENTMASK_SEND);
    switch (t) {
    case (EVENTMASK_RADIO_IRQ):
#if MPC_RADIO_PRESENT
      ;
      static payload_t * rx_buffer = 0;
      chMBFetch(&rf_mb[RF_MB_FREE], (msg_t *)&rx_buffer, TIME_INFINITE);
      if (_radio_rcv_irq(rx_buffer)) {
        chMBPost(&rf_mb[RF_MB_RX], (msg_t)rx_buffer, TIME_INFINITE);
      }
      else {
        chMBPost(&rf_mb[RF_MB_FREE], (msg_t)rx_buffer, TIME_INFINITE);
      }
#else
      __asm("BKPT #0\n");
#endif
      break;
    case (EVENTMASK_UART_IRQ):
      ;
      static payload_t * rx_buffer = 0;
      chMBFetch(&rf_mb[RF_MB_FREE], (msg_t *)&rx_buffer, TIME_INFINITE);
      if (_uart_rcv_irq(rx_buffer)) {
        chMBPost(&rf_mb[RF_MB_RX], (msg_t)rx_buffer, TIME_INFINITE);
      }
      else {
        chMBPost(&rf_mb[RF_MB_FREE], (msg_t)rx_buffer, TIME_INFINITE);
      }
    case (EVENTMASK_SEND):
      // Send part
      LEDSwap();

      payload_t * tx_buffer = 0;
      chMBFetch(&rf_mb[RF_MB_TX], (msg_t *)&tx_buffer, TIME_INFINITE);
      if ((((*tx_buffer).dst_addr & 0x3F) == (MY_ADDR & 0x3F)) ) { //  && ((*tx_buffer).dst_addr != MY_ADDR)) {
#if MPC_UART_PRESENT
        _uart_send(tx_buffer);
#else
        __asm("BKPT #0\n");
#endif
      }
      else {
#if MPC_RADIO_PRESENT
        _radio_send(tx_buffer);
#else
        __asm("BKPT #0\n");
#endif
      }
      chMBPost(&rf_mb[RF_MB_FREE], (msg_t)tx_buffer, TIME_INFINITE);
      break;
    default:
      __asm("BKPT #0\n");
      break;
    }
  }
}

uint8_t MPC_Send_Command(uint8_t rcv_addr, MPC_commands_t command, uint8_t data_size, void *data) {
  payload_t * tx_buffer = 0;
  chMBFetch(&rf_mb[RF_MB_FREE], (msg_t *)&tx_buffer, TIME_INFINITE);
//  (*tx_buffer).src_addr = (MY_ADDR & 0x3F);
  (*tx_buffer).src_addr = MY_ADDR;
  (*tx_buffer).dst_addr = rcv_addr;
  (*tx_buffer).cmd = command;
  (*tx_buffer).pipenum = 1;
  (*tx_buffer).size = 3 + data_size;
  memcpy((void *)(*tx_buffer).data, data, data_size);
  uint8_t ret_size = data_size;

  switch (command) {
  case RF_PING:
  case RF_PONG:
  case RF_PUT:
    chMBPost(&rf_mb[RF_MB_TX], (msg_t)tx_buffer, TIME_INFINITE);
    chEvtSignal(MPC_Thread, (eventmask_t)EVENTMASK_SEND);
    break;
  case RF_GET:
    chMBPost(&rf_mb[RF_MB_TX], (msg_t)tx_buffer, TIME_INFINITE);
    chEvtSignal(MPC_Thread, (eventmask_t)EVENTMASK_SEND);

    ret_size = chMBFetch(&rf_mb[RF_MB_PS], (msg_t *)&tx_buffer, MS2ST(500));
    if (ret_size == MSG_OK) {
      ret_size = (*tx_buffer).size - 3;
      memcpy(data, (void *)(*tx_buffer).data, ret_size);
      chMBPost(&rf_mb[RF_MB_FREE], (msg_t)tx_buffer, TIME_INFINITE);
    }
    break;
  default:
    chMBPost(&rf_mb[RF_MB_FREE], (msg_t)tx_buffer, TIME_INFINITE);
    break;
  }
  return ret_size;
}

void MPC_Start() {
  chThdCreateStatic(waMPC, sizeof(waMPC), NORMALPRIO, MPC, NULL);
  chThdYield();
}

#endif
