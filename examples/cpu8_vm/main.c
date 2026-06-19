#include <stdio.h>
#include "cpu8.h"

/*
 * Tiny program for the fake CPU:
 *
 * 0x0000: LOADI R0, 0x01
 * 0x0003: LOADI R1, 0x20
 * 0x0006: ADD   R0, R1
 * 0x0009: OUT   R0, 0xF0   ; write to I/O port 0xF0
 * 0x000C: HALT
 */

static const uint8_t program[] = {
    CPU8_LOADI, 0x00, 0x01,      /* R0 = 0x01 */
    CPU8_LOADI, 0x01, 0x20,      /* R1 = 0x20 */
    CPU8_ADD,   0x00, 0x01,      /* R0 = R0 + R1 = 0x21 */
    CPU8_OUT,   0x00, 0xF0,      /* OUT R0 -> port 0xF0 */
    CPU8_HALT                      /* stop */
};

int main(void)
{
    cpu8_t cpu;
    cpu8_init(&cpu, program, sizeof(program));

    printf("CPU8 VM starting...\n");

    int steps = 0;
    while (cpu8_step(&cpu)) {
        steps++;
        if (steps > 1000) {
            printf("CPU8: safety stop after %d steps\n", steps);
            break;
        }
    }

    printf("CPU8 VM halted after %d steps\n", steps);
    printf("Final R0 = 0x%02X\n", cpu.regs[0]);
    printf("Flags: Z=%u C=%u N=%u\n", cpu.zf, cpu.cf, cpu.nf);

    return 0;
}
