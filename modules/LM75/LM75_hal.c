#include "ch.h"
#include "hal.h"
#include "core.h"

#if LM75_PRESENT
#include "LM75_hal.h"

/* buffers depth */
#define TMP75_RX_DEPTH 2
#define TMP75_TX_DEPTH 2

/* input buffer */
static uint8_t tmp75_rx_data[TMP75_RX_DEPTH];

/* temperature value */
static int16_t temperature = 0;

static i2cflags_t errors = 0;

#define tmp75_addr 0b1001000

/* This is main function. */
void request_temperature(void){
  int16_t t_int = 0, t_frac = 0;
  msg_t status = MSG_OK;
  systime_t tmo = MS2ST(4);

  i2cAcquireBus(&I2CD2);
  status = i2cMasterReceiveTimeout(&I2CD2, tmp75_addr, tmp75_rx_data, 2, tmo);
  i2cReleaseBus(&I2CD2);

  if (status != MSG_OK){
    errors = i2cGetErrors(&I2CD2);
  }

  t_int = tmp75_rx_data[0] * 100;
  t_frac = (tmp75_rx_data[1] * 100) >> 8;
  temperature = t_int + t_frac;
}

void LM75_init()
{
	i2cStart(&I2CD2,&lm75_i2ccfg);

	  /* tune ports for I2C1*/
	  palSetPadMode(GPIOB, 10, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
	  palSetPadMode(GPIOB, 11, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

	  chThdSleepMilliseconds(100);  /* Just to be safe. */
}

#endif
