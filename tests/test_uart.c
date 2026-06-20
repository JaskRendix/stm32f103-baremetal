#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "gpio_fake_hw.h"
#include "uart_fake_hw.h"
#include "uart.h"

/* Reset fake USART1 + GPIO registers before each test */
static void reset_fake_regs(void)
{
    USART1_FAKE.SR = 0;
    USART1_FAKE.DR = 0;
    USART1_FAKE.BRR = 0;
    USART1_FAKE.CR1 = 0;
    USART1_FAKE.CR2 = 0;
    USART1_FAKE.CR3 = 0;
    USART1_FAKE.GTPR = 0;
    USART1_FAKE.force_txe_stuck = 0;

    GPIO_FAKE.CRL = 0;
    GPIO_FAKE.CRH = 0;
    GPIO_FAKE.IDR = 0;
    GPIO_FAKE.ODR = 0;
    GPIO_FAKE.BSRR = 0;
}

/* --- Test: uart_init computes correct BRR for 115200 baud --- */
static void test_uart_init_baud_115200(void)
{
    reset_fake_regs();
    uart_init(UART_BAUD_115200);

    /* CPU_HZ = 72000000, baud = 115200
     * BRR = round(72000000 / 115200) = 625 = 0x0271
     * (mantissa = 39, fraction = 1; matches reference manual formula) */
    assert(USART1_FAKE.BRR == 625U);
    printf("[PASS] test_uart_init_baud_115200\n");
}

/* --- Test: uart_init computes correct BRR for 9600 baud --- */
static void test_uart_init_baud_9600(void)
{
    reset_fake_regs();
    uart_init(UART_BAUD_9600);

    /* CPU_HZ = 72000000, baud = 9600
     * BRR = round(72000000 / 9600) = 7500 = 0x1D4C
     * (mantissa = 468, fraction = 12; matches the worked example in
     * the STM32F103 reference manual for this exact clock/baud pair) */
    assert(USART1_FAKE.BRR == 7500U);
    printf("[PASS] test_uart_init_baud_9600\n");
}

/* --- Test: uart_init enables UE, TE, RE in CR1 --- */
static void test_uart_init_enables_uart(void)
{
    reset_fake_regs();
    uart_init(UART_BAUD_115200);

    uint32_t expected = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    assert((USART1_FAKE.CR1 & expected) == expected);
    printf("[PASS] test_uart_init_enables_uart\n");
}

/* --- Test: uart_send_char writes DR and returns UART_OK when TXE is set --- */
static void test_uart_send_char_success(void)
{
    reset_fake_regs();
    USART1_FAKE.SR |= (1U << 7U); /* USART_SR_TXE */

    uart_status_t status = uart_send_char('A');

    assert(status == UART_OK);
    assert(USART1_FAKE.DR == (uint32_t)'A');
    printf("[PASS] test_uart_send_char_success\n");
}

/* --- Test: uart_send_char times out and returns UART_TIMEOUT when TXE
 *     never sets, instead of hanging --- */
static void test_uart_send_char_timeout(void)
{
    reset_fake_regs();
    USART1_FAKE.force_txe_stuck = 1; /* TXE will never read as set */

    uart_status_t status = uart_send_char('X');

    assert(status == UART_TIMEOUT);
    printf("[PASS] test_uart_send_char_timeout\n");
}

/* --- Test: uart_send_string sends every character when TXE is set --- */
static void test_uart_send_string_success(void)
{
    reset_fake_regs();
    USART1_FAKE.SR |= (1U << 7U); /* USART_SR_TXE */

    uart_status_t status = uart_send_string("hi");

    /* DR holds only the last character written; success is what we
     * can directly verify here without a fake TX FIFO/history buffer. */
    assert(status == UART_OK);
    assert(USART1_FAKE.DR == (uint32_t)'i');
    printf("[PASS] test_uart_send_string_success\n");
}

/* --- Test: uart_send_string stops and reports timeout if TXE never sets --- */
static void test_uart_send_string_timeout(void)
{
    reset_fake_regs();
    USART1_FAKE.force_txe_stuck = 1;

    uart_status_t status = uart_send_string("hi");

    assert(status == UART_TIMEOUT);
    printf("[PASS] test_uart_send_string_timeout\n");
}

/* --- Simple test runner --- */
int main(void)
{
    printf("Running test_uart...\n");

    test_uart_init_baud_115200();
    test_uart_init_baud_9600();
    test_uart_init_enables_uart();
    test_uart_send_char_success();
    test_uart_send_char_timeout();
    test_uart_send_string_success();
    test_uart_send_string_timeout();

    printf("All 7 test_uart cases passed.\n");
    return 0;
}