#ifndef GPIO_FAKE_HW_H
#define GPIO_FAKE_HW_H

#include <stdint.h>
#include "stm32f103.h"   /* bring in the real definitions first */

/* Fake GPIO register block */
typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
} gpio_fake_t;

/* One fake port instance */
extern gpio_fake_t GPIO_FAKE;

/* Now override the hardware macros so tests use the fake struct */
#undef GPIOx_CRL
#undef GPIOx_CRH
#undef GPIOx_IDR
#undef GPIOx_ODR
#undef GPIOx_BSRR

#define GPIOx_CRL(base)   (GPIO_FAKE.CRL)
#define GPIOx_CRH(base)   (GPIO_FAKE.CRH)
#define GPIOx_IDR(base)   (GPIO_FAKE.IDR)
#define GPIOx_ODR(base)   (GPIO_FAKE.ODR)
#define GPIOx_BSRR(base)  (GPIO_FAKE.BSRR)

#endif
