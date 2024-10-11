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

/* For more info and how to use this library, visit: http://www.teuniz.net/RS-232/ */


#ifndef rs232_INCLUDED
#define rs232_INCLUDED

#include <stddef.h>

#if defined(__linux__) || defined(__FreeBSD__)
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>

#define ADDAPI
#define ADDCALL

typedef int RS232_FD;

#define RS232_INVALID_FD    -1

#else
#include <windows.h>

/* You should define ADD_EXPORTS *only* when building the DLL. */
#ifdef ADD_EXPORTS
  #define ADDAPI __declspec(dllexport)
#else
  #define ADDAPI __declspec(dllimport)
#endif

/* Define calling convention in one place, for convenience. */
#define ADDCALL __cdecl

typedef HANDLE RS232_FD;

#define RS232_INVALID_FD    INVALID_HANDLE_VALUE

#ifndef _SSIZE_T_DEFINED
#ifdef  _WIN64
typedef __int64 ssize_t;
#else
typedef _W64 int ssize_t;
#endif /* defined(_WIN64) */
#define _SSIZE_T_DEFINED
#endif /* !_SSIZE_T_DEFINED */

#endif

#ifdef __cplusplus
extern "C" {
#endif

ADDAPI RS232_FD ADDCALL RS232_Open(const char*, int, const char*, int);
ADDAPI int ADDCALL RS232_Close(RS232_FD);
ADDAPI ssize_t ADDCALL RS232_Read(RS232_FD fd, void *buf, size_t size, int flags, int timeout_msec);
ADDAPI ssize_t ADDCALL RS232_Write(RS232_FD fd, const void *buf, size_t size, int flags, int timeout_msec);
ADDAPI int ADDCALL RS232_IsDCDEnabled(RS232_FD);
ADDAPI int ADDCALL RS232_IsRINGEnabled(RS232_FD);
ADDAPI int ADDCALL RS232_IsCTSEnabled(RS232_FD);
ADDAPI int ADDCALL RS232_IsDSREnabled(RS232_FD);
ADDAPI int ADDCALL RS232_enableDTR(RS232_FD);
ADDAPI int ADDCALL RS232_disableDTR(RS232_FD);
ADDAPI int ADDCALL RS232_enableRTS(RS232_FD);
ADDAPI int ADDCALL RS232_disableRTS(RS232_FD);
ADDAPI int ADDCALL RS232_enableBREAK(RS232_FD);
ADDAPI int ADDCALL RS232_disableBREAK(RS232_FD);
ADDAPI int ADDCALL RS232_flushRX(RS232_FD);
ADDAPI int ADDCALL RS232_flushTX(RS232_FD);
ADDAPI int ADDCALL RS232_flushRXTX(RS232_FD);
ADDAPI int ADDCALL RS232_enableHwFlowControl(RS232_FD);
ADDAPI int ADDCALL RS232_disableHwFlowControl(RS232_FD);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
