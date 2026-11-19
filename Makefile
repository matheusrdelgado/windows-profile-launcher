CC = gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -DUNICODE -D_UNICODE
LDFLAGS ?= -municode -mconsole
LDLIBS ?= -luser32 -ladvapi32
SOURCES = src/main.c src/config.c

all: launcher.exe

launcher.exe: $(SOURCES) src/config.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $@ $(LDLIBS)

clean:
	rm -f launcher.exe

.PHONY: all clean
