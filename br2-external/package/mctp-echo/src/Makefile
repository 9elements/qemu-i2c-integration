CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2

BINARIES = mctp-read mctp-write
SCRIPTS = mctp-echo

all: $(BINARIES)

mctp-read: mctp-read.c mctp-common.h
	$(CC) $(CFLAGS) -o $@ mctp-read.c

mctp-write: mctp-write.c mctp-common.h
	$(CC) $(CFLAGS) -o $@ mctp-write.c

clean:
	rm -f $(BINARIES)

.PHONY: all clean
