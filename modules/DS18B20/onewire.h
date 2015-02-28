#ifndef _ONEWIRE_H
#define _ONEWIRE_H

#define HAL_USE_ONEWIRE        TRUE   // enable driver code

#define ONEWIRE_USE_OW1        TRUE     // 1wire bus #1
#define ONEWIRE_USE_OW2        FALSE    // 1wire bus #2
#define ONEWIRE_USE_SEARCH     FALSE    // compile 1wire search algorithm
#define ONEWIRE_USE_READROM    TRUE    // compile owReadRom
#define ONEWIRE_USE_CRC8       TRUE     // compile owCrc8
#define ONEWIRE_USE_SELECT     FALSE    // compile selective device support
#define ONEWIRE_USE_OVERDRIVE  FALSE    // compile OverDrive speed support

#if HAL_USE_ONEWIRE

#if !ONEWIRE_USE_OW1 && !ONEWIRE_USE_OW2
#error at least one 1-Wire Bus must be defined!
#endif

#if !HAL_USE_UART
#error UART driver must be compiled!
#endif

#if !ONEWIRE_USE_SELECT
#undef ONEWIRE_USE_SEARCH
#endif

#if ONEWIRE_USE_SELECT //|| ONEWIRE_READROM
typedef union
{
  uint64_t id;
  uint8_t arr[8];
} rom_id_t;
#endif /* ONEWIRE_USE_SELECT */

struct OWDriver
{
  UARTDriver* uart;
  UARTConfig cfg;
  uartflags_t errfl;
  thread_t *tp;
#if ONEWIRE_USE_SELECT
  rom_id_t rom;                // Selected device
#endif
#if ONEWIRE_USE_OVERDRIVE
  uint8_t ODmode, ODmodeSet;   // OverDrive mode flags
#endif
#if ONEWIRE_USE_SEARCH
  uint8_t FamilySearch;
  uint8_t LastDiscrepancy;
  uint8_t LastDeviceFlag;
#endif
};

typedef struct OWDriver OWDriver;

#if ONEWIRE_USE_OW1
extern OWDriver OWD1;
#endif

#if ONEWIRE_USE_OW2
extern OWDriver OWD2;
#endif

#ifdef __cplusplus
extern "C" {
#endif

  void owInit( void );
  void owStart( OWDriver *owp, UARTDriver *uartp );
  void owStop( OWDriver *owp );
  msg_t owResetBus( OWDriver *owp );
  uint8_t owCrc8( uint8_t *data, size_t len );
  msg_t owCommand( OWDriver *owp, uint8_t cmd, uint8_t *buf, int len );

#if ONEWIRE_USE_SELECT

  void owSearchStart( OWDriver *owp, uint8_t fam );
  msg_t owSearch( OWDriver *owp, rom_id_t *rom_no );

#if ONEWIRE_USE_OVERDRIVE
  void owSelect( OWDriver *owp, rom_id_t *device, bool_t ovr );
#else
  void owSelect( OWDriver *owp, rom_id_t *device );
#endif /* ONEWIRE_USE_OVERDRIVE */

#endif /* ONEWIRE_USE_SELECT */

#ifdef __cplusplus
}
#endif

#endif

#endif /* _ONEWIRE_H */
