#include "blackbox.h"

/* Old-style (C99-compatible) static assert: flash_program_buffer requires
 * halfword-aligned length, so the frame size must be even. */
typedef char blackbox_frame_size_must_be_even[(sizeof(telemetry_frame_t) % 2U == 0U) ? 1 : -1];

static uint32_t write_index = 0;

void blackbox_erase(void)
{
    for (uint32_t page = 0; page < BLACKBOX_PAGES; page++)
    {
        flash_status_t st = flash_erase_page(BLACKBOX_BASE + (page * FLASH_PAGE_SIZE));
        if (st != FLASH_OK)
        {
            /* caller has no way to know erase failed today — at minimum
             * this should be surfaced, e.g. via a return code */
        }
    }
    write_index = 0;
}

blackbox_status_t blackbox_write_frame(const telemetry_frame_t* frame)
{
    if (write_index >= BLACKBOX_MAX_FRAMES)
    {
        return BLACKBOX_FULL;
    }

    uint32_t addr = BLACKBOX_BASE + (write_index * (uint32_t)sizeof(telemetry_frame_t));

    flash_status_t st =
        flash_program_buffer(addr, (const uint8_t*)frame, sizeof(telemetry_frame_t));
    if (st != FLASH_OK)
    {
        return BLACKBOX_ERR_FLASH;
    }

    write_index++;
    return BLACKBOX_OK;
}

blackbox_status_t blackbox_read_frame(uint32_t index, telemetry_frame_t* out)
{
    if (index >= BLACKBOX_MAX_FRAMES)
    {
        return BLACKBOX_ERR_RANGE;
    }

    uint32_t addr = BLACKBOX_BASE + (index * (uint32_t)sizeof(telemetry_frame_t));
    uint8_t* raw = (uint8_t*)out;

    for (uint32_t i = 0; i < sizeof(telemetry_frame_t); i += 2U)
    {
        uint16_t hw = flash_read_halfword(addr + i);
        raw[i] = (uint8_t)(hw & 0xFFU);
        raw[i + 1] = (uint8_t)(hw >> 8);
    }

    if (out->sync1 != 0xAAU || out->sync2 != 0x55U)
    {
        return BLACKBOX_ERR_CORRUPT; /* unwritten (erased) or corrupted slot */
    }

    uint32_t crc_len = sizeof(telemetry_frame_t) - sizeof(uint16_t);
    if (telemetry_crc16(raw, crc_len) != out->crc)
    {
        return BLACKBOX_ERR_CORRUPT;
    }

    return BLACKBOX_OK;
}

uint32_t blackbox_frame_count(void) { return write_index; }