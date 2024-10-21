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

BUILD_TYPE ?= Release

#CC ?= x86_64-w64-mingw32-gcc
#CC ?= i686-w64-mingw32-gcc
# When on Linux or WSL you can compile for Windows like this: make CC=x86_64-w64-mingw32-gcc or make CC=i686-w64-mingw32-gcc

CC ?= gcc
CFLAGS ?= -Wall -Wextra -Wshadow -Wformat-nonliteral -Wformat-security -Wtype-limits
CPPFLAGS ?=
LDFLAGS ?= -L./ -Wl,-rpath=./

ifeq ($(BUILD_TYPE),Debug)
CFLAGS += -O0 -ggdb3
CPPFLAGS += -DDEBUG
else ifeq ($(BUILD_TYPE),Release)
CFLAGS += -O2
CPPFLAGS += -DNDEBUG
LDFLAGS += -s
else
$(error Unknown BUILD_TYPE, must be either "Debug" or "Release")
endif

ifeq ($(OS),Windows_NT)
#LDFLAGS += -Wl,--subsystem,windows
SO = .dll
EXE = .exe
else
SO = .so
EXE =
endif


all: test_rx test_tx test_rs232

clean :
	$(RM) *.o *$(SO) test_rx$(EXE) test_tx$(EXE) test_rs232$(EXE)

cleanall: clean all

rebuild: clean all

lib: librs232.so

test_rx : demo_rx.o librs232.so
	$(CC) $(CFLAGS) $(CPPFLAGS) -o test_rx$(EXE) $(LDFLAGS) demo_rx.o -l:librs232$(SO)

test_tx : demo_tx.o librs232.so
	$(CC) $(CFLAGS) $(CPPFLAGS) -o test_tx$(EXE) $(LDFLAGS) demo_tx.o -l:librs232$(SO)

test_rs232 : test_rs232.o librs232.so
	$(CC) $(CFLAGS) $(CPPFLAGS) -o test_rs232$(EXE) $(LDFLAGS) test_rs232.o -l:librs232$(SO)

demo_rx.o : demo_rx.c rs232.h rs232_platform.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c demo_rx.c -o $@

demo_tx.o : demo_tx.c rs232.h rs232_platform.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c demo_tx.c -o $@

test_rs232.o : test_rs232.c rs232.h rs232_platform.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c test_rs232.c -o $@

rs232.o : rs232.h rs232_platform.h rs232.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -DRS232_ADD_EXPORTS -fPIC -c rs232.c -o $@

librs232.so: rs232.o
	$(CC) -shared -o librs232$(SO) $(LDFLAGS) rs232.o
