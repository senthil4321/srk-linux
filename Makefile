CC = gcc
CFLAGS = -Wall -Wextra -O2

# Targets
TARGETS = srk-signal-handler/signal_handler srk-capabilities/srk-capabilities

all: $(TARGETS)

# Compile signal_handler.c
srk-signal-handler/signal_handler: srk-signal-handler/signal_handler.c
	$(CC) $(CFLAGS) -o $@ $<

# Compile srk-capabilities.c
srk-capabilities/srk-capabilities: srk-capabilities/srk-capabilities.c
	$(CC) $(CFLAGS) -o $@ $< -lcap

clean:
	rm -f $(TARGETS)

.PHONY: all clean
