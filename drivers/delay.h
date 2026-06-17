#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>
#include "project_config.h"

/* One loop iteration = 3 cycles (subs, bne) */
#define LOOP_CYCLES 3UL

/* Raw assembly loop */
#define _NOP_LOOP(count)                                                                           \
    do                                                                                             \
    {                                                                                              \
        uint32_t _c = (count);                                                                     \
        __asm__ volatile("1: subs %0, %0, #1\n"                                                    \
                         "   bne 1b\n"                                                             \
                         : "+r"(_c)                                                                \
                         :                                                                         \
                         : "cc");                                                                  \
    } while (0)

/* Convert cycles → loop iterations */
#define NOP_LOOP(cycles) _NOP_LOOP((cycles) / LOOP_CYCLES)

/* Delay in nanoseconds */
#define DELAY_NS(ns) NOP_LOOP(((uint64_t)(ns) * CPU_HZ) / 1000000000ULL)

/* Delay in microseconds */
#define DELAY_US(us) DELAY_NS((uint64_t)(us) * 1000ULL)

#endif
