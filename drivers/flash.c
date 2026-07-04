#include "flash.h"
#include "stm32f103.h"

#define FLASH_TIMEOUT_LOOPS 100000UL

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
    flash_status_t st = FLASH_OK;

    if (sr & FLASH_SR_WRPRTERR)
    {
        st = FLASH_ERR_WRP;
    }
    else if (sr & FLASH_SR_PGERR)
    {
        st = FLASH_ERR_PROG;
    }

    /* Sticky flags, write-1-to-clear */
    FLASH_SR = FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR;

    return st;
}

void flash_unlock(void)
{
    if (FLASH_CR & FLASH_CR_LOCK)
    {
        FLASH_KEYR = FLASH_KEY1;
        FLASH_KEYR = FLASH_KEY2;
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
    if ((page_addr - FLASH_BASE_ADDR) % FLASH_PAGE_SIZE != 0U)
    {
        return FLASH_ERR_RANGE;
    }

    flash_unlock();

    flash_status_t st = flash_wait_ready();
    if (st != FLASH_OK)
    {
        flash_lock();
        return st;
    }

    FLASH_CR |= FLASH_CR_PER;
    FLASH_AR = page_addr;
    FLASH_CR |= FLASH_CR_STRT;

#ifndef __arm__
    /* Host build: there is no real flash controller to perform the bulk
     * erase side effect, so simulate it here for host-side tests. This
     * branch never builds for the target. */
    for (uint32_t off = 0; off < FLASH_PAGE_SIZE; off += 2U)
    {
        FLASH_MEM16(page_addr + off) = 0xFFFFU;
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
        for (uint32_t off = 0; off < FLASH_PAGE_SIZE; off += 2U)
        {
            if (flash_read_halfword(page_addr + off) != 0xFFFFU)
            {
                return FLASH_ERR_VERIFY;
            }
        }
    }

    return st;
}

flash_status_t flash_program_halfword(uint32_t addr, uint16_t data)
{
    if (addr & 1U)
    {
        return FLASH_ERR_ALIGN;
    }

    flash_unlock();

    flash_status_t st = flash_wait_ready();
    if (st != FLASH_OK)
    {
        flash_lock();
        return st;
    }

    FLASH_CR |= FLASH_CR_PG;
    FLASH_MEM16(addr) = data;

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
    if (addr & 1U)
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
    if ((addr & 1U) || (len & 1U))
    {
        return FLASH_ERR_ALIGN;
    }

    for (size_t i = 0; i < len; i += 2U)
    {
        uint16_t hw = (uint16_t)data[i] | ((uint16_t)data[i + 1] << 8);
        flash_status_t st = flash_program_halfword((uint32_t)(addr + i), hw);
        if (st != FLASH_OK)
        {
            return st;
        }
    }

    return FLASH_OK;
}