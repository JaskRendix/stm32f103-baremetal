#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f103.h"

/* Configure pin mode + configuration */
void gpio_setup(uint32_t port, uint8_t pin, uint32_t mode, uint32_t cnf);

/* Write full 16‑bit port */
#define GPIO_PORT_WRITE(port, value)                                                               \
    do                                                                                             \
    {                                                                                              \
        GPIOx_ODR(port) = ((value) & 0xFFFFU);                                                     \
    } while (0)

/* Set one pin */
#define GPIO_PIN_SET(port, pin)                                                                    \
    do                                                                                             \
    {                                                                                              \
        GPIOx_BSRR(port) = (1U << (pin));                                                          \
    } while (0)

/* Clear one pin */
#define GPIO_PIN_CLR(port, pin)                                                                    \
    do                                                                                             \
    {                                                                                              \
        GPIOx_BSRR(port) = (1U << ((pin) + 16U));                                                  \
    } while (0)

/* Set multiple pins */
#define GPIO_PIN_SET_GROUP(port, mask)                                                             \
    do                                                                                             \
    {                                                                                              \
        GPIOx_BSRR(port) = ((mask) & 0xFFFFU);                                                     \
    } while (0)

/* Clear multiple pins */
#define GPIO_PIN_CLR_GROUP(port, mask)                                                             \
    do                                                                                             \
    {                                                                                              \
        GPIOx_BSRR(port) = (((mask) & 0xFFFFU) << 16U);                                            \
    } while (0)

/* Toggle one pin */
#define GPIO_PIN_TOG(port, pin)                                                                    \
    do                                                                                             \
    {                                                                                              \
        if (GPIOx_ODR(port) & (1U << (pin)))                                                       \
            GPIO_PIN_CLR(port, pin);                                                               \
        else                                                                                       \
            GPIO_PIN_SET(port, pin);                                                               \
    } while (0)

#endif
