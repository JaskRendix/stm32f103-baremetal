#include "gpio.h"
#include "stm32f103.h"

/* --------------------------------------------------------------------------
 *  Internal helper: compute CRL/CRH pointer and bitfield
 * -------------------------------------------------------------------------- */
static inline volatile uint32_t* gpio_cfg_reg(uint32_t port, uint8_t pin)
{
    return (pin < 8U) ? &GPIOx_CRL(port) : &GPIOx_CRH(port);
}

static inline uint32_t gpio_cfg_shift(uint8_t pin) { return (uint32_t)(pin & 7U) * 4U; }

/* --------------------------------------------------------------------------
 *  Raw setup
 * -------------------------------------------------------------------------- */
void gpio_setup_raw(uint32_t port, uint8_t pin, gpio_mode_t mode, uint32_t cnf_bits)
{
    volatile uint32_t* reg = gpio_cfg_reg(port, pin);
    uint32_t shift = gpio_cfg_shift(pin);

    uint32_t mask = (0xFU << shift); /* MODE[1:0] + CNF[1:0] */
    uint32_t value = ((mode | ((cnf_bits & 0x3U) << 2U)) & 0xFU) << shift;

    /* RMW on CRL/CRH: caller is responsible for any critical section needs */
    *reg = (*reg & ~mask) | value;
}

/* --------------------------------------------------------------------------
 *  Typed input/output setup
 * -------------------------------------------------------------------------- */
void gpio_setup_input(uint32_t port, uint8_t pin, gpio_cnf_input_t cnf, gpio_pull_t pull)
{
    /* MODE = 0b00 for input */
    gpio_setup_raw(port, pin, GPIO_MODE_INPUT, (uint32_t)cnf);

    if (cnf == GPIO_INPUT_CNF_PUPD)
    {
        gpio_set_pull(port, pin, pull);
    }
}

void gpio_setup_output(uint32_t port, uint8_t pin, gpio_mode_t mode, gpio_cnf_output_t cnf)
{
    /* MODE != 0b00 for output */
    gpio_setup_raw(port, pin, mode, (uint32_t)cnf);
}
