#ifndef BLACKBOX_H
#define BLACKBOX_H

#include <stdint.h>
#include "flash.h"
#include "telemetry.h"

/* Reserve the flash region used as an append-only telemetry log.
 * Sized to comfortably hold a full simulated flight's worth of frames. */
#define BLACKBOX_BASE FLASH_BASE_ADDR
#define BLACKBOX_PAGES 8U
#define BLACKBOX_SIZE (BLACKBOX_PAGES * FLASH_PAGE_SIZE)
#define BLACKBOX_MAX_FRAMES (BLACKBOX_SIZE / sizeof(telemetry_frame_t))

typedef enum
{
    BLACKBOX_OK = 0,
    BLACKBOX_FULL,       /* log region exhausted */
    BLACKBOX_ERR_FLASH,  /* underlying flash_program_buffer failed */
    BLACKBOX_ERR_RANGE,  /* read index out of range */
    BLACKBOX_ERR_CORRUPT /* sync bytes or CRC mismatch on read-back */
} blackbox_status_t;

/* Erases the entire black-box region and resets the write cursor.
 * Must be called once before the first write. */
void blackbox_erase(void);

/* Appends one frame to the next free slot. */
blackbox_status_t blackbox_write_frame(const telemetry_frame_t* frame);

/* Reads back frame at the given index and validates sync bytes + CRC. */
blackbox_status_t blackbox_read_frame(uint32_t index, telemetry_frame_t* out);

/* Number of frames successfully written so far this run. */
uint32_t blackbox_frame_count(void);

#endif /* BLACKBOX_H */