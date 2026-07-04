#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "flash.h"
#include "flash_fake_hw.h"

static void test_flash_unlock_sequence(void)
{
    flash_fake_reset();
    assert(FLASH_FAKE.CR & FLASH_CR_LOCK);

    flash_unlock();
    assert(FLASH_FAKE.KEYR == FLASH_KEY2); /* last key value written */
    assert((FLASH_FAKE.CR & FLASH_CR_LOCK) == 0);
    printf("[PASS] test_flash_unlock_sequence\n");
}

static void test_flash_lock_sets_bit(void)
{
    flash_fake_reset();
    FLASH_FAKE.CR &= ~FLASH_CR_LOCK;
    flash_lock();
    assert(FLASH_FAKE.CR & FLASH_CR_LOCK);
    printf("[PASS] test_flash_lock_sets_bit\n");
}

static void test_flash_erase_page_success(void)
{
    flash_fake_reset();
    memset(FLASH_FAKE.memory, 0xAA, FLASH_PAGE_SIZE);

    flash_status_t st = flash_erase_page(FLASH_FAKE_BASE);
    assert(st == FLASH_OK);
    assert(FLASH_FAKE.CR & FLASH_CR_LOCK); /* re-locked afterwards */

    for (uint32_t off = 0; off < FLASH_PAGE_SIZE; off += 2U)
    {
        assert(flash_read_halfword(FLASH_FAKE_BASE + off) == 0xFFFFU);
    }
    printf("[PASS] test_flash_erase_page_success\n");
}

static void test_flash_erase_page_alignment_error(void)
{
    flash_fake_reset();
    flash_status_t st = flash_erase_page(FLASH_FAKE_BASE + 3U);
    assert(st == FLASH_ERR_RANGE);
    printf("[PASS] test_flash_erase_page_alignment_error\n");
}

static void test_flash_program_halfword_success(void)
{
    flash_fake_reset();
    flash_status_t st = flash_program_halfword(FLASH_FAKE_BASE + 0x10U, 0x1234U);
    assert(st == FLASH_OK);
    assert(flash_read_halfword(FLASH_FAKE_BASE + 0x10U) == 0x1234U);
    printf("[PASS] test_flash_program_halfword_success\n");
}

static void test_flash_program_halfword_align_error(void)
{
    flash_fake_reset();
    flash_status_t st = flash_program_halfword(FLASH_FAKE_BASE + 0x11U, 0x1234U);
    assert(st == FLASH_ERR_ALIGN);
    printf("[PASS] test_flash_program_halfword_align_error\n");
}

static void test_flash_program_word(void)
{
    flash_fake_reset();
    flash_status_t st = flash_program_word(FLASH_FAKE_BASE + 0x20U, 0xDEADBEEFUL);
    assert(st == FLASH_OK);
    assert(flash_read_word(FLASH_FAKE_BASE + 0x20U) == 0xDEADBEEFUL);
    printf("[PASS] test_flash_program_word\n");
}

static void test_flash_program_buffer(void)
{
    flash_fake_reset();
    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    flash_status_t st = flash_program_buffer(FLASH_FAKE_BASE + 0x40U, data, sizeof(data));
    assert(st == FLASH_OK);

    for (size_t i = 0; i < sizeof(data); i++)
    {
        uint32_t addr = FLASH_FAKE_BASE + 0x40U + (uint32_t)i;
        uint8_t byte = (uint8_t)(FLASH_MEM16(addr & ~1U) >> ((addr & 1U) * 8U));
        assert(byte == data[i]);
    }
    printf("[PASS] test_flash_program_buffer\n");
}

static void test_flash_program_wrprterr(void)
{
    flash_fake_reset();
    FLASH_FAKE.SR = FLASH_SR_WRPRTERR;

    flash_status_t st = flash_program_halfword(FLASH_FAKE_BASE + 0x60U, 0x5555U);
    assert(st == FLASH_ERR_WRP);
    assert((FLASH_FAKE.SR & FLASH_SR_WRPRTERR) == 0); /* cleared by driver */
    printf("[PASS] test_flash_program_wrprterr\n");
}

static void test_flash_wait_timeout(void)
{
    flash_fake_reset();
    FLASH_FAKE.SR = FLASH_SR_BSY; /* never clears */

    flash_status_t st = flash_program_halfword(FLASH_FAKE_BASE + 0x80U, 0x0001U);
    assert(st == FLASH_ERR_TIMEOUT);
    printf("[PASS] test_flash_wait_timeout\n");
}

int main(void)
{
    printf("Running test_flash...\n");

    test_flash_unlock_sequence();
    test_flash_lock_sets_bit();
    test_flash_erase_page_success();
    test_flash_erase_page_alignment_error();
    test_flash_program_halfword_success();
    test_flash_program_halfword_align_error();
    test_flash_program_word();
    test_flash_program_buffer();
    test_flash_program_wrprterr();
    test_flash_wait_timeout();

    printf("All 10 test_flash cases passed.\n");
    return 0;
}

static void test_flash_consecutive_ops_no_sr_poisoning(void)
{
    flash_fake_reset();

    assert(flash_erase_page(FLASH_FAKE_BASE) == FLASH_OK);
    assert(flash_program_halfword(FLASH_FAKE_BASE, 0x1234U) == FLASH_OK);
    assert(flash_program_halfword(FLASH_FAKE_BASE + 2U, 0x5678U) == FLASH_OK);

    printf("[PASS] test_flash_consecutive_ops_no_sr_poisoning\n");
}
