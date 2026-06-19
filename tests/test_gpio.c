#include <assert.h>
#include <stdint.h>

#include "gpio_fake_hw.h"
#include "gpio.h"

/* Reset fake registers before each test */
static void reset_fake_regs(void)
{
    GPIO_FAKE.CRL = 0;
    GPIO_FAKE.CRH = 0;
    GPIO_FAKE.IDR = 0;
    GPIO_FAKE.ODR = 0;
    GPIO_FAKE.BSRR = 0;
}

/* --- Test: gpio_setup on pin 0 (CRL lowest field) --- */
static void test_gpio_setup_pin0_crl(void)
{
    reset_fake_regs();

    GPIO_FAKE.CRL = 0xFFFFFFFFU;

    gpio_setup(0, 0, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_PUSHPULL);

    uint32_t field = GPIO_FAKE.CRL & 0xFU;
    assert(field == ((GPIO_MODE_OUTPUT_2MHz | (GPIO_CNF_PUSHPULL << 2)) & 0xFU));
}

/* --- Test: gpio_setup on pin 7 (CRL highest field) --- */
static void test_gpio_setup_pin7_crl(void)
{
    reset_fake_regs();

    GPIO_FAKE.CRL = 0x00000000U;

    gpio_setup(0, 7, GPIO_MODE_OUTPUT_50MHz, GPIO_CNF_OPENDRAIN);

    uint32_t shift = 7U * 4U;
    uint32_t field = (GPIO_FAKE.CRL >> shift) & 0xFU;
    assert(field == ((GPIO_MODE_OUTPUT_50MHz | (GPIO_CNF_OPENDRAIN << 2)) & 0xFU));
}

/* --- Test: gpio_setup on pin 8 (CRH lowest field) --- */
static void test_gpio_setup_pin8_crh(void)
{
    reset_fake_regs();

    GPIO_FAKE.CRH = 0xFFFFFFFFU;

    gpio_setup(0, 8, GPIO_MODE_INPUT, GPIO_CNF_FLOATING);

    uint32_t field = GPIO_FAKE.CRH & 0xFU;
    assert(field == ((GPIO_MODE_INPUT | (GPIO_CNF_FLOATING << 2)) & 0xFU));
}

/* --- Test: gpio_setup on pin 15 (CRH highest field) --- */
static void test_gpio_setup_pin15_crh(void)
{
    reset_fake_regs();

    GPIO_FAKE.CRH = 0x00000000U;

    gpio_setup(0, 15, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PUPD);

    uint32_t shift = 7U * 4U;
    uint32_t field = (GPIO_FAKE.CRH >> shift) & 0xFU;
    assert(field == ((GPIO_MODE_INPUT | (GPIO_CNF_INPUT_PUPD << 2)) & 0xFU));
}

/* --- Test: gpio_set_pin / gpio_clear_pin --- */
static void test_gpio_set_clear_pin(void)
{
    reset_fake_regs();

    gpio_set_pin(0, 5);
    assert(GPIO_FAKE.BSRR == (1U << 5));

    reset_fake_regs();

    gpio_clear_pin(0, 5);
    assert(GPIO_FAKE.BSRR == (1U << (5 + 16U)));
}

/* --- Test: gpio_write_pin --- */
static void test_gpio_write_pin(void)
{
    reset_fake_regs();

    gpio_write_pin(0, 3, 1);
    assert(GPIO_FAKE.BSRR == (1U << 3));

    reset_fake_regs();

    gpio_write_pin(0, 3, 0);
    assert(GPIO_FAKE.BSRR == (1U << (3 + 16U)));
}

/* --- Test: gpio_toggle --- */
static void test_gpio_toggle(void)
{
    reset_fake_regs();

    /* Start low → should set */
    GPIO_FAKE.ODR = 0x00000000U;
    gpio_toggle(0, 2);
    assert(GPIO_FAKE.BSRR == (1U << 2));

    /* Start high → should clear */
    reset_fake_regs();
    GPIO_FAKE.ODR = (1U << 2);
    gpio_toggle(0, 2);
    assert(GPIO_FAKE.BSRR == (1U << (2 + 16U)));
}

/* --- Test: gpio_set_pull --- */
static void test_gpio_set_pull(void)
{
    reset_fake_regs();

    gpio_set_pull(0, 9, 1);
    assert(GPIO_FAKE.ODR & (1U << 9));

    gpio_set_pull(0, 9, 0);
    assert((GPIO_FAKE.ODR & (1U << 9)) == 0);
}

/* --- Test: gpio_read_pin / gpio_read_port --- */
static void test_gpio_read(void)
{
    reset_fake_regs();

    GPIO_FAKE.IDR = (1U << 4) | (1U << 7);

    assert(gpio_read_pin(0, 4) == 1U);
    assert(gpio_read_pin(0, 5) == 0U);

    uint16_t port = gpio_read_port(0);
    assert(port == (uint16_t)GPIO_FAKE.IDR);
}

/* --- Simple test runner --- */
int main(void)
{
    test_gpio_setup_pin0_crl();
    test_gpio_setup_pin7_crl();
    test_gpio_setup_pin8_crh();
    test_gpio_setup_pin15_crh();
    test_gpio_set_clear_pin();
    test_gpio_write_pin();
    test_gpio_toggle();
    test_gpio_set_pull();
    test_gpio_read();

    return 0;
}
