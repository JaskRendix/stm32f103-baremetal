#include "project_config.h"
#include "stm32f103.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"

int main(void)
{
    /* Enable clock for LED port */
    RCC_APB2ENR |= LED_PORT_RCC_EN;

    /* Configure LED pin */
    gpio_setup(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_PUSHPULL);

    /* Start SysTick 1ms timebase */
    systick_init();

    /* Bring up UART1 for diagnostic output */
    uart_init(UART_BAUD_115200);
    (void)uart_send_string("LED blink started\r\n");

    uint32_t last = systick_ms();
    for (;;)
    {
        if ((systick_ms() - last) >= 250)
        {
            gpio_toggle(LED_PORT, LED_PIN);
            last = systick_ms();
        }
    }
}