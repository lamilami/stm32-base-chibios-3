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
#include <nRF24.h>
//#include "system.h"
#include "radio.h"
//#include <EERTOS.h>
//#include "target_includes.h"

void MAX_RT(void);
void TX_DS(void);
void RX_DR(void);
void TX_AP(void);
void Radio_Flush(void);
extern void Receive(void);
extern void Transmit(void);
//extern void LEDSwap(void);

#define EVENTMASK_IRQ 0x2
#define EVENTMASK_SEND 0x1

void radio_set_mode(nRF24_operation_mode_t rmode, uint8_t full_address[3]);
void radio_flush();

#define RF_MB_FREE 	0
#define RF_MB_RX 	1
#define RF_MB_TX	2

static mailbox_t rf_mb[3];
static msg_t rf_mb_b[3][RF_MAX_IO_BUFFERS];

static thread_t *Radio_Thread = NULL;
/** The payload sent over the radio. Also contains the recieved data. 
 * Should be read with radio_get_pload_byte(). */
//static xdata uint8_t pload[RF_PAYLOAD_LENGTH];
//static payload_t pload_rx;
//static payload_t pload_tx;
static uint8_t full_rx_addr[3], full_tx_addr[3];

//static payload_t payload[RF_MAX_IO_BUFFERS];

/** The current status of the radio. Should be set with radio_set_status(), 
 * and read with radio_get_status().
 */
volatile static radio_status_t status, status_prev;

//static uint8_t full_address[3] = {RF_BROADBAND_1ST_BYTE, RF_BROADBAND_2ND_BYTE, RF_3RD_BYTE};

THD_WORKING_AREA(waRadio, 256);
//__attribute__((noreturn))
THD_FUNCTION(Radio, arg);

THD_WORKING_AREA(waRadio_Processor, 256);
//__attribute__((noreturn))
THD_FUNCTION(Radio_Processor, arg);

void radio_send_packet(uint8_t *packet, uint8_t length)
{
	radio_set_status(RF_BUSY); // trans. in progress; RF_BUSY
	nRF24_write_tx_payload(packet, length); // load message into radio
//  nRF24_hw_ce_pulse_10us();                                 // send packet
}

radio_status_t radio_get_status(void)
{
	return status;
}

/*
 uint8_t radio_get_pload_byte(uint8_t byte_index)
 {
 return pload_rx.pload[byte_index];
 }*/

void radio_set_status(radio_status_t new_status)
{
	status_prev = status;
	status = new_status;
}

void radio_init(void)
{
	full_tx_addr[0] = RF_BROADBAND_1ST_BYTE;
	full_tx_addr[1] = RF_BROADBAND_2ND_BYTE;
	full_tx_addr[2] = RF_3RD_BYTE;
	full_rx_addr[0] = RF_BROADBAND_1ST_BYTE;
	full_rx_addr[1] = RF_BROADBAND_2ND_BYTE;
	full_rx_addr[2] = RF_3RD_BYTE;
	nRF24_close_pipe(nRF24_ALL); // First close all radio pipes
								 // Pipe 0 and 1 open by default
	nRF24_open_pipe(nRF24_PIPE0, true); // Then open pipe0, w/autoack
										// Changed from sb/radio_sb.c
	nRF24_open_pipe(nRF24_PIPE1, true); // Then open pipe0, w/autoack
										// Changed from sb/radio_sb.c
	nRF24_open_pipe(nRF24_PIPE2, true); // Then open pipe0, w/autoack
										// Changed from sb/radio_sb.c

	nRF24_set_crc_mode(nRF24_CRC_16BIT); // Operates in 16bits CRC mode
	nRF24_set_auto_retr(RF_RETRANSMITS, RF_RETRANS_DELAY);
	// Enables auto retransmit.
	// 3 retrans with 250ms delay
	// Changed from sb/radio_sb.c

	nRF24_set_address_width(nRF24_AW_3BYTES); // 3 bytes address width
//	nRF24_set_address(nRF24_TX, full_rx_addr); // Set device's addresses
//	nRF24_set_address(nRF24_PIPE0, full_rx_addr); // Sets recieving address on
			// pipe0
	full_tx_addr[1] = MY_ADDR;
	full_tx_addr[0] = RF_WORK_PIPE_BYTE;

	nRF24_set_address(nRF24_PIPE1, full_tx_addr); // Sets recieving address on
	// pipe1

	full_tx_addr[0] = RF_FW_PIPE_BYTE;
	nRF24_set_address(nRF24_PIPE2, full_tx_addr); // Sets recieving address on
	// pipe2
//	nRF24_set_operation_mode(nRF24_PRX); // Enter RX mode

	nRF24_set_output_power(RF_OUT_POWER);

	nRF24_write_reg(nRF24_DYNPD, (BIT_0 | BIT_1 | BIT_2)); // Enable DynPD on Pipe0-2
	nRF24_write_reg(nRF24_FEATURE, (BIT_0 | BIT_1 | BIT_2)); // Enable EN_DPL, EN_ACK_PAY, EN_DYN_ACK

	nRF24_set_rf_channel(RF_CHANNEL); // Operating on static channel
									  // Defined in radio.h.
									  // Frequenzy =
									  //        2400 + RF_CHANNEL
	radio_set_mode(nRF24_PRX, full_rx_addr);

	nRF24_set_power_mode(nRF24_PWR_UP); // Power up device

	chThdSleepMilliseconds(RF_POWER_UP_DELAY);

	radio_set_status(RF_IDLE); // Radio now ready
}

#if RADIO_PRESENT

CH_IRQ_HANDLER(EXTI0_1_IRQHandler)
{
	CH_IRQ_PROLOGUE();
//	nRF24_hw_ce_low();
#ifdef DEBUG_Discovery
//	LEDB1On();
#endif
	EXTI_ClearITPendingBit(EXTI_Line1);

	/* Wakes up the thread.*/
	osalSysLockFromISR();
	chEvtSignalI(Radio_Thread, (eventmask_t) EVENTMASK_IRQ);
	osalSysUnlockFromISR();

	CH_IRQ_EPILOGUE()
	;
}

#endif

//static WORKING_AREA(waRadio, 128);
//__attribute__((noreturn))
THD_FUNCTION(Radio,arg)
{
	(void) arg;
//	chRegSetThreadName("Radio");

	nRF24_Init();

	Radio_Thread = chThdGetSelfX();

	static payload_t payload[RF_MAX_IO_BUFFERS];
//	msg_t msg;
	int x;
	for (x = 0; x < RF_MAX_IO_BUFFERS; x++)
	{
		chMBObjectInit(&rf_mb[x], rf_mb_b[x], RF_MAX_IO_BUFFERS);
		msg_t msg = (msg_t) &payload[x];
		chMBPost(&rf_mb[RF_MB_FREE], msg, TIME_INFINITE);
	}

	radio_init();

	chThdCreateStatic(waRadio_Processor, sizeof(waRadio_Processor), NORMALPRIO,
			Radio_Processor, NULL);

//	nRF24_hw_ce_high();

	while (TRUE)
	{
		/* Checks if an IRQ happened else wait.*/
		eventmask_t t = chEvtWaitOne(
				(eventmask_t) EVENTMASK_IRQ | EVENTMASK_SEND);
		if (t == EVENTMASK_IRQ)
		{
			/* Perform processing here.*/
			switch (nRF24_get_clear_irq_flags())
			{
			case (1 << nRF24_IRQ_RX_DR): // Packet received
				LEDB1Swap();
				rf_sended_debug = TRUE;
				RX_DR();
				break;
			default:
#ifdef SPI_DMA
//				__asm("BKPT #0\n");
#endif
				// Break into the debugger
				radio_flush();
				break;
			}
		}
		else
		{
			// Send part
			LEDSwap();

			payload_t * tx_buffer = 0;
			chMBFetch(&rf_mb[RF_MB_TX], (msg_t *) &tx_buffer, TIME_INFINITE);
			switch ((*tx_buffer).pipenum)
			{
			case (nRF24_PIPE1):
				full_tx_addr[0] = RF_WORK_PIPE_BYTE;
				full_tx_addr[1] = (*tx_buffer).dst_addr;
				break;
			default:
				break;
			}

			radio_set_mode(nRF24_PTX, full_tx_addr);
			radio_set_status(RF_BUSY); // trans. in progress; RF_BUSY
			bool sended = TRUE;
			do
			{
				nRF24_write_tx_payload((*tx_buffer).pload, (*tx_buffer).size); // load message into radio
				t = chEvtWaitOne((eventmask_t) EVENTMASK_IRQ);
//				spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
				switch (nRF24_get_clear_irq_flags())
				{
				case (1 << nRF24_IRQ_MAX_RT): // Max retries reached
#ifdef SPI_DMA
//					__asm("BKPT #0\n");
#endif
					rf_sended_debug = FALSE;
//					TX_DS();
					break;
				case (1 << nRF24_IRQ_TX_DS): // Packet sent
					TX_DS();
					chMBPost(&rf_mb[RF_MB_FREE], (msg_t) tx_buffer,
							TIME_INFINITE);
					break;
				default:
					sended = FALSE;
					radio_flush();
//					TX_DS();
#ifdef SPI_DMA
//					__asm("BKPT #0\n");
#endif
					// Break into the debugger
					break;
				}
			} while (!sended);
		}
	}
}

void MAX_RT(void)
{
	nRF24_flush_tx(); // flush tx fifo, avoid fifo jam
	radio_set_status(RF_MAX_RT);
}

void TX_DS(void)
{
	radio_set_status(RF_TX_DS);
	radio_set_mode(nRF24_PRX, full_rx_addr);
}

void RX_DR(void)
{
	radio_set_status(RF_RX_DR);
//	uint16_t datasrc;
	payload_t * rx_buffer = 0;
	chMBFetch(&rf_mb[RF_MB_FREE], (msg_t *) &rx_buffer, TIME_INFINITE);
	while (!nRF24_rx_fifo_empty())
	{
		(*rx_buffer).whole_size_pipenum = nRF24_read_rx_payload(
				(*rx_buffer).pload);
	}
//	pload_rx.size=datasrc.byte.size;
//	while (DMA1_SPI_get_status()!=DMA1_SPI_IDLE);
	switch ((*rx_buffer).pipenum)
	{
	case (nRF24_PIPE1):
		if ((*rx_buffer).pload[1] == MY_ADDR)
		{
//			Radio_Receive_Command();
			chMBPost(&rf_mb[RF_MB_RX], (msg_t) rx_buffer, TIME_INFINITE);
		}
		break;
	default:
//		nRF24_hw_ce_high();
//			SetTask(Receive,HighPriority);
		chMBPost(&rf_mb[RF_MB_FREE], (msg_t) rx_buffer, TIME_INFINITE);
		break;
	}
//	nRF24_hw_ce_high();
//	SetTask(Receive,HighPriority);
}
/*
 void TX_AP(void)
 {
 while (!nRF24_rx_fifo_empty())
 {
 nRF24_read_rx_payload(pload_rx.pload);
 }
 radio_set_status(RF_TX_AP);
 //	nRF24_hw_ce_high();
 }
 */

void radio_flush()
{
	nRF24_flush_rx();
	nRF24_flush_tx();
	nRF24_get_clear_irq_flags();
}

void radio_set_mode(nRF24_operation_mode_t rmode, uint8_t full_address[3])
{
	nRF24_hw_ce_low();
	radio_flush();
	nRF24_set_operation_mode(rmode);
	nRF24_set_address(nRF24_TX, full_address); // Set device's addresses
	nRF24_set_address(nRF24_PIPE0, full_address); // Sets recieving address on
//	nRF24_get_address(nRF24_PIPE0, full_tx_addr);
	nRF24_hw_ce_high();
}

/*
 void Radio_Send(void)
 {
 chEvtSignal(Radio_Thread, (eventmask_t) EVENTMASK_SEND);
 }
 */
void Radio_Send_Command(uint8_t rcv_addr, RF_commands_t command,
		uint8_t data_size, uint8_t *data)
{
	payload_t * tx_buffer = 0;
	chMBFetch(&rf_mb[RF_MB_FREE], (msg_t *) &tx_buffer, TIME_INFINITE);
	(*tx_buffer).src_addr = MY_ADDR;
	(*tx_buffer).dst_addr = rcv_addr;
	(*tx_buffer).cmd = command;
	(*tx_buffer).pipenum = 1;
	(*tx_buffer).size = 3 + data_size;
	ByteArrayCopy(data, (*tx_buffer).data, data_size);

	/*	full_tx_addr[0] = RF_WORK_PIPE_BYTE;
	 full_tx_addr[1] = rcv_addr;
	 pload_tx.src_addr = MY_ADDR;
	 pload_tx.dst_addr = rcv_addr;
	 pload_tx.data[0] = command;
	 pload_tx.pipenum = 1;*/

	switch (command)
	{
	case RF_PING:
	case RF_PONG:
	case RF_GET:
	case RF_PUT:
		chMBPost(&rf_mb[RF_MB_TX], (msg_t) tx_buffer, TIME_INFINITE);
		chEvtSignal(Radio_Thread, (eventmask_t) EVENTMASK_SEND);
		break;
	default:
		chMBPost(&rf_mb[RF_MB_FREE], (msg_t) tx_buffer, TIME_INFINITE);
		break;
	}
}

//__attribute__((noreturn))
THD_FUNCTION(Radio_Processor,arg)
{
	(void) arg;
//	chRegSetThreadName("Radio_Processor");
	payload_t * rx_buffer = 0;
//	uint8_t cnt;
	chThdSleepSeconds(1);
	while (TRUE)
	{
		chMBFetch(&rf_mb[RF_MB_RX], (msg_t *) &rx_buffer, TIME_INFINITE);
//		LEDBlinkS(1);
		chThdSleepMilliseconds(100);
		switch ((*rx_buffer).cmd)
		{
		case RF_PING:
//			LEDB1Swap();
			Radio_Send_Command((*rx_buffer).src_addr, RF_PONG, 0, NULL); // load message into radio
			break;
		case RF_PONG:
//			LEDB1Swap();
			Radio_Send_Command((*rx_buffer).src_addr, RF_PING, 0, NULL);
//			nRF24_hw_ce_high();
			break;
/*		case RF_GET:
			cnt = Core_GetDataById((*rx_buffer).data[0],
					(uint16_t*) &((*rx_buffer).data[1]));
			Radio_Send_Command((*rx_buffer).src_addr, RF_PUT, cnt + 1,
					(*rx_buffer).data);
			break;
		case RF_PUT:
			chSysLock();
			ByteArrayCopy((*rx_buffer).data, PutData[(*rx_buffer).data[0]],
					(*rx_buffer).size);
			chSysUnlock();
			break;
		case RF_SET:
			Core_SetDataById((*rx_buffer).data[0],(uint16_t) ((*rx_buffer).data[1]+256*(*rx_buffer).data[2]));
			Radio_Send_Command((*rx_buffer).src_addr, RF_OK, 0, NULL); // load message into radio
			break;*/
		default:
//		nRF24_hw_ce_high();
			break;
		}
		chMBPost(&rf_mb[RF_MB_FREE], (msg_t) rx_buffer, TIME_INFINITE);
	}
}

void Radio_Start()
{
#if RADIO_PRESENT
	chThdCreateStatic(waRadio, sizeof(waRadio), NORMALPRIO, Radio, NULL);
	chThdYield();
#endif
}
