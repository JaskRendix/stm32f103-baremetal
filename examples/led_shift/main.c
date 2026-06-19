#include "gpio.h"
#include "stm32f103.h"

void delay_ms(uint32_t ms)
{
    volatile uint32_t count = ms * 800U;
    while (count--)
    {
        __asm__("nop");
    }
}

int main(void)
{
    RCC_APB2ENR |= RCC_APB2ENR_IOPAEN;

    for (uint8_t p = 0; p < 4; p++)
    {
        gpio_setup(GPIOA, p, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_PUSHPULL);
    }

    uint8_t i = 0;

    while (1)
    {
        gpio_set_pin(GPIOA, i);
        delay_ms(200);
        gpio_clear_pin(GPIOA, i);
        i = (i + 1) % 4;
    }
}
