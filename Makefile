CC = gcc
CFLAGS = -Wall -Wextra -O2

# Targets
TARGETS = srk-signal-handler/signal_handler srk-capabilities/srk-capabilities srk-syslog/srk-syslog srk-kernel-buffer/srk-kernel-buffer.ko

all: $(TARGETS)

# Compile signal_handler.c
srk-signal-handler/signal_handler: srk-signal-handler/signal_handler.c
	$(CC) $(CFLAGS) -o $@ $<

# Compile srk-capabilities.c
srk-capabilities/srk-capabilities: srk-capabilities/srk-capabilities.c
    $(CC) $(CFLAGS) -o $@ $< -lcap

# Compile srk-syslog.c
srk-syslog/srk-syslog: srk-syslog/srk-syslog.c
	$(CC) $(CFLAGS) -o $@ $<

# Compile and sign srk-kernel-buffer.c
srk-kernel-buffer/srk-kernel-buffer.ko: srk-kernel-buffer/srk-kernel-buffer.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/srk-kernel-buffer modules
	sh srk-kernel-buffer/sign-module.sh

clean:
	rm -f srk-signal-handler/signal_handler srk-capabilities/srk-capabilities srk-syslog/srk-syslog
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/srk-kernel-buffer clean

.PHONY: all clean
