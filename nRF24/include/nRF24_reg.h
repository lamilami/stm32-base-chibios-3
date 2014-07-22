/* Copyright (c) 2006 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 *
 * $Rev: 1731 $
 *
 */

/** @file
 * Register definitions for the nRF HAL module
 * @defgroup nordic_nrf24l01p_reg nRF24L01 Register definitions
 * @{
 * @ingroup nordic_hal_nrf
 * Header file defining register mapping with bit definitions.\ This file is radio-chip dependent, and are included with the hal_nrf.h
 */

#ifndef nRF24_REG_H__
#define nRF24_REG_H__

typedef enum
{
/** @name - Instruction Set - */
//@{
/* nRF24L01 Instruction Definitions */

    nRF24_NRF_WRITE_REG       = 0x20,  /**< Register write command */
    nRF24_RD_RX_PAYLOAD_WIDTH = 0x60,  /**< Read RX payload command */
    nRF24_RD_RX_PAYLOAD       = 0x61,  /**< Read RX payload command */
    nRF24_WR_TX_PAYLOAD       = 0xA0,  /**< Write TX payload command */
    nRF24_WR_ACK_PAYLOAD      = 0xA8,  /**< Write ACK payload command */
    nRF24_WR_NAC_TX_PAYLOAD   = 0xB0,  /**< Write ACK payload command */
    nRF24_FLUSH_TX            = 0xE1,  /**< Flush TX register command */
    nRF24_FLUSH_RX            = 0xE2,  /**< Flush RX register command */
    nRF24_REUSE_TX_PL         = 0xE3,  /**< Reuse TX payload command */
    nRF24_LOCK_UNLOCK         = 0x50,  /**< Lock/unlcok exclusive features */
    nRF24_NOP                 = 0xFF,  /**< No Operation command, used for reading status register */
//@}
} nRF24_command_t;

enum
{
/** @name  - Register Memory Map - */
//@{
/* nRF24L01 * Register Definitions * */
    nRF24_CONFIG      = 0x00,  /**< nRF24L01 config register */
    nRF24_EN_AA       = 0x01,  /**< nRF24L01 enable Auto-Acknowledge register */
    nRF24_EN_RXADDR   = 0x02,  /**< nRF24L01 enable RX addresses register */
    nRF24_SETUP_AW    = 0x03,  /**< nRF24L01 setup of address width register */
    nRF24_SETUP_RETR  = 0x04,  /**< nRF24L01 setup of automatic retransmission register */
    nRF24_RF_CH       = 0x05,  /**< nRF24L01 RF channel register */
    nRF24_RF_SETUP    = 0x06,  /**< nRF24L01 RF setup register */
    nRF24_STATUS      = 0x07,  /**< nRF24L01 status register */
    nRF24_OBSERVE_TX  = 0x08,  /**< nRF24L01 transmit observe register */
    nRF24_CD          = 0x09,  /**< nRF24L01 carrier detect register */
    nRF24_RX_ADDR_P0  = 0x0A,  /**< nRF24L01 receive address data pipe0 */
    nRF24_RX_ADDR_P1  = 0x0B,  /**< nRF24L01 receive address data pipe1 */
    nRF24_RX_ADDR_P2  = 0x0C,  /**< nRF24L01 receive address data pipe2 */
    nRF24_RX_ADDR_P3  = 0x0D,  /**< nRF24L01 receive address data pipe3 */
    nRF24_RX_ADDR_P4  = 0x0E,  /**< nRF24L01 receive address data pipe4 */
    nRF24_RX_ADDR_P5  = 0x0F,  /**< nRF24L01 receive address data pipe5 */
    nRF24_TX_ADDR     = 0x10,  /**< nRF24L01 transmit address */
    nRF24_RX_PW_P0    = 0x11,  /**< nRF24L01 \# of bytes in rx payload for pipe0 */
    nRF24_RX_PW_P1    = 0x12,  /**< nRF24L01 \# of bytes in rx payload for pipe1 */
    nRF24_RX_PW_P2    = 0x13,  /**< nRF24L01 \# of bytes in rx payload for pipe2 */
    nRF24_RX_PW_P3    = 0x14,  /**< nRF24L01 \# of bytes in rx payload for pipe3 */
    nRF24_RX_PW_P4    = 0x15,  /**< nRF24L01 \# of bytes in rx payload for pipe4 */
    nRF24_RX_PW_P5    = 0x16,  /**< nRF24L01 \# of bytes in rx payload for pipe5 */
    nRF24_FIFO_STATUS = 0x17,  /**< nRF24L01 FIFO status register */
    nRF24_DYNPD       = 0x1C,  /**< nRF24L01 Dynamic payload setup */
    nRF24_FEATURE     = 0x1D   /**< nRF24L01 Exclusive feature setup */
};
//@}

/* nRF24L01 related definitions */
/* Interrupt definitions */
/* Operation mode definitions */

/** An enum describing the radio's irq sources.
 *
 */
typedef enum {
    nRF24_IRQ_MAX_RT = 4,     /**< Max retries interrupt */
    nRF24_IRQ_TX_DS,          /**< TX data sent interrupt */
    nRF24_IRQ_RX_DR           /**< RX data received interrupt */
} nRF24_irq_source_t;

/* Operation mode definitions */
/** An enum describing the radio's power mode.
 *
 */
typedef enum {
    nRF24_PTX,            /**< Primary TX operation */
    nRF24_PRX             /**< Primary RX operation */
} nRF24_operation_mode_t;

/** An enum describing the radio's power mode.
 *
 */
typedef enum {
    nRF24_PWR_DOWN,       /**< Device power-down */
    nRF24_PWR_UP          /**< Device power-up */
} nRF24_pwr_mode_t;

/** An enum describing the radio's output power mode's.
 *
 */
typedef enum {
    nRF24_18DBM,          /**< Output power set to -18dBm */
    nRF24_12DBM,          /**< Output power set to -12dBm */
    nRF24_6DBM,           /**< Output power set to -6dBm  */
    nRF24_0DBM            /**< Output power set to 0dBm   */
} nRF24_output_power_t;

/** An enum describing the radio's on-air datarate.
 *
 */
typedef enum {
    nRF24_1MBPS,          /**< Datarate set to 1 Mbps  */
    nRF24_2MBPS           /**< Datarate set to 2 Mbps  */
} nRF24_datarate_t;

/** An enum describing the radio's PLL mode.
 *
 */
typedef enum {
    nRF24_PLL_UNLOCK,     /**< PLL unlocked, normal operation  */
    nRF24_PLL_LOCK        /**< PLL locked, test mode  */
} nRF24_pll_mode_t;

/** An enum describing the radio's LNA mode.
 *
 */
typedef enum {
    nRF24_LNA_LCURR,      /**< LNA set to low current mode */
    nRF24_LNA_HCURR       /**< LNA set to high current mode */
} nRF24_lna_mode_t;

/** An enum describing the radio's CRC mode.
 *
 */
typedef enum {
    nRF24_CRC_OFF,        /**< CRC check disabled */
    nRF24_CRC_8BIT = 2,   /**< CRC check set to 8-bit */
    nRF24_CRC_16BIT       /**< CRC check set to 16-bit */
} nRF24_crc_mode_t;

/** An enum describing the read/write payload command.
 *
 */
typedef enum {
    nRF24_TX_PAYLOAD = 7,   /**< TX payload definition */
    nRF24_RX_PAYLOAD,        /**< RX payload definition */
    nRF24_ACK_PAYLOAD
} nRF24_payload_command_t;

/** Structure containing the radio's address map.
 * Pipe0 contains 5 unique address bytes,
 * while pipe[1..5] share the 4 MSB bytes, set in pipe1.
 * <p><b> - Remember that the LSB byte for all pipes have to be unique! -</b>
 */
// nRF24L01 Address struct

/*
//typedef struct {
//    uint8_t p0[5];            ///< Pipe0 address, 5 bytes
//    uint8_t p1[5];            ///< Pipe1 address, 5 bytes, 4 MSB bytes shared for pipe1 to pipe5 */
//    uint8_t p2[1];            ///< Pipe2 address, 1 byte
//    uint8_t p3[1];            ///< Pipe3 address, 1 byte
//    uint8_t p4[1];            ///< Pipe3 address, 1 byte
//    uint8_t p5[1];            ///< Pipe3 address, 1 byte
//    uint8_t tx[5];            ///< TX address, 5 byte */
//} nRF24_l01_addr_map;


/** An enum describing the nRF24L01 pipe addresses and TX address.
 *
 */
typedef enum {
    nRF24_PIPE0,              /**< Select pipe0 */
    nRF24_PIPE1,              /**< Select pipe1 */
    nRF24_PIPE2,              /**< Select pipe2 */
    nRF24_PIPE3,              /**< Select pipe3 */
    nRF24_PIPE4,              /**< Select pipe4 */
    nRF24_PIPE5,              /**< Select pipe5 */
    nRF24_TX,                 /**< Refer to TX address*/
    nRF24_ALL = 0XFF          /**< Close or open all pipes*/
                                /**< @see nRF24_set_address @see nRF24_get_address
                                 @see nRF24_open_pipe  @see nRF24_close_pipe */
} nRF24_address_t;

/** An enum describing the radio's address width.
 *
 */
typedef enum {
    nRF24_AW_3BYTES = 3,      /**< Set address width to 3 bytes */
    nRF24_AW_4BYTES,          /**< Set address width to 4 bytes */
    nRF24_AW_5BYTES           /**< Set address width to 5 bytes */
} nRF24_address_width_t;

enum
{
/** @name CONFIG register bit definitions */
//@{

    nRF24_CONFIG_MASK_RX_DR  = 6,     /**< CONFIG register bit 6 */
    nRF24_CONFIG_MASK_TX_DS  = 5,     /**< CONFIG register bit 5 */
    nRF24_CONFIG_MASK_MAX_RT = 4,     /**< CONFIG register bit 4 */
    nRF24_CONFIG_EN_CRC      = 3,     /**< CONFIG register bit 3 */
    nRF24_CONFIG_CRCO        = 2,     /**< CONFIG register bit 2 */
    nRF24_CONFIG_PWR_UP      = 1,     /**< CONFIG register bit 1 */
    nRF24_CONFIG_PRIM_RX     = 0,     /**< CONFIG register bit 0 */
//@}

/** @name RF_SETUP register bit definitions */
//@{
    nRF24_RF_SETUP_PLL_LOCK  = 4,     /**< RF_SETUP register bit 4 */
    nRF24_RF_SETUP_RF_DR     = 3,     /**< RF_SETUP register bit 3 */
    nRF24_RF_SETUP_RF_PWR1   = 2,     /**< RF_SETUP register bit 2 */
    nRF24_RF_SETUP_RF_PWR0   = 1,     /**< RF_SETUP register bit 1 */
    nRF24_RF_SETUP_LNA_HCURR = 0,     /**< RF_SETUP register bit 0 */
//@}

/* STATUS 0x07 */
/** @name STATUS register bit definitions */
//@{
    nRF24_STATUS_RX_DR   = 6,     /**< STATUS register bit 6 */
    nRF24_STATUS_TX_DS   = 5,     /**< STATUS register bit 5 */
    nRF24_STATUS_MAX_RT  = 4,     /**< STATUS register bit 4 */
    nRF24_STATUS_TX_FULL = 0,     /**< STATUS register bit 0 */
//@}

/* FIFO_STATUS 0x17 */
/** @name FIFO_STATUS register bit definitions */
//@{
    nRF24_FIFO_STATUS_TX_REUSE     = 6,     /**< FIFO_STATUS register bit 6 */
    nRF24_FIFO_STATUS_TX_FIFO_FULL = 5,     /**< FIFO_STATUS register bit 5 */
    nRF24_FIFO_STATUS_TX_EMPTY     = 4,     /**< FIFO_STATUS register bit 4 */
    nRF24_FIFO_STATUS_RX_FULL      = 1,     /**< FIFO_STATUS register bit 1 */
    nRF24_FIFO_STATUS_RX_EMPTY     = 0      /**< FIFO_STATUS register bit 0 */
//@}
};



#define BIT_0 0x01 /**< The value of bit 0 */
#define BIT_1 0x02 /**< The value of bit 1 */
#define BIT_2 0x04 /**< The value of bit 2 */
#define BIT_3 0x08 /**< The value of bit 3 */
#define BIT_4 0x10 /**< The value of bit 4 */
#define BIT_5 0x20 /**< The value of bit 5 */
#define BIT_6 0x40 /**< The value of bit 6 */
#define BIT_7 0x80 /**< The value of bit 7 */

#endif // nRF24_REG_H__
/** @} */
