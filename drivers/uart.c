#include "uart.h"
#include "gpio.h"

/*
 * USARTDIV = f_CK / (16 * baud), stored in BRR as:
 *   bits 15:4 = mantissa (integer part of USARTDIV)
 *   bits 3:0  = fraction (fractional part of USARTDIV, in 1/16ths)
 *
 * The packed register value (mantissa << 4) | fraction is numerically
 * equal to round(USARTDIV * 16) == round(f_CK / baud). That single
 * rounded division IS the BRR value to write -- no further shifting
 * is needed, since USARTDIV's own fractional part is already expressed
 * in sixteenths.
 *
 * Computed in fixed point (round-to-nearest) to avoid floating point
 * on a Cortex-M3 without an FPU.
 */
static uint16_t uart_compute_brr(uint32_t pclk, uint32_t baud)
{
    /* round(pclk / baud) using integer arithmetic: (pclk + baud/2) / baud */
    uint32_t brr = (pclk + (baud / 2U)) / baud;

    return (uint16_t)brr;
}

void uart_init(uart_baud_t baud)
{
    /* 1. Enable clocks for USART1 and GPIOA */
    RCC_APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

    /* 2. Configure PA9 (TX) as Alternate Function Push-Pull, 50MHz */
    gpio_setup(GPIOA, 9, GPIO_MODE_OUTPUT_50MHz, GPIO_CNF_AF_PUSHPULL);

    /* 3. Configure PA10 (RX) as Floating Input */
    gpio_setup(GPIOA, 10, GPIO_MODE_INPUT, GPIO_CNF_FLOATING);

    /* 4. Baud rate. USART1 is on APB2, which runs at CPU_HZ given the
     * PPRE2_DIV_1 setting in core/startup.c. */
    USART1_BRR = uart_compute_brr(CPU_HZ, (uint32_t)baud);

    /* 5. Enable USART, transmitter, and receiver */
    USART1_CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

uart_status_t uart_send_char(char c)
{
    uint32_t timeout = UART_DEFAULT_TIMEOUT;

    while (!uart_tx_ready())
    {
        if (--timeout == 0U)
        {
            return UART_TIMEOUT;
        }
    }

    USART1_DR = (uint32_t)(uint8_t)c;
    return UART_OK;
}

uart_status_t uart_send_string(const char* str)
{
    while (*str)
    {
        uart_status_t status = uart_send_char(*str++);
        if (status != UART_OK)
        {
            return status;
        }
    }
    return UART_OK;
}

uart_status_t uart_read_char(char* out)
{
    uint32_t timeout = UART_DEFAULT_TIMEOUT;

    while (!uart_rx_ready())
    {
        if (--timeout == 0U)
        {
            return UART_TIMEOUT;
        }
    }

    *out = (char)(uint8_t)USART1_DR;
    return UART_OK;
}