CC = gcc
CFLAGS = -Wall -g -O2
LDFLAGS = -lssl -lcrypto

TARGETS = aes_victim perf_spy key_extractor perf_capabilities_demo perf_sampling_demo

all: $(TARGETS)

aes_victim: aes_victim.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

perf_spy: perf_spy.c
	$(CC) $(CFLAGS) -o $@ $<

key_extractor: key_extractor.c simple_aes.h
	$(CC) $(CFLAGS) -o $@ key_extractor.c

perf_capabilities_demo: perf_capabilities_demo.c
	$(CC) $(CFLAGS) -o $@ perf_capabilities_demo.c

perf_sampling_demo: perf_sampling_demo.c
	$(CC) $(CFLAGS) -o $@ perf_sampling_demo.c

clean:
	rm -f $(TARGETS) *.o

test: all
	@echo "=== Testing AES Victim ==="
	@echo "Run in one terminal: sudo ./aes_victim"
	@echo "Run in another terminal: sudo ./perf_spy \$$(pgrep aes_victim)"
	@echo ""
	@echo "=== Testing Key Extractor ==="
	@echo "Run: sudo ./key_extractor"
	@echo ""
	@echo "Note: Requires root privileges for perf monitoring"

.PHONY: all clean test
