#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

/* -------------------------------------------------------------------------- */
/* System configuration                                                       */
/* -------------------------------------------------------------------------- */

#define HSE_VALUE 8000000UL
#define SYSCLK_FREQ 72000000UL
#define CPU_HZ SYSCLK_FREQ

/* -------------------------------------------------------------------------- */
/* Board pin assignments                                                      */
/* -------------------------------------------------------------------------- */

#define LED_PORT GPIOC
#define LED_PIN 13

/* RCC bit for LED port */
#define LED_PORT_RCC_EN RCC_APB2ENR_IOPCEN

/* -------------------------------------------------------------------------- */
/* Feature toggles                                                            */
/* -------------------------------------------------------------------------- */

#define ENABLE_USB 0
#define ENABLE_CAN 0
#define ENABLE_DEBUG 1

#endif
