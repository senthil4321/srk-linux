CC = gcc
CFLAGS = -Wall -Wextra -O2

# Compile srk-capabilities.c
srk-capabilities: srk-capabilities.c
	$(CC) $(CFLAGS) -o $@ $< -lcap

clean:
	rm -f srk-capabilities

.PHONY: all clean
