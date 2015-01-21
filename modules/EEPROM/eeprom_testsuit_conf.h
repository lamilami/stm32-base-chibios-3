#ifndef __EEPROM_CONFIG_H__
#define __EEPROM_CONFIG_H__

/* Configuration for test suit. */

static const SPIConfig SPI1Config =
{	NULL, GPIOB, 12, SPI_CR1_BR_1 | SPI_CR1_BR_0, // | SPI_CR1_MSTR,
//  SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 // | SPI_CR2_FRXTH
};

#define USE_EEPROM_TEST_SUIT    1           /* build test suit for EEPROM abstraction layer */
#define EEPROM_PAGE_SIZE        128         /* page size in bytes. Consult datasheet. */
#define EEPROM_SIZE             65535       /* total amount of memory in bytes */
#define EEPROM_SPID             SPID2       /* ChibiOS SPI driver used to communicate with EEPROM */
#define EEPROM_SPIDCONFIG       SPI1Config  /* Config of SPI device driver */
#define EEPROM_WRITE_TIME_MS    5          /* time to write one page in ms. Consult datasheet! */
#define EEPROM_DRIVER_NAME      "25xx"

#endif /* __EEPROM_CONFIG_H__ */
