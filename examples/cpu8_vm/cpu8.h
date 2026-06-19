#ifndef CPU8_H
#define CPU8_H

#include <stdint.h>
#include <stddef.h>

#define CPU8_RAM_SIZE 256
#define CPU8_NUM_REGS 8

typedef struct {
    uint8_t regs[CPU8_NUM_REGS]; /* R0..R7 */
    uint16_t pc;                 /* program counter */
    uint8_t sp;                  /* stack pointer (in RAM) */
    uint8_t zf;                  /* zero flag */
    uint8_t cf;                  /* carry flag */
    uint8_t nf;                  /* negative flag */
    uint8_t ram[CPU8_RAM_SIZE];  /* data memory + stack + I/O */
    const uint8_t *rom;          /* program memory */
    size_t rom_size;
} cpu8_t;

/* Opcodes */
enum {
    CPU8_NOP   = 0x00,
    CPU8_HALT  = 0xFF,

    CPU8_LOADI = 0x10, /* LOADI Rn, imm8        : 10 nn ii */
    CPU8_LOAD  = 0x11, /* LOAD  Rn, [addr8]     : 11 nn aa */
    CPU8_STORE = 0x12, /* STORE Rn, [addr8]     : 12 nn aa */

    CPU8_ADD   = 0x20, /* ADD   Rn, Rm          : 20 nn mm */
    CPU8_SUB   = 0x21, /* SUB   Rn, Rm          : 21 nn mm */

    CPU8_JMP   = 0x30, /* JMP   addr16          : 30 lo hi */
    CPU8_JZ    = 0x31, /* JZ    addr16          : 31 lo hi */
    CPU8_JNZ   = 0x32, /* JNZ   addr16          : 32 lo hi */

    CPU8_OUT   = 0x40  /* OUT   Rn, addr8 (I/O) : 40 nn aa */
};

void cpu8_init(cpu8_t *cpu, const uint8_t *rom, size_t rom_size);
int  cpu8_step(cpu8_t *cpu); /* returns 0 on HALT, 1 on continue */

#endif
