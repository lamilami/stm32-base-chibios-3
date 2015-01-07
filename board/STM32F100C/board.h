/*
    ChibiOS/RT - Copyright (C) 2006-2014 Giovanni Di Sirio

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

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for STMicroelectronics STM32VL-Discovery board.
 */

/*
 * Board identifier.
 */
#define BOARD_ST_STM32F100C_BASE
#define BOARD_NAME                  "ST STM32F100C-Base"


#define LEDSwap() GPIOC->ODR ^= PAL_PORT_BIT(13)
#define LEDB1Swap() GPIOB->ODR ^= PAL_PORT_BIT(9)

/*
 * Board oscillators-related settings.
 * NOTE: HSE not fitted.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK            32768
#endif

#define STM32_LSEDRV                (3 << 3)

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                0
#endif

//#define STM32_HSE_BYPASS

/*
 * MCU type as defined in the ST header.
 */
//#define STM32F0XX_LD

/*
 * IO pins assignments.
 */
#define GPIOA_PIN0                	0
#define GPIOA_PIN1                  1
#define GPIOA_PIN2                  2
#define GPIOA_PIN3                  3
#define GPIOA_PIN4                  4
#define GPIOA_PIN5                  5
#define GPIOA_PIN6                  6
#define GPIOA_PIN7                  7
#define GPIOA_PIN8                  8
#define GPIOA_PIN9                  9
#define GPIOA_PIN10                 10
#define GPIOA_PIN11                 11
#define GPIOA_PIN12                 12
#define GPIOA_SWDAT                 13
#define GPIOA_SWCLK                 14
#define GPIOA_PIN15                 15

#define GPIOB_PIN0                  0
#define GPIOB_PIN1                  1
#define GPIOB_PIN2                  2
#define GPIOB_PIN3                  3
#define GPIOB_PIN4                  4
#define GPIOB_PIN5                  5
#define GPIOB_PIN6                  6
#define GPIOB_PIN7                  7
#define GPIOB_PIN8                  8
#define GPIOB_PIN9                  9
#define GPIOB_PIN10                 10
#define GPIOB_PIN11                 11
#define GPIOB_PIN12                 12
#define GPIOB_PIN13                 13
#define GPIOB_PIN14                 14
#define GPIOB_PIN15                 15

#define GPIOC_PIN0                  0
#define GPIOC_PIN1                  1
#define GPIOC_PIN2                  2
#define GPIOC_PIN3                  3
#define GPIOC_PIN4                  4
#define GPIOC_PIN5                  5
#define GPIOC_PIN6                  6
#define GPIOC_PIN7                  7
#define GPIOC_PIN8                  8
#define GPIOC_PIN9                  9
#define GPIOC_PIN10                 10
#define GPIOC_PIN11                 11
#define GPIOC_PIN12                 12
#define GPIOC_PIN13                 13
#define GPIOC_OSC32_IN              14
#define GPIOC_OSC32_OUT             15

#define GPIOD_OSC_IN                0
#define GPIOD_OSC_OUT               1
#define GPIOD_PIN0                  0
#define GPIOD_PIN1                  1
#define GPIOD_PIN2                  2
#define GPIOD_PIN3                  3
#define GPIOD_PIN4                  4
#define GPIOD_PIN5                  5
#define GPIOD_PIN6                  6
#define GPIOD_PIN7                  7
#define GPIOD_PIN8                  8
#define GPIOD_PIN9                  9
#define GPIOD_PIN10                 10
#define GPIOD_PIN11                 11
#define GPIOD_PIN12                 12
#define GPIOD_PIN13                 13
#define GPIOD_PIN14                 14
#define GPIOD_PIN15                 15

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA0  - Normal input      (BUTTON).
 * PA2  - Alternate output  (USART2 TX).
 * PA3  - Normal input      (USART2 RX).
 * PA4  - Push pull output  (SPI1 NSS), initially high state.
 * PA5  - Alternate output  (SPI1 SCK).
 * PA6  - Normal input      (SPI1 MISO).
 * PA7  - Alternate output  (SPI1 MOSI).
 * PA9  - Alternate output  (USART1 TX).
 * PA10 - Normal input      (USART1 RX).
 */
//#define VAL_GPIOACRL            0xB4B34B84      /*  PA7...PA0 */
//#define VAL_GPIOACRH            0x88844444      /* PA15...PA8 */
//#define VAL_GPIOAODR            0x0000a000

#define VAL_GPIOACRL            0x44444444      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x88844444      /* PA15...PA8 */
#define VAL_GPIOAODR            0x0000a000

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB12 - Push pull output  (SPI2 NSS), initially high state.
 * PB13 - Alternate output  (SPI2 SCK).
 * PB14 - Normal input      (SPI2 MISO).
 * PB15 - Alternate output  (SPI2 MOSI).
 */

//#define VAL_GPIOBCRL            0x88888888      /*  PB7...PB0 */
//#define VAL_GPIOBCRH            0xB4B38888      /* PB15...PB8 */
//#define VAL_GPIOBODR            0x0000FFFF

#define VAL_GPIOBCRL            0x44484444      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x44444444      /* PB15...PB8 */
#define VAL_GPIOBODR            0x00000010

/*
 * Port C setup.
 * Everything input with pull-up except:
 * PC8  - Push-pull output (LED4), initially low state.
 * PC9  - Push-pull output (LED3), initially low state.
 */
//#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
//#define VAL_GPIOCCRH            0x88388833      /* PC15...PC8 */
//#define VAL_GPIOCODR            0x0000DFFF

#define VAL_GPIOCCRL            0x44444444      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x44344444      /* PC15...PC8 */
#define VAL_GPIOCODR            0x00000000

/*
 * Port D setup.
 * Everything input with pull-up except:
 * PD0  - Normal input (XTAL).
 * PD1  - Normal input (XTAL).
 * PD1  - Push-pull output (Load LED), initially low state.
 */
#define VAL_GPIODCRL            0x88888834      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0x00000000

//#define VAL_GPIODCRL            0x44444434      /*  PD7...PD0 */
//#define VAL_GPIODCRH            0x44444444      /* PD15...PD8 */
//#define VAL_GPIODODR            0x00000000

/*
 * Port E setup.
 * Everything input with pull-up except:
 */
//#define VAL_GPIOECRL            0x88888888      /*  PE7...PE0 */
//#define VAL_GPIOECRH            0x88888888      /* PE15...PE8 */
//#define VAL_GPIOEODR            0x0000FFFF

#define VAL_GPIOECRL            0x44444444      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x44444444      /* PE15...PE8 */
#define VAL_GPIOEODR            0x00000000

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
