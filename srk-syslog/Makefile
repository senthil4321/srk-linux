CC = gcc
CFLAGS = -Wall -Wextra -O2

# Compile srk-syslog.c
srk-syslog: srk-syslog.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f srk-syslog

.PHONY: all clean
