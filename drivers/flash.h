#ifndef FLASH_H
#define FLASH_H

#include <stddef.h>
#include <stdint.h>

/* STM32F103 page size: 1KB on low/medium-density devices, 2KB on
 * high-density (>=256K flash). Override in project_config.h if your
 * target part is high-density. */
#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 1024UL
#endif

#ifndef FLASH_BASE_ADDR
#define FLASH_BASE_ADDR FLASH_MEM_BASE
#endif

/* Direct access to mapped flash memory (not the FLASH_R control registers).
 * Overridden by flash_fake_hw.h in host tests. */
#define FLASH_MEM16(addr) (*(volatile uint16_t*)(uintptr_t)(addr))

typedef enum
{
    FLASH_OK = 0,
    FLASH_ERR_TIMEOUT,
    FLASH_ERR_WRP,    /* write-protected page */
    FLASH_ERR_PROG,   /* programming error (target not erased) */
    FLASH_ERR_VERIFY, /* read-back did not match */
    FLASH_ERR_ALIGN,  /* address/length not halfword-aligned */
    FLASH_ERR_RANGE   /* address not page-aligned for erase */
} flash_status_t;

void flash_unlock(void);
void flash_lock(void);

flash_status_t flash_erase_page(uint32_t page_addr);

flash_status_t flash_program_halfword(uint32_t addr, uint16_t data);
flash_status_t flash_program_word(uint32_t addr, uint32_t data);
flash_status_t flash_program_buffer(uint32_t addr, const uint8_t* data, size_t len);

uint16_t flash_read_halfword(uint32_t addr);
uint32_t flash_read_word(uint32_t addr);

#endif /* FLASH_H */