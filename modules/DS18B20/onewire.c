/*
 * onewire.c
 *
 *  Created on: 13.02.2012
 *      Author: di
 */
#include "ch.h"
#include "hal.h"
#include "onewire.h"
#include "core.h"

#if DS18B20_PRESENT

#ifdef OW_USART1

#undef OW_USART2
#undef OW_USART3
#undef OW_USART4

#define OW_USART 		USART1
#define OW_DMA_CH_RX 	DMA1_Channel5
#define OW_DMA_CH_TX 	DMA1_Channel4
#define OW_DMA_FLAG		DMA_ISR_TCIF5
#define OW_USART_CLK	STM32_USART1CLK

#endif

#ifdef OW_USART2

#undef OW_USART1
#undef OW_USART3
#undef OW_USART4

#define OW_USART 		USART2
#define OW_DMA_CH_RX 	DMA1_Channel6
#define OW_DMA_CH_TX 	DMA1_Channel7
#define OW_DMA_FLAG		DMA1_FLAG_TC6

#endif

// Буфер для приема/передачи по 1-wire
uint8_t ow_buf[8];

#define OW_0	0x00
#define OW_1	0xff
#define OW_R_1	0xff

//-----------------------------------------------------------------------------
// функция преобразует один байт в восемь, для передачи через USART
// ow_byte - байт, который надо преобразовать
// ow_bits - ссылка на буфер, размером не менее 8 байт
//-----------------------------------------------------------------------------
void OW_toBits(uint8_t ow_byte, uint8_t *ow_bits) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		if (ow_byte & 0x01) {
			*ow_bits = OW_1;
		} else {
			*ow_bits = OW_0;
		}
		ow_bits++;
		ow_byte = ow_byte >> 1;
	}
}

//-----------------------------------------------------------------------------
// обратное преобразование - из того, что получено через USART опять собирается байт
// ow_bits - ссылка на буфер, размером не менее 8 байт
//-----------------------------------------------------------------------------
uint8_t OW_toByte(uint8_t *ow_bits) {
	uint8_t ow_byte, i;
	ow_byte = 0;
	for (i = 0; i < 8; i++) {
		ow_byte = ow_byte >> 1;
		if (*ow_bits == OW_R_1) {
			ow_byte |= 0x80;
		}
		ow_bits++;
	}

	return ow_byte;
}

//-----------------------------------------------------------------------------
// инициализирует USART и DMA
//-----------------------------------------------------------------------------
uint8_t OW_Init() {
//	GPIO_InitTypeDef GPIO_InitStruct;
//	USART_InitTypeDef USART_InitStructure;

#ifdef STM32F100C8

	if (OW_USART == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,
				ENABLE);

		// USART TX
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOA, &GPIO_InitStruct);

		// USART RX
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOA, &GPIO_InitStruct);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	}

	if (OW_USART == USART2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,
				ENABLE);

		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOA, &GPIO_InitStruct);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	}
#else

//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	rccEnableAHB(RCC_AHBENR_GPIOAEN, TRUE);
	rccEnableUSART1(TRUE);
	rccEnableDMA1(TRUE);
	rccEnableAPB2(RCC_APB2ENR_SYSCFGCOMPEN, TRUE);

//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);

// USART TX
	/*	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	 GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	 GPIO_Init(((GPIO_TypeDef *) GPIOA_BASE), &GPIO_InitStruct);*/

	palSetPadMode(GPIOA, GPIOA_PIN9,
			PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_MID | PAL_STM32_PUDR_PULLUP
					| PAL_STM32_OTYPE_OPENDRAIN);

//	GPIO_PinAFConfig(((GPIO_TypeDef *) GPIOA_BASE), GPIO_PinSource9, GPIO_AF_1);

	/*
	 // USART RX
	 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	 //		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	 //		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	 GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	 //		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	 //		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	 GPIO_Init(GPIOA, &GPIO_InitStruct);
	 */
//	SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART1Rx, ENABLE);
//	SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART1Tx, ENABLE);
	SYSCFG->CFGR1 |= (uint32_t) SYSCFG_CFGR1_USART1RX_DMA_RMP
			| SYSCFG_CFGR1_USART1TX_DMA_RMP;
//	SYSCFG->CFGR1 |= (uint32_t) SYSCFG_CFGR1_USART1TX_DMA_RMP;

#endif

	/*	USART_InitStructure.USART_BaudRate = 100000;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	 USART_InitStructure.USART_StopBits = USART_StopBits_2;
	 USART_InitStructure.USART_Parity = USART_Parity_No;
	 USART_InitStructure.USART_HardwareFlowControl =
	 USART_HardwareFlowControl_None;
	 USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	 USART_Init(OW_USART, &USART_InitStructure);
	 */

	uint32_t tmpreg = 0;

	OW_USART->CR1 &= (uint32_t) ~((uint32_t) USART_CR1_UE);

	/*---------------------------- USART CR2 Configuration -----------------------*/
	tmpreg = OW_USART->CR2;
	/* Clear STOP[13:12] bits */
	tmpreg &= (uint32_t) ~((uint32_t) USART_CR2_STOP);

	/* Configure the USART Stop Bits, Clock, CPOL, CPHA and LastBit ------------*/
	/* Set STOP[13:12] bits according to USART_StopBits value */
	tmpreg |= (uint32_t) USART_CR2_STOP_1;

	/* Write to USART CR2 */OW_USART->CR2 = tmpreg;

	/*---------------------------- USART CR1 Configuration -----------------------*/
	tmpreg = OW_USART->CR1;
	/* Clear M, PCE, PS, TE and RE bits */
	tmpreg &= (uint32_t) ~((uint32_t)(
			USART_CR1_M | USART_CR1_PCE | USART_CR1_PS));

	/* Configure the USART Word Length, Parity and mode ----------------------- */
	/* Set the M bits according to USART_WordLength value */
	/* Set PCE and PS bits according to USART_Parity value */
	/* Set TE and RE bits according to USART_Mode value */
	tmpreg |= (uint32_t) USART_CR1_RE | USART_CR1_TE;

	/* Write to USART CR1 */
	OW_USART->CR1 = tmpreg;

	/*---------------------------- USART CR3 Configuration -----------------------*/
	tmpreg = OW_USART->CR3;
	/* Clear CTSE and RTSE bits */
	tmpreg &= (uint32_t) ~((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE));

	/* Configure the USART HFC -------------------------------------------------*/
	/* Set CTSE and RTSE bits according to USART_HardwareFlowControl value */
//	tmpreg |= USART_InitStruct->USART_HardwareFlowControl;
	/* Write to USART CR3 */
	OW_USART->CR3 = tmpreg;

	OW_USART->BRR = OW_USART_CLK / 100000; //24000000

	// Здесь вставим разрешение работы USART в полудуплексном режиме
//	USART_HalfDuplexCmd(OW_USART, ENABLE);

	/* Enable the Half-Duplex mode by setting the HDSEL bit in the CR3 register */
	OW_USART->CR3 |= USART_CR3_HDSEL;

//	USART_Cmd(OW_USART, ENABLE);

	/* Enable the selected USART by setting the UE bit in the CR1 register */
	OW_USART->CR1 |= USART_CR1_UE;

	return OW_OK;
}

//-----------------------------------------------------------------------------
// осуществляет сброс и проверку на наличие устройств на шине
//-----------------------------------------------------------------------------
uint8_t OW_Reset() {
	uint8_t ow_presence;
	/*	USART_InitTypeDef USART_InitStructure;

	 USART_InitStructure.USART_BaudRate = 9600;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	 USART_InitStructure.USART_StopBits = USART_StopBits_2;
	 USART_InitStructure.USART_Parity = USART_Parity_No;
	 USART_InitStructure.USART_HardwareFlowControl =
	 USART_HardwareFlowControl_None;
	 USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	 USART_Init(OW_USART, &USART_InitStructure);

	 USART_Cmd(OW_USART, ENABLE);

	 */

	/* Disable USART */
	OW_USART->CR1 &= (uint32_t) ~((uint32_t) USART_CR1_UE);

	OW_USART->BRR = OW_USART_CLK / 9600;

	/* Enable the selected USART by setting the UE bit in the CR1 register */
	OW_USART->CR1 |= USART_CR1_UE;

	// отправляем 0xf0 на скорости 9600
//	USART_ClearFlag(OW_USART, USART_FLAG_TC);

	OW_USART->ICR = USART_ISR_TC;

//	USART_SendData(OW_USART, 0xf0);

	/* Transmit Data */
	OW_USART->TDR = (0xf0 & (uint16_t) 0x01FF);

	uint16_t cntr;
	cntr = 0;

	while ((cntr < 5) && ((OW_USART->ISR & USART_ISR_TC) == RESET)) {
		cntr++;
//		chThdYield();
		osalThreadSleepMilliseconds(1);
//		chThdSleepMilliseconds(10);
	}

	ow_presence = (uint16_t)(OW_USART->RDR & (uint16_t) 0x01FF);

	/*	USART_InitStructure.USART_BaudRate = 100000;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	 USART_InitStructure.USART_StopBits = USART_StopBits_2;
	 USART_InitStructure.USART_Parity = USART_Parity_No;
	 USART_InitStructure.USART_HardwareFlowControl =
	 USART_HardwareFlowControl_None;
	 USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	 USART_Init(OW_USART, &USART_InitStructure);
	 */

	/* Disable USART */
	OW_USART->CR1 &= (uint32_t) ~((uint32_t) USART_CR1_UE);

	OW_USART->BRR = OW_USART_CLK / 100000;

	if (cntr == 5) {
		return OW_ERROR;
	}

	if (ow_presence != 0xf0) {
		return OW_OK;
	}

	return OW_NO_DEVICE;
}

//-----------------------------------------------------------------------------
// процедура общения с шиной 1-wire
// sendReset - посылать RESET в начале общения.
// 		OW_SEND_RESET или OW_NO_RESET
// command - массив байт, отсылаемых в шину. Если нужно чтение - отправляем OW_READ_SLOTH
// cLen - длина буфера команд, столько байт отошлется в шину
// data - если требуется чтение, то ссылка на буфер для чтения
// dLen - длина буфера для чтения. Прочитается не более этой длины
// readStart - с какого символа передачи начинать чтение (нумеруются с 0)
//		можно указать OW_NO_READ, тогда можно не задавать data и dLen
//-----------------------------------------------------------------------------
uint8_t OW_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen,
		uint8_t *data, uint8_t dLen, uint8_t readStart) {

	// если требуется сброс - сбрасываем и проверяем на наличие устройств
	if (sendReset == OW_SEND_RESET) {
		uint8_t reset_out = OW_Reset();
		if (reset_out == OW_NO_DEVICE) {
			return OW_NO_DEVICE;
		}
		if (reset_out == OW_ERROR) {
			return OW_ERROR;
		}
	}

	while (cLen > 0) {

		OW_toBits(*command, ow_buf);
		command++;
		cLen--;

		/*

		 DMA_InitTypeDef DMA_InitStructure;

		 // DMA на чтение
		 DMA_DeInit(OW_DMA_CH_RX);
		 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (OW_USART->RDR);
		 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ow_buf;
		 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		 DMA_InitStructure.DMA_BufferSize = 8;
		 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		 DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
		 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		 DMA_Init(OW_DMA_CH_RX, &DMA_InitStructure);
		 */

		/* Disable the selected DMAy Channelx */
		OW_DMA_CH_RX->CCR &= (uint16_t)(~DMA_CCR_EN);

		/* Reset interrupt pending bits for DMA1 Channel5 */
		DMA1->IFCR |= ((uint32_t)(
				DMA_ISR_GIF5 | DMA_ISR_TCIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5));

		/*--------------------------- DMAy Channelx CCR Configuration ----------------*/

		/* Write to DMAy Channelx CCR */
		OW_DMA_CH_RX->CCR = DMA_CCR_MINC;

		/*--------------------------- DMAy Channelx CNDTR Configuration --------------*/
		/* Write to DMAy Channelx CNDTR */
		OW_DMA_CH_RX->CNDTR = 8;

		/*--------------------------- DMAy Channelx CPAR Configuration ---------------*/
		/* Write to DMAy Channelx CPAR */
		OW_DMA_CH_RX->CPAR = (uint32_t) & (OW_USART->RDR);

		/*--------------------------- DMAy Channelx CMAR Configuration ---------------*/
		/* Write to DMAy Channelx CMAR */
		OW_DMA_CH_RX->CMAR = (uint32_t) ow_buf;

		/*
		 // DMA на запись
		 DMA_DeInit(OW_DMA_CH_TX);
		 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (OW_USART->TDR);
		 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ow_buf;
		 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		 DMA_InitStructure.DMA_BufferSize = 8;
		 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		 DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
		 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		 DMA_Init(OW_DMA_CH_TX, &DMA_InitStructure);
		 */

		/* Disable the selected DMAy Channelx */
		OW_DMA_CH_TX->CCR &= (uint16_t)(~DMA_CCR_EN);

		/* Reset interrupt pending bits for DMA1 Channel5 */
		DMA1->IFCR |= ((uint32_t)(
				DMA_ISR_GIF4 | DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4));

		/* Configure DMAy Channelx: data transfer, data size, priority level and mode */
		OW_DMA_CH_TX->CCR = DMA_CCR_DIR | DMA_CCR_MINC;

		/*--------------------------- DMAy Channelx CNDTR Configuration --------------*/
		/* Write to DMAy Channelx CNDTR */
		OW_DMA_CH_TX->CNDTR = 8;

		/*--------------------------- DMAy Channelx CPAR Configuration ---------------*/
		/* Write to DMAy Channelx CPAR */
		OW_DMA_CH_TX->CPAR = (uint32_t) & (OW_USART->TDR);

		/*--------------------------- DMAy Channelx CMAR Configuration ---------------*/
		/* Write to DMAy Channelx CMAR */
		OW_DMA_CH_TX->CMAR = (uint32_t) ow_buf;

		// старт цикла отправки
//		USART_ClearFlag(OW_USART,
//				USART_FLAG_RXNE | USART_FLAG_TC | USART_FLAG_TXE);

		OW_USART->ICR = USART_ISR_RXNE | USART_ISR_TC | USART_ISR_TXE;

//		USART_DMACmd(OW_USART, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
		OW_USART->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;

//		DMA_Cmd(OW_DMA_CH_RX, ENABLE);
		OW_DMA_CH_RX->CCR |= DMA_CCR_EN;
//		DMA_Cmd(OW_DMA_CH_TX, ENABLE);
		OW_DMA_CH_TX->CCR |= DMA_CCR_EN;
//		USART_Cmd(OW_USART, ENABLE);
		/* Enable the selected USART by setting the UE bit in the CR1 register */
		OW_USART->CR1 |= USART_CR1_UE;

		uint8_t cntr;
		cntr = 0;

// 		chThdSleepMilliseconds(100);

// Ждем, пока не примем 8 байт
		while ((cntr < 5) && ((DMA1->ISR & OW_DMA_FLAG) == RESET)) {
			cntr++;
			chThdSleepMilliseconds(1);
		}

		// отключаем DMA
//		DMA_Cmd(OW_DMA_CH_TX, DISABLE);
		/* Disable the selected DMAy Channelx */OW_DMA_CH_TX->CCR &= (uint16_t)(
				~DMA_CCR_EN);
//		DMA_Cmd(OW_DMA_CH_RX, DISABLE);
		OW_DMA_CH_RX->CCR &= (uint16_t)(~DMA_CCR_EN);

//		USART_DMACmd(OW_USART, USART_DMAReq_Tx | USART_DMAReq_Rx, DISABLE);
		OW_USART->CR3 &= (uint32_t) ~(USART_CR3_DMAT | USART_CR3_DMAR);

		if (cntr == 5) {
			return OW_ERROR;
		}

		// если прочитанные данные кому-то нужны - выкинем их в буфер
		if (readStart == 0 && dLen > 0) {
			*data = OW_toByte(ow_buf);
			data++;
			dLen--;
		} else {
			if (readStart != OW_NO_READ) {
				readStart--;
			}
		}
	}

	return OW_OK;
}

// send nbits bits from ow_buf to 1-wire
void OW_SendBits(uint8_t nbits) {
	/*
	 DMA_InitTypeDef DMA_InitStructure;
	 // DMA for reading
	 DMA_DeInit(OW_DMA_CH_RX);
	 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (OW_USART->RDR);
	 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ow_buf;
	 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	 DMA_InitStructure.DMA_BufferSize = nbits;
	 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	 DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	 DMA_Init(OW_DMA_CH_RX, &DMA_InitStructure);
*/
	/* Disable the selected DMAy Channelx */
	OW_DMA_CH_RX->CCR &= (uint16_t)(~DMA_CCR_EN);

	/* Reset interrupt pending bits for DMA1 Channel5 */
	DMA1->IFCR |= ((uint32_t)(
			DMA_ISR_GIF5 | DMA_ISR_TCIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5));

	/*--------------------------- DMAy Channelx CCR Configuration ----------------*/

	/* Write to DMAy Channelx CCR */
	OW_DMA_CH_RX->CCR = DMA_CCR_MINC;

	/*--------------------------- DMAy Channelx CNDTR Configuration --------------*/
	/* Write to DMAy Channelx CNDTR */
	OW_DMA_CH_RX->CNDTR = nbits;

	/*--------------------------- DMAy Channelx CPAR Configuration ---------------*/
	/* Write to DMAy Channelx CPAR */
	OW_DMA_CH_RX->CPAR = (uint32_t) & (OW_USART->RDR);

	/*--------------------------- DMAy Channelx CMAR Configuration ---------------*/
	/* Write to DMAy Channelx CMAR */
	OW_DMA_CH_RX->CMAR = (uint32_t) ow_buf;



/*
	 // DMA for writting
	 DMA_DeInit(OW_DMA_CH_TX);
	 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (OW_USART->TDR);
	 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ow_buf;
	 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	 DMA_InitStructure.DMA_BufferSize = nbits;
	 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	 DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	 DMA_Init(OW_DMA_CH_TX, &DMA_InitStructure);
*/

		/* Disable the selected DMAy Channelx */
		OW_DMA_CH_TX->CCR &= (uint16_t)(~DMA_CCR_EN);

		/* Reset interrupt pending bits for DMA1 Channel5 */
		DMA1->IFCR |= ((uint32_t)(
				DMA_ISR_GIF4 | DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4));

		/* Configure DMAy Channelx: data transfer, data size, priority level and mode */
		OW_DMA_CH_TX->CCR = DMA_CCR_DIR | DMA_CCR_MINC;

		/*--------------------------- DMAy Channelx CNDTR Configuration --------------*/
		/* Write to DMAy Channelx CNDTR */
		OW_DMA_CH_TX->CNDTR = nbits;

		/*--------------------------- DMAy Channelx CPAR Configuration ---------------*/
		/* Write to DMAy Channelx CPAR */
		OW_DMA_CH_TX->CPAR = (uint32_t) & (OW_USART->TDR);

		/*--------------------------- DMAy Channelx CMAR Configuration ---------------*/
		/* Write to DMAy Channelx CMAR */
		OW_DMA_CH_TX->CMAR = (uint32_t) ow_buf;


	 // start send cycle
//	 USART_ClearFlag(OW_USART, USART_FLAG_RXNE | USART_FLAG_TC | USART_FLAG_TXE);
		OW_USART->ICR = USART_ISR_RXNE | USART_ISR_TC | USART_ISR_TXE;

//	 USART_DMACmd(OW_USART, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
		OW_USART->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;

//	 DMA_Cmd(OW_DMA_CH_RX, ENABLE);
		OW_DMA_CH_RX->CCR |= DMA_CCR_EN;

//	 DMA_Cmd(OW_DMA_CH_TX, ENABLE);
		OW_DMA_CH_TX->CCR |= DMA_CCR_EN;

//	 USART_Cmd(OW_USART, ENABLE);
		OW_USART->CR1 |= USART_CR1_UE;

	 // wait end of transmission
	 //	while OW_DMA_TRANSFER_END;
	 uint8_t cntr;
	 cntr = 0;
	 // Ждем, пока не примем 8 байт
	 while ((cntr < 5) && ((DMA1->ISR & OW_DMA_FLAG) == RESET)) {
	 cntr++;
	 chThdSleepMicroseconds(nbits*100);
	 }

	 // turn off DMA
//	 DMA_Cmd(OW_DMA_CH_TX, DISABLE);
	 	OW_DMA_CH_TX->CCR &= (uint16_t)(~DMA_CCR_EN);
//	 DMA_Cmd(OW_DMA_CH_RX, DISABLE);
		OW_DMA_CH_RX->CCR &= (uint16_t)(~DMA_CCR_EN);
//	 USART_DMACmd(OW_USART, USART_DMAReq_Tx | USART_DMAReq_Rx, DISABLE);
		OW_USART->CR3 &= (uint32_t) ~(USART_CR3_DMAT | USART_CR3_DMAR);
}

//-----------------------------------------------------------------------------
// Данная функция осуществляет сканирование сети 1-wire и записывает найденные
//   ID устройств в массив buf, по 8 байт на каждое устройство.
// переменная num ограничивает количество находимых устройств, чтобы не переполнить
// буфер.
//-----------------------------------------------------------------------------
/*
 * scan 1-wire bus
 * 		num - max number of devices
 * 		buf - array for devices' ID's (8*num bytes)
 * return amount of founded devices
 */
uint8_t OW_Scan(uint8_t *buf, uint8_t num) {
	unsigned long path, next, pos;
	uint8_t bit, chk;
	uint8_t cnt_bit, cnt_byte, cnt_num;
	path = 0;
	cnt_num = 0;
	do {
		//(issue the 'ROM search' command)
		if (0 == OW_WriteCmd(OW_SEARCH_ROM))
			return 0;
		next = 0; // next path to follow
		pos = 1; // path bit pointer
		for (cnt_byte = 0; cnt_byte != 8; cnt_byte++) {
			buf[cnt_num * 8 + cnt_byte] = 0;
			for (cnt_bit = 0; cnt_bit != 8; cnt_bit++) {
				//(read two bits, 'bit' and 'chk', from the 1-wire bus)
				OW_toBits(OW_R_1, ow_buf);
				OW_SendBits(2);
				bit = (ow_buf[0] == OW_1);
				chk = (ow_buf[1] == OW_1);
				if (bit && chk)
					return 0; // error
				if (!bit && !chk) { // collision, both are zero
					if (pos & path)
						bit = 1; // if we've been here before
					else
						next = (path & (pos - 1)) | pos; // else, new branch for next
					pos <<= 1;
				}
				//(save this bit as part of the current ROM value)
				if (bit)
					buf[cnt_num * 8 + cnt_byte] |= (1 << cnt_bit);
				//(write 'bit' to the 1-wire bus)
				OW_toBits(bit, ow_buf);
				OW_SendBits(1);
			}
		}
		//(output the just-completed ROM value)
		path = next;
		cnt_num++;
	} while (path && cnt_num < num);
	return cnt_num;
}

//***************************************************************************
// CRC8
// Для серийного номера вызывать 8 раз
// Для данных вызвать 9 раз
// Если в результате crc == 0, то чтение успешно
//***************************************************************************
unsigned char crc8(unsigned char data, unsigned char crc)
#define CRC8INIT   0x00
#define CRC8POLY   0x18              //0X18 = X^8+X^5+X^4+X^0
{
	unsigned char bit_counter;
	unsigned char feedback_bit;
	bit_counter = 8;
	do {
		feedback_bit = (crc ^ data) & 0x01;
		if (feedback_bit == 0x01)
			crc = crc ^ CRC8POLY;
		crc = (crc >> 1) & 0x7F;
		if (feedback_bit == 0x01)
			crc = crc | 0x80;
		data = data >> 1;
		bit_counter--;
	} while (bit_counter > 0);
	return crc;
}

#endif
