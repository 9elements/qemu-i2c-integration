CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2

TARGET = mctp-echo
SRC = mctp-echo.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
