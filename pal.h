/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012,2013 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    pal.h
 * @brief   I/O Ports Abstraction Layer macros, types and structures.
 *
 * @addtogroup PAL
 * @{
 */

#ifndef _PAL_H_
#define _PAL_H_

//#include "pal_lld.h"
/**
 * @brief Digital I/O port sized unsigned type.
 */
typedef uint32_t ioportmask_t;

/**
 * @brief   Digital I/O modes.
 */
typedef uint32_t iomode_t;

/**
 * @brief   Port Identifier.
 * @details This type can be a scalar or some kind of pointer, do not make
 *          any assumption about it, use the provided macros when populating
 *          variables of this type.
 */
typedef GPIO_TypeDef * ioportid_t;

/**
 * @brief   Reads an I/O port.
 * @details This function is implemented by reading the GPIO IDR register, the
 *          implementation has no side effects.
 * @note    This function is not meant to be invoked directly by the application
 *          code.
 *
 * @param[in] port      port identifier
 * @return              The port bits.
 *
 * @notapi
 */
#define pal_lld_readport(port) ((port)->IDR)

/**
 * @brief   Reads the output latch.
 * @details This function is implemented by reading the GPIO ODR register, the
 *          implementation has no side effects.
 * @note    This function is not meant to be invoked directly by the application
 *          code.
 *
 * @param[in] port      port identifier
 * @return              The latched logical states.
 *
 * @notapi
 */
#define pal_lld_readlatch(port) ((port)->ODR)

/**
 * @brief   Writes on a I/O port.
 * @details This function is implemented by writing the GPIO ODR register, the
 *          implementation has no side effects.
 * @note    Writing on pads programmed as pull-up or pull-down has the side
 *          effect to modify the resistor setting because the output latched
 *          data is used for the resistor selection.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be written on the specified port
 *
 * @notapi
 */
#define pal_lld_writeport(port, bits) ((port)->ODR = (bits))

/**
 * @brief   Sets a bits mask on a I/O port.
 * @details This function is implemented by writing the GPIO BSRR register, the
 *          implementation has no side effects.
 * @note    Writing on pads programmed as pull-up or pull-down has the side
 *          effect to modify the resistor setting because the output latched
 *          data is used for the resistor selection.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be ORed on the specified port
 *
 * @notapi
 */
#define pal_lld_setport(port, bits) ((port)->BSRR = (bits))

/**
 * @brief   Clears a bits mask on a I/O port.
 * @details This function is implemented by writing the GPIO BRR register, the
 *          implementation has no side effects.
 * @note    Writing on pads programmed as pull-up or pull-down has the side
 *          effect to modify the resistor setting because the output latched
 *          data is used for the resistor selection.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be cleared on the specified port
 *
 * @notapi
 */
#define pal_lld_clearport(port, bits) ((port)->BRR = (bits))



/**
 * @brief   I/O bus descriptor.
 * @details This structure describes a group of contiguous digital I/O lines
 *          that have to be handled as bus.
 * @note    I/O operations on a bus do not affect I/O lines on the same port but
 *          not belonging to the bus.
 */
typedef struct {
  /**
   * @brief Port identifier.
   */
  ioportid_t            portid;
  /**
   * @brief Bus mask aligned to port bit 0.
   * @note  The bus mask implicitly define the bus width. A logical AND is
   *        performed on the bus data.
   */
  ioportmask_t          mask;
  /**
   * @brief Offset, within the port, of the least significant bit of the bus.
   */
  uint_fast8_t          offset;
} IOBus;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Port bit helper macro.
 * @details This macro calculates the mask of a bit within a port.
 *
 * @param[in] n         bit position within the port
 * @return              The bit mask.
 */
#if !defined(PAL_PORT_BIT) || defined(__DOXYGEN__)
#define PAL_PORT_BIT(n) ((ioportmask_t)(1 << (n)))
#endif

/**
 * @brief   Bits group mask helper.
 * @details This macro calculates the mask of a bits group.
 *
 * @param[in] width         group width
 * @return                  The group mask.
 */
#if !defined(PAL_GROUP_MASK) || defined(__DOXYGEN__)
#define PAL_GROUP_MASK(width) ((ioportmask_t)(1 << (width)) - 1)
#endif

/**
 * @brief   Data part of a static I/O bus initializer.
 * @details This macro should be used when statically initializing an I/O bus
 *          that is part of a bigger structure.
 *
 * @param[in] name      name of the IOBus variable
 * @param[in] port      I/O port descriptor
 * @param[in] width     bus width in bits
 * @param[in] offset     bus bit offset within the port
 */
#define _IOBUS_DATA(name, port, width, offset)                              \
  {port, PAL_GROUP_MASK(width), offset}

/**
 * @brief   Static I/O bus initializer.
 *
 * @param[in] name      name of the IOBus variable
 * @param[in] port      I/O port descriptor
 * @param[in] width     bus width in bits
 * @param[in] offset    bus bit offset within the port
 */
#define IOBUS_DECL(name, port, width, offset)                               \
  IOBus name = _IOBUS_DATA(name, port, width, offset)

/**
 * @name    Macro Functions
 * @{
 */
/**
 * @brief   PAL subsystem initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @param[in] config pointer to an architecture specific configuration
 *            structure. This structure is defined in the low level driver
 *            header.
 *
 * @init
 */
#define palInit(config) pal_lld_init(config)

/**
 * @brief   Reads the physical I/O port states.
 * @note    The default implementation always return zero and computes the
 *          parameter eventual side effects.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @return              The port logical states.
 *
 * @special
 */
#if !defined(pal_lld_readport) || defined(__DOXYGEN__)
#define palReadPort(port) ((void)(port), 0)
#else
#define palReadPort(port) pal_lld_readport(port)
#endif

/**
 * @brief   Reads the output latch.
 * @details The purpose of this function is to read back the latched output
 *          value.
 * @note    The default implementation always return zero and computes the
 *          parameter eventual side effects.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @return              The latched logical states.
 *
 * @special
 */
#if !defined(pal_lld_readlatch) || defined(__DOXYGEN__)
#define palReadLatch(port) ((void)(port), 0)
#else
#define palReadLatch(port) pal_lld_readlatch(port)
#endif

/**
 * @brief   Writes a bits mask on a I/O port.
 * @note    The default implementation does nothing except computing the
 *          parameters eventual side effects.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be written on the specified port
 *
 * @special
 */
#if !defined(pal_lld_writeport) || defined(__DOXYGEN__)
#define palWritePort(port, bits) ((void)(port), (void)(bits))
#else
#define palWritePort(port, bits) pal_lld_writeport(port, bits)
#endif

/**
 * @brief   Sets a bits mask on a I/O port.
 * @note    The operation is not guaranteed to be atomic on all the
 *          architectures, for atomicity and/or portability reasons you may
 *          need to enclose port I/O operations between @p chSysLock() and
 *          @p chSysUnlock().
 * @note    The default implementation is non atomic and not necessarily
 *          optimal. Low level drivers may  optimize the function by using
 *          specific hardware or coding.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be ORed on the specified port
 *
 * @special
 */
#if !defined(pal_lld_setport) || defined(__DOXYGEN__)
#define palSetPort(port, bits)                                              \
  palWritePort(port, palReadLatch(port) | (bits))
#else
#define palSetPort(port, bits) pal_lld_setport(port, bits)
#endif

/**
 * @brief   Clears a bits mask on a I/O port.
 * @note    The operation is not guaranteed to be atomic on all the
 *          architectures,  for atomicity and/or portability reasons you may
 *          need to enclose port I/O operations between @p chSysLock() and
 *          @p chSysUnlock().
 * @note    The default implementation is non atomic and not necessarily
 *          optimal. Low level drivers may  optimize the function by using
 *          specific hardware or coding.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be cleared on the specified port
 *
 * @special
 */
#if !defined(pal_lld_clearport) || defined(__DOXYGEN__)
#define palClearPort(port, bits)                                            \
  palWritePort(port, palReadLatch(port) & ~(bits))
#else
#define palClearPort(port, bits) pal_lld_clearport(port, bits)
#endif

/**
 * @brief   Toggles a bits mask on a I/O port.
 * @note    The operation is not guaranteed to be atomic on all the
 *          architectures, for atomicity and/or portability reasons you may
 *          need to enclose port I/O operations between @p chSysLock() and
 *          @p chSysUnlock().
 * @note    The default implementation is non atomic and not necessarily
 *          optimal. Low level drivers may  optimize the function by using
 *          specific hardware or coding.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be XORed on the specified port
 *
 * @special
 */
#if !defined(pal_lld_toggleport) || defined(__DOXYGEN__)
#define palTogglePort(port, bits)                                           \
  palWritePort(port, palReadLatch(port) ^ (bits))
#else
#define palTogglePort(port, bits) pal_lld_toggleport(port, bits)
#endif

/**
 * @brief   Reads a group of bits.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask, a logical AND is performed on the input
 *                      data
 * @param[in] offset    group bit offset within the port
 * @return              The group logical states.
 *
 * @special
 */
#if !defined(pal_lld_readgroup) || defined(__DOXYGEN__)
#define palReadGroup(port, mask, offset)                                    \
  ((palReadPort(port) >> (offset)) & (mask))
#else
#define palReadGroup(port, mask, offset) pal_lld_readgroup(port, mask, offset)
#endif

/**
 * @brief   Writes a group of bits.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask, a logical AND is performed on the
 *                      output  data
 * @param[in] offset    group bit offset within the port
 * @param[in] bits      bits to be written. Values exceeding the group
 *                      width are masked.
 *
 * @special
 */
#if !defined(pal_lld_writegroup) || defined(__DOXYGEN__)
#define palWriteGroup(port, mask, offset, bits)                             \
  palWritePort(port, (palReadLatch(port) & ~((mask) << (offset))) |         \
                     (((bits) & (mask)) << (offset)))
#else
#define palWriteGroup(port, mask, offset, bits)                             \
  pal_lld_writegroup(port, mask, offset, bits)
#endif


/**
 * @brief   Pads group mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 * @note    Programming an unknown or unsupported mode is silently ignored.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @param[in] mode      group mode
 *
 * @special
 */
#if !defined(pal_lld_setgroupmode) || defined(__DOXYGEN__)
#define palSetGroupMode(port, mask, offset, mode)
#else
#define palSetGroupMode(port, mask, offset, mode)                           \
  pal_lld_setgroupmode(port, mask, offset, mode)
#endif

/**
 * @brief   Reads an input pad logical state.
 * @note    The default implementation not necessarily optimal. Low level
 *          drivers may  optimize the function by using specific hardware
 *          or coding.
 * @note    The default implementation internally uses the @p palReadPort().
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @return              The logical state.
 * @retval PAL_LOW      low logical state.
 * @retval PAL_HIGH     high logical state.
 *
 * @special
 */
#if !defined(pal_lld_readpad) || defined(__DOXYGEN__)
#define palReadPad(port, pad) ((palReadPort(port) >> (pad)) & 1)
#else
#define palReadPad(port, pad) pal_lld_readpad(port, pad)
#endif

/**
 * @brief   Writes a logical state on an output pad.
 * @note    The operation is not guaranteed to be atomic on all the
 *          architectures, for atomicity and/or portability reasons you may
 *          need to enclose port I/O operations between @p chSysLock() and
 *          @p chSysUnlock().
 * @note    The default implementation is non atomic and not necessarily
 *          optimal. Low level drivers may  optimize the function by using
 *          specific hardware or coding.
 * @note    The default implementation internally uses the @p palReadLatch()
 *          and @p palWritePort().
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @param[in] bit       logical value, the value must be @p PAL_LOW or
 *                      @p PAL_HIGH
 *
 * @special
 */
#if !defined(pal_lld_writepad) || defined(__DOXYGEN__)
#define palWritePad(port, pad, bit)                                         \
  palWritePort(port, (palReadLatch(port) & ~PAL_PORT_BIT(pad)) |            \
                     (((bit) & 1) << pad))
#else
#define palWritePad(port, pad, bit) pal_lld_writepad(port, pad, bit)
#endif

/**
 * @brief   Sets a pad logical state to @p PAL_HIGH.
 * @note    The operation is not guaranteed to be atomic on all the
 *          architectures, for atomicity and/or portability reasons you may
 *          need to enclose port I/O operations between @p chSysLock() and
 *          @p chSysUnlock().
 * @note    The default implementation is non atomic and not necessarily
 *          optimal. Low level drivers may  optimize the function by using
 *          specific hardware or coding.
 * @note    The default implementation internally uses the @p palSetPort().
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @special
 */
#if !defined(pal_lld_setpad) || defined(__DOXYGEN__)
#define palSetPad(port, pad) palSetPort(port, PAL_PORT_BIT(pad))
#else
#define palSetPad(port, pad) pal_lld_setpad(port, pad)
#endif

/**
 * @brief   Clears a pad logical state to @p PAL_LOW.
 * @note    The operation is not guaranteed to be atomic on all the
 *          architectures, for atomicity and/or portability reasons you may
 *          need to enclose port I/O operations between @p chSysLock() and
 *          @p chSysUnlock().
 * @note    The default implementation is non atomic and not necessarily
 *          optimal. Low level drivers may  optimize the function by using
 *          specific hardware or coding.
 * @note    The default implementation internally uses the @p palClearPort().
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @special
 */
#if !defined(pal_lld_clearpad) || defined(__DOXYGEN__)
#define palClearPad(port, pad) palClearPort(port, PAL_PORT_BIT(pad))
#else
#define palClearPad(port, pad) pal_lld_clearpad(port, pad)
#endif

/**
 * @brief   Toggles a pad logical state.
 * @note    The operation is not guaranteed to be atomic on all the
 *          architectures, for atomicity and/or portability reasons you may
 *          need to enclose port I/O operations between @p chSysLock() and
 *          @p chSysUnlock().
 * @note    The default implementation is non atomic and not necessarily
 *          optimal. Low level drivers may  optimize the function by using
 *          specific hardware or coding.
 * @note    The default implementation internally uses the @p palTogglePort().
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @special
 */
#if !defined(pal_lld_togglepad) || defined(__DOXYGEN__)
#define palTogglePad(port, pad) palTogglePort(port, PAL_PORT_BIT(pad))
#else
#define palTogglePad(port, pad) pal_lld_togglepad(port, pad)
#endif

/**
 * @brief   Pad mode setup.
 * @details This function programs a pad with the specified mode.
 * @note    The default implementation not necessarily optimal. Low level
 *          drivers may  optimize the function by using specific hardware
 *          or coding.
 * @note    Programming an unknown or unsupported mode is silently ignored.
 * @note    The function can be called from any context.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @param[in] mode      pad mode
 *
 * @special
 */
#if !defined(pal_lld_setpadmode) || defined(__DOXYGEN__)
#define palSetPadMode(port, pad, mode)                                      \
  palSetGroupMode(port, PAL_PORT_BIT(pad), 0, mode)
#else
#define palSetPadMode(port, pad, mode) pal_lld_setpadmode(port, pad, mode)
#endif
/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  ioportmask_t palReadBus(IOBus *bus);
  void palWriteBus(IOBus *bus, ioportmask_t bits);
  void palSetBusMode(IOBus *bus, iomode_t mode);
#ifdef __cplusplus
}
#endif

#endif /* _PAL_H_ */
