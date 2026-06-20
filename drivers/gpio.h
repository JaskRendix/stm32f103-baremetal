#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f103.h"

/* --------------------------------------------------------------------------
 *  Mode enums
 * -------------------------------------------------------------------------- */
typedef enum
{
    GPIO_MODE_INPUT = 0x0,
    GPIO_MODE_OUTPUT_10MHz = 0x1,
    GPIO_MODE_OUTPUT_2MHz = 0x2,
    GPIO_MODE_OUTPUT_50MHz = 0x3
} gpio_mode_t;

/* --------------------------------------------------------------------------
 *  Strongly-typed CNF enums
 * -------------------------------------------------------------------------- */

/* Input configuration (CNF[1:0] when MODE = 0b00) */
typedef enum
{
    GPIO_INPUT_CNF_ANALOG = 0x0,   /* Analog input */
    GPIO_INPUT_CNF_FLOATING = 0x1, /* Floating input */
    GPIO_INPUT_CNF_PUPD = 0x2      /* Input with pull-up / pull-down */
} gpio_cnf_input_t;

/* Output configuration (CNF[1:0] when MODE != 0b00) */
typedef enum
{
    GPIO_OUTPUT_CNF_PUSHPULL = 0x0,    /* General purpose push-pull */
    GPIO_OUTPUT_CNF_OPENDRAIN = 0x1,   /* General purpose open-drain */
    GPIO_OUTPUT_CNF_AF_PUSHPULL = 0x2, /* Alternate function push-pull */
    GPIO_OUTPUT_CNF_AF_OPENDRAIN = 0x3 /* Alternate function open-drain */
} gpio_cnf_output_t;

/* Explicit pull configuration for INPUT_PUPD mode */
typedef enum
{
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_t;

/* --------------------------------------------------------------------------
 *  Legacy CNF enum + legacy gpio_setup for compatibility
 * -------------------------------------------------------------------------- */
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
 *  Core configuration API
 * -------------------------------------------------------------------------- */

void gpio_setup_raw(uint32_t port, uint8_t pin, gpio_mode_t mode, uint32_t cnf_bits);

void gpio_setup_input(uint32_t port, uint8_t pin, gpio_cnf_input_t cnf, gpio_pull_t pull);

void gpio_setup_output(uint32_t port, uint8_t pin, gpio_mode_t mode, gpio_cnf_output_t cnf);

/**
 * @brief Legacy API: configure a GPIO pin (mode + configuration).
 */
static inline void gpio_setup(uint32_t port, uint8_t pin, gpio_mode_t mode, gpio_cnf_t cnf)
{
    if (mode == GPIO_MODE_INPUT)
    {
        gpio_cnf_input_t icnf;

        switch (cnf)
        {
        case GPIO_CNF_ANALOG:
            icnf = GPIO_INPUT_CNF_ANALOG;
            break;
        case GPIO_CNF_FLOATING:
            icnf = GPIO_INPUT_CNF_FLOATING;
            break;
        case GPIO_CNF_INPUT_PUPD:
        default:
            icnf = GPIO_INPUT_CNF_PUPD;
            break;
        }

        /* Legacy behavior: INPUT_PUPD with no explicit pull -> leave ODR as-is */
        gpio_setup_input(port, pin, icnf, GPIO_PULL_NONE);
    }
    else
    {
        gpio_cnf_output_t ocnf;

        switch (cnf)
        {
        case GPIO_CNF_OPENDRAIN:
            ocnf = GPIO_OUTPUT_CNF_OPENDRAIN;
            break;
        case GPIO_CNF_AF_PUSHPULL:
            ocnf = GPIO_OUTPUT_CNF_AF_PUSHPULL;
            break;
        case GPIO_CNF_AF_OPENDRAIN:
            ocnf = GPIO_OUTPUT_CNF_AF_OPENDRAIN;
            break;
        case GPIO_CNF_PUSHPULL:
        default:
            ocnf = GPIO_OUTPUT_CNF_PUSHPULL;
            break;
        }

        gpio_setup_output(port, pin, mode, ocnf);
    }
}

/* --------------------------------------------------------------------------
 *  Pin write helpers
 * -------------------------------------------------------------------------- */

static inline void gpio_write_pin(uint32_t port, uint8_t pin, uint8_t value)
{
    GPIOx_BSRR(port) = (1U << pin) << (value ? 0U : 16U);
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
 *  Pull-up / pull-down configuration
 * -------------------------------------------------------------------------- */
static inline void gpio_set_pull(uint32_t port, uint8_t pin, gpio_pull_t pull)
{
    /* Legacy compatibility:
     * 0 → pull-down
     * 1 → pull-up
     *
     * Typed API:
     * GPIO_PULL_UP   (1)
     * GPIO_PULL_DOWN (2)
     * GPIO_PULL_NONE (0)
     */

    if (pull == GPIO_PULL_UP || pull == 1)
    {
        /* Pull-up: ODR bit = 1 */
        GPIOx_ODR(port) |= (1U << pin);
    }
    else if (pull == GPIO_PULL_DOWN || pull == 0)
    {
        /* Pull-down: ODR bit = 0 */
        GPIOx_ODR(port) &= ~(1U << pin);
    }
    else
    {
        /* GPIO_PULL_NONE → leave ODR unchanged */
    }
}

/* --------------------------------------------------------------------------
 *  Toggle helper
 * -------------------------------------------------------------------------- */
static inline void gpio_toggle(uint32_t port, uint8_t pin)
{
    if (GPIOx_ODR(port) & (1U << pin))
    {
        /* Pin is high → clear it */
        GPIOx_BSRR(port) = (1U << (pin + 16U));
    }
    else
    {
        /* Pin is low → set it */
        GPIOx_BSRR(port) = (1U << pin);
    }
}

static inline void gpio_toggle_fast(uint32_t port, uint8_t pin) { GPIOx_ODR(port) ^= (1U << pin); }

#endif /* GPIO_H */
