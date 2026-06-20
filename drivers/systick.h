#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include "stm32f103.h"
#include "project_config.h"

/*
 * SysTick provides a 1 ms timebase.
 * CPU_HZ must be defined in project_config.h.
 */

void systick_init(void);

/* Returns system uptime in milliseconds */
uint32_t systick_ms(void);

/* Non-blocking delay API */
void delay_start_ms(uint32_t ms);
uint8_t delay_is_done(void);

#endif /* SYSTICK_H */
