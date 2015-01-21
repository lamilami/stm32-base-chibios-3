#include "ch.h"
#include "hal.h"
#include "core.h"

#if	RADIO_PRESENT

#include <nRF24_HAL.h>

extern void nRF24_irq_ext_handler(EXTDriver *extp, expchannel_t channel);

void nRF24_Init(void) {

	palSetPadMode(NRF_CE_IRQ_Port, NRF_CE_Pin, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(NRF_SPI_NSS_Port, NRF_SPI_NSS_Pin, PAL_MODE_OUTPUT_PUSHPULL);

	palSetGroupMode(NRF_SPI_Port,
			PAL_PORT_BIT(NRF_SPI_SCK_Pin) | PAL_PORT_BIT(NRF_SPI_MISO_Pin) | PAL_PORT_BIT(NRF_SPI_MOSI_Pin),
			0, SPI_PAL_PIN_MODE);

	SPI_REMAP_CMD;

	nRF24_hw_ce_low();

	palSetPadMode(NRF_CE_IRQ_Port, NRF_IRQ_Pin, PAL_MODE_INPUT_PULLUP);

	EXTChannelConfig extcfg;
	extcfg.mode = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART
			| NRF_IRQ_EXT_Port;
	extcfg.cb = nRF24_irq_ext_handler;

	chSysLock();
	extSetChannelModeI(&EXTD1, NRF_IRQ_Pin, &extcfg);
	extChannelEnableI(&EXTD1, NRF_IRQ_Pin);
	chSysUnlock();

	spiStart(&SPID1, &hs_spicfg); /* Setup transfer parameters.       */
}

void nRF24_hw_ce_low(void) {
	palClearPad(NRF_CE_IRQ_Port, NRF_CE_Pin);
}

void nRF24_hw_ce_high(void) {
	palSetPad(NRF_CE_IRQ_Port, NRF_CE_Pin);
}

#endif
