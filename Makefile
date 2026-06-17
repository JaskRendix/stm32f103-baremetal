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
	@clang-format -i \
		core/*.c core/*.h \
		drivers/*.c drivers/*.h \
		include/*.h \
		src/*.c

# Run clang-tidy on all source files
tidy:
	@clang-tidy \
		core/*.c \
		drivers/*.c \
		src/*.c \
		-- -Iinclude -Icore -Idrivers

# Build and run host-side tests
test:
	@cmake -S . -B build_tests
	@cmake --build build_tests
	@cd build_tests && ./test_gpio

# Build and run host-side tests
test-clean:
	@rm -rf build_tests
	@make test

.PHONY: all clean rebuild format tidy test
