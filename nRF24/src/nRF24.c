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
 * $LastChangedRevision: 2132 $
 */

/** @file
 *
 * @author Runar Kjellhaug
 *
 */

#include <stdint.h>
#include <stdbool.h>

//#include "nordic_common.h"
#include "nRF24.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BIT(index) ((uint8_t)1 << (uint8_t)(index))
#define BIT_COND(data,index,condition) (((uint8_t)(data) & ~BIT(index)) | ((condition) ? BIT(index) : (uint8_t)0))

void nRF24_set_irq_mode(nRF24_irq_source_t int_source, bool irq_state)
{
	nRF24_write_reg(nRF24_CONFIG,
			BIT_COND(nRF24_read_reg(nRF24_CONFIG), int_source, !irq_state));
}

uint8_t nRF24_get_clear_irq_flags(void)
{
	enum
	{
		IRQ_FLAGS = BIT(nRF24_IRQ_MAX_RT) | BIT(nRF24_IRQ_TX_DS)
				| BIT(nRF24_IRQ_RX_DR)
	};
	return nRF24_write_reg(nRF24_STATUS, IRQ_FLAGS) & IRQ_FLAGS;
}

void nRF24_clear_irq_flag(nRF24_irq_source_t int_source)
{
	nRF24_write_reg(nRF24_STATUS, BIT(int_source));
}

bool nRF24_get_irq_mode(nRF24_irq_source_t int_type)
{
	return !(nRF24_read_reg(nRF24_CONFIG) & BIT(int_type));
}

uint8_t nRF24_get_irq_flags(void)
{
	return nRF24_nop() & (BIT(6) | BIT(5) | BIT(4));
}

void nRF24_set_crc_mode(nRF24_crc_mode_t crc_mode)
{
	nRF24_write_reg(nRF24_CONFIG,
			(nRF24_read_reg(nRF24_CONFIG) & ~(BIT(3) | BIT(2)))
					| ((uint8_t) crc_mode << 2u));
}

void nRF24_open_pipe(nRF24_address_t pipe_num, bool auto_ack)
{
	switch (pipe_num)
	{
	case nRF24_PIPE0:
	case nRF24_PIPE1:
	case nRF24_PIPE2:
	case nRF24_PIPE3:
	case nRF24_PIPE4:
	case nRF24_PIPE5:
		nRF24_write_reg(nRF24_EN_RXADDR,
				nRF24_read_reg(nRF24_EN_RXADDR) | BIT(pipe_num));
		nRF24_write_reg(nRF24_EN_AA,
				BIT_COND(nRF24_read_reg(nRF24_EN_AA), pipe_num, auto_ack));
		break;

	case nRF24_ALL:
		nRF24_write_reg(nRF24_EN_RXADDR, (uint8_t) ~(BIT(7) | BIT(6)));

		if (auto_ack)
			nRF24_write_reg(nRF24_EN_AA, (uint8_t) ~(BIT(7) | BIT(6)));
		else
			nRF24_write_reg(nRF24_EN_AA, 0);
		break;

	default:
		break;
	}
}

void nRF24_close_pipe(nRF24_address_t pipe_num)
{
	switch (pipe_num)
	{
	case nRF24_PIPE0:
	case nRF24_PIPE1:
	case nRF24_PIPE2:
	case nRF24_PIPE3:
	case nRF24_PIPE4:
	case nRF24_PIPE5:
		nRF24_write_reg(nRF24_EN_RXADDR,
				nRF24_read_reg(nRF24_EN_RXADDR) & ~BIT(pipe_num));
		nRF24_write_reg(nRF24_EN_AA,
				nRF24_read_reg(nRF24_EN_AA) & ~BIT(pipe_num));
		break;

	case nRF24_ALL:
		nRF24_write_reg(nRF24_EN_RXADDR, 0);
		nRF24_write_reg(nRF24_EN_AA, 0);
		break;

	default:
		break;
	}
}

void nRF24_set_address(nRF24_address_t address, const uint8_t *addr)
{
	switch (address)
	{
	case nRF24_TX:
	case nRF24_PIPE0:
	case nRF24_PIPE1:
		nRF24_write_multibyte_reg((uint8_t) address, addr, 0);
		break;

	case nRF24_PIPE2:
	case nRF24_PIPE3:
	case nRF24_PIPE4:
	case nRF24_PIPE5:
		nRF24_write_reg(nRF24_RX_ADDR_P0 + (uint8_t) address, *addr);
		break;

	default:
		break;
	}
}

void nRF24_set_auto_retr(uint8_t retr, uint16_t delay)
{
	nRF24_write_reg(nRF24_SETUP_RETR, (((delay / 250) - 1) << 4) | retr);
}

void nRF24_set_address_width(nRF24_address_width_t address_width)
{
	nRF24_write_reg(nRF24_SETUP_AW, (uint8_t) address_width - 2);
}

void nRF24_set_rx_payload_width(uint8_t pipe_num, uint8_t payload_width)
{
	nRF24_write_reg(nRF24_RX_PW_P0 + pipe_num, payload_width);
}

uint8_t nRF24_get_crc_mode(void)
{
	return (nRF24_read_reg(nRF24_CONFIG) & (BIT(3) | BIT(2)))
			>> nRF24_CONFIG_CRCO;
}

uint8_t nRF24_get_pipe_status(uint8_t pipe_num)
{
	uint8_t en_rx, en_aa;

	en_rx = nRF24_read_reg(nRF24_EN_RXADDR) & (1 << pipe_num);
	en_aa = nRF24_read_reg(nRF24_EN_AA) & (1 << pipe_num);

	en_rx >>= pipe_num;
	en_aa >>= pipe_num;

	return (en_aa << 1) + en_rx;
}

uint8_t nRF24_get_address(uint8_t address, uint8_t *addr)
{
	switch (address)
	{
	case nRF24_PIPE0:
	case nRF24_PIPE1:
	case nRF24_TX:
		return nRF24_read_multibyte_reg(address, addr);

	default:
		*addr = nRF24_read_reg(nRF24_RX_ADDR_P0 + address);
		return nRF24_get_address_width();
	}
}

uint8_t nRF24_get_auto_retr_status(void)
{
	return nRF24_read_reg(nRF24_OBSERVE_TX);
}

uint8_t nRF24_get_packet_lost_ctr(void)
{
	return (nRF24_read_reg(nRF24_OBSERVE_TX)
			& (BIT(7) | BIT(6) | BIT(5) | BIT(4))) >> 4;
}

uint8_t nRF24_get_address_width(void)
{
	return (nRF24_read_reg(nRF24_SETUP_AW) + 2);
}

uint8_t nRF24_get_rx_payload_width(uint8_t pipe_num)
{
	return nRF24_read_reg(nRF24_RX_PW_P0 + pipe_num);
}

void nRF24_set_operation_mode(nRF24_operation_mode_t op_mode)
{
	nRF24_write_reg(nRF24_CONFIG,
			BIT_COND(nRF24_read_reg(nRF24_CONFIG), nRF24_CONFIG_PRIM_RX, op_mode == nRF24_PRX));
}

void nRF24_set_power_mode(nRF24_pwr_mode_t pwr_mode)
{
	nRF24_write_reg(nRF24_CONFIG,
			BIT_COND(nRF24_read_reg(nRF24_CONFIG), nRF24_CONFIG_PWR_UP, pwr_mode == nRF24_PWR_UP));
}

void nRF24_set_rf_channel(uint8_t channel)
{
	nRF24_write_reg(nRF24_RF_CH, channel);
}

void nRF24_set_output_power(nRF24_output_power_t power)
{
	nRF24_write_reg(nRF24_RF_SETUP,
			(nRF24_read_reg(nRF24_RF_SETUP)
					& ~((1 << nRF24_RF_SETUP_RF_PWR1)
							| (1 << nRF24_RF_SETUP_RF_PWR0)))
					| ((uint8_t) power << 1));
}

void nRF24_set_datarate(nRF24_datarate_t datarate)
{
	nRF24_write_reg(nRF24_CONFIG,
			BIT_COND(nRF24_read_reg(nRF24_RF_SETUP), nRF24_RF_SETUP_RF_DR, datarate != nRF24_1MBPS));
}

uint8_t nRF24_get_operation_mode(void)
{
	return (nRF24_read_reg(nRF24_CONFIG) & (1 << nRF24_CONFIG_PRIM_RX))
			>> nRF24_CONFIG_PRIM_RX;
}

uint8_t nRF24_get_power_mode(void)
{
	return (nRF24_read_reg(nRF24_CONFIG) & (1 << nRF24_PWR_UP)) >> nRF24_PWR_UP;
}

uint8_t nRF24_get_rf_channel(void)
{
	return nRF24_read_reg(nRF24_RF_CH);
}

uint8_t nRF24_get_output_power(void)
{
	return (nRF24_read_reg(nRF24_RF_SETUP)
			& ((1 << nRF24_RF_SETUP_RF_PWR1) | (1 << nRF24_RF_SETUP_RF_PWR0)))
			>> nRF24_RF_SETUP_RF_PWR0;
}

uint8_t nRF24_get_datarate(void)
{
	return (nRF24_read_reg(nRF24_RF_SETUP) & (1 << nRF24_RF_SETUP_RF_DR))
			>> nRF24_RF_SETUP_RF_DR;
}

bool nRF24_rx_fifo_empty(void)
{
	return nRF24_get_rx_data_source() == 7;
}

bool nRF24_rx_fifo_full(void)
{
	return (bool) ((nRF24_read_reg(nRF24_FIFO_STATUS)
			>> nRF24_FIFO_STATUS_RX_EMPTY) & 1);
}

bool nRF24_tx_fifo_empty(void)
{
	return (bool) ((nRF24_read_reg(nRF24_FIFO_STATUS)
			>> nRF24_FIFO_STATUS_TX_EMPTY) & 1);
}

bool nRF24_tx_fifo_full(void)
{
	return (bool) ((nRF24_read_reg(nRF24_FIFO_STATUS)
			>> nRF24_FIFO_STATUS_TX_FIFO_FULL) & 1);
}

uint8_t nRF24_get_tx_fifo_status(void)
{
	return ((nRF24_read_reg(nRF24_FIFO_STATUS)
			& ((1 << nRF24_FIFO_STATUS_TX_FIFO_FULL)
					| (1 << nRF24_FIFO_STATUS_TX_EMPTY))) >> 4);
}

uint8_t nRF24_get_rx_fifo_status(void)
{
	return (nRF24_read_reg(nRF24_FIFO_STATUS)
			& ((1 << nRF24_FIFO_STATUS_RX_FULL)
					| (1 << nRF24_FIFO_STATUS_RX_EMPTY)));
}

uint8_t nRF24_get_fifo_status(void)
{
	return nRF24_read_reg(nRF24_FIFO_STATUS);
}

uint8_t nRF24_get_transmit_attempts(void)
{
	return nRF24_read_reg(nRF24_OBSERVE_TX)
			& (BIT(3) | BIT(2) | BIT(1) | BIT(0));
}

bool nRF24_get_carrier_detect(void)
{
	return nRF24_read_reg(nRF24_CD) & 1;
}

void nRF24_write_tx_payload(const uint8_t *tx_payload, uint8_t length)
{
	nRF24_write_multibyte_reg(nRF24_TX_PAYLOAD, tx_payload, length);
}

void nRF24_setup_dyn_pl(uint8_t setup)
{
	nRF24_write_reg(nRF24_DYNPD, setup & ~0xC0);
}

void nRF24_enable_dynamic_pl(void)
{
	nRF24_write_reg(nRF24_FEATURE, (nRF24_read_reg(nRF24_FEATURE) | 0x04));
}

void nRF24_disable_dynamic_pl(void)
{
	nRF24_write_reg(nRF24_FEATURE, (nRF24_read_reg(nRF24_FEATURE) & ~0x04));
}

void nRF24_enable_ack_pl(void)
{
	nRF24_write_reg(nRF24_FEATURE, (nRF24_read_reg(nRF24_FEATURE) | 0x02));
}

void nRF24_disable_ack_pl(void)
{
	nRF24_write_reg(nRF24_FEATURE, (nRF24_read_reg(nRF24_FEATURE) & ~0x02));
}

void nRF24_enable_dynamic_ack(void)
{
	nRF24_write_reg(nRF24_FEATURE, (nRF24_read_reg(nRF24_FEATURE) | 0x01));
}

void nRF24_disable_dynamic_ack(void)
{
	nRF24_write_reg(nRF24_FEATURE, (nRF24_read_reg(nRF24_FEATURE) & ~0x01));
}

void nRF24_write_ack_payload(uint8_t pipe, const uint8_t *tx_payload,
		uint8_t length)
{
//  nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);
	//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
	spiSelect(&SPID1); /* Slave Select assertion.          */

//  nRF24_hw_spi_rw(nRF24_WR_ACK_PAYLOAD | pipe);
	spiPolledExchange(&SPID1, nRF24_WR_ACK_PAYLOAD | pipe);
	while (length--)
//    nRF24_hw_spi_rw(*tx_payload++);
		spiPolledExchange(&SPID1, *tx_payload++);

//  nRF24_hw_spi_ss(nRF24_SPI_SS_HIGH);
	spiUnselect(&SPID1); /* Slave Select de-assertion.       */
	//spiReleaseBus(&SPID1);              /* Ownership release.               */
}

uint8_t nRF24_read_rx_fifo_payload_width()
{
	uint8_t temp;

//  nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);
	//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
	spiSelect(&SPID1); /* Slave Select assertion.          */

//  nRF24_hw_spi_rw(nRF24_RD_RX_PAYLOAD_WIDTH);
	spiPolledExchange(&SPID1, nRF24_RD_RX_PAYLOAD_WIDTH);
//  temp = nRF24_hw_spi_rw(0);
	temp = spiPolledExchange(&SPID1,0);
//  nRF24_hw_spi_ss(nRF24_SPI_SS_HIGH);
	spiUnselect(&SPID1); /* Slave Select de-assertion.       */
	//spiReleaseBus(&SPID1);              /* Ownership release.               */

	return temp;
}

void nRF24_lock_unlock()
{
	/*  nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);

	 nRF24_hw_spi_rw(nRF24_LOCK_UNLOCK);
	 nRF24_hw_spi_rw(0x73);

	 nRF24_hw_spi_ss(nRF24_SPI_SS_HIGH);*/
	//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
	spiSelect(&SPID1); /* Slave Select assertion.          */
	spiPolledExchange(&SPID1, nRF24_LOCK_UNLOCK);
	spiPolledExchange(&SPID1, 0x73);
	spiUnselect(&SPID1); /* Slave Select de-assertion.       */
	//spiReleaseBus(&SPID1);              /* Ownership release.               */
}

uint8_t nRF24_get_rx_data_source(void)
{
	return ((nRF24_nop() & (BIT(3) | BIT(2) | BIT(1))) >> 1);
}

// Fixed: returns length==0 and pipe==7 means FIFO empty

uint16_t nRF24_read_rx_payload(uint8_t *rx_payload)
{
	return nRF24_read_multibyte_reg(nRF24_RX_PAYLOAD, rx_payload);
}

void nRF24_reuse_tx(void)
{
	nRF24_write_reg(nRF24_REUSE_TX_PL, 0);
}

bool nRF24_get_reuse_tx_status(void)
{
	return (bool) ((nRF24_get_fifo_status() & (1 << nRF24_FIFO_STATUS_TX_REUSE))
			>> nRF24_FIFO_STATUS_TX_REUSE);
}

void nRF24_flush_rx(void)
{
	nRF24_write_reg(nRF24_FLUSH_RX, 0);
}

void nRF24_flush_tx(void)
{
	nRF24_write_reg(nRF24_FLUSH_TX, 0);
}

uint8_t nRF24_nop(void)
{
	return nRF24_write_reg(nRF24_NOP, 0);
}

void nRF24_set_pll_mode(nRF24_pll_mode_t pll_mode)
{
	nRF24_write_reg(nRF24_RF_SETUP,
			BIT_COND(nRF24_read_reg(nRF24_RF_SETUP), nRF24_PLL_LOCK, pll_mode == nRF24_PLL_LOCK));
}

nRF24_pll_mode_t nRF24_get_pll_mode(void)
{
	return (nRF24_pll_mode_t) ((nRF24_read_reg(nRF24_RF_SETUP)
			& (1 << nRF24_PLL_LOCK)) >> nRF24_PLL_LOCK);
}

void nRF24_set_lna_gain(nRF24_lna_mode_t lna_gain)
{
	nRF24_write_reg(nRF24_RF_SETUP,
			BIT_COND(nRF24_read_reg(nRF24_RF_SETUP), nRF24_LNA_HCURR, lna_gain == nRF24_LNA_HCURR));
}

nRF24_lna_mode_t nRF24_get_lna_gain(void)
{
	return (nRF24_lna_mode_t) ((nRF24_read_reg(nRF24_RF_SETUP)
			& (1 << nRF24_LNA_HCURR)) >> nRF24_LNA_HCURR);
}

uint8_t nRF24_read_reg(uint8_t reg)
{
	uint8_t temp;
	//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
	spiSelect(&SPID1); /* Slave Select assertion.          */
	spiPolledExchange(&SPID1, reg);
	temp = spiPolledExchange(&SPID1,0);
	spiUnselect(&SPID1); /* Slave Select de-assertion.       */
	//spiReleaseBus(&SPID1);              /* Ownership release.               */

	/*  nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);
	 nRF24_hw_spi_rw(reg);
	 temp = nRF24_hw_spi_rw(0);
	 nRF24_hw_spi_ss(nRF24_SPI_SS_HIGH);*/

	return temp;
}

uint8_t nRF24_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t retval;
//  nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);
	//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
	spiSelect(&SPID1); /* Slave Select assertion.          */
	if (reg < nRF24_NRF_WRITE_REG) // i.e. this is a register access
	{
//    retval = nRF24_hw_spi_rw(nRF24_NRF_WRITE_REG + reg);
		retval = spiPolledExchange(&SPID1,nRF24_NRF_WRITE_REG + reg);
//	  nRF24_hw_spi_rw(value);
		spiPolledExchange(&SPID1, value);
	}
	else // single byte cmd OR future command/register access
	{
		if (!(reg == nRF24_FLUSH_TX) && !(reg == nRF24_FLUSH_RX)
				&& !(reg == nRF24_REUSE_TX_PL) && !(reg == nRF24_NOP))
		{
//      retval = nRF24_hw_spi_rw(reg);
			retval = spiPolledExchange(&SPID1,reg);
//    	nRF24_hw_spi_rw(value);
			spiPolledExchange(&SPID1, value);
		}
		else // single byte L01 command
		{
//      retval = nRF24_hw_spi_rw(reg);
			retval = spiPolledExchange(&SPID1,reg);
		}
	}
//  nRF24_hw_spi_ss(nRF24_SPI_SS_HIGH);
	spiUnselect(&SPID1); /* Slave Select de-assertion.       */
	//spiReleaseBus(&SPID1);              /* Ownership release.               */

	return retval;
}

uint16_t nRF24_read_multibyte_reg(uint8_t reg, uint8_t *pbuf)
{
	uint8_t ctr, length;
	switch (reg)
	{
	case nRF24_PIPE0:
	case nRF24_PIPE1:
	case nRF24_TX:
		length = ctr = nRF24_get_address_width();
//      nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);
		//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
		spiSelect(&SPID1); /* Slave Select assertion.          */
//      nRF24_hw_spi_rw(nRF24_RX_ADDR_P0 + reg);
		spiPolledExchange(&SPID1, nRF24_RX_ADDR_P0 + reg);
		break;

	case nRF24_RX_PAYLOAD:
		if ((reg = nRF24_get_rx_data_source()) < 7)
		{
			length = ctr = nRF24_read_rx_fifo_payload_width();

//        nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);
			//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
			spiSelect(&SPID1); /* Slave Select assertion.          */
			//nRF24_hw_spi_rw(nRF24_RD_RX_PAYLOAD);
			spiPolledExchange(&SPID1, nRF24_RD_RX_PAYLOAD);
		}
		else
		{
			//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
			ctr = length = 0;
		}
		break;

	default:
		//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
		ctr = length = 0;
		break;
	}

	/*  while(ctr--)
	 {
	 //    *pbuf++ = nRF24_hw_spi_rw(0);
	 *pbuf++ = spiPolledExchange(&SPID1,0);
	 }*/

#ifdef SPI_DMA
	if (ctr > 0)
		spiReceive(&SPID1, ctr, pbuf);
#else
	while (ctr--)
	{
		//    *pbuf++ = nRF24_hw_spi_rw(0);
		*pbuf++ = spiPolledExchange(&SPID1,0);
	}
#endif

//  nRF24_hw_spi_ss(nRF24_SPI_SS_HIGH);
	spiUnselect(&SPID1); /* Slave Select de-assertion.       */
	//spiReleaseBus(&SPID1);              /* Ownership release.               */

	return (((uint16_t) reg << 8) | length);
}

void nRF24_write_multibyte_reg(uint8_t reg, const uint8_t *pbuf, uint8_t length)
{
	switch (reg)
	{
	case nRF24_PIPE0:
	case nRF24_PIPE1:
	case nRF24_TX:
		length = nRF24_get_address_width();
//      nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);
		//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
		spiSelect(&SPID1); /* Slave Select assertion.          */
//      nRF24_hw_spi_rw(nRF24_NRF_WRITE_REG + nRF24_RX_ADDR_P0 + reg);
		spiPolledExchange(&SPID1, nRF24_NRF_WRITE_REG + nRF24_RX_ADDR_P0 + reg);
		break;

	case nRF24_TX_PAYLOAD:
//      nRF24_hw_spi_ss(nRF24_SPI_SS_LOW);
		//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
		spiSelect(&SPID1); /* Slave Select assertion.          */
//      nRF24_hw_spi_rw(nRF24_WR_TX_PAYLOAD);
		spiPolledExchange(&SPID1, nRF24_WR_TX_PAYLOAD);
		break;
	default:
		//spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
		break;
	}

	/*  while(length--)
	 //    nRF24_hw_spi_rw(*pbuf++);
	 spiPolledExchange(&SPID1,*pbuf++);*/
#ifdef SPI_DMA
	if (length > 0)
		spiSend(&SPID1, length, pbuf);
#else
	while (length--)
	spiPolledExchange(&SPID1, *pbuf++);
#endif
//	else
//		spiUnselect(&SPID1); /* Slave Select de-assertion.       */

//  nRF24_hw_spi_ss(nRF24_SPI_SS_HIGH);
	spiUnselect(&SPID1); /* Slave Select de-assertion.       */
	//spiReleaseBus(&SPID1);              /* Ownership release.               */
}

#ifdef __cplusplus
} // extern "C"
#endif
