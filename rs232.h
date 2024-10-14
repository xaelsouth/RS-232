/*
***************************************************************************
*
* Author: Teunis van Beelen, Xael South
*
* Copyright (C) 2005 - 2023 Teunis van Beelen
* Copyright (C) 2024 - 2024 Xael South
*
* Email: teuniz@protonmail.com
*        xael.south@yandex.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/

/* 
 * For more info and how to use this library, visit: https://github.com/xaelsouth/RS-232
 *                                                   https://www.teuniz.net/RS-232
 */

#ifndef RS232_H_INCLUDED
#define RS232_H_INCLUDED

#include <stddef.h>
#include "rs232_platform.h"

#ifndef WITH_RS232_LOCK
#define WITH_RS232_LOCK  0
#endif

/** Hardware flow control is enabled using the RTS/CTS lines. */
#define RS232_FLAGS_HWFLOWCTRL  (1 << 0)


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens the serial interface.
 *
 * @param[in] devname Serial interface device like /dev/ttyUSB0 on Linux or COM1 on Windows.
 *            Use \\.\COM10 on Windows for all interfaces number abobe COM9.
 * 
 * @param[in] baudrate expressed in baud per second i.e 115200
 * 
 * @param[in] mode is a string in the form of "8N1", "7O2", "8E1", etc.
 * 
 * @param[in] flags can be combined using the bit-wise OR operator.
 *            At the moment only RS232_FLAGS_HWFLOWCTRL flag is supported.
 * 
 * @return File descriptor or RS232_INVALID_FD if something went wrong while opening interface.
 */
RS232_ADDAPI RS232_FD RS232_ADDCALL RS232_Open(const char *devname, int baudrate, const char *mode, int flags);

/**
 * @brief Closes the serial interface.
 * 
 * @param[in] fd file descriptor.
 * 
 * @return 
 */
RS232_ADDAPI int RS232_ADDCALL RS232_Close(RS232_FD);

/**
 * @brief Reads from serial interface up to size bytes and stores them in buf.
 * 
 * @param[in] fd file descriptor.
 * 
 * @param[out] buf is a buffer where data read from serial interface will be stored.
 * 
 * @param[in] size is the buffer size.
 * 
 * @param[in] timeout_msec is the timeout in milliseconds. 0: non-blocking read, INT_MAX: blocking read.
 * 
 * @return Amount of bytes received (and stored): >= 0 if could read successfully or -1 if an error occured.
 */
RS232_ADDAPI ssize_t RS232_ADDCALL RS232_Read(RS232_FD fd, void *buf, size_t size, int flags, int timeout_msec);

/**
 * @brief Writes to serial interface up to size bytes stored in buf.
 * 
 * @param[in] fd file descriptor.
 * 
 * @param[in] buf is a buffer with data to send via the serial interface.
 * 
 * @param[in] size is the amount of data to send.
 * 
 * @param[in] timeout_msec is the timeout in milliseconds. 0: non-blocking write, INT_MAX: blocking write.
 * 
 * @return Amount of bytes sent: >=0 if could write successfully or -1 if an error occured.
 */
RS232_ADDAPI ssize_t RS232_ADDCALL RS232_Write(RS232_FD fd, const void *buf, size_t size, int flags, int timeout_msec);

/**.
 * @brief Checks the status of the DCD-pin.
 *
 * @param[in] fd file descriptor.
 *
 * @return 1 if the the CTS line is high (active state), 0 if is low (inactive state), -1 on error.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_IsDCDEnabled(RS232_FD fd);

/**.
 * @brief Checks the status of the RING-pin.
 *
 * @param[in] fd file descriptor.
 *
 * @return 1 if the the CTS line is high (active state), 0 if is low (inactive state), -1 on error.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_IsRINGEnabled(RS232_FD fd);

/**.
 * @brief Checks the status of the CTS-pin.
 *
 * @param[in] fd file descriptor.
 *
 * @return 1 if the the CTS line is high (active state), 0 if is low (inactive state), -1 on error.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_IsCTSEnabled(RS232_FD fd);

/**.
 * @brief Checks the status of the DSR-pin.
 *
 * @param[in] fd file descriptor.
 *
 * @return 1 if the the CTS line is high (active state), 0 if is low (inactive state), -1 on error.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_IsDSREnabled(RS232_FD fd);

/**.
 * @brief Sets the DTR line high (active state).
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_enableDTR(RS232_FD fd);

/**.
 * @brief Sets the DTR line low (inactive state).
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_disableDTR(RS232_FD fd);

/**.
 * @brief Sets the RTS line high (active state).
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_enableRTS(RS232_FD fd);

/**.
 * @brief Sets the RTS line low (inactive state).
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_disableRTS(RS232_FD fd);

/**.
 * @brief Enable sending break characters (0x00).
 * @note  On Linux sends only one break character.
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_enableBREAK(RS232_FD fd);

/**.
 * @brief Disables sending of break characters.
 * @note  On Linux RS232_disableBREAK should "complete" every RS232_enableBREAK.
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_disableBREAK(RS232_FD fd);

/**.
 * @brief Flushes data received but not read.
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_flushRX(RS232_FD fd);

/**.
 * @brief Flushes data written but not transmitted.
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_flushTX(RS232_FD fd);

/**.
 * @brief Flushes both data received but not read, and data written but not transmitted.
 *
 * @param[in] fd file descriptor.
 *
 * @return 0 on success or -1 otherwise.
 */
RS232_ADDAPI int RS232_ADDCALL RS232_flushRXTX(RS232_FD fd);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RS232_H_INCLUDED */
