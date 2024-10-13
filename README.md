# RS-232 for Linux, FreeBSD and Windows

Website: https://github.com/xaelsouth/RS-232

This project is heavilly based on work of Teunis van Beelen, see https://www.teuniz.net/RS-232

What is different to the original version:
  * Serial interfaces can be opened by file name like /dev/ttyUSB0 or COM1.
  * Read and write have been reworked to support timeouts.
  * Implemented support for flags for following commands: RS232_Open, RS232_Read, RS232_Write.
  * RS232 can be build as shared object (library).

To include this library into your project:
  * Put the two files rs232.h and rs232.c in your project source directory.
  * Write #include "rs232.h" in your sourcefiles that needs access to the library.
  * Add the file rs232.c to your project settings in order to get it compiled and
    linked with your program.

Or just link your project with librs232.so.

## How to compile
Compiling the demo can be done as follows:
  * gcc demo_rx.c rs232.c -Wall -Wextra -o test_rx
  * gcc demo_tx.c rs232.c -Wall -Wextra -o test_tx
  * gcc test_rs232.c rs232.c -Wall -Wextra -o test_rs232

Or use the Makefile by entering "make".

Run the demo by typing:
  * ./test_rx
  * ./test_tx
  * ./test_rs232

test_rs232 implements multiple unit tests. Use null-modem cable to be able trun them.

