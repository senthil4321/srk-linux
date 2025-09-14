CC = gcc
CFLAGS = -Wall -Wextra -O2

# Compile signal_handler.c
signal_handler: signal_handler.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f signal_handler

.PHONY: all clean
