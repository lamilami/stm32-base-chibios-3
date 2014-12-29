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

// Буфер для приема/передачи по 1-wire
uint8_t ow_buf[8];

#define OW_0	0x00
#define OW_1	0xff
#define OW_R_1	0xff

//static UARTConfig uart1_cfg_high = {NULL, NULL, rxend, rxchar, rxerr, 100000, 0, USART_CR2_STOP_1, USART_CR3_HDSEL};
//static UARTConfig uart1_cfg_low = {NULL, NULL, rxend, rxchar, rxerr, 9600, 0, USART_CR2_STOP_1, USART_CR3_HDSEL};

static SerialConfig sd_cfg_high = {100000, 0, USART_CR2_STOP_1, USART_CR3_HDSEL};
static SerialConfig sd_cfg_low = {9600, 0, USART_CR2_STOP_1, USART_CR3_HDSEL};

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
    }
    else {
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

  rccEnableUSART1(TRUE);

  palSetPadMode(GPIOA, GPIOA_PIN9, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

#else

//  rccEnableAHB(RCC_AHBENR_GPIOAEN, TRUE);
  rccEnableUSART1(TRUE);
//  rccEnableAPB2(RCC_APB2ENR_SYSCFGCOMPEN, TRUE);

  palSetPadMode(GPIOA, GPIOA_PIN9,
                PAL_MODE_ALTERNATE(1) | PAL_STM32_OSPEED_MID | PAL_STM32_PUDR_PULLUP | PAL_STM32_OTYPE_OPENDRAIN);

#endif

  sdStart(&SD1, &sd_cfg_high);

  return OW_OK;
}

//-----------------------------------------------------------------------------
// осуществляет сброс и проверку на наличие устройств на шине
//-----------------------------------------------------------------------------
uint8_t OW_Reset() {
  uint8_t ow_presence;

  sdStart(&SD1, &sd_cfg_low);

  sdPut(&SD1, (0xf0 & (uint16_t)0x01FF));
  ow_presence = sdGet(&SD1);

  sdStart(&SD1, &sd_cfg_high);

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
uint8_t OW_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen, uint8_t *data, uint8_t dLen, uint8_t readStart) {

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

    sdWrite(&SD1, ow_buf, 8);
    sdRead(&SD1, ow_buf, 8);

    // если прочитанные данные кому-то нужны - выкинем их в буфер
    if (readStart == 0 && dLen > 0) {
      *data = OW_toByte(ow_buf);
      data++;
      dLen--;
    }
    else {
      if (readStart != OW_NO_READ) {
        readStart--;
      }
    }
  }

  return OW_OK;
}

// send nbits bits from ow_buf to 1-wire
void OW_SendBits(uint8_t nbits) {

  sdWrite(&SD1, ow_buf, nbits);
  sdRead(&SD1, ow_buf, nbits);

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
