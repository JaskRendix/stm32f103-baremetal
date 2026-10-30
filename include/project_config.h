#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

/* -------------------------------------------------------------------------- */
/* System configuration                                                       */
/* -------------------------------------------------------------------------- */

#define HSE_VALUE 8000000UL
#define SYSCLK_FREQ 72000000UL
#define CPU_HZ SYSCLK_FREQ

/* -------------------------------------------------------------------------- */
/* Flash configuration                                                        */
/* -------------------------------------------------------------------------- */

/* STM32F103 flash base address */
#define FLASH_BASE_ADDR 0x08000000UL

/* Adjust for your target device */
#define FLASH_SIZE_BYTES (64UL * 1024UL)

/* F103 low/medium density = 1KB pages.
 * High density devices (256KB+) use 2KB pages.
 */
#define FLASH_PAGE_SIZE 1024UL

/* -------------------------------------------------------------------------- */
/* Blackbox configuration                                                     */
/* -------------------------------------------------------------------------- */

/* Reserve the last flash page for blackbox logging */
#define BLACKBOX_FLASH_START (FLASH_BASE_ADDR + FLASH_SIZE_BYTES - FLASH_PAGE_SIZE)

#define BLACKBOX_FLASH_SIZE FLASH_PAGE_SIZE

/* -------------------------------------------------------------------------- */
/* Board pin assignments                                                      */
/* -------------------------------------------------------------------------- */

#define LED_PORT GPIOC
#define LED_PIN 13

/* RCC bit for LED port */
#define LED_PORT_RCC_EN RCC_APB2ENR_IOPCEN

/* -------------------------------------------------------------------------- */
/* UART configuration                                                         */
/* -------------------------------------------------------------------------- */

/* Default debug UART */
#define DEBUG_UART_BAUDRATE 115200UL

/* -------------------------------------------------------------------------- */
/* Feature toggles                                                            */
/* -------------------------------------------------------------------------- */

#define ENABLE_USB 0
#define ENABLE_CAN 0
#define ENABLE_DEBUG 1
#define ENABLE_BLACKBOX 1

/* -------------------------------------------------------------------------- */
/* Build information                                                          */
/* -------------------------------------------------------------------------- */

#define PROJECT_NAME "stm32f103-baremetal"
#define PROJECT_VERSION "1.0.0"

#endif /* PROJECT_CONFIG_H */
