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
 * (PPRE2_DIV_1 -> APB2 == SYSCLK == CPU_HZ). If PPRE2 is changed,
 * update the baud-rate source clock accordingly.
 */

typedef enum
{
    UART_BAUD_9600 = 9600U,
    UART_BAUD_115200 = 115200U
} uart_baud_t;

typedef enum
{
    UART_OK = 0,
    UART_TIMEOUT = -1,
    UART_INVALID_ARG = -2,
    UART_ERROR = -3,
    UART_NO_DATA = -4,
    UART_BUSY = -5
} uart_status_t;

/*
 * Busy-wait iteration limit.
 *
 * This is NOT a calibrated time value. Actual elapsed time depends
 * on compiler optimization level and CPU clock frequency.
 */
#define UART_DEFAULT_TIMEOUT ((uint32_t)100000U)

void uart_init(uart_baud_t baud);

/* Blocking transmit */
uart_status_t uart_send_char(char c);

/* Blocking string transmit */
uart_status_t uart_send_string(const char* str);

/* Blocking receive */
uart_status_t uart_read_char(char* out);

/* Non-blocking helpers */
uart_status_t uart_try_send_char(char c);
uart_status_t uart_try_read_char(char* out);

/* Wait until the final byte has completely left the UART */
uart_status_t uart_flush(void);

/* Fast status-check helpers */
static inline uint32_t uart_tx_ready(void) { return (USART1_SR & USART_SR_TXE) != 0U; }

static inline uint32_t uart_rx_ready(void) { return (USART1_SR & USART_SR_RXNE) != 0U; }

#endif /* UART_H */
