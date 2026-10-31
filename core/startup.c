#include <stdint.h>
#include "stm32f103.h"

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

/* Optional C/C++ runtime initialization support */
extern void (*const __init_array_start[])(void);
extern void (*const __init_array_end[])(void);

/* Startup timeout for HSE/PLL readiness checks */
#define RCC_STARTUP_TIMEOUT 1000000U

int main(void);

static void runtime_init(void)
{
    for (void (*const* fn)(void) = __init_array_start; fn < __init_array_end; ++fn)
    {
        (*fn)();
    }
}

__attribute__((noreturn)) static void fault_halt(void)
{
    for (;;)
    {
    }
}

__attribute__((noreturn)) void Reset_Handler(void)
{
    /* Copy .data from FLASH to RAM */
    uint32_t* src = &_sidata;
    uint32_t* dst = &_sdata;

    while (dst < &_edata)
    {
        *dst++ = *src++;
    }

    /* Zero .bss */
    dst = &_sbss;

    while (dst < &_ebss)
    {
        *dst++ = 0U;
    }

    /* Enable external high-speed oscillator (HSE) */
    RCC_CR |= RCC_CR_HSEON;

    uint32_t timeout = RCC_STARTUP_TIMEOUT;

    while ((RCC_CR & RCC_CR_HSERDY) == 0U)
    {
        if (--timeout == 0U)
        {
            fault_halt();
        }
    }

    /*
     * Configure Flash before raising SYSCLK.
     *
     * 72 MHz operation requires:
     *   - 2 wait states
     *   - prefetch buffer enabled
     */
    FLASH_ACR = FLASH_PRFTBE | FLASH_ACR_LATENCY_2;

    /* Configure PLL: HSE (8 MHz) × 9 = 72 MHz */
    SET_REG(RCC_CFGR,
            RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL | RCC_CFGR_PPRE1 |
                RCC_CFGR_PPRE2 | RCC_CFGR_HPRE | RCC_CFGR_ADCPRE | RCC_CFGR_USBPRE,

            RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL_MUL_9 | RCC_CFGR_PPRE1_DIV_2 |
                RCC_CFGR_PPRE2_DIV_1 | RCC_CFGR_HPRE_DIV_1 | RCC_CFGR_ADCPRE_DIV_6 |
                RCC_CFGR_USBPRE_DIV_1_5);

    /* Enable PLL */
    RCC_CR |= RCC_CR_PLLON;

    timeout = RCC_STARTUP_TIMEOUT;

    while ((RCC_CR & RCC_CR_PLLRDY) == 0U)
    {
        if (--timeout == 0U)
        {
            fault_halt();
        }
    }

    /* Switch SYSCLK to PLL */
    SET_REG(RCC_CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);

    while ((RCC_CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
    }

    /* PLL is now the system clock, HSI can be disabled */
    RCC_CR &= ~RCC_CR_HSION;

    /* Run static constructors (safe for C, required for C++) */
    runtime_init();

    /* Enter application */
    (void)main();

    /* main() should never return */
    fault_halt();
}

/* Default weak main */
__attribute__((weak)) int main(void)
{
    for (;;)
    {
    }
}
