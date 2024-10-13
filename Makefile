#
#
# Author: Teunis van Beelen, Xael South
#
# Copyright (C) 2005 - 2023 Teunis van Beelen
# Copyright (C) 2024 - 2024 Xael South
#
# Email: teuniz@protonmail.com
#        xael.south@yandex.com
#

CC ?= gcc
#CC ?= x86_64-w64-mingw32-gcc
#CC ?= i686-w64-mingw32-gcc
# Or just compile by: make CC=x86_64-w64-mingw32-gcc or make CC=i686-w64-mingw32-gcc

CFLAGS ?= -DNDEBUG -O2 -Wall -Wextra -Wshadow -Wformat-nonliteral -Wformat-security -Wtype-limits

LDFLAGS ?= -L./ -Wl,-rpath=./

all: test_rx test_tx test_rs232

clean :
	$(RM) test_rx test_tx test_rs232 rs232.o librs232.so demo_rx.o demo_tx.o test_rs232.o

cleanall: clean all

rebuild: clean all

test_rx : demo_rx.o librs232.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o test_rx demo_rx.o -l:librs232.so

test_tx : demo_tx.o librs232.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o test_tx demo_tx.o -l:librs232.so

test_rs232 : test_rs232.o librs232.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o test_rs232 test_rs232.o -l:librs232.so

demo_rx.o : demo_rx.c rs232.h rs232.c
	$(CC) $(CFLAGS) -c demo_rx.c -o demo_rx.o

demo_tx.o : demo_tx.c rs232.h rs232.c
	$(CC) $(CFLAGS) -c demo_tx.c -o demo_tx.o

test_rs232.o : test_rs232.c rs232.h rs232.c
	$(CC) $(CFLAGS) -c test_rs232.c -o test_rs232.o

rs232.o : rs232.h rs232.c
	$(CC) $(CFLAGS) -DADD_EXPORTS -fPIC -c rs232.c -o rs232.o

librs232.so: rs232.o
	$(CC) -s -shared -o librs232.so rs232.o
