# stm32f103‑baremetal

Bare‑metal firmware for the STM32F103 (Cortex‑M3).  
This fork restructures the original project to provide a clean build system, host‑side examples, and a testable driver layer.  
The focus is full control over the MCU with tooling that supports iteration, testing, and extension.

---

## Build

Original project: [https://github.com/gnif/STM32F103-BareMetal](https://github.com/gnif/STM32F103-BareMetal)

```
make
```

Produces:

```
build/out.elf
```

---

## Directory layout

core/        Startup and low‑level MCU support  
drivers/     Register‑level peripheral drivers  
include/     Public headers  
src/         Application code  
fake_hw/     Host‑side hardware models  
examples/    Host‑side examples  
tests/       Host‑side test suite  
cmake/       Toolchain and build configuration

---

## Debug

```
make BUILD_TYPE=Debug
```

Or:

```
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -B build_debug
ninja -C build_debug
```

---

## Clean

```
make clean
```

## Full rebuild

```
make rebuild
```

---

# Formatting & Static Analysis

## Format

```
make format
```

## clang‑tidy

```
make tidy
```

---

# Host‑side Tests

```
make test
```

Runs all tests in `build_tests/`.

```
make test-clean
```

Removes the test build directory.

The drivers use deterministic fake hardware models, so GPIO, UART, SysTick, and Flash logic can be validated on the host.

---

# Host‑side Examples

```
make examples
make examples-clean
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
Simulates a rocket flight, packs telemetry frames with CRC16, writes them into a flash‑backed blackbox, then reads the region back and checks every frame. Altitude climbs, velocity climbs, flash gets filled, recovery proves the frames survived.

---

# Flash and Blackbox

The flash driver implements STM32F1 erase/program semantics and includes a host‑side emulator.  
The blackbox uses a contiguous flash region as an append‑only log.  
Frames are packed, halfword‑aligned, CRC‑verified, and recovered exactly as stored.
