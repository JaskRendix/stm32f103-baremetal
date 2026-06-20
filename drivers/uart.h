#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "project_config.h"
#include "stm32f103.h"

/*
 * USART1 driver (TX = PA9, RX = PA10 on STM32F103).
 *
 * USART1 is clocked from APB2. CPU_HZ in project_config.h is assumed
 * to be the APB2 clock as actually configured in core/startup.c
 * (PPRE2_DIV_1 -> APB2 == SYSCLK == CPU_HZ). If PPRE2 is ever changed
 * to divide APB2, the baud rate calculation below must be updated to
 * use the real APB2 clock instead of CPU_HZ directly.
 */

typedef enum
{
    UART_BAUD_9600 = 9600U,
    UART_BAUD_115200 = 115200U
} uart_baud_t;

typedef enum
{
    UART_OK = 0,
    UART_TIMEOUT = -1
} uart_status_t;

/* Default timeout in busy-wait iterations for blocking operations.
 * Not a calibrated time value -- just a bound to guarantee these
 * calls cannot hang forever on dead/missing hardware. */
#define UART_DEFAULT_TIMEOUT 100000UL

void uart_init(uart_baud_t baud);

/* Returns UART_OK on success, UART_TIMEOUT if TXE never set within
 * UART_DEFAULT_TIMEOUT iterations. */
uart_status_t uart_send_char(char c);

/* Sends the full string. Stops and returns UART_TIMEOUT on the first
 * character that times out; otherwise returns UART_OK. */
uart_status_t uart_send_string(const char* str);

/* Reads one received byte into *out. Returns UART_OK once RXNE is set
 * and a byte has been read, or UART_TIMEOUT if no byte arrives within
 * UART_DEFAULT_TIMEOUT iterations. *out is left unchanged on timeout. */
uart_status_t uart_read_char(char* out);

/* Fast status-check helpers */
static inline uint32_t uart_tx_ready(void) { return (USART1_SR & USART_SR_TXE) != 0U; }

static inline uint32_t uart_rx_ready(void) { return (USART1_SR & USART_SR_RXNE) != 0U; }

#endif /* UART_H */