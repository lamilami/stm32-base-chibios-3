/* Copyright (c) 2007 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision: 2310 $
 */

/** @file
 * @ingroup Main
 * Radio functions.
 *
 * This file handles all radio communication for the example application, i.e. 
 * radio_init, radio_send_packet and radio_interrupt function.
 *
 * @author Per Kristian Schanke
 */

#include "ch.h"
#include "hal.h"
#include "core.h"

#if MPC_RADIO_PRESENT

#include <nRF24.h>
#include "radio.h"

//void RX_DR(void);
void Radio_Flush(void);

void radio_set_mode(nRF24_operation_mode_t rmode, uint8_t full_address[3]);
void radio_flush();

static uint8_t full_rx_addr[3], full_tx_addr[3];     //, test_addr[3];

/** The current status of the radio. Should be set with radio_set_status(), 
 * and read with radio_get_status().
 */
volatile static radio_status_t status, status_prev;

//static uint8_t full_address[3] = {RF_BROADBAND_1ST_BYTE, RF_BROADBAND_2ND_BYTE, RF_3RD_BYTE};

radio_status_t radio_get_status(void) {
  return status;
}

void radio_set_status(radio_status_t new_status) {
  status_prev = status;
  status = new_status;
}

void radio_init(void) {
  nRF24_Init();

  full_rx_addr[0] = full_tx_addr[0] = RF_BROADBAND_1ST_BYTE;
  full_rx_addr[1] = full_tx_addr[1] = RF_BROADBAND_2ND_BYTE;
  full_rx_addr[2] = full_tx_addr[2] = RF_3RD_BYTE;

  nRF24_close_pipe(nRF24_ALL);     // First close all radio pipes
  // Pipe 0 and 1 open by default
  nRF24_open_pipe(nRF24_PIPE0, true);     // Then open pipe0, w/autoack
  // Changed from sb/radio_sb.c
  nRF24_open_pipe(nRF24_PIPE1, true);     // Then open pipe0, w/autoack
  // Changed from sb/radio_sb.c
  nRF24_open_pipe(nRF24_PIPE2, true);     // Then open pipe0, w/autoack
  // Changed from sb/radio_sb.c

  nRF24_set_crc_mode(nRF24_CRC_16BIT);     // Operates in 16bits CRC mode
  nRF24_set_auto_retr(RF_RETRANSMITS, RF_RETRANS_DELAY);
  // Enables auto retransmit.
  // 3 retrans with 250ms delay
  // Changed from sb/radio_sb.c

  nRF24_set_address_width(nRF24_AW_3BYTES);     // 3 bytes address width

  // pipe0
  full_tx_addr[1] = (MY_ADDR & 0x3F);
  full_tx_addr[0] = RF_WORK_PIPE_BYTE;

  nRF24_set_address(nRF24_PIPE1, full_tx_addr);     // Sets recieving address on pipe1
//  nRF24_get_address(nRF24_PIPE1, test_addr);

  full_tx_addr[0] = RF_FW_PIPE_BYTE;
  nRF24_set_address(nRF24_PIPE2, full_tx_addr);     // Sets recieving address on pipe2

  nRF24_set_output_power(RF_OUT_POWER);

  nRF24_write_reg(nRF24_DYNPD, (BIT_0 | BIT_1 | BIT_2));     // Enable DynPD on Pipe0-2
  nRF24_write_reg(nRF24_FEATURE, (BIT_0 | BIT_1 | BIT_2));     // Enable EN_DPL, EN_ACK_PAY, EN_DYN_ACK

  nRF24_set_rf_channel(RF_CHANNEL);     // Operating on static channel
  // Defined in radio.h.
  // Frequenzy =
  //        2400 + RF_CHANNEL
  radio_set_mode(nRF24_PRX, full_rx_addr);

  nRF24_set_power_mode(nRF24_PWR_UP);     // Power up device

  chThdSleepMilliseconds(RF_POWER_UP_DELAY);

  radio_set_status(RF_IDLE);     // Radio now ready
}

void nRF24_irq_ext_handler(EXTDriver *extp, expchannel_t channel) {

  (void)extp;

  /* Wakes up the thread.*/
  osalSysLockFromISR();
  chEvtSignalI(MPC_Thread, (eventmask_t)EVENTMASK_RADIO_IRQ);
  osalSysUnlockFromISR();
}

/*
bool inline RX_DR(payload_t * rx_buffer) {
  radio_set_status(RF_RX_DR);

  while (!nRF24_rx_fifo_empty()) {
    (*rx_buffer).whole_size_pipenum = nRF24_read_rx_payload((*rx_buffer).pload);
  }
  switch ((*rx_buffer).pipenum) {
  case (nRF24_PIPE1):
    if (((*rx_buffer).pload[1] & 0x3F) == (MY_ADDR & 0x3F)) {
      return TRUE;
    }
    break;
  default:
    break;
  }
  return FALSE;
}
*/

bool inline _radio_rcv_irq(payload_t * rx_buffer) {
  /* Perform processing here.*/
  switch (nRF24_get_clear_irq_flags()) {
  case (1 << nRF24_IRQ_RX_DR):     // Packet received
//    LEDB1Swap();
    rf_sended_debug = TRUE;
    radio_set_status(RF_RX_DR);

    while (!nRF24_rx_fifo_empty()) {
      (*rx_buffer).whole_size_pipenum = nRF24_read_rx_payload((*rx_buffer).pload);
    }
    switch ((*rx_buffer).pipenum) {
    case (nRF24_PIPE1):
      if (((*rx_buffer).pload[1] & 0x3F) == (MY_ADDR & 0x3F)) {
        return TRUE;
      }
      break;
    default:
      break;
    }
    break;
  default:
#ifdef SPI_DMA
//              __asm("BKPT #0\n");
#endif
    // Break into the debugger
    radio_flush();
    break;
  }
  return FALSE;
}

void inline _radio_send(payload_t * tx_buffer) {

  switch ((*tx_buffer).pipenum) {
  case (nRF24_PIPE1):
    full_tx_addr[0] = RF_WORK_PIPE_BYTE;
    full_tx_addr[1] = ((*tx_buffer).dst_addr & 0x3F);
    break;
  default:
    break;
  }

  radio_set_mode(nRF24_PTX, full_tx_addr);
  radio_set_status(RF_BUSY);     // trans. in progress; RF_BUSY
  bool sended = TRUE;
  do {
    nRF24_write_tx_payload((*tx_buffer).pload, (*tx_buffer).size);     // load message into radio
    chEvtWaitOne((eventmask_t)EVENTMASK_RADIO_IRQ);
    switch (nRF24_get_clear_irq_flags()) {
    case (1 << nRF24_IRQ_MAX_RT):     // Max retries reached
      rf_sended_debug = FALSE;
      radio_set_status(RF_MAX_RT);
      radio_set_mode(nRF24_PRX, full_rx_addr);
      break;
    case (1 << nRF24_IRQ_TX_DS):     // Packet sent
      radio_set_status(RF_TX_DS);
      radio_set_mode(nRF24_PRX, full_rx_addr);
      break;
    default:
      sended = FALSE;
      radio_flush();
      break;
    }
  } while (!sended);
}

void radio_flush() {
  nRF24_flush_rx();
  nRF24_flush_tx();
  nRF24_get_clear_irq_flags();
}

void radio_set_mode(nRF24_operation_mode_t rmode, uint8_t full_address[3]) {
  nRF24_hw_ce_low();
  radio_flush();
  nRF24_set_operation_mode(rmode);
  nRF24_set_address(nRF24_TX, full_address);     // Set device's addresses
  nRF24_set_address(nRF24_PIPE0, full_address);     // Sets recieving address on
  nRF24_hw_ce_high();
}

#endif
