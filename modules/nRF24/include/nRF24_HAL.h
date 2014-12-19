#ifndef nRF24_HAL_H
#define nRF24_HAL_H

#include <HAL.h>
#include <nRF24_reg.h>

#ifdef STM32F100C8

#define NRF_CE_Pin 		GPIOB_PIN6
#define NRF_IRQ_Pin 	GPIOB_PIN7
#define NRF_CE_IRQ_Port		GPIOB

#define SPI_PAL_PIN_MODE	PAL_MODE_STM32_ALTERNATE_PUSHPULL

#define NRF_SPI_NSS_Pin GPIOA_PIN15
#define NRF_SPI_SCK_Pin GPIOB_PIN3
#define NRF_SPI_MISO_Pin GPIOB_PIN4
#define NRF_SPI_MOSI_Pin GPIOB_PIN5

#define NRF_SPI_Port		GPIOB
#define NRF_SPI_NSS_Port	GPIOA

#define SPI_REMAP_CMD	AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP

#define NRF_IRQ_EXT_Port	EXT_MODE_GPIOB

#else

#define NRF_CE_Pin GPIOF_PIN0
#define NRF_IRQ_Pin GPIOF_PIN1

#define NRF_CE_IRQ_Port		GPIOF

#define SPI_PAL_PIN_MODE	PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL

#define NRF_SPI_NSS_Pin GPIOA_PIN4
#define NRF_SPI_SCK_Pin GPIOA_PIN5
#define NRF_SPI_MISO_Pin GPIOA_PIN6
#define NRF_SPI_MOSI_Pin GPIOA_PIN7

#define NRF_SPI_Port		GPIOA
#define NRF_SPI_NSS_Port	GPIOA

#define SPI_REMAP_CMD

#define NRF_IRQ_EXT_Port	EXT_MODE_GPIOF
#endif

/*
 * SPI configuration.
 */

#ifdef STM32F100C8
static const SPIConfig hs_spicfg = { NULL, NRF_SPI_NSS_Port, NRF_SPI_NSS_Pin,
		SPI_CR1_BR_0 };
#else
static const SPIConfig hs_spicfg = {NULL, GPIOA, 4, SPI_CR1_BR_1,
	SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0
};
#endif

typedef enum {
	DMA1_SPI_IDLE, /**< Channel3 is idle */
	DMA1_SPI_END, /**< Transfer completed */
	DMA1_SPI_BUSY, /**< Channel3 busy */
	DMA1_SPI_MULTI_BUSY, /**< Channel3 busy */
	DMA1_SPI_ERROR, /**< Channel3 error */
} DMA1_SPI_status_t;

typedef enum {
	DMA1_SPI_READ, /**< Channel3 is idle */
	DMA1_SPI_WRITE /**< Transfer completed */
} DMA1_SPI_direction_t;

void nRF24_Init(void);

// Hardware-dependent functions
void nRF24_hw_ce_low(void);
void nRF24_hw_ce_high(void);

#endif
