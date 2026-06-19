#include "gpio.h"
#include "stm32f103.h"

/**
 * @brief Configure a GPIO pin (mode + configuration).
 *
 * STM32F1 GPIO configuration registers:
 *  - CRL: pins 0–7
 *  - CRH: pins 8–15
 *
 * Each pin uses a 4‑bit field:
 *   MODE[1:0] | CNF[1:0]
 */
void gpio_setup(uint32_t port, uint8_t pin, gpio_mode_t mode, gpio_cnf_t cnf)
{
    /* Select CRL (0–7) or CRH (8–15) */
    volatile uint32_t* reg = (pin < 8) ? &GPIOx_CRL(port) : &GPIOx_CRH(port);

    /* Shift for the 4‑bit field */
    uint32_t shift = (pin & 7U) * 4U;

    /* Build mask and value */
    uint32_t mask = (0xFU << shift);       /* MODE + CNF = 4 bits */
    uint32_t value = ((mode | (cnf << 2U)) /* CNF must be shifted */
                      & 0xFU)
                     << shift;

    /* Apply configuration */
    *reg = (*reg & ~mask) | value;
}
