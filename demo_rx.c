/**************************************************

file: demo_rx.c
purpose: simple demo that receives characters from
the serial port and print them on the screen,
exit the program by pressing Ctrl-C

compile with the command: gcc demo_rx.c rs232.c -Wall -Wextra -o test_rx

**************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "rs232.h"

#if WINDOWS_BUILD == 0
#include <unistd.h>
#else
#include <windows.h>
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
  int flags_read = 0;
  int read_timeout = 500; /* Milliseconds. */

  char buf[4096];

  fprintf(stdout, "Using serial port %s.\n", devname);

  RS232_FD fd = RS232_Open(devname, baudrate, mode, flags_open);

  if (fd == RS232_INVALID_FD)
  {
    return EXIT_FAILURE;
  }

  while (true)
  {
    ssize_t read_bytes = RS232_Read(fd, buf, sizeof(buf), flags_read, read_timeout);

    if (read_bytes > 0)
    {
      for (ssize_t i = 0; i < read_bytes; i++)
      {
        if (isprint(buf[i]) == 0)  /* replace unreadable control-codes by dots */
        {
          buf[i] = '.';
        }

        putc(buf[i], stdout);
      }
    }
    else
    {
      fprintf(stdout, "Nothing read: timeout after %d.\n", read_timeout);
    }
  }

  return EXIT_SUCCESS;
}