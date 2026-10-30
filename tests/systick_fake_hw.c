#include "systick_fake_hw.h"

systick_fake_t SysTick_FAKE = {0};

void systick_fake_reset(void)
{
    SysTick_FAKE.CTRL = 0;
    SysTick_FAKE.LOAD = 0;
    SysTick_FAKE.VAL = 0;
    SysTick_FAKE.CALIB = 0;
}
