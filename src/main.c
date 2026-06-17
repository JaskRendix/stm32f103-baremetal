#include "project_config.h"
#include "stm32f103.h"
#include "gpio.h"
#include "delay.h"

int main(void)
{
    /* Enable clock for LED port */
    RCC_APB2ENR |= LED_PORT_RCC_EN;

    /* Configure LED pin */
    gpio_setup(LED_PORT, LED_PIN, GPIO_CR_MODE_OUT_2MHZ, GPIO_CR_CNF_OUT_PUSHPULL);

    for (;;)
    {
        GPIO_PIN_TOG(LED_PORT, LED_PIN);

        for (int i = 0; i < 250; i++)
        {
            DELAY_US(1000);
        }
    }
}
