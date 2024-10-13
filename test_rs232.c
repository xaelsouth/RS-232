#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "rs232.h"

#if WINDOWS_BUILD == 0
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define sleep(msecs) usleep(msecs*1000)
#else
#include <windows.h>

#define sleep(msecs) Sleep(msecs)
#endif

#if defined(NDEBUG)
#define my_assert(expr) do { if (!(expr)) abort(); } while(0)
#else
#include <assert.h>

#define my_assert(expr)   assert(expr)
#endif

static void test_write_read_256bytes(RS232_FD src, RS232_FD dst)
{

  int flags = 0, timeout_msec = 1000;
  ssize_t written_bytes = 0, read_bytes = 0;
  uint8_t tx_buf[256], rx_buf[256];

  for (size_t i = 0; i < sizeof(tx_buf); i++)
  {
    tx_buf[i] = i;
    rx_buf[i] = 0;
  }

  written_bytes = RS232_Write(src, tx_buf, sizeof(tx_buf), flags, timeout_msec);
  my_assert(written_bytes == sizeof(tx_buf));

  read_bytes = RS232_Read(dst, rx_buf, sizeof(rx_buf), flags, timeout_msec);
  my_assert(read_bytes == sizeof(rx_buf));

  for (size_t i = 0; i < sizeof(tx_buf); i++)
  {
    my_assert(tx_buf[i] == rx_buf[i]);
  }
}

static void test_write_read_256bytes_nonblocking(RS232_FD src, RS232_FD dst)
{

  int flags = 0, timeout_msec = 0;
  ssize_t written_bytes = 0, read_bytes = 0;
  uint8_t tx_buf[256], rx_buf[256];

  for (size_t i = 0; i < sizeof(tx_buf); i++)
  {
    tx_buf[i] = i;
    rx_buf[i] = 0;
  }

  do
  {
    ssize_t _written = RS232_Write(src, tx_buf + written_bytes, sizeof(tx_buf) - written_bytes, flags, 0);
    my_assert(written_bytes >= 0);
    written_bytes += _written;
  }
  while (written_bytes < (ssize_t)sizeof(tx_buf));
  my_assert(written_bytes == sizeof(tx_buf));

  do
  {
    ssize_t _read = RS232_Read(dst, rx_buf + read_bytes, sizeof(rx_buf) - read_bytes, flags, timeout_msec);
    my_assert(_read >= 0);
    read_bytes += _read;
  }
  while (read_bytes < (ssize_t)sizeof(rx_buf));
  my_assert(read_bytes == sizeof(rx_buf));

  for (size_t i = 0; i < sizeof(tx_buf); i++)
  {
    my_assert(tx_buf[i] == rx_buf[i]);
  }
}

static void test_cts_rts(RS232_FD src, RS232_FD dst)
{

  int err, status;

  err = RS232_disableRTS(src);
  my_assert(err == 0);
  err = RS232_disableRTS(dst);
  my_assert(err == 0);

  status = RS232_IsCTSEnabled(src);
  my_assert(status == 0);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 0);

  err = RS232_enableRTS(src);
  my_assert(err == 0);
  err = RS232_enableRTS(dst);
  my_assert(err == 0);

  status = RS232_IsCTSEnabled(src);
  my_assert(status == 1);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);
}

static void test_dtr_dsr(RS232_FD src, RS232_FD dst)
{

  int err, status;

  err = RS232_disableDTR(src);
  my_assert(err == 0);
  err = RS232_disableDTR(dst);
  my_assert(err == 0);

  status = RS232_IsDSREnabled(src);
  my_assert(status == 0);
  status = RS232_IsDSREnabled(dst);
  my_assert(status == 0);

  err = RS232_enableDTR(src);
  my_assert(err == 0);
  err = RS232_enableDTR(dst);
  my_assert(err == 0);

  status = RS232_IsDSREnabled(src);
  my_assert(status == 1);
  status = RS232_IsDSREnabled(dst);
  my_assert(status == 1);
}

static void test_break(RS232_FD src, RS232_FD dst)
{

  int err;
  int flags = 0, timeout_msec = 1000;
  ssize_t read_bytes = 0;
  uint8_t rx_buf[256];

  for (size_t i = 0; i < sizeof(rx_buf); i++)
  {
    rx_buf[i] = i;
  }

  err = RS232_disableBREAK(src);
  my_assert(err == 0);

  err = RS232_disableBREAK(dst);
  my_assert(err == 0);

  err = RS232_flushRXTX(src);
  my_assert(err == 0);

  err = RS232_flushRXTX(dst);
  my_assert(err == 0);

  /* Send sizeof(rx_buf) break characters. */
  for (size_t i = 0; i < sizeof(rx_buf); i++)
  {
    err = RS232_enableBREAK(src);
    my_assert(err == 0);

    sleep(1);

    err = RS232_disableBREAK(src);
    my_assert(err == 0);
  }

  do
  {
    ssize_t _read = RS232_Read(dst, rx_buf + read_bytes, sizeof(rx_buf) - read_bytes, flags, timeout_msec);
    my_assert(_read >= 0);
    read_bytes += _read;
  }
  while (read_bytes < (ssize_t)sizeof(rx_buf));
  my_assert(read_bytes == sizeof(rx_buf));

  for (size_t i = 0; i < sizeof(rx_buf); i++)
  {
    my_assert(rx_buf[i] == 0);
  }

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

  RS232_FD src = RS232_Open(argv_1, baudrate, mode, RS232_FLAGS_HWFLOWCTRL);
  my_assert(src != RS232_INVALID_FD);

  RS232_FD dst = RS232_Open(argv_2, baudrate, mode, RS232_FLAGS_HWFLOWCTRL);
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
  int flags = 0, timeout_msec = 1000;

  RS232_FD src = RS232_Open(argv_1, baudrate, mode, RS232_FLAGS_HWFLOWCTRL);
  my_assert(src != RS232_INVALID_FD);

  RS232_FD dst = RS232_Open(argv_2, baudrate, mode, RS232_FLAGS_HWFLOWCTRL);
  my_assert(dst != RS232_INVALID_FD);

  /* RTS lines must be set active - check it by reading CTS! */
  status = RS232_IsCTSEnabled(src);
  my_assert(status == 1);
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 1);

  for (int i = 0; i < 5000 && status == 1; i++)
  {
    uint8_t tx_buf = 0xF0;
    written_bytes = RS232_Write(src, &tx_buf, sizeof(tx_buf), flags, timeout_msec);
    my_assert(written_bytes >= 0);

    status = RS232_IsCTSEnabled(dst);
  }
  status = RS232_IsCTSEnabled(dst);
  my_assert(status == 0); /* Must be low (inactive) as too much data has been received on the interface without being read. */

  status = RS232_IsCTSEnabled(src);
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
  test_cts_rts(src, dst);
  test_dtr_dsr(src, dst);
  test_break(src, dst);

  err = RS232_Close(src);
  my_assert(err == 0);

  err = RS232_Close(dst);
  my_assert(err == 0);

  test_hwflowcontrol(argv[1], argv[2], 115200, "8E1");
  test_hwflowcontrol(argv[1], argv[2], 115200, "8O1");
  test_hwflowcontrol(argv[1], argv[2], 115200, "8N1");
  test_hwflowcontrol2(argv[1], argv[2], 2400, "8N1");

  return EXIT_SUCCESS;
}