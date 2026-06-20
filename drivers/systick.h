#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include "stm32f103.h"
#include "project_config.h"

extern volatile uint32_t systick_ticks;

void SysTick_Handler(void);

/*
 * SysTick provides a 1 ms timebase.
 * CPU_HZ must be defined in project_config.h.
 */
void systick_init(void);

/* Returns system uptime in milliseconds */
uint32_t systick_ms(void);

/*
 * Non-blocking delay API.
 *
 * delay_is_done() reflects whether a delay previously started with
 * delay_start_ms() has elapsed. It does not rely on the tick target
 * value itself, so a target that happens to equal 0 (e.g. ms == 0,
 * or systick_ticks wrapping around after ~49.7 days at 1 kHz) is
 * never confused with "no delay running".
 */
void delay_start_ms(uint32_t ms);
uint8_t delay_is_done(void);

#endif /* SYSTICK_H */