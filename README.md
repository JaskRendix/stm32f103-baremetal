# stm32f103‑baremetal

Bare‑metal firmware for the STM32F103 (Cortex‑M3).  
This fork restructures the original project to provide a cleaner build system, host‑side examples, and a testable driver layer.

The goal is to keep the low‑level, register‑focused approach while adding tooling that makes development and verification straightforward.

It is intended for cases where you want full control over the MCU, but also want modern tooling and a workflow that supports iteration, testing, and extension.

---

## Build

Original project: **[https://github.com/gnif/STM32F103-BareMetal](https://github.com/gnif/STM32F103-BareMetal)**

---

## Release

```
make
```

Runs CMake (Ninja) and produces:

```
build/out.elf
```

---

## Directory layout

core/        Startup code and low-level MCU support
drivers/     Register-level peripheral drivers
src/         Application code
include/     Public headers
examples/    Host-side examples
tests/       Host-side test suite
cmake/       Toolchain and build configuration

---

## Debug

```
make BUILD_TYPE=Debug
```

Or with a dedicated directory:

```
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -B build_debug
ninja -C build_debug
```

---

## Clean

```
make clean
```

---

## Full rebuild

```
make rebuild
```

---

# Formatting & Static Analysis

## Format all sources

```
make format
```

Recursively formats all `.c` and `.h` files.

## clang‑tidy

```
make tidy
```

Runs clang‑tidy on all `.c` files with correct include paths.

---

# Host‑side Tests

```
make test
```

Builds and runs tests in `build_tests/`.

```
make test-clean
```

Removes the test build directory and reruns.

---

# Host‑side Examples

These examples run on the host, not the MCU.  

## Build all examples

```
make examples
```

## Clean examples

```
make examples-clean
```

## Run a specific example

```
make run-example EX=<name>
```

---

# Examples

### `led_shift`
Simple LED chaser logic. Four GPIOs taking turns like a scuffed round‑robin queue.

### `morse_code`
Encodes and blinks Morse patterns. Your GPIO pin becomes the friend who only communicates in dots and dashes.

### `mosquito_trap`
Simulates a UV/blue LED trap with sensor logic. LED rave for bugs that should’ve stayed home.

### `cpu8_vm`
A tiny 8‑bit virtual CPU with registers and RAM. It’s the odd one: a full instruction engine sitting in a bare‑metal project like it wandered in from another repo, which is why people stop and inspect it.

### `rocket_telemetry`
Simulates a rocket flight and emits telemetry frames with CRC. Altitude climbs, velocity climbs, CRC keeps the packets honest.
