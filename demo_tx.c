/**************************************************

file: demo_tx.c
purpose: simple demo that transmits characters to
the serial port and print them on the screen,
exit the program by pressing Ctrl-C

compile with the command: gcc demo_tx.c rs232.c -Wall -Wextra -o test_tx

**************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "rs232.h"

#if WINDOWS_BUILD == 0
#include <unistd.h>

#define sleep(msecs) usleep(msecs*1000)
#else
#include <windows.h>

#define sleep(msecs) Sleep(msecs)
#endif

int main(int argc, char *argv[])
{

  if (argc < 2)
  {
    fprintf(stderr, "Usage example: %s /dev/ttyUSB0.\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *devname = argv[1];
  int baudrate = 115200;
  const char *mode = "8N1";
  int flags_open = 0;
  int flags_write = 0;
  int write_timeout = 2000; /* Milliseconds. */

  const char str[2][512] = {
    [0] = "The quick brown fox jumped over the lazy grey dog.\r\n",
    [1] = "Happy serial programming!\r\n",
  };

  fprintf(stdout, "Using serial port %s.\n", devname);

  RS232_FD fd = RS232_Open(devname, baudrate, mode, flags_open);

  if (fd == RS232_INVALID_FD)
  {
    return EXIT_FAILURE;
  }

  for (int i = 0; true; i ^= 1)
  {
    RS232_Write(fd, str[i], strlen(str[i]), flags_write, write_timeout);
    sleep(1000);
  }

  return EXIT_SUCCESS;
}