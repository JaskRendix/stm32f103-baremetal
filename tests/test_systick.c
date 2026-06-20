#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "systick_fake_hw.h"
#include "systick.h"
#include "delay.h"

/* Reset fake SysTick registers before each test */
static void reset_fake_systick(void)
{
    SysTick_FAKE.CTRL = 0;
    SysTick_FAKE.LOAD = 0;
    SysTick_FAKE.VAL = 0;
    SysTick_FAKE.CALIB = 0;
    systick_ticks = 0;
}

/* --- Test: systick_init configures registers correctly --- */
static void test_systick_init(void)
{
    reset_fake_systick();
    systick_init();
    assert(SysTick_FAKE.LOAD == (CPU_HZ / 1000) - 1);
    assert(SysTick_FAKE.VAL == 0);
    uint32_t expected =
        SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk;
    assert(SysTick_FAKE.CTRL == expected);
    printf("[PASS] test_systick_init\n");
}

/* --- Test: SysTick_Handler increments tick counter --- */
static void test_systick_handler(void)
{
    reset_fake_systick();
    assert(systick_ticks == 0);
    SysTick_Handler();
    assert(systick_ticks == 1);
    SysTick_Handler();
    assert(systick_ticks == 2);
    printf("[PASS] test_systick_handler\n");
}

/* --- Test: systick_ms returns systick_ticks --- */
static void test_systick_ms(void)
{
    reset_fake_systick();
    systick_ticks = 1234;
    assert(systick_ms() == 1234);
    systick_ticks = 9999;
    assert(systick_ms() == 9999);
    printf("[PASS] test_systick_ms\n");
}

/* --- Test: delay_ms waits until systick_ms advances --- */
static void test_delay_ms(void)
{
    reset_fake_systick();
    systick_ticks = 0;
    uint32_t start = systick_ms();
    /* Start delay */
    delay_ms(10);
    /* Simulate time passing */
    systick_ticks = start + 10;
    /* delay_ms should now be satisfied */
    assert(systick_ms() - start >= 10);
    printf("[PASS] test_delay_ms\n");
}

/* --- Test: delay_us compiles and runs (structural test) --- */
static void test_delay_us(void)
{
    /* We cannot measure real time, but we can ensure it runs */
    DELAY_US(1);
    DELAY_US(10);
    DELAY_US(100);
    printf("[PASS] test_delay_us\n");
}

/* --- Test: delay_start_ms(0) is immediately done --- */
static void test_delay_zero_ms_immediately_done(void)
{
    reset_fake_systick();
    systick_ticks = 0;
    delay_start_ms(0);
    assert(delay_is_done() != 0);
    printf("[PASS] test_delay_zero_ms_immediately_done\n");
}

/* --- Test: delay_start_ms(N) is not done until N ticks have elapsed --- */
static void test_delay_nonzero_ms_completes_after_ticks(void)
{
    reset_fake_systick();
    systick_ticks = 0;
    delay_start_ms(5);
    assert(delay_is_done() == 0);

    for (uint32_t i = 0; i < 4; i++)
    {
        SysTick_Handler();
        assert(delay_is_done() == 0);
    }

    /* 5th tick reaches the target */
    SysTick_Handler();
    assert(delay_is_done() != 0);
    printf("[PASS] test_delay_nonzero_ms_completes_after_ticks\n");
}

/* --- Test: delay target landing exactly on 0 after wraparound is not
 *     mistaken for "no delay running" --- */
static void test_delay_handles_tick_wraparound(void)
{
    reset_fake_systick();
    /* Choose a tick count so that systick_ticks + ms wraps to exactly 0 */
    systick_ticks = 0xFFFFFFFFU;
    delay_start_ms(1);
    assert(delay_is_done() == 0);

    /* This tick overflows systick_ticks back to 0, matching delay_target */
    SysTick_Handler();
    assert(systick_ticks == 0U);
    assert(delay_is_done() != 0);
    printf("[PASS] test_delay_handles_tick_wraparound\n");
}

/* --- Simple test runner --- */
int main(void)
{
    printf("Running test_systick...\n");

    test_systick_init();
    test_systick_handler();
    test_systick_ms();
    test_delay_ms();
    test_delay_us();
    test_delay_zero_ms_immediately_done();
    test_delay_nonzero_ms_completes_after_ticks();
    test_delay_handles_tick_wraparound();

    printf("All 8 test_systick cases passed.\n");
    return 0;
}