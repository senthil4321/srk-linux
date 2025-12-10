CC = gcc
CFLAGS = -Wall -O2 -g
PYTHON = python3

TARGETS = collect_timing_data predict_unknown_key

all: $(TARGETS)

collect_timing_data: collect_timing_data.c vulnerable_aes.h
	$(CC) $(CFLAGS) -o $@ collect_timing_data.c

predict_unknown_key: predict_unknown_key.c vulnerable_aes.h
	$(CC) $(CFLAGS) -o $@ predict_unknown_key.c

run-collect: collect_timing_data
	@echo "=== Collecting timing data ==="
	./collect_timing_data timing_data.csv

run-ml: timing_data.csv
	@echo "=== Running ML analysis ==="
	$(PYTHON) ml_key_recovery.py timing_data.csv

run-predict: predict_unknown_key
	@echo "=== Collecting data for unknown key ==="
	./predict_unknown_key
	@echo ""
	@echo "=== Predicting unknown key ==="
	$(PYTHON) predict_with_model.py unknown_key_data.csv

demo: all run-collect run-ml
	@echo ""
	@echo "=== Training Demo Complete ==="

full-demo: all
	@echo "=== Phase 1: Training with Known Key ==="
	@echo ""
	./collect_timing_data timing_data.csv
	@echo ""
	$(PYTHON) ml_key_recovery.py timing_data.csv
	@echo ""
	@echo "=== Phase 2: Predicting Unknown Key ==="
	@echo ""
	./predict_unknown_key
	@echo ""
	$(PYTHON) predict_with_model.py unknown_key_data.csv
	@echo ""
	@echo "=== Full Demo Complete ==="

clean:
	rm -f $(TARGETS) *.o timing_data.csv unknown_key_data.csv timing_analysis.png unknown_key_prediction.png

test: all
	@echo "=== Testing data collection ==="
	./collect_timing_data test_data.csv
	@if [ -f test_data.csv ]; then \
		echo "Data collection successful!"; \
		wc -l test_data.csv; \
		rm -f test_data.csv; \
	fi

.PHONY: all run-collect run-ml run-predict demo full-demo clean test
