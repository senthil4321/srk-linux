CC = gcc
CFLAGS = -Wall -O2 -g
LDFLAGS = -lssl -lcrypto

TARGETS = aes_victim perf_spy

all: $(TARGETS)

aes_victim: aes_victim.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

perf_spy: perf_spy.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) *.o

test: all
	@echo "=== Testing AES Victim ==="
	@echo "Run in one terminal: sudo ./aes_victim"
	@echo "Run in another terminal: sudo ./perf_spy \$$(pgrep aes_victim)"
	@echo ""
	@echo "Note: Requires root privileges for perf monitoring"

.PHONY: all clean test
