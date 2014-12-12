#include "ch.h"
#include "hal.h"
#include "core.h"

#if	RADIO_PRESENT

#include <nRF24_HAL.h>
//#include <HAL.h>
//#include <EERTOS.h>
#include <nRF24.h>

/*RCC_Periphery
 GPIOA_RCC =  { RCC_AHBPeriphClockCmd, RCC_AHBPeriph_GPIOA },
 SPI1_RCC =   { RCC_APB2PeriphClockCmd, RCC_APB2Periph_SPI1 };
 */

extern void nRF24_irq_ext_handler(EXTDriver *extp, expchannel_t channel);

static volatile DMA1_SPI_status_t dma_status;
void nRF24_dma_spi_start(uint8_t *rxbuf, uint8_t *txbuf, uint8_t length,
		DMA1_SPI_direction_t direction);

void nRF24_GPIO_init(void) {
//	GPIO_InitTypeDef GPIO_InitStructure;
//	SPI_InitTypeDef SPI_InitStructure;

#ifdef STM32F100C8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = NRF_CE_Pin;
	GPIO_Init(NRF_CE_IRQ_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_Pin;
	GPIO_Init(NRF_CE_IRQ_Port, &GPIO_InitStructure);

	/** SPI1 GPIO Configuration
	 PA15   ------> SPI1_NSS
	 PB3   ------> SPI1_SCK
	 PB4   ------> SPI1_MISO
	 PB5   ------> SPI1_MOSI
	 */

	/*Enable or disable APB2 peripheral clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);

	/*Configure GPIO pin : PA */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*Configure GPIO pin : PB */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*Configure peripheral I/O remapping */
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);

#else

//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	rccEnableAHB(RCC_AHBENR_GPIOFEN, TRUE);

	/*
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_InitStructure.GPIO_Pin = NRF_CE_Pin;
	 GPIO_Init(((GPIO_TypeDef *) GPIOF_BASE), &GPIO_InitStructure);
	 */
	palSetPadMode(GPIOF, NRF_CE_Pin,
			PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST ); // | PAL_STM32_PUDR_FLOATING);

	/*
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	 GPIO_InitStructure.GPIO_Pin = NRF_IRQ_Pin;
	 GPIO_Init(((GPIO_TypeDef *) GPIOF_BASE), &GPIO_InitStructure);
	 */

//Moved to nRF24_IRQ_Init
//	palSetPadMode(GPIOF, NRF_IRQ_Pin,
//			PAL_MODE_INPUT_PULLUP | PAL_STM32_OSPEED_LOWEST);
// Тактирование модуля SPI1 и порта А
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	rccEnableAHB(RCC_AHBENR_GPIOAEN, TRUE);

	// Настраиваем ноги SPI1 для работы в режиме альтернативной функции
	/*	GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource7, GPIO_AF_0);
	 GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource5, GPIO_AF_0);
	 GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource6, GPIO_AF_0);

	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	 GPIO_Init(((GPIO_TypeDef *) GPIOA_BASE), &GPIO_InitStructure);*/

	palSetPadMode(GPIOA, GPIOA_PIN5,
			PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN6,
			PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);
	palSetPadMode(GPIOA, GPIOA_PIN7,
			PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING | PAL_STM32_OTYPE_PUSHPULL);

	/*
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	 GPIO_Init(((GPIO_TypeDef *) GPIOA_BASE), &GPIO_InitStructure);
	 */
	palSetPadMode(GPIOA, GPIOA_PIN4,
			PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING);

#endif

	nRF24_hw_ce_low();
}

void nRF24_IRQ_Init(void) {
#ifdef STDLib
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure; //структура инициализации внешнего прерывания

	/* Connect EXTI Lines to pin */GPIO_EXTILineConfig(
			nRF24_IRQ_EXTI_PortSource, nRF24_IRQ_EXTI_PinSource);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = nRF24_IRQ_EXTI_Line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0-15 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = nRF24_IRQ_Channel;
//	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//	EXTI_GenerateSWInterrupt(EXTI_Line0);
	NVIC_EnableIRQ(nRF24_IRQ_Channel);
#endif
//	palSetPad(GPIOF, GPIOF_PIN1);
//	palSetPadMode(GPIOF, GPIOF_PIN1, PAL_MODE_INPUT);
	palSetPadMode(GPIOF, NRF_IRQ_Pin,
			PAL_MODE_INPUT_PULLUP | PAL_STM32_OSPEED_LOWEST);

	EXTChannelConfig extcfg;
	extcfg.mode = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART
			| EXT_MODE_GPIOF;
	extcfg.cb = nRF24_irq_ext_handler;

	chSysLock();
	extSetChannelModeI(&EXTD1, NRF_IRQ_Pin, &extcfg);
	extChannelEnableI(&EXTD1, NRF_IRQ_Pin);
	chSysUnlock();
}

void nRF24_Init(void) {
	nRF24_GPIO_init();
//    nRF24_set_power_mode(nRF24_PWR_DOWN);
//    nRF24_flush_rx();
//    nRF24_flush_tx();
//    nRF24_get_clear_irq_flags ();
	nRF24_IRQ_Init();
	spiStart(&SPID1, &hs_spicfg); /* Setup transfer parameters.       */
}

void nRF24_hw_ce_low(void) {
//    GPIO_ResetBits(NRF_CE_IRQ_Port, NRF_CE_Pin);
//	NRF_CE_IRQ_Port->BRR = NRF_CE_Pin;
	palClearPad(NRF_CE_IRQ_Port, NRF_CE_Pin);
}

void nRF24_hw_ce_high(void) {
//    GPIO_SetBits(NRF_CE_IRQ_Port, NRF_CE_Pin);
//	NRF_CE_IRQ_Port->BSRRL = NRF_CE_Pin;
	palSetPad(NRF_CE_IRQ_Port, NRF_CE_Pin);
}

#endif
