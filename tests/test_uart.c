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
    USART1_FAKE.force_tc_stuck = 0;

    RCC_APB2ENR_FAKE = 0;

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
    USART1_FAKE.SR |= USART_SR_TXE; /* USART_SR_TXE */

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
    USART1_FAKE.SR |= USART_SR_TXE; /* USART_SR_TXE */

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

/* --- Test: uart_read_char reads a byte and returns UART_OK when RXNE is set --- */
static void test_uart_read_char_success(void)
{
    reset_fake_regs();
    USART1_FAKE.SR |= USART_SR_RXNE; /* USART_SR_RXNE */
    USART1_FAKE.DR = (uint32_t)'Q';

    char c = 0;
    uart_status_t status = uart_read_char(&c);

    assert(status == UART_OK);
    assert(c == 'Q');
    printf("[PASS] test_uart_read_char_success\n");
}

/* --- Test: uart_read_char times out and leaves *out unchanged when
 *     RXNE never sets, instead of hanging --- */
static void test_uart_read_char_timeout(void)
{
    reset_fake_regs();
    /* RXNE never set: USART1_FAKE.SR left at 0 from reset_fake_regs() */

    char c = 'Z';
    uart_status_t status = uart_read_char(&c);

    assert(status == UART_TIMEOUT);
    assert(c == 'Z'); /* unchanged on timeout */
    printf("[PASS] test_uart_read_char_timeout\n");
}

static void test_uart_send_string_null(void)
{
    reset_fake_regs();

    uart_status_t status = uart_send_string(NULL);

    assert(status == UART_INVALID_ARG);

    printf("[PASS] test_uart_send_string_null\n");
}

static void test_uart_read_char_null(void)
{
    reset_fake_regs();

    uart_status_t status = uart_read_char(NULL);

    assert(status == UART_INVALID_ARG);

    printf("[PASS] test_uart_read_char_null\n");
}

static void test_uart_try_send_char_success(void)
{
    reset_fake_regs();

    USART1_FAKE.SR |= USART_SR_TXE;

    uart_status_t status = uart_try_send_char('A');

    assert(status == UART_OK);
    assert(USART1_FAKE.DR == 'A');

    printf("[PASS] test_uart_try_send_char_success\n");
}

static void test_uart_try_send_char_busy(void)
{
    reset_fake_regs();

    uart_status_t status = uart_try_send_char('A');

    assert(status == UART_BUSY);

    printf("[PASS] test_uart_try_send_char_busy\n");
}

static void test_uart_try_read_char_success(void)
{
    reset_fake_regs();

    USART1_FAKE.SR |= USART_SR_RXNE;
    USART1_FAKE.DR = 'K';

    char c = 0;

    uart_status_t status = uart_try_read_char(&c);

    assert(status == UART_OK);
    assert(c == 'K');

    printf("[PASS] test_uart_try_read_char_success\n");
}

static void test_uart_try_read_char_no_data(void)
{
    reset_fake_regs();

    char c = 0;

    uart_status_t status = uart_try_read_char(&c);

    assert(status == UART_NO_DATA);

    printf("[PASS] test_uart_try_read_char_no_data\n");
}

static void test_uart_try_read_char_null(void)
{
    reset_fake_regs();

    uart_status_t status = uart_try_read_char(NULL);

    assert(status == UART_INVALID_ARG);

    printf("[PASS] test_uart_try_read_char_null\n");
}

static void test_uart_flush_success(void)
{
    reset_fake_regs();

    USART1_FAKE.SR |= USART_SR_TC;

    uart_status_t status = uart_flush();

    assert(status == UART_OK);

    printf("[PASS] test_uart_flush_success\n");
}

static void test_uart_flush_timeout(void)
{
    reset_fake_regs();

    USART1_FAKE.force_tc_stuck = 1;

    uart_status_t status = uart_flush();

    assert(status == UART_TIMEOUT);

    printf("[PASS] test_uart_flush_timeout\n");
}

static void test_uart_read_char_error_flags(void)
{
    reset_fake_regs();

    USART1_FAKE.SR |= USART_SR_ORE;

    char c = 0;

    assert(uart_read_char(&c) == UART_ERROR);

    reset_fake_regs();
    USART1_FAKE.SR |= USART_SR_FE;

    assert(uart_read_char(&c) == UART_ERROR);

    reset_fake_regs();
    USART1_FAKE.SR |= USART_SR_NE;

    assert(uart_read_char(&c) == UART_ERROR);

    reset_fake_regs();
    USART1_FAKE.SR |= USART_SR_PE;

    assert(uart_read_char(&c) == UART_ERROR);

    printf("[PASS] test_uart_read_char_error_flags\n");
}

static void test_uart_try_read_char_error(void)
{
    reset_fake_regs();

    USART1_FAKE.SR |= USART_SR_ORE;

    char c = 0;

    assert(uart_try_read_char(&c) == UART_ERROR);

    printf("[PASS] test_uart_try_read_char_error\n");
}

static void test_uart_init_enables_clocks(void)
{
    reset_fake_regs();

    uart_init(UART_BAUD_115200);

    uint32_t expected = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

    assert((RCC_APB2ENR_FAKE & expected) == expected);

    printf("[PASS] test_uart_init_enables_clocks\n");
}

static void test_uart_init_configures_gpio(void)
{
    reset_fake_regs();

    uart_init(UART_BAUD_115200);

    /* Verify PA9 and PA10 configuration.
       Exact values depend on gpio_setup()
       implementation and register layout. */

    assert(GPIO_FAKE.CRH != 0);

    printf("[PASS] test_uart_init_configures_gpio\n");
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
    test_uart_read_char_success();
    test_uart_read_char_timeout();
    test_uart_send_string_null();
    test_uart_read_char_null();
    test_uart_try_send_char_success();
    test_uart_try_send_char_busy();
    test_uart_try_read_char_success();
    test_uart_try_read_char_no_data();
    test_uart_try_read_char_null();
    test_uart_flush_success();
    test_uart_flush_timeout();
    test_uart_read_char_error_flags();
    test_uart_try_read_char_error();
    test_uart_init_enables_clocks();
    test_uart_init_configures_gpio();

    printf("All test_uart cases passed.\n");
    return 0;
}