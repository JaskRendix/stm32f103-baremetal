#include "systick.h"

/* Monotonic 1 ms system tick counter */
volatile uint32_t systick_ticks = 0;

/* For non-blocking delay */
static volatile uint32_t delay_target = 0;
static volatile bool delay_active = false;

void SysTick_Handler(void)
{
    systick_ticks++;

    /* Use signed delta comparison to safely handle 32-bit tick wraparound */
    if (delay_active && ((int32_t)(systick_ticks - delay_target) >= 0))
    {
        delay_active = false;
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

void delay_start_ms(uint32_t ms)
{
    delay_target = systick_ticks + ms;
    /*
     * Setting delay_active = true after delay_target avoids a race where
     * SysTick_Handler could observe delay_active set but delay_target
     * still holding its previous value.
     */
    delay_active = true;

    /* ms == 0 means "already elapsed" by definition */
    if (ms == 0U)
    {
        delay_active = false;
    }
}

bool delay_is_done(void) { return !delay_active; }
