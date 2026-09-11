CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O0 -g -I. -Istub -I../../src
SRCDIR = ../../src
COMMON = $(SRCDIR)/random_generator.c furi_stub.c
BUILD = build

$(BUILD)/tests: test_runner.c $(COMMON) rng_control.h stub/furi.h stub/furi_hal.h
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) test_runner.c $(COMMON) -o $(BUILD)/tests

.PHONY: run clean
run: $(BUILD)/tests
	./$(BUILD)/tests
clean:
	rm -rf $(BUILD)
