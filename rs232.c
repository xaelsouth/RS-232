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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "rs232.h"

#define RS232_PERROR(...)
#define RS232_FPRINTF(fd, ...)
#define RS232_FPRINTF_DEBUG(fd, ...)

#if !defined(RS232_ADD_EXPORTS)
#undef RS232_PERROR
#define RS232_PERROR(...)             perror(__VA_ARGS__)

#undef  RS232_FPRINTF
#define RS232_FPRINTF(fd, ...)        fprintf(fd, __VA_ARGS__)

#if defined(DEBUG)
#undef  RS232_FPRINTF_DEBUG
#define RS232_FPRINTF_DEBUG(fd, ...)  fprintf(fd, __VA_ARGS__)
#endif
#endif

#if WINDOWS_BUILD == 0

RS232_FD _RS232_Open(const char *devname, int baudrate, const char *mode, int flags)
{

  int cbits = CS8, cpar = 0, ipar = IGNPAR, bstop = 0;
  int fd, err, status;

  if (devname == NULL)
  {
    RS232_FPRINTF(stderr, "Illegal device.\n");
    return RS232_INVALID_FD;
  }

  if (mode == NULL)
  {
    RS232_FPRINTF(stderr, "Invalid mode.\n");
    return RS232_INVALID_FD;
  }

  if (strlen(mode) != 3)
  {
    RS232_FPRINTF(stderr, "Invalid mode '%s'.\n", mode);
    return RS232_INVALID_FD;
  }

  switch (baudrate)
  {
    case      50 :
      baudrate = B50;
      break;
    case      75 :
      baudrate = B75;
      break;
    case     110 :
      baudrate = B110;
      break;
    case     134 :
      baudrate = B134;
      break;
    case     150 :
      baudrate = B150;
      break;
    case     200 :
      baudrate = B200;
      break;
    case     300 :
      baudrate = B300;
      break;
    case     600 :
      baudrate = B600;
      break;
    case    1200 :
      baudrate = B1200;
      break;
    case    1800 :
      baudrate = B1800;
      break;
    case    2400 :
      baudrate = B2400;
      break;
    case    4800 :
      baudrate = B4800;
      break;
    case    9600 :
      baudrate = B9600;
      break;
    case   19200 :
      baudrate = B19200;
      break;
    case   38400 :
      baudrate = B38400;
      break;
    case   57600 :
      baudrate = B57600;
      break;
    case  115200 :
      baudrate = B115200;
      break;
    case  230400 :
      baudrate = B230400;
      break;
    case  460800 :
      baudrate = B460800;
      break;
#if defined(__linux__)
    case  500000 :
      baudrate = B500000;
      break;
    case  576000 :
      baudrate = B576000;
      break;
    case  921600 :
      baudrate = B921600;
      break;
    case 1000000 :
      baudrate = B1000000;
      break;
    case 1152000 :
      baudrate = B1152000;
      break;
    case 1500000 :
      baudrate = B1500000;
      break;
    case 2000000 :
      baudrate = B2000000;
      break;
    case 2500000 :
      baudrate = B2500000;
      break;
    case 3000000 :
      baudrate = B3000000;
      break;
    case 3500000 :
      baudrate = B3500000;
      break;
    case 4000000 :
      baudrate = B4000000;
      break;
#endif
    default      :
      RS232_FPRINTF(stderr, "Invalid baudrate %d.\n", baudrate);
      return RS232_INVALID_FD;
      break;
  }

  switch (mode[0])
  {
    case '8':
      cbits = CS8;
      break;
    case '7':
      cbits = CS7;
      break;
    case '6':
      cbits = CS6;
      break;
    case '5':
      cbits = CS5;
      break;
    default :
      RS232_FPRINTF(stderr, "Invalid number of data-bits '%c'.\n", mode[0]);
      return RS232_INVALID_FD;
      break;
  }

  switch (mode[1])
  {
    case 'N':
      /* FALLTHRU */
    case 'n':
      cpar = 0;
      ipar = IGNPAR;
      break;
    case 'E':
      /* FALLTHRU */
    case 'e':
      cpar = PARENB;
      ipar = INPCK;
      break;
    case 'O':
      /* FALLTHRU */
    case 'o':
      cpar = (PARENB | PARODD);
      ipar = INPCK;
      break;
    default :
      RS232_FPRINTF(stderr, "Invalid parity '%c'.\n", mode[1]);
      return RS232_INVALID_FD;
      break;
  }

  switch (mode[2])
  {
    case '1':
      bstop = 0;
      break;
    case '2':
      bstop = CSTOPB;
      break;
    default :
      RS232_FPRINTF(stderr, "Invalid number of stop bits '%c'.\n", mode[2]);
      return RS232_INVALID_FD;
      break;
  }

  /*
   * https://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html
   * https://man7.org/linux/man-pages/man3/termios.3.html
   */

  fd = open(devname, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == RS232_INVALID_FD)
  {
    RS232_PERROR("Unable to open comport ");
    return RS232_INVALID_FD;
  }

  #if WITH_RS232_LOCK
  /* lock access so that another process can't also use the port */
  if (flock(fd, LOCK_EX | LOCK_NB) != 0)
  {
    close(fd);
    RS232_PERROR("Another process has locked the comport.");
    return RS232_INVALID_FD;
  }
  #endif

  struct termios old_port_settings;
  err = tcgetattr(fd, &old_port_settings);
  if (err == -1)
  {
    close(fd);
    RS232_PERROR("Unable to read portsettings ");
    return RS232_INVALID_FD;
  }

  struct termios new_port_settings = old_port_settings;
  new_port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
  if ((flags & RS232_FLAGS_HWFLOWCTRL) == RS232_FLAGS_HWFLOWCTRL)
  {
    new_port_settings.c_cflag |= CRTSCTS;
  }
  new_port_settings.c_iflag = ipar;
  new_port_settings.c_oflag = 0;
  new_port_settings.c_lflag = 0;
  new_port_settings.c_cc[VMIN] = 0;      /* block untill n bytes are received */
  new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

  cfsetispeed(&new_port_settings, baudrate);
  cfsetospeed(&new_port_settings, baudrate);

  bool debian_bug_218131;

  err = tcsetattr(fd, TCSANOW, &new_port_settings);
  debian_bug_218131 = (err == -1);
  if (debian_bug_218131) return fd;
  if (err == -1)
  {
    tcsetattr(fd, TCSANOW, &old_port_settings);
    close(fd);
    RS232_PERROR("Unable to adjust portsettings ");
    return RS232_INVALID_FD;
  }

  /* https://man7.org/linux/man-pages/man4/tty_ioctl.4.html */

  err = ioctl(fd, TIOCMGET, &status);
  debian_bug_218131 = (err == -1);
  if (debian_bug_218131) return fd;
  if (err == -1)
  {
    tcsetattr(fd, TCSANOW, &old_port_settings);
    close(fd);
    RS232_PERROR("Unable to get portstatus ");
    return RS232_INVALID_FD;
  }

  status &= ~TIOCM_DTR;    /* Turn off DTR. */

  if ((flags & RS232_FLAGS_HWFLOWCTRL) == 0)
  {
    status |= TIOCM_RTS;     /* Turn on RTS as no HW flow control enabled. */
  }

  err = ioctl(fd, TIOCMSET, &status);
  debian_bug_218131 = (err == -1);
  if (debian_bug_218131) return fd;
  if (err == -1)
  {
    tcsetattr(fd, TCSANOW, &old_port_settings);
    close(fd);
    RS232_PERROR("Unable to set portstatus ");
    return RS232_INVALID_FD;
  }

  return fd;
}

int RS232_Close(RS232_FD fd)
{

  int status, err;
  bool debian_bug_218131;

  err = ioctl(fd, TIOCMGET, &status);
  debian_bug_218131 = (err == -1);
  if (debian_bug_218131) return close(fd);
  if (err == -1)
  {
    RS232_PERROR("Unable to get portstatus");
    return -1;
  }

  status &= ~TIOCM_DTR;    /* turn off DTR */
  status &= ~TIOCM_RTS;    /* turn off RTS */

  err = ioctl(fd, TIOCMSET, &status);
  debian_bug_218131 = (err == -1);
  if (debian_bug_218131) return close(fd);
  if (err == -1)
  {
    RS232_PERROR("Unable to set portstatus");
    return -1;
  }

  return close(fd);
}

static ssize_t _RS232_Read(RS232_FD fd, void *buf, size_t size, int flags, int timeout_msec)
{

  ssize_t read_bytes = -1;
  int fdcount;
  fd_set readfds;
  struct timeval timeout;
  (void)flags;

  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);

  timeout.tv_sec = timeout_msec / 1000;
  timeout.tv_usec = (timeout_msec % 1000) * 1000;

  fdcount = select(fd + 1, &readfds, NULL, NULL, &timeout);

  if (fdcount == -1)
  {
    RS232_FPRINTF(stderr, "Error in select: %d.\n", errno);
  }
  else if (fdcount == 0)
  {
    RS232_FPRINTF_DEBUG(stderr, "No data received within %d milliseconds.\n", timeout_msec);
    read_bytes = 0;
  }
  else
  {
    if (FD_ISSET(fd, &readfds))
    {
      read_bytes = read(fd, buf, size);
      if (read_bytes < 0)
      {
        RS232_FPRINTF_DEBUG(stderr, "Can't read data.\n");
      }
    }
  }

  return read_bytes;
}

static ssize_t _RS232_Write(RS232_FD fd, const void *buf, size_t size, int flags, int timeout_msec)
{

  ssize_t written_bytes = -1;
  int fdcount;
  fd_set writefds;
  struct timeval timeout;
  (void)flags;

  FD_ZERO(&writefds);
  FD_SET(fd, &writefds);

  timeout.tv_sec = timeout_msec / 1000;
  timeout.tv_usec = (timeout_msec % 1000) * 1000;

  fdcount = select(fd + 1, NULL, &writefds, NULL, &timeout);

  if (fdcount == -1)
  {
    RS232_FPRINTF(stderr, "Error in select: %d.\n", errno);
  }
  else if (fdcount == 0)
  {
    RS232_FPRINTF(stderr, "No data sent within %d milliseconds.\n", timeout_msec);
  }
  else
  {
    if (FD_ISSET(fd, &writefds))
    {
      written_bytes = write(fd, buf, size);
      if (written_bytes < 0)
      {
        RS232_FPRINTF_DEBUG(stderr, "Can't write data.\n");
      }
    }
  }

  return written_bytes;
}

/*
Constant  Description
TIOCM_LE        DSR (data set ready/line enable)
TIOCM_DTR       DTR (data terminal ready)
TIOCM_RTS       RTS (request to send)
TIOCM_ST        Secondary TXD (transmit)
TIOCM_SR        Secondary RXD (receive)
TIOCM_CTS       CTS (clear to send)
TIOCM_CAR       DCD (data carrier detect)
TIOCM_CD        see TIOCM_CAR
TIOCM_RNG       RNG (ring)
TIOCM_RI        see TIOCM_RNG
TIOCM_DSR       DSR (data set ready)

https://man7.org/linux/man-pages/man4/tty_ioctl.4.html
*/

int RS232_IsDCDEnabled(RS232_FD fd)
{

  int status;

  if (ioctl(fd, TIOCMGET, &status) == -1) return -1;

  return (status & TIOCM_CAR) ? 1 : 0;
}

int RS232_IsRINGEnabled(RS232_FD fd)
{

  int status;

  if (ioctl(fd, TIOCMGET, &status) == -1) return -1;

  return (status & TIOCM_RNG) ? 1 : 0;
}

int RS232_IsCTSEnabled(RS232_FD fd)
{

  int status;

  if (ioctl(fd, TIOCMGET, &status) == -1) return -1;

  return (status & TIOCM_CTS) ? 1 : 0;
}

int RS232_IsDSREnabled(RS232_FD fd)
{

  int status;

  if (ioctl(fd, TIOCMGET, &status) == -1) return -1;

  return (status & TIOCM_DSR) ? 1 : 0;
}

int RS232_enableDTR(RS232_FD fd)
{

  int status;

  if (ioctl(fd, TIOCMGET, &status) == -1) return -1;

  status |= TIOCM_DTR;    /* turn on DTR */

  if (ioctl(fd, TIOCMSET, &status) == -1) return -1;

  return 0;
}


int RS232_disableDTR(RS232_FD fd)
{

  int status;

  if (ioctl(fd, TIOCMGET, &status) == -1) return -1;

  status &= ~TIOCM_DTR;    /* turn off DTR */

  if (ioctl(fd, TIOCMSET, &status) == -1) return -1;

  return 0;
}

int RS232_enableRTS(RS232_FD fd)
{

  int status;

  if (ioctl(fd, TIOCMGET, &status) == -1) return -1;

  status |= TIOCM_RTS;    /* turn on RTS */

  if (ioctl(fd, TIOCMSET, &status) == -1) return -1;

  return 0;
}

int RS232_disableRTS(RS232_FD fd)
{

  int status;

  if (ioctl(fd, TIOCMGET, &status) == -1) return -1;

  status &= ~TIOCM_RTS;    /* turn off RTS */

  if (ioctl(fd, TIOCMSET, &status) == -1) return -1;

  return 0;
}

int RS232_enableBREAK(RS232_FD fd)
{

  if (tcsendbreak(fd, 0) == -1)  /* Turn break on, that is, start sending zero bits. */
  {
    RS232_FPRINTF(stderr, "Unable to turn break on.\n");
    return -1;
  }

  return 0;
}

int RS232_disableBREAK(RS232_FD fd)
{

  if (ioctl(fd, TIOCCBRK, NULL) == -1)  /* Turn break off, that is, stop sending zero bits. */
  {
    RS232_FPRINTF(stderr, "Unable to turn break off.\n");
    return -1;
  }

  return 0;
}

int RS232_flushRX(RS232_FD fd)
{

  return tcflush(fd, TCIFLUSH);
}

int RS232_flushTX(RS232_FD fd)
{

  return tcflush(fd, TCOFLUSH);
}

int RS232_flushRXTX(RS232_FD fd)
{

  return tcflush(fd, TCIOFLUSH);
}

#else  /* Windows */

RS232_FD _RS232_Open(const char *devname, int baudrate, const char *mode, int flags)
{

  if (devname == NULL)
  {
    RS232_FPRINTF(stderr, "Illegal device.\n");
    return RS232_INVALID_FD;
  }

  if (mode == NULL)
  {
    RS232_FPRINTF(stderr, "Invalid mode.\n");
    return RS232_INVALID_FD;
  }

  if (strlen(mode) != 3)
  {
    RS232_FPRINTF(stderr, "Invalid mode '%s'.\n", mode);
    return RS232_INVALID_FD;
  }

  /*
   * https://msdn.microsoft.com/en-us/library/windows/desktop/aa363145%28v=vs.85%29.aspx
   * https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-buildcommdcbandtimeoutsa
   *
   * https://technet.microsoft.com/en-us/library/cc732236.aspx
   * https://docs.microsoft.com/en-us/previous-versions/windows/it-pro/windows-server-2012-R2-and-2012/cc732236(v=ws.11)
   *
   * https://docs.microsoft.com/en-us/windows/desktop/api/winbase/ns-winbase-_dcb
   * https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-buildcommdcba
   */

  RS232_FD fd = CreateFileA(devname,
                            GENERIC_READ | GENERIC_WRITE,
                            #if WITH_RS232_LOCK
                            0,                          /* No share: access locked. */
                            #else
                            FILE_SHARE_READ | FILE_SHARE_WRITE, /* Share for read and write. */
                            #endif
                            NULL,                       /* No security. */
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                            NULL);                      /* No templates. */

  if (fd == RS232_INVALID_FD)
  {
    RS232_FPRINTF(stderr, "Unable to open comport %s.\n", devname);
    return RS232_INVALID_FD;
  }

  DCB port_settings;

  if (!GetCommState(fd, &port_settings))
  {
    RS232_FPRINTF(stderr, "Unable to get comport settings.\n");
    CloseHandle(fd);
    return RS232_INVALID_FD;
  }

  switch (baudrate)
  {
    case     110 :
      port_settings.BaudRate = CBR_110;
      break;
    case     300 :
      port_settings.BaudRate = CBR_300;
      break;
    case     600 :
      port_settings.BaudRate = CBR_600;
      break;
    case    1200 :
      port_settings.BaudRate = CBR_1200;
      break;
    case    2400 :
      port_settings.BaudRate = CBR_2400;
      break;
    case    4800 :
      port_settings.BaudRate = CBR_4800;
      break;
    case    9600 :
      port_settings.BaudRate = CBR_9600;
      break;
    case   19200 :
      port_settings.BaudRate = CBR_19200;
      break;
    case   38400 :
      port_settings.BaudRate = CBR_38400;
      break;
    case   57600 :
      port_settings.BaudRate = CBR_57600;
      break;
    case  115200 :
      port_settings.BaudRate = CBR_115200;
      break;
    case  128000 :
      port_settings.BaudRate = CBR_128000;
      break;
    case  256000 :
      port_settings.BaudRate = CBR_256000;
      break;
    default      :
      RS232_FPRINTF(stderr, "Invalid baudrate.\n");
      CloseHandle(fd);
      return RS232_INVALID_FD;
      break;
  }

  switch (mode[0])
  {
    case '8':
      port_settings.ByteSize = 8;
      break;
    case '7':
      port_settings.ByteSize = 7;
      break;
    case '6':
      port_settings.ByteSize = 6;
      break;
    case '5':
      port_settings.ByteSize = 5;
      break;
    default :
      RS232_FPRINTF(stderr, "Invalid number of data-bits '%c'.\n", mode[0]);
      CloseHandle(fd);
      return RS232_INVALID_FD;
      break;
  }

  switch (mode[1])
  {
    case 'N':
      /* FALLTHRU */
    case 'n':
      port_settings.Parity = NOPARITY;
      break;
    case 'E':
      /* FALLTHRU */
    case 'e':
      port_settings.Parity = EVENPARITY;
      break;
    case 'O':
      /* FALLTHRU */
    case 'o':
      port_settings.Parity = ODDPARITY;
      break;
    default :
      RS232_FPRINTF(stderr, "Invalid parity '%c'.\n", mode[1]);
      CloseHandle(fd);
      return RS232_INVALID_FD;
      break;
  }

  switch (mode[2])
  {
    case '1':
      port_settings.StopBits = ONESTOPBIT;
      break;
    case '2':
      port_settings.StopBits = TWOSTOPBITS;
      break;
    default :
      RS232_FPRINTF(stderr, "Invalid number of stop bits '%c'.\n", mode[2]);
      CloseHandle(fd);
      return RS232_INVALID_FD;
      break;
  }

  if ((flags & RS232_FLAGS_HWFLOWCTRL) == RS232_FLAGS_HWFLOWCTRL)
  {
    port_settings.fOutxCtsFlow = TRUE;
    port_settings.fRtsControl = RTS_CONTROL_HANDSHAKE;
  }
  else
  {
    port_settings.fOutxCtsFlow = FALSE;
    port_settings.fRtsControl = RTS_CONTROL_ENABLE;
  }

  port_settings.fOutxDsrFlow = FALSE;
  port_settings.fDsrSensitivity = FALSE;
  port_settings.fDtrControl = DTR_CONTROL_DISABLE;

  if (!SetCommState(fd, &port_settings))
  {
    RS232_FPRINTF(stderr, "Unable to set comport settings.\n");
    CloseHandle(fd);
    return RS232_INVALID_FD;
  }

  COMMTIMEOUTS Cptimeouts;

  Cptimeouts.ReadIntervalTimeout = MAXDWORD;
  Cptimeouts.ReadTotalTimeoutMultiplier = 0;
  Cptimeouts.ReadTotalTimeoutConstant = 0;
  Cptimeouts.WriteTotalTimeoutMultiplier = 0;
  Cptimeouts.WriteTotalTimeoutConstant = 0;

  if (!SetCommTimeouts(fd, &Cptimeouts))
  {
    RS232_FPRINTF(stderr, "Unable to set comport timeouts.\n");
    CloseHandle(fd);
    return RS232_INVALID_FD;
  }

  if (!SetCommMask(fd, EV_ERR))
  {
    RS232_FPRINTF(stderr, "Unable to clear event mask.\n");
    CloseHandle(fd);
    return RS232_INVALID_FD;
  }

  return fd;
}

static ssize_t _RS232_Read(RS232_FD fd, void *buf, size_t size, int flags, int timeout_msec)
{

  ssize_t read_bytes;
  DWORD dwRead, lastError;
  COMMTIMEOUTS Cptimeouts;
  OVERLAPPED ov = { 0 };
  (void)flags;

  if (!GetCommTimeouts(fd, &Cptimeouts)) return (ssize_t)-1;

  Cptimeouts.ReadIntervalTimeout = MAXDWORD;
  Cptimeouts.ReadTotalTimeoutMultiplier = 0;
  Cptimeouts.ReadTotalTimeoutConstant = timeout_msec;
  if (!SetCommTimeouts(fd, &Cptimeouts)) return (ssize_t)-1;

  ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (!ov.hEvent) return (ssize_t)-1;

  if (ReadFile(fd, buf, (DWORD)size, &dwRead, &ov))
  {
    /* ReadFile completed immediately. */
    read_bytes = (ssize_t)dwRead;
  }
  else
  {
    lastError = GetLastError();
    if (lastError == ERROR_IO_PENDING || lastError == ERROR_SUCCESS)
      read_bytes = (GetOverlappedResult(fd, &ov, &dwRead, TRUE)) ? (ssize_t)dwRead : (ssize_t)-1;
    else
      read_bytes = (ssize_t)-1;
  }

  CloseHandle(ov.hEvent);

  return read_bytes;
}

static ssize_t _RS232_Write(RS232_FD fd, const void *buf, size_t size, int flags, int timeout_msec)
{

  ssize_t written_bytes;
  DWORD dwWritten, lastError;
  COMMTIMEOUTS Cptimeouts;
  OVERLAPPED ov = { 0 };
  (void)flags;

  if (!GetCommTimeouts(fd, &Cptimeouts)) return (ssize_t)-1;

  Cptimeouts.WriteTotalTimeoutMultiplier = 0;
  Cptimeouts.WriteTotalTimeoutConstant = timeout_msec;
  if (!SetCommTimeouts(fd, &Cptimeouts)) return (ssize_t)-1;

  ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (!ov.hEvent) return (ssize_t)-1;

  if (WriteFile(fd, buf, (DWORD)size, &dwWritten, &ov))
  {
    /* WriteFile completed immediately. */
    written_bytes = dwWritten;
  }
  else
  {
    lastError = GetLastError();
    if (lastError == ERROR_IO_PENDING || lastError == ERROR_SUCCESS)
      written_bytes = GetOverlappedResult(fd, &ov, &dwWritten, TRUE) ? (ssize_t)dwWritten : (ssize_t)-1;
    else
      written_bytes = (ssize_t)-1;
  }

  CloseHandle(ov.hEvent);

  return written_bytes;
}

RS232_ADDAPI int RS232_ADDCALL RS232_Close(RS232_FD fd)
{

  return CloseHandle(fd) ? 0 : -1;
}

/*
 * https://msdn.microsoft.com/en-us/library/windows/desktop/aa363258%28v=vs.85%29.aspx
 * https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcommmodemstatus
 */

RS232_ADDAPI int RS232_ADDCALL RS232_IsDCDEnabled(RS232_FD fd)
{

  DWORD status;

  if (!GetCommModemStatus(fd, &status)) return -1;

  return (status & MS_RLSD_ON) ? 1 : 0;
}


RS232_ADDAPI int RS232_ADDCALL RS232_IsRINGEnabled(RS232_FD fd)
{

  DWORD status;

  if (!GetCommModemStatus(fd, &status)) return -1;

  return (status & MS_RING_ON) ? 1 : 0;
}

RS232_ADDAPI int RS232_ADDCALL RS232_IsCTSEnabled(RS232_FD fd)
{

  DWORD status;

  if (!GetCommModemStatus(fd, &status)) return -1;

  return (status & MS_CTS_ON) ? 1 : 0;
}

RS232_ADDAPI int RS232_ADDCALL RS232_IsDSREnabled(RS232_FD fd)
{

  DWORD status;

  if (!GetCommModemStatus(fd, &status)) return -1;

  return (status & MS_DSR_ON) ? 1 : 0;
}

RS232_ADDAPI int RS232_ADDCALL RS232_enableDTR(RS232_FD fd)
{

  return EscapeCommFunction(fd, SETDTR) ? 0 : -1;
}

RS232_ADDAPI int RS232_ADDCALL RS232_disableDTR(RS232_FD fd)
{

  return EscapeCommFunction(fd, CLRDTR) ? 0 : -1;
}

RS232_ADDAPI int RS232_ADDCALL RS232_enableRTS(RS232_FD fd)
{

  return EscapeCommFunction(fd, SETRTS) ? 0 : -1;
}


RS232_ADDAPI int RS232_ADDCALL RS232_disableRTS(RS232_FD fd)
{

  return EscapeCommFunction(fd, CLRRTS) ? 0 : -1;
}

/*
 * https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setcommbreak
 */

RS232_ADDAPI int RS232_ADDCALL RS232_enableBREAK(RS232_FD fd)
{

  if (!SetCommBreak(fd))  /* Turn break on, that is, start sending zero bits. */
  {
    RS232_FPRINTF(stderr, "Unable to turn break on.\n");
    return -1;
  }

  return 0;
}

/*
 * https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-clearcommbreak
 */

RS232_ADDAPI int RS232_ADDCALL RS232_disableBREAK(RS232_FD fd)
{

  if (!ClearCommBreak(fd))  /* Turn break off, that is, stop sending zero bits. */
  {
    RS232_FPRINTF(stderr, "Unable to turn break off.\n");
    return -1;
  }

  return 0;
}

/*
 * https://msdn.microsoft.com/en-us/library/windows/desktop/aa363428%28v=vs.85%29.aspx
 * https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-purgecomm
 */

RS232_ADDAPI int RS232_ADDCALL RS232_flushRX(RS232_FD fd)
{

  return PurgeComm(fd, PURGE_RXCLEAR | PURGE_RXABORT) ? 0 : -1;
}


RS232_ADDAPI int RS232_ADDCALL RS232_flushTX(RS232_FD fd)
{

  return PurgeComm(fd, PURGE_TXCLEAR | PURGE_TXABORT) ? 0 : -1;
}


RS232_ADDAPI int RS232_ADDCALL RS232_flushRXTX(RS232_FD fd)
{

  return (PurgeComm(fd, PURGE_RXCLEAR | PURGE_RXABORT) &&
          PurgeComm(fd, PURGE_TXCLEAR | PURGE_TXABORT))
         ? 0 : -1;
}

#endif

RS232_ADDAPI ssize_t RS232_ADDCALL RS232_Read(RS232_FD fd, void *_buf, size_t size, int flags, int timeout_msec)
{

  ssize_t total = 0;
  uint8_t *buf = _buf;
  struct timespec start, end, diff;

  while (size > 0)
  {
    RS232_FPRINTF_DEBUG(stderr, "%s:%d: %p, %zu\n", __FUNCTION__, __LINE__, buf, size);

    clock_gettime(CLOCK_MONOTONIC, &start);
    ssize_t read_bytes = _RS232_Read(fd, buf, size, flags, timeout_msec);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if (read_bytes < 0) break; /* Break on error. */

    buf += read_bytes;
    size -= read_bytes;
    total += read_bytes;

    timerspecsub(&end, &start, &diff);
    timeout_msec -= timespecsub_to_msec(&diff);

    if (timeout_msec <= 0) break; /* Time is up. */
  }

  return total;
}

RS232_ADDAPI ssize_t RS232_ADDCALL RS232_Write(RS232_FD fd, const void *_buf, size_t size, int flags, int timeout_msec)
{

  ssize_t total = 0;
  const uint8_t *buf = _buf;
  struct timespec start, end, diff;

  while (size > 0)
  {
    RS232_FPRINTF_DEBUG(stderr, "%s:%d: %p, %zu\n", __FUNCTION__, __LINE__, buf, size);

    clock_gettime(CLOCK_MONOTONIC, &start);
    ssize_t written_bytes = _RS232_Write(fd, buf, size, flags, timeout_msec);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if (written_bytes < 0) break; /* Break on error. */

    buf += written_bytes;
    size -= written_bytes;
    total += written_bytes;

    timerspecsub(&end, &start, &diff);
    timeout_msec -= timespecsub_to_msec(&diff);

    if (timeout_msec <= 0) break; /* Time is up. */
  }

  return total;
}

RS232_ADDAPI RS232_FD RS232_ADDCALL RS232_Open(const char *devname, int baudrate, const char *mode, int flags)
{

  int attempts = 15; /* Empirical value to wait up to 15 seconds. */

  RS232_FD fd = _RS232_Open(devname, baudrate, mode, flags);

  while (fd == RS232_INVALID_FD && attempts > 0)
  {
    msleep(1000);
    fd = _RS232_Open(devname, baudrate, mode, flags);
    attempts--;
  }

  return fd;
}