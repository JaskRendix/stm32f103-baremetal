# Simple wrapper Makefile for Ninja + CMake

BUILD_DIR := build
TOOLCHAIN := cmake/arm-none-eabi-gcc.cmake

# Default: configure + build
all:
	@cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN) -S . -B $(BUILD_DIR)
	@ninja -C $(BUILD_DIR)

# Clean build directory
clean:
	@rm -rf $(BUILD_DIR)

# Full rebuild (clean + configure + build)
rebuild: clean all

# Format all C and header files
format:
	@find core drivers include src tests examples -type f \( -name "*.c" -o -name "*.h" \) -print0 | xargs -0 clang-format -i

# Run clang-tidy on all source files
tidy:
	@FILES="$$(find core drivers src tests examples -type f -name '*.c')"; \
	if [ -z "$$FILES" ]; then \
		echo "No C files found"; exit 0; \
	fi; \
	clang-tidy $$FILES -- -Iinclude -Icore -Idrivers

# Build and run host-side tests
test:
	@cmake -S . -B build_tests
	@cmake --build build_tests
	@cd build_tests && ./test_gpio && ./test_systick && ./test_uart

# Build and run host-side tests
test-clean:
	@rm -rf build_tests
	@make test

# Build all host-side examples
examples:
	@cmake -S . -B build_examples -DBUILD_EXAMPLES=ON
	@cmake --build build_examples

# Clean examples build directory
examples-clean:
	@rm -rf build_examples

# Build + run a specific example (e.g. make run-example EX=rocket_telemetry)
run-example:
	@if [ -z "$(EX)" ]; then \
		echo "Usage: make run-example EX=<example_name>"; exit 1; \
	fi
	@cmake -S . -B build_examples -DBUILD_EXAMPLES=ON
	@cmake --build build_examples --target $(EX)
	@cd build_examples/examples/$(EX) && ./$(EX)

.PHONY: all clean rebuild format tidy test examples examples-clean run-example
