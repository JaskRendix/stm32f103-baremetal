#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>
#include "project_config.h"

#ifndef CPU_HZ
#error "CPU_HZ must be defined in project_config.h"
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(CPU_HZ > 0, "CPU_HZ must be greater than zero");
#endif

#ifndef LOOP_CYCLES
#define LOOP_CYCLES 3UL
#endif

#if defined(__arm__) || defined(__thumb__) || defined(__ARM_ARCH)

#include "systick.h"

/*
 * Approximate software delay loop.
 * Actual timing depends on MCU, flash wait states,
 * compiler settings, and interrupts.
 */
#define _NOP_LOOP(count)                                                                           \
    do                                                                                             \
    {                                                                                              \
        uint32_t _c = (count);                                                                     \
        if (_c > 0U)                                                                               \
        {                                                                                          \
            __asm__ volatile("1: subs %0, %0, #1\n"                                                \
                             "   bne 1b\n"                                                         \
                             : "+r"(_c)                                                            \
                             :                                                                     \
                             : "cc", "memory");                                                    \
        }                                                                                          \
    } while (0)

/*
 * Intended for short hardware delays only.
 * For longer waits use delay_ms() or a timer.
 */
static inline void delay_ns(uint64_t ns)
{
    uint64_t cycles = ((ns * CPU_HZ) + 999999999ULL) / 1000000000ULL;

    uint32_t loops = (uint32_t)((cycles + LOOP_CYCLES - 1U) / LOOP_CYCLES);

    _NOP_LOOP(loops);
}

static inline void delay_us(uint32_t us) { delay_ns((uint64_t)us * 1000ULL); }

/* Legacy compatibility */
#define DELAY_NS(ns) delay_ns(ns)
#define DELAY_US(us) delay_us(us)

static inline void delay_ms(uint32_t ms)
{
    uint32_t start = systick_ms();

    while ((systick_ms() - start) < ms)
    {
        /* busy wait */
    }
}

static inline void delay_ms_start(uint32_t ms) { delay_start_ms(ms); }

static inline uint8_t delay_ms_done(void) { return (uint8_t)delay_is_done(); }

#else

/* Host build / unit tests */

static inline void delay_ns(uint64_t ns) { (void)ns; }

static inline void delay_us(uint32_t us) { (void)us; }

static inline void delay_ms(uint32_t ms) { (void)ms; }

static inline void delay_ms_start(uint32_t ms) { (void)ms; }

static inline uint8_t delay_ms_done(void) { return 1U; }

#define DELAY_NS(ns) delay_ns(ns)
#define DELAY_US(us) delay_us(us)

#endif

#endif /* DELAY_H */
