#ifndef nRF24_HAL_H
#define nRF24_HAL_H

#include <HAL.h>
#include <nRF24_reg.h>

#ifdef STM32F100C8
#include "stm32f10x.h"
#else
#include "stm32f0xx.h"
#endif

/*
 * Maximum speed SPI configuration (18MHz, CPHA=0, CPOL=0, MSb first).
 */

#ifdef STM32F100C8
static const SPIConfig hs_spicfg =
{	NULL, GPIOA, 4, SPI_CR1_BR_1, // | SPI_CR1_MSTR,
//  SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 // | SPI_CR2_FRXTH
};
#else
static const SPIConfig hs_spicfg =
{ NULL, GPIOA, 4, SPI_CR1_BR_1, // | SPI_CR1_MSTR,
		SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 // | SPI_CR2_FRXTH
};
#endif

typedef enum
{
	DMA1_SPI_IDLE, /**< Channel3 is idle */
	DMA1_SPI_END, /**< Transfer completed */
	DMA1_SPI_BUSY, /**< Channel3 busy */
	DMA1_SPI_MULTI_BUSY, /**< Channel3 busy */
	DMA1_SPI_ERROR, /**< Channel3 error */
} DMA1_SPI_status_t;

typedef enum
{
	DMA1_SPI_READ, /**< Channel3 is idle */
	DMA1_SPI_WRITE /**< Transfer completed */
} DMA1_SPI_direction_t;

#ifdef STM32F100C8
#define NRF_CE_Pin GPIO_Pin_6
#define NRF_IRQ_Pin GPIO_Pin_7
#define NRF_CE_IRQ_Port		GPIOB

#define nRF24_IRQ_EXTI_PortSource		GPIO_PortSourceGPIOB
#define nRF24_IRQ_EXTI_PinSource		GPIO_PinSource7
#define nRF24_IRQ_EXTI_Line				EXTI_Line7
#define nRF24_IRQ_Channel				EXTI9_5_IRQn

#define NRF_SPI_NSS_Pin GPIO_Pin_15
#define NRF_SPI_SCK_Pin GPIO_Pin_3
#define NRF_SPI_MISO_Pin GPIO_Pin_4
#define NRF_SPI_MOSI_Pin GPIO_Pin_5

#define NRF_SPI_Port		GPIOB
#define NRF_SPI_NSS_Port	GPIOA
#else
#define GPIO_EXTILineConfig SYSCFG_EXTILineConfig

#define NRF_CE_Pin GPIO_Pin_0
#define NRF_IRQ_Pin GPIO_Pin_1

#define NRF_CE_IRQ_Port		((GPIO_TypeDef *) GPIOF_BASE)
#define nRF24_IRQ_EXTI_PortSource		EXTI_PortSourceGPIOF
#define nRF24_IRQ_EXTI_PinSource		EXTI_PinSource1
#define nRF24_IRQ_EXTI_Line				EXTI_Line1
#define nRF24_IRQ_Channel				EXTI0_1_IRQn

#define NRF_SPI_NSS_Pin GPIO_Pin_4
#define NRF_SPI_SCK_Pin GPIO_Pin_5
#define NRF_SPI_MISO_Pin GPIO_Pin_6
#define NRF_SPI_MOSI_Pin GPIO_Pin_7

#define NRF_SPI_Port		((GPIO_TypeDef *) GPIOA_BASE)
#endif

void nRF24_Init(void);

// Hardware-dependent functions

typedef enum
{
	nRF24_SPI_SS_LOW = 0, nRF24_SPI_SS_HIGH = 1
} nRF24_spi_ss_level_t;

//uint8_t nRF24_hw_spi_rw(uint8_t value, bool first_rw);
//void nRF24_hw_spi_multi_rw(uint8_t *pbuf, uint8_t length, DMA1_SPI_direction_t direction);
DMA1_SPI_status_t DMA1_SPI_get_status(void);
DMA1_SPI_status_t DMA1_SPI_set_status(DMA1_SPI_status_t status_val);
void nRF24_hw_spi_ss(nRF24_spi_ss_level_t level);
void nRF24_hw_csn_high(void);
void nRF24_hw_csn_low(void);
void nRF24_hw_ce_low(void);
void nRF24_hw_ce_high(void);
//void nRF24_hw_ce_pulse_10us(void);

#endif
