#include "ch.h"
#include "hal.h"
#include "core.h"

#if	RADIO_PRESENT

#include <nRF24_HAL.h>
#include <nRF24.h>

extern void nRF24_irq_ext_handler(EXTDriver *extp, expchannel_t channel);

static volatile DMA1_SPI_status_t dma_status;
void nRF24_dma_spi_start(uint8_t *rxbuf, uint8_t *txbuf, uint8_t length,
		DMA1_SPI_direction_t direction);

void nRF24_GPIO_init(void) {

	palSetPadMode(NRF_CE_IRQ_Port, NRF_CE_Pin, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(NRF_SPI_NSS_Port, NRF_SPI_NSS_Pin, PAL_MODE_OUTPUT_PUSHPULL);

	palSetGroupMode(NRF_SPI_Port,
			NRF_SPI_SCK_Pin|NRF_SPI_MISO_Pin|NRF_SPI_MOSI_Pin, 0,
#ifdef PAL_MODE_STM32_ALTERNATE_PUSHPULL
			PAL_MODE_STM32_ALTERNATE_PUSHPULL);
#else
	PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST
	| PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
#endif

	/*
	 palSetPadMode(NRF_SPI_Port, NRF_SPI_MISO_Pin,
	 PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	 palSetPadMode(NRF_SPI_Port, NRF_SPI_MOSI_Pin,
	 PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	 */

#ifdef STM32F100C8

//	rccEnableAPB2(RCC_APB2ENR_IOPAEN, TRUE);
//	rccEnableAPB2(RCC_APB2ENR_IOPBEN, TRUE);
//	rccEnableAPB2(RCC_APB2ENR_AFIOEN, TRUE);

	/*Configure peripheral I/O remapping */AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP;

#else

	rccEnableAHB(RCC_AHBENR_GPIOFEN, TRUE);

//	palSetPadMode(GPIOF, NRF_CE_Pin,
//			PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST);

	rccEnableAHB(RCC_AHBENR_GPIOAEN, TRUE);

	palSetPadMode(GPIOA, GPIOA_PIN5,
			PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST
			| PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN6,
			PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST
			| PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN7,
			PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST
			| PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);

//	palSetPadMode(GPIOA, GPIOA_PIN4,
//			PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST
//					| PAL_STM32_PUDR_FLOATING);

#endif

	nRF24_hw_ce_low();
}

void nRF24_IRQ_Init(void) {

	palSetPadMode(NRF_CE_IRQ_Port, NRF_IRQ_Pin, PAL_MODE_INPUT_PULLUP);

	EXTChannelConfig extcfg;
	extcfg.mode = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART
			| NRF_IRQ_EXT_Port;
	extcfg.cb = nRF24_irq_ext_handler;

	chSysLock();
	extSetChannelModeI(&EXTD1, NRF_IRQ_Pin, &extcfg);
	extChannelEnableI(&EXTD1, NRF_IRQ_Pin);
	chSysUnlock();
}

void nRF24_Init(void) {
	nRF24_GPIO_init();
	nRF24_IRQ_Init();
	spiStart(&SPID1, &hs_spicfg); /* Setup transfer parameters.       */
}

void nRF24_hw_ce_low(void) {
	palClearPad(NRF_CE_IRQ_Port, NRF_CE_Pin);
}

void nRF24_hw_ce_high(void) {
	palSetPad(NRF_CE_IRQ_Port, NRF_CE_Pin);
}

#endif
