#include <string.h>
#include "flash_fake_hw.h"

flash_fake_t FLASH_FAKE;

void flash_fake_reset(void)
{
    FLASH_FAKE.KEYR = 0;
    FLASH_FAKE.SR = 0;
    FLASH_FAKE.CR = FLASH_CR_LOCK; /* reset value: locked */
    FLASH_FAKE.AR = 0;
    memset(FLASH_FAKE.memory, 0xFF, sizeof(FLASH_FAKE.memory));
}