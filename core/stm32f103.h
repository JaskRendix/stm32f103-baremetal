#ifndef STM32F103_H
#define STM32F103_H

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Base addresses                                                             */
/* -------------------------------------------------------------------------- */

#define PERIPH_BASE 0x40000000UL
#define RCC_BASE (PERIPH_BASE + 0x21000UL)
#define FLASH_BASE (PERIPH_BASE + 0x22000UL)
#define GPIO_BASE (PERIPH_BASE + 0x10800UL)

#define USBFS_BASE (PERIPH_BASE + 0x05C00UL)
#define USB_SRAM_BASE (PERIPH_BASE + 0x06000UL)
#define CAN_SRAM_BASE USB_SRAM_BASE

/* -------------------------------------------------------------------------- */
/* Register access                                                            */
/* -------------------------------------------------------------------------- */

#define REG32(addr) (*(volatile uint32_t*)(addr))

#define SET_REG(reg, mask, value)                                                                  \
    do                                                                                             \
    {                                                                                              \
        (reg) = ((reg) & ~(mask)) | (value);                                                       \
    } while (0)

/* -------------------------------------------------------------------------- */
/* RCC registers                                                              */
/* -------------------------------------------------------------------------- */

#define RCC_CR REG32(RCC_BASE + 0x00)
#define RCC_CFGR REG32(RCC_BASE + 0x04)
#define RCC_CIR REG32(RCC_BASE + 0x08)
#define RCC_APB2RSTR REG32(RCC_BASE + 0x0C)
#define RCC_APB1RSTR REG32(RCC_BASE + 0x10)
#define RCC_AHBENR REG32(RCC_BASE + 0x14)
#define RCC_APB2ENR REG32(RCC_BASE + 0x18)
#define RCC_APB1ENR REG32(RCC_BASE + 0x1C)
#define RCC_BDCR REG32(RCC_BASE + 0x20)
#define RCC_CSR REG32(RCC_BASE + 0x24)

/* -------------------------------------------------------------------------- */
/* RCC_CR bits                                                                */
/* -------------------------------------------------------------------------- */

#define RCC_CR_HSION (1U << 0)
#define RCC_CR_HSIRDY (1U << 1)
#define RCC_CR_HSEON (1U << 16)
#define RCC_CR_HSERDY (1U << 17)
#define RCC_CR_HSEBYP (1U << 18)
#define RCC_CR_CSSON (1U << 19)
#define RCC_CR_PLLON (1U << 24)
#define RCC_CR_PLLRDY (1U << 25)

/* -------------------------------------------------------------------------- */
/* RCC_CFGR masks                                                             */
/* -------------------------------------------------------------------------- */

#define RCC_CFGR_SW (3U << 0)
#define RCC_CFGR_SWS (3U << 2)
#define RCC_CFGR_HPRE (0xFU << 4)
#define RCC_CFGR_PPRE1 (7U << 8)
#define RCC_CFGR_PPRE2 (7U << 11)
#define RCC_CFGR_ADCPRE (3U << 14)
#define RCC_CFGR_PLLSRC (1U << 16)
#define RCC_CFGR_PLLXTPRE (1U << 17)
#define RCC_CFGR_PLLMUL (0xFU << 18)
#define RCC_CFGR_USBPRE (1U << 22)

/* -------------------------------------------------------------------------- */
/* RCC_CFGR values                                                            */
/* -------------------------------------------------------------------------- */

#define RCC_CFGR_SW_HSI (0U << 0)
#define RCC_CFGR_SW_HSE (1U << 0)
#define RCC_CFGR_SW_PLL (2U << 0)

#define RCC_CFGR_SWS_HSI (0U << 2)
#define RCC_CFGR_SWS_HSE (1U << 2)
#define RCC_CFGR_SWS_PLL (2U << 2)

#define RCC_CFGR_HPRE_DIV_1 (0U << 4)
#define RCC_CFGR_HPRE_DIV_2 (8U << 4)
#define RCC_CFGR_HPRE_DIV_4 (9U << 4)
#define RCC_CFGR_HPRE_DIV_8 (10U << 4)
#define RCC_CFGR_HPRE_DIV_16 (11U << 4)
#define RCC_CFGR_HPRE_DIV_64 (12U << 4)
#define RCC_CFGR_HPRE_DIV_128 (13U << 4)
#define RCC_CFGR_HPRE_DIV_256 (14U << 4)
#define RCC_CFGR_HPRE_DIV_512 (15U << 4)

#define RCC_CFGR_PPRE1_DIV_1 (0U << 8)
#define RCC_CFGR_PPRE1_DIV_2 (4U << 8)
#define RCC_CFGR_PPRE1_DIV_4 (5U << 8)
#define RCC_CFGR_PPRE1_DIV_8 (6U << 8)
#define RCC_CFGR_PPRE1_DIV_16 (7U << 8)

#define RCC_CFGR_PPRE2_DIV_1 (0U << 11)
#define RCC_CFGR_PPRE2_DIV_2 (4U << 11)
#define RCC_CFGR_PPRE2_DIV_4 (5U << 11)
#define RCC_CFGR_PPRE2_DIV_8 (6U << 11)
#define RCC_CFGR_PPRE2_DIV_16 (7U << 11)

#define RCC_CFGR_ADCPRE_DIV_2 (0U << 14)
#define RCC_CFGR_ADCPRE_DIV_4 (1U << 14)
#define RCC_CFGR_ADCPRE_DIV_6 (2U << 14)
#define RCC_CFGR_ADCPRE_DIV_8 (3U << 14)

#define RCC_CFGR_PLLSRC_HSI (0U << 16)
#define RCC_CFGR_PLLSRC_HSE (1U << 16)

#define RCC_CFGR_PLLXTPRE_DIV_1 (0U << 17)
#define RCC_CFGR_PLLXTPRE_DIV_2 (1U << 17)

#define RCC_CFGR_PLLMUL_MUL_9 (7U << 18)

#define RCC_CFGR_USBPRE_DIV_1_5 (0U << 22)
#define RCC_CFGR_USBPRE_DIV_1 (1U << 22)

/* -------------------------------------------------------------------------- */
/* RCC_APB2ENR bits                                                           */
/* -------------------------------------------------------------------------- */

#define RCC_APB2ENR_AFIOEN (1U << 0)
#define RCC_APB2ENR_IOPAEN (1U << 2)
#define RCC_APB2ENR_IOPBEN (1U << 3)
#define RCC_APB2ENR_IOPCEN (1U << 4)
#define RCC_APB2ENR_IOPDEN (1U << 5)
#define RCC_APB2ENR_IOPEEN (1U << 6)
#define RCC_APB2ENR_IOPFEN (1U << 7)
#define RCC_APB2ENR_IOPGEN (1U << 8)
#define RCC_APB2ENR_ADC1EN (1U << 9)
#define RCC_APB2ENR_ADC2EN (1U << 10)
#define RCC_APB2ENR_TIM1EN (1U << 11)
#define RCC_APB2ENR_SPI1EN (1U << 12)
#define RCC_APB2ENR_TIM8EN (1U << 13)
#define RCC_APB2ENR_USART1EN (1U << 14)
#define RCC_APB2ENR_ADC3EN (1U << 15)
#define RCC_APB2ENR_TIM9EN (1U << 19)
#define RCC_APB2ENR_TIM10EN (1U << 20)
#define RCC_APB2ENR_TIM11EN (1U << 21)

/* -------------------------------------------------------------------------- */
/* RCC_APB1ENR bits                                                           */
/* -------------------------------------------------------------------------- */

#define RCC_APB1ENR_TIM2EN (1U << 0)
#define RCC_APB1ENR_TIM3EN (1U << 1)
#define RCC_APB1ENR_TIM4EN (1U << 2)
#define RCC_APB1ENR_TIM5EN (1U << 3)
#define RCC_APB1ENR_TIM6EN (1U << 4)
#define RCC_APB1ENR_TIM7EN (1U << 5)
#define RCC_APB1ENR_TIM12EN (1U << 6)
#define RCC_APB1ENR_TIM13EN (1U << 7)
#define RCC_APB1ENR_TIM14EN (1U << 8)
#define RCC_APB1ENR_WWDGEN (1U << 11)
#define RCC_APB1ENR_SPI2EN (1U << 14)
#define RCC_APB1ENR_SPI3EN (1U << 15)
#define RCC_APB1ENR_USART2EN (1U << 17)
#define RCC_APB1ENR_USART3EN (1U << 18)
#define RCC_APB1ENR_UART4EN (1U << 19)
#define RCC_APB1ENR_UART5EN (1U << 20)
#define RCC_APB1ENR_I2C1EN (1U << 21)
#define RCC_APB1ENR_I2C2EN (1U << 22)
#define RCC_APB1ENR_USBEN (1U << 23)
#define RCC_APB1ENR_CANEN (1U << 25)
#define RCC_APB1ENR_BKPEN (1U << 27)
#define RCC_APB1ENR_PWREN (1U << 28)
#define RCC_APB1ENR_DACEN (1U << 29)

/* -------------------------------------------------------------------------- */
/* FLASH                                                                      */
/* -------------------------------------------------------------------------- */

#define FLASH_ACR REG32(FLASH_BASE + 0x00)

#define FLASH_ACR_LATENCY_0 (0U << 0)
#define FLASH_ACR_LATENCY_1 (1U << 0)
#define FLASH_ACR_LATENCY_2 (2U << 0)

#define FLASH_ACR_LATENCY (7U << 0)
#define FLASH_HLFCYA (1U << 3)
#define FLASH_PRFTBE (1U << 4)

/* -------------------------------------------------------------------------- */
/* GPIO                                                                       */
/* -------------------------------------------------------------------------- */

#define GPIOA 0x0000
#define GPIOB 0x0400
#define GPIOC 0x0800
#define GPIOD 0x0C00
#define GPIOE 0x1000
#define GPIOF 0x1400
#define GPIOG 0x1800

#define GPIOx_CRL(p) REG32(GPIO_BASE + (p) + 0x00)
#define GPIOx_CRH(p) REG32(GPIO_BASE + (p) + 0x04)
#define GPIOx_IDR(p) REG32(GPIO_BASE + (p) + 0x08)
#define GPIOx_ODR(p) REG32(GPIO_BASE + (p) + 0x0C)
#define GPIOx_BSRR(p) REG32(GPIO_BASE + (p) + 0x10)
#define GPIOx_BRR(p) REG32(GPIO_BASE + (p) + 0x14)
#define GPIOx_LCKR(p) REG32(GPIO_BASE + (p) + 0x18)

#define GPIO_CR_MODE 0x3U
#define GPIO_CR_CNF 0xCU

#define GPIO_CR_MODE_IN (0U << 0)
#define GPIO_CR_MODE_OUT_10MHZ (1U << 0)
#define GPIO_CR_MODE_OUT_2MHZ (2U << 0)
#define GPIO_CR_MODE_OUT_50MHZ (3U << 0)

#define GPIO_CR_CNF_IN_ANALOG (0U << 2)
#define GPIO_CR_CNF_IN_FLOAT (1U << 2)
#define GPIO_CR_CNF_IN_PULL (2U << 2)
#define GPIO_CR_CNF_OUT_PUSHPULL (0U << 2)
#define GPIO_CR_CNF_OUT_OPENDRAIN (1U << 2)
#define GPIO_CR_CNF_ALT_PUSHPULL (2U << 2)
#define GPIO_CR_CNF_ALT_OPENDRAIN (3U << 2)

/* -------------------------------------------------------------------------- */
/* USB FS                                                                     */
/* -------------------------------------------------------------------------- */

#define USB_CNTR REG32(USBFS_BASE + 0x40)
#define USB_ISTR REG32(USBFS_BASE + 0x44)
#define USB_FNR REG32(USBFS_BASE + 0x48)
#define USB_DADDR REG32(USBFS_BASE + 0x4C)

/* -------------------------------------------------------------------------- */
/* Cortex-M3 SysTick                                                          */
/* -------------------------------------------------------------------------- */

typedef struct
{
    volatile uint32_t CTRL;  /* Control and status register */
    volatile uint32_t LOAD;  /* Reload value register       */
    volatile uint32_t VAL;   /* Current value register      */
    volatile uint32_t CALIB; /* Calibration register        */
} SysTick_Type;

/* SysTick base address (Cortex-M core peripheral) */
#define SysTick ((SysTick_Type*)0xE000E010UL)

/* SysTick CTRL register bit definitions */
#define SysTick_CTRL_ENABLE_Msk (1UL << 0)
#define SysTick_CTRL_TICKINT_Msk (1UL << 1)
#define SysTick_CTRL_CLKSOURCE_Msk (1UL << 2)

/* -------------------------------------------------------------------------- */
/* USART1                                                                    */
/* -------------------------------------------------------------------------- */

#define USART1_BASE (PERIPH_BASE + 0x13800UL)

#define USART1_SR REG32(USART1_BASE + 0x00)
#define USART1_DR REG32(USART1_BASE + 0x04)
#define USART1_BRR REG32(USART1_BASE + 0x08)
#define USART1_CR1 REG32(USART1_BASE + 0x0C)
#define USART1_CR2 REG32(USART1_BASE + 0x10)
#define USART1_CR3 REG32(USART1_BASE + 0x14)
#define USART1_GTPR REG32(USART1_BASE + 0x18)

/* USART1_SR bits */
#define USART_SR_PE (1U << 0)
#define USART_SR_FE (1U << 1)
#define USART_SR_NE (1U << 2)
#define USART_SR_ORE (1U << 3)
#define USART_SR_IDLE (1U << 4)
#define USART_SR_RXNE (1U << 5)
#define USART_SR_TC (1U << 6)
#define USART_SR_TXE (1U << 7)
#define USART_SR_LBD (1U << 8)
#define USART_SR_CTS (1U << 9)

/* USART1_CR1 bits */
#define USART_CR1_SBK (1U << 0)
#define USART_CR1_RWU (1U << 1)
#define USART_CR1_RE (1U << 2)
#define USART_CR1_TE (1U << 3)
#define USART_CR1_IDLEIE (1U << 4)
#define USART_CR1_RXNEIE (1U << 5)
#define USART_CR1_TCIE (1U << 6)
#define USART_CR1_TXEIE (1U << 7)
#define USART_CR1_PEIE (1U << 8)
#define USART_CR1_PS (1U << 9)
#define USART_CR1_PCE (1U << 10)
#define USART_CR1_WAKE (1U << 11)
#define USART_CR1_M (1U << 12)
#define USART_CR1_UE (1U << 13)

#endif /* STM32F103_H */
