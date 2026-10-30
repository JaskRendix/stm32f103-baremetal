#include "flash.h"
#include "stm32f103.h"

#define FLASH_TIMEOUT_LOOPS 100000UL

bool flash_addr_valid(uint32_t addr)
{
    return (addr >= FLASH_BASE_ADDR) && (addr < (FLASH_BASE_ADDR + FLASH_SIZE_BYTES));
}

bool flash_range_valid(uint32_t addr, uint32_t size)
{
    return (addr >= FLASH_BASE_ADDR) && (size <= FLASH_SIZE_BYTES) &&
           ((addr - FLASH_BASE_ADDR) <= (FLASH_SIZE_BYTES - size));
}

bool flash_is_page_aligned(uint32_t addr)
{
    return flash_addr_valid(addr) && ((addr - FLASH_BASE_ADDR) % FLASH_PAGE_SIZE == 0U);
}

uint32_t flash_page_start(uint32_t addr)
{
    if (!flash_addr_valid(addr))
    {
        return 0U;
    }
    return FLASH_BASE_ADDR + ((addr - FLASH_BASE_ADDR) / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
}

static flash_status_t flash_wait_ready(void)
{
    uint32_t timeout = FLASH_TIMEOUT_LOOPS;

    while ((FLASH_SR & FLASH_SR_BSY) && (timeout != 0U))
    {
        timeout--;
    }

    return (FLASH_SR & FLASH_SR_BSY) ? FLASH_ERR_TIMEOUT : FLASH_OK;
}

static flash_status_t flash_check_errors(void)
{
    uint32_t sr = FLASH_SR;
    uint32_t sticky = sr & (FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR);
    flash_status_t st = FLASH_OK;

    if (sr & FLASH_SR_WRPRTERR)
    {
        st = FLASH_ERR_WRP;
    }
    else if (sr & FLASH_SR_PGERR)
    {
        st = FLASH_ERR_PROG;
    }

    if (sticky != 0U)
    {
#ifdef __arm__
        /* Real HW: write-1-to-clear */
        FLASH_SR = sticky;
#else
        /* Fake HW: emulate write-1-to-clear by clearing bits explicitly */
        FLASH_SR &= ~sticky;
#endif
    }

    return st;
}

static flash_status_t flash_prepare(void)
{
    flash_unlock();

    flash_status_t st = flash_wait_ready();
    if (st != FLASH_OK)
    {
        flash_lock();
        return st;
    }

    st = flash_check_errors();
    if (st != FLASH_OK)
    {
        flash_lock();
    }

    return st;
}

#ifndef __arm__
static void flash_fake_program(uint32_t addr, uint16_t value)
{
    uint16_t old = FLASH_MEM16(addr);

    if ((~old) & value)
    {
        FLASH_SR |= FLASH_SR_PGERR;
    }
    else
    {
        FLASH_MEM16(addr) = old & value;
    }
}
#endif

void flash_unlock(void)
{
    if (FLASH_CR & FLASH_CR_LOCK)
    {
        FLASH_KEYR = FLASH_KEY1;
        FLASH_KEYR = FLASH_KEY2;

#ifndef __arm__
        /* Host build: simulate hardware side effect of clearing LOCK. */
        FLASH_CR &= ~FLASH_CR_LOCK;
#endif
    }
}

void flash_lock(void) { FLASH_CR |= FLASH_CR_LOCK; }

uint16_t flash_read_halfword(uint32_t addr) { return FLASH_MEM16(addr); }

uint32_t flash_read_word(uint32_t addr)
{
    uint32_t lo = flash_read_halfword(addr);
    uint32_t hi = flash_read_halfword(addr + 2U);
    return lo | (hi << 16);
}

flash_status_t flash_erase_page(uint32_t page_addr)
{
    if (!flash_is_page_aligned(page_addr))
    {
        return FLASH_ERR_RANGE;
    }

    flash_status_t st = flash_prepare();
    if (st != FLASH_OK)
    {
        return st;
    }

    FLASH_CR |= FLASH_CR_PER;
    FLASH_AR = page_addr;
    FLASH_CR |= FLASH_CR_STRT;

#ifndef __arm__
    /* Host build: simulate page erase side effect (reset to 0xFFFF) */
    for (uint32_t off = 0U; off < FLASH_PAGE_SIZE; off += FLASH_PROGRAM_GRANULE)
    {
        FLASH_MEM16(page_addr + off) = FLASH_ERASE_VALUE;
    }
#endif

    st = flash_wait_ready();
    FLASH_CR &= ~FLASH_CR_PER;

    if (st == FLASH_OK)
    {
        st = flash_check_errors();
    }

    flash_lock();

    if (st == FLASH_OK)
    {
        for (uint32_t off = 0U; off < FLASH_PAGE_SIZE; off += FLASH_PROGRAM_GRANULE)
        {
            if (flash_read_halfword(page_addr + off) != FLASH_ERASE_VALUE)
            {
                return FLASH_ERR_VERIFY;
            }
        }
    }

    return st;
}

flash_status_t flash_program_halfword(uint32_t addr, uint16_t data)
{
    if (!flash_range_valid(addr, FLASH_PROGRAM_GRANULE))
    {
        return FLASH_ERR_RANGE;
    }
    if (addr & (FLASH_PROGRAM_GRANULE - 1U))
    {
        return FLASH_ERR_ALIGN;
    }

    flash_status_t st = flash_prepare();
    if (st != FLASH_OK)
    {
        return st;
    }

    FLASH_CR |= FLASH_CR_PG;

#ifndef __arm__
    flash_fake_program(addr, data);
#else
    /* Real hardware write */
    FLASH_MEM16(addr) = data;
#endif

    st = flash_wait_ready();
    FLASH_CR &= ~FLASH_CR_PG;

    if (st == FLASH_OK)
    {
        st = flash_check_errors();
    }

    flash_lock();

    if (st == FLASH_OK && flash_read_halfword(addr) != data)
    {
        st = FLASH_ERR_VERIFY;
    }

    return st;
}

flash_status_t flash_program_word(uint32_t addr, uint32_t data)
{
    if (!flash_range_valid(addr, 4U))
    {
        return FLASH_ERR_RANGE;
    }
    if (addr & 3U) /* Enforce 32-bit alignment for words */
    {
        return FLASH_ERR_ALIGN;
    }

    flash_status_t st = flash_program_halfword(addr, (uint16_t)(data & 0xFFFFU));
    if (st != FLASH_OK)
    {
        return st;
    }

    return flash_program_halfword(addr + 2U, (uint16_t)(data >> 16));
}

flash_status_t flash_program_buffer(uint32_t addr, const uint8_t* data, size_t len)
{
    if (len == 0U)
    {
        return FLASH_OK;
    }
    if (!flash_range_valid(addr, (uint32_t)len))
    {
        return FLASH_ERR_RANGE;
    }
    if (addr & (FLASH_PROGRAM_GRANULE - 1U))
    {
        return FLASH_ERR_ALIGN;
    }
    if (len & (FLASH_PROGRAM_GRANULE - 1U))
    {
        return FLASH_ERR_LENGTH;
    }

    /* Optimized bulk session: unlock and wait once for the whole sequence */
    flash_status_t st = flash_prepare();
    if (st != FLASH_OK)
    {
        return st;
    }

    for (size_t i = 0U; i < len; i += FLASH_PROGRAM_GRANULE)
    {
        uint16_t hw = (uint16_t)data[i] | ((uint16_t)data[i + 1] << 8);
        uint32_t target_addr = addr + (uint32_t)i;

        FLASH_CR |= FLASH_CR_PG;

#ifndef __arm__
        flash_fake_program(target_addr, hw);
#else
        FLASH_MEM16(target_addr) = hw;
#endif

        st = flash_wait_ready();
        FLASH_CR &= ~FLASH_CR_PG;

        if (st != FLASH_OK)
        {
            break;
        }

        st = flash_check_errors();
        if (st != FLASH_OK)
        {
            break;
        }
    }

    flash_lock();

    if (st == FLASH_OK)
    {
        for (size_t i = 0U; i < len; i += FLASH_PROGRAM_GRANULE)
        {
            uint16_t expected = (uint16_t)data[i] | ((uint16_t)data[i + 1] << 8);
            if (flash_read_halfword(addr + (uint32_t)i) != expected)
            {
                return FLASH_ERR_VERIFY;
            }
        }
    }

    return st;
}
