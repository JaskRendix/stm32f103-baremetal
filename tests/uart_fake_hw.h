#ifndef UART_FAKE_HW_H
#define UART_FAKE_HW_H

#include <stdint.h>
#include "stm32f103.h"

/*
 * Host-side fake USART1 register block, mirroring tests/gpio_fake_hw.h.
 *
 * This header is force-included (via -include, see tests/CMakeLists.txt)
 * ahead of stm32f103.h when building host tests, so that the real
 * USART1_SR / USART1_DR / USART1_BRR / USART1_CR1 macros used by
 * drivers/uart.c resolve to this fake struct instead of real memory-mapped
 * registers.
 */

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;

    /*
     * Test-control field, not a real register. When nonzero, TXE
     * (USART_SR_TXE) is forced to never appear set in SR, regardless
     * of what test code writes to SR.CR1, so tests can deterministically
     * exercise the uart_send_char timeout path without spinning the
     * real UART_DEFAULT_TIMEOUT iteration count.
     */
    volatile uint32_t force_txe_stuck;
} uart_fake_regs_t;

extern uart_fake_regs_t USART1_FAKE;

/*
 * uart_init() writes to RCC_APB2ENR directly to gate the USART1 and
 * GPIOA peripheral clocks. On host builds RCC_BASE is not a real
 * memory-mapped address, so RCC_APB2ENR must also be faked here, or
 * uart_init() segfaults the moment it runs on the host. This fake
 * backing field is shared with anything else in a future host test
 * that also calls into code touching RCC_APB2ENR.
 */
extern uint32_t RCC_APB2ENR_FAKE;

#undef RCC_APB2ENR
#define RCC_APB2ENR RCC_APB2ENR_FAKE

#undef USART1_SR
#undef USART1_DR
#undef USART1_BRR
#undef USART1_CR1
#undef USART1_CR2
#undef USART1_CR3
#undef USART1_GTPR

/*
 * SR is read through a helper so force_txe_stuck can mask TXE on read
 * without affecting other bits or requiring callers to change.
 *
 * NOTE: this makes USART1_SR a read-only expression in this fake build,
 * not an lvalue. Code that only reads USART1_SR (the current uart.c)
 * works fine. If any driver code is ever changed to write USART1_SR
 * directly (clearing flags, etc.), this fake will fail to compile and
 * will need a real backing field plus a separate write path.
 */
static inline uint32_t uart_fake_read_sr(void)
{
    uint32_t sr = USART1_FAKE.SR;
    if (USART1_FAKE.force_txe_stuck)
    {
        sr &= ~(1U << 7U); /* USART_SR_TXE */
    }
    return sr;
}

#define USART1_SR uart_fake_read_sr()
#define USART1_DR USART1_FAKE.DR
#define USART1_BRR USART1_FAKE.BRR
#define USART1_CR1 USART1_FAKE.CR1
#define USART1_CR2 USART1_FAKE.CR2
#define USART1_CR3 USART1_FAKE.CR3
#define USART1_GTPR USART1_FAKE.GTPR

#endif /* UART_FAKE_HW_H */