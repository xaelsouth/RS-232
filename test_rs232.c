/**************************************************

file: test_rs232.c
purpose: Simple demo that implements multiple unit tests.
         Use null-modem cable to run it.

Compile with the command: gcc test_rs232.c rs232.c -Wall -Wextra -o test_rs232

**************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "rs232.h"

#if defined(NDEBUG)
#define my_assert(expr) do { if (!(expr)) abort(); } while(0)
#else
#include <assert.h>

#define my_assert(expr)   assert(expr)
#endif

/**
 * @brief Cheap try-to-open. The best practice for any application is to wait for some amount
 *        of time after calling the Close method before attempting to call the Open method,
 *        as the port may not be closed instantly.
 *        
 * @note https://learn.microsoft.com/en-us/dotnet/api/system.io.ports.serialport.close?view=net-8.0&redirectedfrom=MSDN#System_IO_Ports_SerialPort_Close
 */
static RS232_FD try_RS232_Open(const char *devname, int baudrate, const char *mode, int flags)
{

  int attempts = 15;

  RS232_FD fd = RS232_Open(devname, baudrate, mode, flags);

  while (fd == RS232_INVALID_FD && attempts--)
  {
    msleep(1000);
    fd = RS232_Open(devname, baudrate, mode, flags);
  }

  return fd;
}

static void test_write_read_256bytes(RS232_FD src, RS232_FD dst)
{

  int flags = 0, timeout_msec = 1000, err;
  ssize_t written_bytes, read_bytes;
  uint8_t tx_buf[256], rx_buf[256];

  for (size_t i = 0; i < sizeof(tx_buf); i++)
  {
    tx_buf[i] = i;
    rx_buf[i] = 0;
  }

  err = RS232_flushRXTX(src);
  my_assert(err == 0);

  err = RS232_flushRXTX(dst);
  my_assert(err == 0);

  written_bytes = RS232_Write(src, tx_buf, sizeof(tx_buf), flags, timeout_msec);
  my_assert(written_bytes == sizeof(tx_buf));

  read_bytes = RS232_Read(dst, rx_buf, sizeof(rx_buf), flags, timeout_msec);
  my_assert(read_bytes == (ssize_t)sizeof(rx_buf));

  for (ssize_t i = 0; i < read_bytes; i++)
  {
    my_assert(tx_buf[i] == rx_buf[i]);
  }
}

static void test_write_read_256bytes_nonblocking(RS232_FD src, RS232_FD dst)
{

  int flags = 0, timeout_msec = 0, err;
  ssize_t written_bytes, read_bytes;
  uint8_t tx_buf[256], rx_buf[256];

  for (size_t i = 0; i < sizeof(tx_buf); i++)
  {
    tx_buf[i] = i;
    rx_buf[i] = 0;
  }

  err = RS232_flushRXTX(src);
  my_assert(err == 0);

  err = RS232_flushRXTX(dst);
  my_assert(err == 0);

  written_bytes = RS232_Write(src, tx_buf, sizeof(tx_buf), flags, timeout_msec);
  my_assert(written_bytes == sizeof(tx_buf));

  read_bytes = RS232_Read(dst, rx_buf, sizeof(rx_buf), flags, timeout_msec);
  my_assert(read_bytes <= (ssize_t)sizeof(rx_buf) && read_bytes >= 0);

  for (ssize_t i = 0; i < read_bytes; i++)
  {
    my_assert(tx_buf[i] == rx_buf[i]);
  }
}

static void test_cts_rts(const char *argv_1, const char *argv_2, int baudrate, const char *mode)
{

  int err, status;

  RS232_FD src = try_RS232_Open(argv_1, baudrate, mode, 0);
  my_assert(src != RS232_INVALID_FD);

  RS232_FD dst = try_RS232_Open(argv_2, baudrate, mode, 0);
  my_assert(dst != RS232_INVALID_FD);

  status = RS232_IsCTSEnabled(src);
  my_assert(status == 1);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);

  err = RS232_disableRTS(src);
  my_assert(err == 0);
  err = RS232_disableRTS(dst);
  my_assert(err == 0);

  /* Give OS some time to switch lines. */
  msleep(100);

  status = RS232_IsCTSEnabled(src);
  my_assert(status == 0);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 0);

  err = RS232_enableRTS(src);
  my_assert(err == 0);
  err = RS232_enableRTS(dst);
  my_assert(err == 0);

  /* Give OS some time to switch lines. */
  msleep(100);

  status = RS232_IsCTSEnabled(src);
  my_assert(status == 1);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);

  err = RS232_Close(src);
  my_assert(err == 0);

  err = RS232_Close(dst);
  my_assert(err == 0);
}

static void test_dtr_dsr(const char *argv_1, const char *argv_2, int baudrate, const char *mode)
{

  int err, status;

  RS232_FD src = try_RS232_Open(argv_1, baudrate, mode, 0);
  my_assert(src != RS232_INVALID_FD);

  RS232_FD dst = try_RS232_Open(argv_2, baudrate, mode, 0);
  my_assert(dst != RS232_INVALID_FD);

  status = RS232_IsCTSEnabled(src);
  my_assert(status == 1);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);

  err = RS232_disableDTR(src);
  my_assert(err == 0);
  err = RS232_disableDTR(dst);
  my_assert(err == 0);

  /* Give OS some time to switch lines. */
  msleep(100);

  status = RS232_IsDSREnabled(src);
  my_assert(status == 0);
  status = RS232_IsDSREnabled(dst);
  my_assert(status == 0);

  err = RS232_enableDTR(src);
  my_assert(err == 0);
  err = RS232_enableDTR(dst);
  my_assert(err == 0);

  /* Give OS some time to switch lines. */
  msleep(100);

  status = RS232_IsDSREnabled(src);
  my_assert(status == 1);
  status = RS232_IsDSREnabled(dst);
  my_assert(status == 1);

  err = RS232_Close(src);
  my_assert(err == 0);

  err = RS232_Close(dst);
  my_assert(err == 0);
}

static void test_break(RS232_FD src, RS232_FD dst)
{

  int err;
  int flags = 0, timeout_msec = 1;
  ssize_t read_bytes;
  uint8_t rx_buf[256];

  for (size_t i = 0; i < sizeof(rx_buf); i++)
  {
    rx_buf[i] = 0xF0;
  }

  err = RS232_disableBREAK(src);
  my_assert(err == 0);

  err = RS232_disableBREAK(dst);
  my_assert(err == 0);

  err = RS232_flushRXTX(src);
  my_assert(err == 0);

  err = RS232_flushRXTX(dst);
  my_assert(err == 0);

  int zeros = 0;

  /* Send break characters. */
  for (size_t i = 0; i < 1000; i++)
  {
    err = RS232_enableBREAK(src);
    my_assert(err == 0);

    read_bytes = RS232_Read(dst, rx_buf, sizeof(rx_buf), flags, timeout_msec);
    for (ssize_t k = 0; k < read_bytes; k++)
    {
      if (rx_buf[k] == 0) ++zeros;
    }

    err = RS232_disableBREAK(src);
    my_assert(err == 0);

    if (zeros >= 100) break;
  }
  my_assert(zeros >= 100);

  err = RS232_flushRXTX(src);
  my_assert(err == 0);

  err = RS232_flushRXTX(dst);
  my_assert(err == 0);

  read_bytes = RS232_Read(dst, rx_buf, sizeof(rx_buf), flags, timeout_msec);
  my_assert(read_bytes == 0);
}

static void test_hwflowcontrol(const char *argv_1, const char *argv_2, int baudrate, const char *mode)
{

  int err, status;

  RS232_FD src = try_RS232_Open(argv_1, baudrate, mode, RS232_FLAGS_HWFLOWCTRL);
  my_assert(src != RS232_INVALID_FD);

  RS232_FD dst = try_RS232_Open(argv_2, baudrate, mode, RS232_FLAGS_HWFLOWCTRL);
  my_assert(dst != RS232_INVALID_FD);

  /* RTS lines must be set active - check it by reading CTS! */
  status = RS232_IsCTSEnabled(src);
  my_assert(status == 1);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);

  test_write_read_256bytes(src, dst);

  err = RS232_Close(src);
  my_assert(err == 0);

  err = RS232_Close(dst);
  my_assert(err == 0);
}

static void test_hwflowcontrol2(const char *argv_1, const char *argv_2, int baudrate, const char *mode)
{


  int err, status;
  ssize_t written_bytes = 0;
  int flags = 0, timeout_msec = 0;
  uint8_t tx_buf[256];

  for (size_t i = 0; i < sizeof(tx_buf); i++)
  {
    tx_buf[i] = 0xF0;
  }  

  RS232_FD src = try_RS232_Open(argv_1, baudrate, mode, RS232_FLAGS_HWFLOWCTRL);
  my_assert(src != RS232_INVALID_FD);

  RS232_FD dst = try_RS232_Open(argv_2, baudrate, mode, RS232_FLAGS_HWFLOWCTRL);
  my_assert(dst != RS232_INVALID_FD);

  /* RTS lines must be set active - check it by reading CTS! */
  status = RS232_IsCTSEnabled(src);
  my_assert(status == 1);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);

  err = RS232_flushRXTX(src);
  my_assert(err == 0);

  err = RS232_flushRXTX(dst);
  my_assert(err == 0);

  for (int i = 0; i < 5000 && status == 1; i++)
  {
    written_bytes = RS232_Write(src, tx_buf, sizeof(tx_buf), flags, timeout_msec);
    my_assert(written_bytes >= 0);

    status = RS232_IsCTSEnabled(src);
  }
  my_assert(status == 0); /* Must be low (inactive) as too much data has been received on the interface without being read. */

  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);

  err = RS232_Close(src);
  my_assert(err == 0);

  err = RS232_Close(dst);
  my_assert(err == 0);
}

int main(int argc, char *argv[])
{

  if (argc < 3)
  {
    fprintf(stderr, "Usage: %s /dev/ttyUSB0 /dev/ttyUSB1.\n", argv[0]);
    fprintf(stderr, "Hint: Use null-modem cable to make tests running.\n");
    return EXIT_FAILURE;
  }

  int err, status;
  RS232_FD src = RS232_Open(argv[1], 115200, "8N1", 0);
  my_assert(src != RS232_INVALID_FD);

  RS232_FD dst = RS232_Open(argv[2], 115200, "8N1", 0);
  my_assert(dst != RS232_INVALID_FD);

  /* RTS lines must be set active - check it by reading CTS! */
  status = RS232_IsCTSEnabled(src);
  my_assert(status == 1);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);

  test_write_read_256bytes(src, dst);
  test_write_read_256bytes_nonblocking(src, dst);
  test_break(src, dst);

  err = RS232_Close(src);
  my_assert(err == 0);

  err = RS232_Close(dst);
  my_assert(err == 0);

  test_cts_rts(argv[1], argv[2], 115200, "7N1");
  test_dtr_dsr(argv[1], argv[2], 57600,  "8E2");
  test_cts_rts(argv[1], argv[2], 38400,  "7O1");
  test_dtr_dsr(argv[1], argv[2], 19200,  "8N2");
  test_cts_rts(argv[1], argv[2], 9600,   "7E1");
  test_dtr_dsr(argv[1], argv[2], 4800,   "8O2");
  test_cts_rts(argv[1], argv[2], 2400,   "7N1");
  test_dtr_dsr(argv[1], argv[2], 1200,   "8E2");
  test_cts_rts(argv[1], argv[2], 600,    "7O1");
  test_dtr_dsr(argv[1], argv[2], 300,    "8N2");

  test_hwflowcontrol(argv[1], argv[2], 115200, "8E1");
  test_hwflowcontrol(argv[1], argv[2], 115200, "8O1");
  test_hwflowcontrol(argv[1], argv[2], 115200, "8N1");
  //test_hwflowcontrol2(argv[1], argv[2],   300, "8N1");

  fprintf(stdout, "All tests passed!\n");

  return EXIT_SUCCESS;
}