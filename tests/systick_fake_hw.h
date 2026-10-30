#ifndef SYSTICK_FAKE_HW_H
#define SYSTICK_FAKE_HW_H

#include <stdint.h>
#include "stm32f103.h"

/* Fake SysTick register block */
typedef struct
{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} systick_fake_t;

/* One fake instance */
extern systick_fake_t SysTick_FAKE;

/* Reset fake registers to power-on state */
void systick_fake_reset(void);

/* Override SysTick pointer */

#ifdef SysTick
#undef SysTick
#endif

#define SysTick ((systick_fake_t*)&SysTick_FAKE)

#endif /* SYSTICK_FAKE_HW_H */
