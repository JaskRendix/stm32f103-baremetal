#ifndef FLASH_FAKE_HW_H
#define FLASH_FAKE_HW_H

#include <stdint.h>
#include "stm32f103.h" /* real definitions first */
#include "flash.h"

#define FLASH_FAKE_PAGES 4U
#define FLASH_FAKE_SIZE (FLASH_FAKE_PAGES * FLASH_PAGE_SIZE)
#define FLASH_FAKE_BASE FLASH_BASE_ADDR

typedef struct
{
    volatile uint32_t KEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    uint8_t memory[FLASH_FAKE_SIZE];
} flash_fake_t;

extern flash_fake_t FLASH_FAKE;

/* Resets registers to power-on state (CR.LOCK set) and memory to erased (0xFF) */
void flash_fake_reset(void);

#undef FLASH_KEYR
#undef FLASH_SR
#undef FLASH_CR
#undef FLASH_AR
#undef FLASH_MEM16

#define FLASH_KEYR (FLASH_FAKE.KEYR)
#define FLASH_SR (FLASH_FAKE.SR)
#define FLASH_CR (FLASH_FAKE.CR)
#define FLASH_AR (FLASH_FAKE.AR)
#define FLASH_MEM16(addr) (*(uint16_t*)&FLASH_FAKE.memory[(addr) - FLASH_FAKE_BASE])

#endif /* FLASH_FAKE_HW_H */