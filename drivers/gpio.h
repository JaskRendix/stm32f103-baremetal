#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f103.h"

/* --------------------------------------------------------------------------
 *  Strongly‑typed mode and configuration enums
 * -------------------------------------------------------------------------- */
typedef enum
{
    GPIO_MODE_INPUT = 0x0,
    GPIO_MODE_OUTPUT_10MHz = 0x1,
    GPIO_MODE_OUTPUT_2MHz = 0x2,
    GPIO_MODE_OUTPUT_50MHz = 0x3
} gpio_mode_t;

typedef enum
{
    /* Input modes */
    GPIO_CNF_ANALOG = 0x0,
    GPIO_CNF_FLOATING = 0x1,
    GPIO_CNF_INPUT_PUPD = 0x2,

    /* Output modes */
    GPIO_CNF_PUSHPULL = 0x0,
    GPIO_CNF_OPENDRAIN = 0x1,
    GPIO_CNF_AF_PUSHPULL = 0x2,
    GPIO_CNF_AF_OPENDRAIN = 0x3
} gpio_cnf_t;

/* --------------------------------------------------------------------------
 *  Pin configuration
 * -------------------------------------------------------------------------- */
void gpio_setup(uint32_t port, uint8_t pin, gpio_mode_t mode, gpio_cnf_t cnf);

/* --------------------------------------------------------------------------
 *  Pin write helpers
 * -------------------------------------------------------------------------- */
static inline void gpio_write_pin(uint32_t port, uint8_t pin, uint8_t value)
{
    if (value)
    {
        GPIOx_BSRR(port) = (1U << pin);
    }
    else
    {
        GPIOx_BSRR(port) = (1U << (pin + 16U));
    }
}

static inline void gpio_set_pin(uint32_t port, uint8_t pin) { GPIOx_BSRR(port) = (1U << pin); }

static inline void gpio_clear_pin(uint32_t port, uint8_t pin)
{
    GPIOx_BSRR(port) = (1U << (pin + 16U));
}

/* --------------------------------------------------------------------------
 *  Pin read helpers
 * -------------------------------------------------------------------------- */
static inline uint32_t gpio_read_pin(uint32_t port, uint8_t pin)
{
    return (GPIOx_IDR(port) >> pin) & 1U;
}

static inline uint16_t gpio_read_port(uint32_t port) { return (uint16_t)GPIOx_IDR(port); }

/* --------------------------------------------------------------------------
 *  Pull‑up / pull‑down configuration
 * -------------------------------------------------------------------------- */
static inline void gpio_set_pull(uint32_t port, uint8_t pin, uint8_t pull_up)
{
    if (pull_up)
    {
        GPIOx_ODR(port) |= (1U << pin);
    }
    else
    {
        GPIOx_ODR(port) &= ~(1U << pin);
    }
}

/* --------------------------------------------------------------------------
 *  Toggle helper
 * -------------------------------------------------------------------------- */
static inline void gpio_toggle(uint32_t port, uint8_t pin)
{
    if (GPIOx_ODR(port) & (1U << pin))
    {
        gpio_clear_pin(port, pin);
    }
    else
    {
        gpio_set_pin(port, pin);
    }
}

#endif /* GPIO_H */
