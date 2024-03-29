/*
 ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "hal.h"
//#include "nRF24.h"

#if HAL_USE_PAL || defined(__DOXYGEN__)
/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
const PALConfig pal_default_config =
{
{ VAL_GPIOA_MODER, VAL_GPIOA_OTYPER, VAL_GPIOA_OSPEEDR, VAL_GPIOA_PUPDR, VAL_GPIOA_ODR, VAL_GPIOA_AFRL, VAL_GPIOA_AFRH },
{ VAL_GPIOB_MODER, VAL_GPIOB_OTYPER, VAL_GPIOB_OSPEEDR, VAL_GPIOB_PUPDR, VAL_GPIOB_ODR, VAL_GPIOB_AFRL, VAL_GPIOB_AFRH },
{ VAL_GPIOC_MODER, VAL_GPIOC_OTYPER, VAL_GPIOC_OSPEEDR, VAL_GPIOC_PUPDR, VAL_GPIOC_ODR, VAL_GPIOC_AFRL, VAL_GPIOC_AFRH },
{ VAL_GPIOF_MODER, VAL_GPIOF_OTYPER, VAL_GPIOF_OSPEEDR, VAL_GPIOF_PUPDR, VAL_GPIOF_ODR, VAL_GPIOF_AFRL, VAL_GPIOF_AFRH } };
#endif

/**
 * @brief   Early initialization code.
 * @details This initialization must be performed just after stack setup
 *          and before any other initialization.
 */
void __early_init(void)
{

	stm32_clock_init();
}

#if HAL_USE_MMC_SPI || defined(__DOXYGEN__)
/**
 * @brief   MMC_SPI card detection.
 */
bool_t mmc_lld_is_card_inserted(MMCDriver *mmcp)
{

	(void)mmcp;
	/* TODO: Fill the implementation.*/
	return TRUE;
}

/**
 * @brief   MMC_SPI card write protection detection.
 */
bool_t mmc_lld_is_write_protected(MMCDriver *mmcp)
{

	(void)mmcp;
	/* TODO: Fill the implementation.*/
	return FALSE;
}
#endif

/**
 * @brief   Board-specific initialization code.
 * @todo    Add your board-specific code, if any.
 */
void boardInit(void)
{
#ifdef DEBUG_Discovery

//	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOC Periph clock enable */
//		  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* Configure PC8 and PC9 in output pushpull mode */
	/*		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_Init(((GPIO_TypeDef *) GPIOA_BASE), &GPIO_InitStructure);*/


	/* GPIOB Periph clock enable */
//		  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	/* Configure PB1 */
	/*		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_Init(((GPIO_TypeDef *) GPIOB_BASE), &GPIO_InitStructure);*/

	palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(GPIOA, GPIOA_PIN0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(GPIOA, GPIOA_PIN1, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
//	palSetPadMode(GPIOA, GPIOA_PIN2, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(GPIOA, GPIOA_PIN3, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

//	palSetPadMode(GPIOF, GPIOF_PIN0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	//	  LEDB1On();

	//for B1Swap

#endif
}
