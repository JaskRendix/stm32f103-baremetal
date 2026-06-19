#include "gpio.h"
#include "stm32f103.h"

#define UV_LED_PIN 0U
#define BLUE_LED_PIN 1U
#define ZAPPER_GRID_PIN 2U
#define LDR_SENSOR_PIN 3U

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

    gpio_setup(GPIOA, UV_LED_PIN, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_PUSHPULL);
    gpio_setup(GPIOA, BLUE_LED_PIN, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_PUSHPULL);
    gpio_setup(GPIOA, ZAPPER_GRID_PIN, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_PUSHPULL);

    gpio_setup(GPIOA, LDR_SENSOR_PIN, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PUPD);
    gpio_set_pull(GPIOA, LDR_SENSOR_PIN, 1);

    while (1)
    {
        uint32_t is_dark = (gpio_read_pin(GPIOA, LDR_SENSOR_PIN) == 0);

        if (is_dark)
        {
            gpio_set_pin(GPIOA, ZAPPER_GRID_PIN);

            gpio_set_pin(GPIOA, UV_LED_PIN);
            gpio_clear_pin(GPIOA, BLUE_LED_PIN);
            delay_ms(1500);

            gpio_clear_pin(GPIOA, UV_LED_PIN);
            for (int i = 0; i < 5; i++)
            {
                gpio_set_pin(GPIOA, BLUE_LED_PIN);
                delay_ms(100);
                gpio_clear_pin(GPIOA, BLUE_LED_PIN);
                delay_ms(100);
            }

            gpio_set_pin(GPIOA, UV_LED_PIN);
            gpio_set_pin(GPIOA, BLUE_LED_PIN);
            delay_ms(2000);
        }
        else
        {
            gpio_clear_pin(GPIOA, UV_LED_PIN);
            gpio_clear_pin(GPIOA, BLUE_LED_PIN);
            gpio_clear_pin(GPIOA, ZAPPER_GRID_PIN);
            delay_ms(5000);
        }
    }
}
