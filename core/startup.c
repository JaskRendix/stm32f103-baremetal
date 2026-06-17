#include "stm32f103.h"

extern unsigned int _sidata;
extern unsigned int _sdata;
extern unsigned int _edata;
extern unsigned int _sbss;
extern unsigned int _ebss;

int main(void);

void Reset_Handler(void)
{
    /* Copy .data from FLASH to RAM */
    unsigned int* src = &_sidata;
    unsigned int* dst = &_sdata;

    while (dst < &_edata)
    {
        *dst++ = *src++;
    }

    /* Zero .bss */
    dst = &_sbss;
    while (dst < &_ebss)
    {
        *dst++ = 0;
    }

    /* Enable HSE */
    RCC_CR |= RCC_CR_HSEON;
    while ((RCC_CR & RCC_CR_HSERDY) == 0)
    {
    }

    /* Switch SYSCLK to HSE */
    SET_REG(RCC_CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSE);
    while ((RCC_CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE)
    {
    }

    /* Disable HSI */
    SET_REG(RCC_CR, RCC_CR_HSION, 0);

    /* Flash latency for 72 MHz */
    SET_REG(FLASH_ACR, FLASH_ACR_LATENCY | FLASH_HLFCYA | FLASH_PRFTBE, FLASH_ACR_LATENCY_2);

    /* PLL config: HSE → 72 MHz */
    SET_REG(RCC_CFGR,
            RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL | RCC_CFGR_PPRE1 |
                RCC_CFGR_PPRE2 | RCC_CFGR_HPRE | RCC_CFGR_ADCPRE | RCC_CFGR_USBPRE,

            0 | RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL_MUL_9 | RCC_CFGR_PPRE1_DIV_2 |
                RCC_CFGR_PPRE2_DIV_1 | RCC_CFGR_HPRE_DIV_1 | RCC_CFGR_ADCPRE_DIV_6 |
                RCC_CFGR_USBPRE_DIV_1_5);

    /* Enable PLL */
    RCC_CR |= RCC_CR_PLLON;
    while ((RCC_CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Switch SYSCLK to PLL */
    SET_REG(RCC_CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    while ((RCC_CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
    }

    /* Jump to main */
    main();

    /* If main returns, loop forever */
    while (1)
    {
    }
}

/* Default weak main */
__attribute__((weak)) int main(void)
{
    while (1)
    {
    }
}
