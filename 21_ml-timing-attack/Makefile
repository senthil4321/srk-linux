CC = gcc
CFLAGS = -Wall -O2 -g
PYTHON = python3

TARGETS = collect_timing_data

all: $(TARGETS)

collect_timing_data: collect_timing_data.c vulnerable_aes.h
	$(CC) $(CFLAGS) -o $@ collect_timing_data.c

run-collect: collect_timing_data
	@echo "=== Collecting timing data ==="
	./collect_timing_data timing_data.csv

run-ml: timing_data.csv
	@echo "=== Running ML analysis ==="
	$(PYTHON) ml_key_recovery.py timing_data.csv

demo: all run-collect run-ml

clean:
	rm -f $(TARGETS) *.o timing_data.csv timing_analysis.png

test: all
	@echo "=== Testing data collection ==="
	./collect_timing_data test_data.csv
	@if [ -f test_data.csv ]; then \
		echo "Data collection successful!"; \
		wc -l test_data.csv; \
		rm -f test_data.csv; \
	fi

.PHONY: all run-collect run-ml demo clean test
