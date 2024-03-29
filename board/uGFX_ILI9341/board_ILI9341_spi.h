/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/addons/gdisp/board_ILI9341_spi.h
 * @brief   GDISP Graphic Driver subsystem board interface for the ILI9341 display.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define LCD_SPI_PORT	GPIOB
#define LCD_MOSI	15
#define LCD_MISO	14
#define LCD_SCK		13

#define LCD_SPI_CS_PORT	GPIOA
#define LCD_CS		10

#define LCD_CTRL_PORT	GPIOA
#define LCD_DC		11
#define LCD_RES		12

#define LCD_DC_CMD	palClearPad(LCD_CTRL_PORT, LCD_DC)
#define LCD_DC_DATA	palSetPad(LCD_CTRL_PORT, LCD_DC)
//#define LCD_SCK_SET	palSetPad(LCD_PORT, LCD_SCK)
//#define LCD_SCK_RES	palClearPad(LCD_PORT, LCD_SCK)
#define LCD_CS_RES	palSetPad(LCD_SPI_CS_PORT, LCD_CS)
#define LCD_CS_SET	palClearPad(LCD_SPI_CS_PORT, LCD_CS)

#define LCD_SPI_PAL_PIN_MODE	PAL_MODE_STM32_ALTERNATE_PUSHPULL

#define LCD_SPI_REMAP_CMD

/**
 * SPI configuration structure.
 * Speed 12 MHz, CPHA=0, CPOL=0, 8bits frames, MSb transmitted first.
 * Soft slave select.
 */
static const SPIConfig spi2cfg = {NULL, LCD_SPI_CS_PORT, LCD_CS,
                                  (SPI_CR1_MSTR | SPI_CR1_SPE | SPI_CR1_SSM | SPI_CR1_SSI)};

static void send_data(uint16_t data);

/**
 * @brief   Initialise the board for the display.
 *
 * @param[in] g			The GDisplay structure
 *
 * @note	Set the g->board member to whatever is appropriate. For multiple
 * 			displays this might be a pointer to the appropriate register set.
 *
 * @notapi
 */
static inline void init_board(GDisplay *g) {

  // As we are not using multiple displays we set g->board to NULL as we don't use it.
//  g->board = 0;

  gfxSleepMilliseconds(50);

  palSetPadMode(LCD_SPI_CS_PORT, LCD_CS, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(LCD_CTRL_PORT, LCD_DC, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(LCD_CTRL_PORT, LCD_RES, PAL_MODE_OUTPUT_PUSHPULL);

  palSetGroupMode(LCD_SPI_PORT, PAL_PORT_BIT(LCD_SCK) | PAL_PORT_BIT(LCD_MISO) | PAL_PORT_BIT(LCD_MOSI), 0,
                  LCD_SPI_PAL_PIN_MODE);

  LCD_SPI_REMAP_CMD;

  gfxSleepMilliseconds(50);

  spiStart(&SPID2, &spi2cfg);
  spiSelect(&SPID2);
}

/**
 * @brief   After the initialisation.
 *
 * @param[in] g			The GDisplay structure
 *
 * @notapi
 */
static inline void post_init_board(GDisplay *g) {
  (void)g;
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] g			The GDisplay structure
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static inline void setpin_reset(GDisplay *g, bool_t state) {
  (void)g;

  if (state == TRUE) {
    palClearPad(LCD_CTRL_PORT, LCD_RES);
  }
  else {
    palSetPad(LCD_CTRL_PORT, LCD_RES);
  }
}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] percent		0 to 100%
 * 
 * @notapi
 */
static inline void set_backlight(GDisplay *g, uint8_t percent) {
  (void)g;
  (void)percent;
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline void acquire_bus(GDisplay *g) {
  (void)g;
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline void release_bus(GDisplay *g) {
  (void)g;
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data			The data to send
 * 
 * @notapi
 */
static inline void send_data(uint16_t data) {
// http://forum.easyelectronics.ru/viewtopic.php?p=262122#p262122
  while (!(SPI2->SR & SPI_SR_TXE))
    ;     // ��� ����� �� �������� ��������� - � ������ �� SPI_DR
  SPI2->DR = data;     // ��������� � SPI_DR ��� �������

}

/**
 * @brief   Send data to the index register.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] index			The index register to set
 *
 * @notapi
 */
static inline void write_index(GDisplay *g, uint16_t index) {
  (void)g;

  while (SPI2->SR & SPI_SR_BSY)
    ;
  __asm("NOP\n");
  __asm("NOP\n");
//  __asm("NOP\n");
//  __asm("NOP\n");
//  __asm("NOP\n");
  LCD_CS_RES;
  LCD_DC_CMD;     // ��������� ������� � ����� ������
  LCD_CS_SET;
  send_data(index);
  __asm("NOP\n");
  __asm("NOP\n");
  __asm("NOP\n");
  __asm("NOP\n");
//  __asm("NOP\n");
  while (SPI2->SR & SPI_SR_BSY)
    ;     // ���� ���� ���������� (==1) -- ������ SPI �����
  /* ������ ���� �������� ��������� �������� ������� ��������� � ���������� �����
   * ����� ������ ��� �������� �������� � ��������.
   */
  LCD_DC_DATA;     // ��������� ������� � ����� ������
}

/**
 * @brief   Send data to the lcd with DC control.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] data			The data to send
 * 
 * @notapi
 */
static inline void write_data(GDisplay *g, uint16_t data) {
  (void)g;

  send_data(data);
}

/**
 * @brief   Set the bus in read mode
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline void setreadmode(GDisplay *g) {
  (void)g;
}

/**
 * @brief   Set the bus back into write mode
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline void setwritemode(GDisplay *g) {
  (void)g;
}

/**
 * @brief   Read data from the lcd.
 * @return	The data from the lcd
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static inline uint16_t read_data(GDisplay *g) {
  (void)g;
  return 0;
}

#endif /* _GDISP_LLD_BOARD_H */

