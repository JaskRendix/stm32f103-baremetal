#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f103.h"

#ifdef HOST_TESTING
#include <assert.h>
#define GPIO_ASSERT_PIN(pin) assert((pin) < 16U)
#else
#define GPIO_ASSERT_PIN(pin) ((void)0)
#endif

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
    GPIO_INPUT_CNF_ANALOG = 0x0,
    GPIO_INPUT_CNF_FLOATING = 0x1,
    GPIO_INPUT_CNF_PUPD = 0x2
} gpio_cnf_input_t;

/* Output configuration (CNF[1:0] when MODE != 0b00) */
typedef enum
{
    GPIO_OUTPUT_CNF_PUSHPULL = 0x0,
    GPIO_OUTPUT_CNF_OPENDRAIN = 0x1,
    GPIO_OUTPUT_CNF_AF_PUSHPULL = 0x2,
    GPIO_OUTPUT_CNF_AF_OPENDRAIN = 0x3
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
} gpio_cnf_legacy_t;

/* Preserve the original type name for existing application code. */
typedef gpio_cnf_legacy_t gpio_cnf_t;

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
    GPIO_ASSERT_PIN(pin);

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

        /* Legacy behavior: INPUT_PUPD with no explicit pull leaves ODR as-is. */
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
    GPIO_ASSERT_PIN(pin);
    GPIOx_BSRR(port) = value ? (1U << pin) : (1U << (pin + 16U));
}

static inline void gpio_set_pin(uint32_t port, uint8_t pin)
{
    GPIO_ASSERT_PIN(pin);
    GPIOx_BSRR(port) = (1U << pin);
}

static inline void gpio_clear_pin(uint32_t port, uint8_t pin)
{
    GPIO_ASSERT_PIN(pin);
    GPIOx_BSRR(port) = (1U << (pin + 16U));
}

/* --------------------------------------------------------------------------
 *  Pin read helpers
 * -------------------------------------------------------------------------- */

static inline uint32_t gpio_read_pin(uint32_t port, uint8_t pin)
{
    GPIO_ASSERT_PIN(pin);
    return (GPIOx_IDR(port) >> pin) & 1U;
}

static inline uint16_t gpio_read_port(uint32_t port) { return (uint16_t)GPIOx_IDR(port); }

/* --------------------------------------------------------------------------
 *  Pull-up / pull-down configuration
 * -------------------------------------------------------------------------- */

/* Configure pull state for pins configured as INPUT_PUPD.
 * On STM32F1 the pull direction is selected via the ODR bit.
 */
static inline void gpio_set_pull(uint32_t port, uint8_t pin, gpio_pull_t pull)
{
    GPIO_ASSERT_PIN(pin);

    switch (pull)
    {
    case GPIO_PULL_UP:
        GPIOx_ODR(port) |= (1U << pin);
        break;

    case GPIO_PULL_DOWN:
        GPIOx_ODR(port) &= ~(1U << pin);
        break;

    case GPIO_PULL_NONE:
    default:
        break;
    }
}

/* --------------------------------------------------------------------------
 *  Toggle helpers
 * -------------------------------------------------------------------------- */

static inline void gpio_toggle(uint32_t port, uint8_t pin)
{
    GPIO_ASSERT_PIN(pin);

    /* Toggle based on ODR state rather than the actual pin level. */
    if (GPIOx_ODR(port) & (1U << pin))
    {
        GPIOx_BSRR(port) = (1U << (pin + 16U));
    }
    else
    {
        GPIOx_BSRR(port) = (1U << pin);
    }
}

/* Non-atomic ODR read-modify-write; not safe if another context modifies ODR. */
static inline void gpio_toggle_fast(uint32_t port, uint8_t pin)
{
    GPIO_ASSERT_PIN(pin);
    GPIOx_ODR(port) ^= (1U << pin);
}

#endif /* GPIO_H */
