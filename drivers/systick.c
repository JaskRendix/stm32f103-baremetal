#include "systick.h"

/* Global millisecond counter */
volatile uint32_t systick_ticks = 0;

/* For non-blocking delay */
static volatile uint32_t delay_target = 0;

void SysTick_Handler(void)
{
    systick_ticks++;

    if (delay_target && systick_ticks >= delay_target)
    {
        delay_target = 0;
    }
}

uint32_t systick_ms(void) { return systick_ticks; }

void systick_init(void)
{
    /* Reload = CPU_HZ / 1000 - 1 */
    SysTick->LOAD = (CPU_HZ / 1000U) - 1U;

    SysTick->VAL = 0;

    /* Enable SysTick: processor clock, interrupt, counter */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void delay_start_ms(uint32_t ms) { delay_target = systick_ticks + ms; }

uint8_t delay_is_done(void) { return (delay_target == 0); }
