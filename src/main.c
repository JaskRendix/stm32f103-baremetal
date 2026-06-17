#include "project_config.h"
#include "stm32f103.h"
#include "gpio.h"
#include "delay.h"

int main(void)
{
    /* Enable clock for LED port */
    RCC_APB2ENR |= LED_PORT_RCC_EN;

    /* Configure LED pin */
    gpio_setup(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_PUSHPULL);

    for (;;)
    {
        gpio_toggle(LED_PORT, LED_PIN);

        for (int i = 0; i < 250; i++)
        {
            DELAY_US(1000);
        }
    }
}
