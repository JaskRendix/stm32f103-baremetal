#ifndef FLASH_H
#define FLASH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "project_config.h"
#include "stm32f103.h"

/* -------------------------------------------------------------------------- */
/* Flash geometry fallbacks                                                   */
/* -------------------------------------------------------------------------- */

/* Allow project_config.h to override these values. */

#ifndef FLASH_BASE_ADDR
#define FLASH_BASE_ADDR FLASH_MEM_BASE
#endif

#ifndef FLASH_SIZE_BYTES
#define FLASH_SIZE_BYTES (64UL * 1024UL)
#endif

#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 1024UL
#endif

#define FLASH_ERASE_VALUE 0xFFFFU
#define FLASH_PROGRAM_GRANULE 2U

/* -------------------------------------------------------------------------- */
/* Flash memory access                                                        */
/* -------------------------------------------------------------------------- */

/* Direct access to mapped flash memory (not FLASH controller registers).
 * Overridden by flash_fake_hw.h in host-side tests.
 */
#define FLASH_MEM16(addr) (*(volatile uint16_t*)(uintptr_t)(addr))

/* -------------------------------------------------------------------------- */
/* Status codes                                                               */
/* -------------------------------------------------------------------------- */

typedef enum
{
    FLASH_OK = 0,

    FLASH_ERR_TIMEOUT,
    FLASH_ERR_WRP,    /* write-protected page */
    FLASH_ERR_PROG,   /* programming error */
    FLASH_ERR_VERIFY, /* read-back verification failed */
    FLASH_ERR_ALIGN,  /* address alignment error */
    FLASH_ERR_LENGTH, /* length alignment error */
    FLASH_ERR_RANGE   /* address outside valid flash region */
} flash_status_t;

/* -------------------------------------------------------------------------- */
/* Controller operations                                                      */
/* -------------------------------------------------------------------------- */

void flash_unlock(void);
void flash_lock(void);

/* -------------------------------------------------------------------------- */
/* Erase operations                                                           */
/* -------------------------------------------------------------------------- */

flash_status_t flash_erase_page(uint32_t page_addr);

/* -------------------------------------------------------------------------- */
/* Program operations                                                         */
/* -------------------------------------------------------------------------- */

flash_status_t flash_program_halfword(uint32_t addr, uint16_t data);
flash_status_t flash_program_word(uint32_t addr, uint32_t data);
flash_status_t flash_program_buffer(uint32_t addr, const uint8_t* data, size_t len);

/* -------------------------------------------------------------------------- */
/* Read operations                                                            */
/* -------------------------------------------------------------------------- */

uint16_t flash_read_halfword(uint32_t addr);
uint32_t flash_read_word(uint32_t addr);

/* -------------------------------------------------------------------------- */
/* Address helpers                                                            */
/* -------------------------------------------------------------------------- */

bool flash_addr_valid(uint32_t addr);
bool flash_range_valid(uint32_t addr, uint32_t size);
bool flash_is_page_aligned(uint32_t addr);
uint32_t flash_page_start(uint32_t addr);

#endif /* FLASH_H */
