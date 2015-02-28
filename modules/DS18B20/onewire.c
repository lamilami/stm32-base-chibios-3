#include "ch.h"
#include "hal.h"
#include "onewire.h"
#include "core.h"

#ifndef _ONEWIRE_C
#define _ONEWIRE_C

#if DS18B20_PRESENT

#if ONEWIRE_USE_OW1
OWDriver OWD1;
#endif

#if ONEWIRE_USE_OW2
OWDriver OWD2;
#endif

static const uint8_t owReadPattern[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#if ONEWIRE_USE_CRC8
static const uint8_t ow_crc_table[] = {0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65, 157, 195,
                                       33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220, 35, 125, 159, 193,
                                       66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98, 190, 224, 2, 92, 223, 129,
                                       99, 61, 124, 34, 192, 158, 29, 67, 161, 255, 70, 24, 250, 164, 39, 121, 155, 197,
                                       132, 218, 56, 102, 229, 187, 89, 7, 219, 133, 103, 57, 186, 228, 6, 88, 25, 71,
                                       165, 251, 120, 38, 196, 154, 101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27,
                                       69, 198, 152, 122, 36, 248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216,
                                       91, 5, 231, 185, 140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113,
                                       147, 205, 17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14,
                                       80, 175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238, 50,
                                       108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115, 202, 148,
                                       118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139, 87, 9, 235, 181,
                                       54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22, 233, 183, 85, 11, 136,
                                       214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168, 116, 42, 200, 150, 21, 75,
                                       169, 247, 182, 232, 10, 84, 215, 137, 107, 53};

uint8_t owCrc8(uint8_t *data, size_t len) {
  uint8_t crc8 = 0;
  size_t i;

  for (i = 0; i < len; i++)
    crc8 = ow_crc_table[crc8 ^ data[i]]; // See Application Note 27

  return crc8;
}

#endif /* ONEWIRE_USE_CRC8 */

static void rx_char_cb(UARTDriver *uartp, uint16_t c) {
  OWDriver* owp;

  (void)c;

#if ONEWIRE_USE_OW1
  if (uartp == OWD1.uart)
    owp = &OWD1;
#endif

#if ONEWIRE_USE_OW2
  if( uartp == OWD2.uart ) owp = &OWD2;
#endif

  if (owp->tp) {
    chSysLockFromISR();
    owp->tp->p_u.rdymsg = (msg_t)0; // set RDY_OK
    chSchReadyI(owp->tp);
    owp->tp = NULL;
    chSysUnlockFromISR();
  }
}

static void rx_err_cb(UARTDriver *uartp, uartflags_t e) {
  OWDriver* owp;

#if ONEWIRE_USE_OW1
  if (uartp == OWD1.uart)
    owp = &OWD1;
#endif

#if ONEWIRE_USE_OW2
  if( uartp == OWD2.uart ) owp = &OWD2;
#endif

  owp->errfl = e;
}

static void rx_buf_cb(UARTDriver *uartp) {
  OWDriver* owp;

#if ONEWIRE_USE_OW1
  if (uartp == OWD1.uart)
    owp = &OWD1;
#endif

#if ONEWIRE_USE_OW2
  if( uartp == OWD2.uart ) owp = &OWD2;
#endif

  if (owp->tp) {
    chSysLockFromISR();
    owp->tp->p_u.rdymsg = (msg_t)0; // set RDY_OK
    chSchReadyI(owp->tp);
    owp->tp = NULL;
    chSysUnlockFromISR();
  }
}

static void owObjectInit(OWDriver *owp) {
  owp->cfg.rxend_cb = rx_buf_cb;
  owp->cfg.rxchar_cb = rx_char_cb;
  owp->cfg.rxerr_cb = rx_err_cb;
  owp->tp = NULL;
#if ONEWIRE_USE_OVERDRIVE
  owp->ODmode = FALSE;
  owp->ODmodeSet = FALSE;
#endif
}

void owInit(void) {
#if ONEWIRE_USE_OW1
  owObjectInit(&OWD1);
#endif
#if ONEWIRE_USE_OW2
  owObjectInit( &OWD2 );
#endif
}

void owStart(OWDriver *owp, UARTDriver *uartp) {
  owp->uart = uartp;

  owp->cfg.speed = 9600;
  uartStart(owp->uart, &owp->cfg);
}

void owStop(OWDriver *owp) {
  uartStop(owp->uart);
}

#if ONEWIRE_USE_SELECT

#if ONEWIRE_USE_OVERDRIVE
void owSelect( OWDriver *owp, rom_id_t *device, bool_t ovr )
#else
void owSelect( OWDriver *owp, rom_id_t *device )
#endif /* ONEWIRE_USE_OVERDRIVE */
{
  if( device ) owp->rom.id = device->id;
#if ONEWIRE_USE_OVERDRIVE
  owp->ODmode = ovr;
  owp->ODmodeSet = FALSE;
#endif /* ONEWIRE_USE_OVERDRIVE */
}
#endif /* ONEWIRE_USE_SELECT */

static msg_t owSendByte(OWDriver *owp, uint8_t data) {
  static uint8_t rxbuf[8], txbuf[8];
  int i;
  msg_t result;

  for (i = 0; i < 8; i++) {
    txbuf[i] = (data & 1) ? 0xFF : 0x80;
    data >>= 1;
  }

  osalThreadSleepMicroseconds(1000);

  uartStartReceive(owp->uart, 8, &rxbuf);

  chSysLock();
  owp->tp = chThdGetSelfX();
//  osalThreadSleepS(OSAL_US2ST(1000));
  uartStartSendI(owp->uart, 8, &txbuf);
  result = chSchGoSleepTimeoutS(CH_STATE_SUSPENDED, MS2ST(100));
  owp->tp = NULL;
  chSysUnlock();

  if (MSG_TIMEOUT == result) {
    uartStopReceive(owp->uart);
    return MSG_RESET;
  }

  return MSG_OK;
}

static msg_t owReceiveByte(OWDriver *owp, uint8_t *data) {
  uint8_t rxbuf[8];
  int i;
  msg_t result;

  uartStartReceive(owp->uart, 8, rxbuf);

  chSysLock();
  owp->tp = chThdGetSelfX();
  uartStartSendI(owp->uart, 8, owReadPattern);
  result = chSchGoSleepTimeoutS(CH_STATE_SUSPENDED, MS2ST(50));
  owp->tp = NULL;
  chSysUnlock();

  if (MSG_TIMEOUT == result) {
    uartStopReceive(owp->uart);
    return MSG_RESET;
  }

  for (i = 0; i < 8; i++) {
    *data >>= 1;
    if (0xFF == rxbuf[i])
      *data |= 0x80;
  }

  return MSG_OK;
}

static const uint8_t owResetPattern[] = {0xF0}; // was F8

msg_t owResetBus(OWDriver *owp) {
  msg_t result;

  owp->errfl = 0;
#if ONEWIRE_USE_OVERDRIVE
  owp->cfg.speed = owp->ODmodeSet ? 57143 : 7142; // ovr/std speed
#else
  owp->cfg.speed = 9600; // std speed
#endif
  uartStart(owp->uart, &owp->cfg);

  chSysLock();
  owp->tp = chThdGetSelfX();
  uartStartSendI(owp->uart, 1, owResetPattern);
  result = chSchGoSleepTimeoutS(CH_STATE_SUSPENDED, MS2ST(50));
  owp->tp = NULL;
  chSysUnlock();

  if (MSG_TIMEOUT == result)
    return MSG_RESET; // bus error
  if (owp->errfl & UART_FRAMING_ERROR)
    return MSG_RESET; // bus error
  if (0xF8 == owp->uart->rxbuf)
    return MSG_TIMEOUT; // no devices on the bus

  return MSG_OK;
}

#if ONEWIRE_USE_SEARCH

static uint8_t owReceiveBit( OWDriver *owp )
{
  chSysLock();
  owp->tp = chThdGetSelfX();
  uartStartSendI( owp->uart, 1, owReadPattern );
  chSchGoSleepTimeoutS( CH_STATE_SUSPENDED, MS2ST(50) );
  owp->tp = NULL;
  chSysUnlock();

  if( 0xFF == owp->uart->rxbuf ) return 1;

  return 0;
}

static msg_t owSendBit( OWDriver *owp, uint8_t data )
{
  uint8_t txbuf;
  msg_t result;

  txbuf = (data & 1) ? 0xFF : 0x80;

  chSysLock();
  owp->tp = chThdGetSelfX();
  uartStartSendI( owp->uart, 1, &txbuf );
  result = chSchGoSleepTimeoutS( CH_STATE_SUSPENDED, MS2ST(50) );
  owp->tp = NULL;
  chSysUnlock();

  return (RDY_TIMEOUT == result) ? RDY_RESET : RDY_OK;
}

// fam - target family or 0 for all devices on the bus
void owSearchStart( OWDriver *owp, uint8_t fam/*, bool_t alarm*/)
{
  owp->FamilySearch = fam;
  owp->LastDiscrepancy = fam ? 64 : 0;
  owp->LastDeviceFlag = FALSE;
}

// RDY_OK - ok, RDY_RESET - bus error, RDY_TIMEOUT - no [more] devices
msg_t owSearch( OWDriver *owp, rom_id_t *rom_no )
{
  msg_t result;
  uint8_t last_zero;
  uint8_t id_bit, cmp_id_bit;
  uint8_t search_direction;
  unsigned id_bit_number, rom_byte_number;
  uint8_t rom_byte_mask;

// initialize for search
  id_bit_number = 1;
  last_zero = 0;
  rom_byte_number = 0;
  rom_byte_mask = 1;

  rom_no->id = 0;
  rom_no->arr[0] = owp->FamilySearch;

// if the last call was not the last one
  if( owp->LastDeviceFlag ) return RDY_TIMEOUT;

  result = owResetBus( owp );// start the new search cycle
  if( RDY_OK != result ) return result;

  owp->cfg.speed = 125000;// perform search process on the standard speed
  uartStart( owp->uart, &owp->cfg );

  owSendByte( owp, 0xF0 );// issue the search command

  do
  {
    id_bit = owReceiveBit( owp ); // read a bit and its complement
    cmp_id_bit = owReceiveBit( owp );

    if( id_bit && cmp_id_bit ) return RDY_RESET;// check for no devices on 1-wire

// all devices coupled have 0 or 1
    if( id_bit != cmp_id_bit )
    search_direction = id_bit;// bit write value for search
    else
    {
// if this discrepancy if before the Last Discrepancy
// on a previous next then pick the same as last time
      if( id_bit_number < owp->LastDiscrepancy )
      search_direction = (rom_no->arr[rom_byte_number] & rom_byte_mask) ? 1 : 0;
      else
// if equal to last pick 1, if not then pick 0
      search_direction = (id_bit_number == owp->LastDiscrepancy) ? 1 : 0;

      if( search_direction == 0 ) last_zero = id_bit_number;// if 0 was picked then record its position in LastZero
    }

// set or clear the bit in the ROM byte rom_byte_number
// with mask rom_byte_mask
    if( search_direction == 1 )
    rom_no->arr[rom_byte_number] |= rom_byte_mask;
    else
    rom_no->arr[rom_byte_number] &= ~rom_byte_mask;

    owSendBit( owp, search_direction );// serial number search direction write bit

    id_bit_number++;// increment the byte counter id_bit_number
    rom_byte_mask <<= 1;// and shift the mask rom_byte_mask

// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
    if( rom_byte_mask == 0 )
    {
      rom_byte_number++;
      rom_byte_mask = 1;
    }
  }while( id_bit_number < 65 ); // loop until through all ROM bytes 0-7

  owp->LastDiscrepancy = last_zero;

  if( 0 == owp->LastDiscrepancy ) owp->LastDeviceFlag = TRUE;// check for last device

  return RDY_OK;
}

#endif /* ONEWIRE_USE_SEARCH */

// len < 0 -rxdata, len > 0 -tx data
msg_t owCommand(OWDriver *owp, uint8_t cmd, uint8_t *buf, int len) {
  size_t i;
  msg_t result;

// reset bus

  result = owResetBus(owp);
  if (MSG_OK != result)
    return result; // error

// reset OK, send ROM command

#if ONEWIRE_USE_OVERDRIVE
  owp->cfg.speed = owp->ODmodeSet ? 714286 : 125000; // ovr/std speed
#else
  owp->cfg.speed = 100000; // std speed
#endif
  uartStart(owp->uart, &owp->cfg);

#if ONEWIRE_USE_OVERDRIVE
  if( owp->ODmode && !owp->ODmodeSet ) // switch slave to OverDrive mode
  {
#if ONEWIRE_USE_SELECT
    if( owp->rom.arr[0] ) // device selected?	(checking the family byte should be enough)
    owSendByte( owp, 0x69 );// use "overdrive match rom"
    else
#endif /* ONEWIRE_USE_SELECT */
    owSendByte( owp, 0x3C ); // "overdrive skip rom"

    owp->cfg.speed = 714286;// switch to OverDrive speed
    uartStart(owp->uart, &owp->cfg);

    owp->ODmodeSet = TRUE;

#if ONEWIRE_USE_SELECT		
    if( owp->rom.arr[0] ) // device selected? - send rom ID at OverDrive speed
    for( i = 0; i < 8; i++ ) owSendByte( owp, owp->rom.arr[i] );
#endif /* ONEWIRE_USE_SELECT */
  }
  else
  {
#endif /* ONEWIRE_USE_OVERDRIVE */
#if ONEWIRE_USE_SELECT
  if( owp->rom.arr[0] ) // device selected - use "match rom"
  {
    owSendByte( owp, 0x55 );
    for( i = 0; i < 8; i++ ) owSendByte( owp, owp->rom.arr[i] ); // send device serial
  }
  else // bcast
#endif /* ONEWIRE_USE_SELECT */
  owSendByte(owp, 0xCC); // use "skip rom"
#if ONEWIRE_USE_OVERDRIVE
           }
#endif

// send function command

  owSendByte(owp, cmd);

// tx/rx data

  if (len > 0) // command with parameter?
    for (i = 0; i < len; i++)
      owSendByte(owp, buf[i]);
  else if (len < 0) {
    len = -len;
    for (i = 0; i < len; i++)
      owReceiveByte(owp, buf + i);
  }

  return MSG_OK;
}

#if ONEWIRE_USE_READROM

// read ROMID of single device on the bus
msg_t owReadRom(OWDriver *owp, uint8_t *buf) {
  size_t i;
  msg_t result;

// reset bus
  result = owResetBus(owp);
  if (MSG_OK != result)
    return result; // error

// reset OK, send ROM command
#if ONEWIRE_USE_OVERDRIVE
  owp->cfg.speed = owp->ODmodeSet ? 714286 : 125000; // ovr/std speed
#else
  owp->cfg.speed = 100000; // std speed
#endif
  uartStart(owp->uart, &owp->cfg);

  result = owSendByte(owp, 0x33); // read rom

  if (MSG_OK == result)
    for (i = 0; i < 8; i++) {
      result = owReceiveByte(owp, buf + i);
      if (MSG_OK != result)
        return MSG_RESET;
    }
  else
    return MSG_RESET;

  return MSG_OK;
}

#endif /* ONEWIRE_USE_READROM */

#endif /* HAL_USE_ONEWIRE */

#endif /* _ONEWIRE_C */
