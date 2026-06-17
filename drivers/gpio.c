#include "gpio.h"
#include "stm32f103.h"

void gpio_setup(uint32_t port, uint8_t pin, uint32_t mode, uint32_t cnf)
{
    uint32_t shift = (pin & 7U) * 4U;

    volatile uint32_t* reg = (pin < 8) ? &GPIOx_CRL(port) : &GPIOx_CRH(port);

    uint32_t mask = (GPIO_CR_MODE | GPIO_CR_CNF) << shift;
    uint32_t value = (mode | cnf) << shift;

    *reg = (*reg & ~mask) | value;
}
