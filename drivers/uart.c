#include <stddef.h>
#include "uart.h"
#include "gpio.h"

/*
 * USARTDIV = f_CK / (16 * baud), stored in BRR as:
 *   bits 15:4 = mantissa
 *   bits 3:0  = fraction in 1/16ths
 *
 * BRR = round(USARTDIV * 16)
 *     = round(f_CK / baud)
 */
static uint16_t uart_compute_brr(uint32_t pclk, uint32_t baud)
{
    if ((pclk == 0U) || (baud == 0U))
    {
        return 0U;
    }

    return (uint16_t)((pclk + (baud / 2U)) / baud);
}

/*
 * Check and clear UART error conditions.
 *
 * On STM32F1, many error flags are cleared by:
 *   1. Reading SR
 *   2. Reading DR
 */
static uart_status_t uart_check_errors(void)
{
    uint32_t errors = USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE;

    if ((USART1_SR & errors) != 0U)
    {
        volatile uint32_t dummy;

        dummy = USART1_SR;
        dummy = USART1_DR;
        (void)dummy;

        return UART_ERROR;
    }

    return UART_OK;
}

void uart_init(uart_baud_t baud)
{
    /* Enable clocks */
    RCC_APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

    /* PA9  = TX */
    gpio_setup(GPIOA, 9, GPIO_MODE_OUTPUT_50MHz, GPIO_CNF_AF_PUSHPULL);

    /* PA10 = RX */
    gpio_setup(GPIOA, 10, GPIO_MODE_INPUT, GPIO_CNF_FLOATING);

    /* Configure baud rate */
    USART1_BRR = uart_compute_brr(CPU_HZ, (uint32_t)baud);

    /* Known configuration state:
     * 8 data bits
     * no parity
     * 1 stop bit
     */
    USART1_CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

uart_status_t uart_try_send_char(char c)
{
    if (!uart_tx_ready())
    {
        return UART_BUSY;
    }

    USART1_DR = (uint32_t)(uint8_t)c;

    return UART_OK;
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
    if (str == NULL)
    {
        return UART_INVALID_ARG;
    }

    while (*str != '\0')
    {
        uart_status_t status = uart_send_char(*str++);

        if (status != UART_OK)
        {
            return status;
        }
    }

    return UART_OK;
}

uart_status_t uart_try_read_char(char* out)
{
    if (out == NULL)
    {
        return UART_INVALID_ARG;
    }

    if (uart_check_errors() != UART_OK)
    {
        return UART_ERROR;
    }

    if (!uart_rx_ready())
    {
        return UART_NO_DATA;
    }

    *out = (char)(uint8_t)USART1_DR;

    return UART_OK;
}

uart_status_t uart_read_char(char* out)
{
    uint32_t timeout = UART_DEFAULT_TIMEOUT;

    if (out == NULL)
    {
        return UART_INVALID_ARG;
    }

    while (!uart_rx_ready())
    {
        if (uart_check_errors() != UART_OK)
        {
            return UART_ERROR;
        }

        if (--timeout == 0U)
        {
            return UART_TIMEOUT;
        }
    }

    *out = (char)(uint8_t)USART1_DR;

    return UART_OK;
}

uart_status_t uart_flush(void)
{
    uint32_t timeout = UART_DEFAULT_TIMEOUT;

    while ((USART1_SR & USART_SR_TC) == 0U)
    {
        if (--timeout == 0U)
        {
            return UART_TIMEOUT;
        }
    }

    return UART_OK;
}
