#include "cpu8.h"
#include <stdio.h>

static uint8_t fetch8(cpu8_t* cpu)
{
    if (cpu->pc >= cpu->rom_size)
    {
        return CPU8_HALT;
    }
    return cpu->rom[cpu->pc++];
}

static uint16_t fetch16(cpu8_t* cpu)
{
    uint8_t lo = fetch8(cpu);
    uint8_t hi = fetch8(cpu);
    return (uint16_t)((hi << 8) | lo);
}

static void update_flags(cpu8_t* cpu, uint8_t result, uint16_t wide)
{
    cpu->zf = (result == 0);
    cpu->nf = (result & 0x80) != 0;
    cpu->cf = (wide > 0xFF);
}

void cpu8_init(cpu8_t* cpu, const uint8_t* rom, size_t rom_size)
{
    for (int i = 0; i < CPU8_NUM_REGS; ++i)
    {
        cpu->regs[i] = 0;
    }
    cpu->pc = 0;
    cpu->sp = CPU8_RAM_SIZE - 1;
    cpu->zf = cpu->cf = cpu->nf = 0;
    for (int i = 0; i < CPU8_RAM_SIZE; ++i)
    {
        cpu->ram[i] = 0;
    }
    cpu->rom = rom;
    cpu->rom_size = rom_size;
}

/* Simple I/O: write to "port" addr8 just prints to stdout */
static void cpu8_io_write(cpu8_t* cpu, uint8_t addr, uint8_t value)
{
    (void)cpu;
    printf("[CPU8 I/O] port 0x%02X <= 0x%02X (%u)\n", addr, value, value);
}

int cpu8_step(cpu8_t* cpu)
{
    uint8_t op = fetch8(cpu);

    switch (op)
    {
    case CPU8_NOP:
        return 1;

    case CPU8_HALT:
        return 0;

    case CPU8_LOADI:
    {
        uint8_t r = fetch8(cpu);
        uint8_t imm = fetch8(cpu);
        if (r < CPU8_NUM_REGS)
        {
            cpu->regs[r] = imm;
            update_flags(cpu, cpu->regs[r], cpu->regs[r]);
        }
        return 1;
    }

    case CPU8_LOAD:
    {
        uint8_t r = fetch8(cpu);
        uint8_t addr = fetch8(cpu);
        if (r < CPU8_NUM_REGS)
        {
            cpu->regs[r] = cpu->ram[addr];
            update_flags(cpu, cpu->regs[r], cpu->regs[r]);
        }
        return 1;
    }

    case CPU8_STORE:
    {
        uint8_t r = fetch8(cpu);
        uint8_t addr = fetch8(cpu);
        if (r < CPU8_NUM_REGS)
        {
            cpu->ram[addr] = cpu->regs[r];
        }
        return 1;
    }

    case CPU8_ADD:
    {
        uint8_t r = fetch8(cpu);
        uint8_t m = fetch8(cpu);
        if (r < CPU8_NUM_REGS && m < CPU8_NUM_REGS)
        {
            uint16_t wide = (uint16_t)cpu->regs[r] + (uint16_t)cpu->regs[m];
            cpu->regs[r] = (uint8_t)wide;
            update_flags(cpu, cpu->regs[r], wide);
        }
        return 1;
    }

    case CPU8_SUB:
    {
        uint8_t r = fetch8(cpu);
        uint8_t m = fetch8(cpu);
        if (r < CPU8_NUM_REGS && m < CPU8_NUM_REGS)
        {
            int16_t wide = (int16_t)((int16_t)cpu->regs[r] - (int16_t)cpu->regs[m]);
            uint8_t result = (uint8_t)wide;
            cpu->regs[r] = result;
            cpu->zf = (result == 0);
            cpu->nf = (result & 0x80) != 0;
            cpu->cf = (wide < 0);
        }
        return 1;
    }

    case CPU8_JMP:
    {
        uint16_t addr = fetch16(cpu);
        cpu->pc = addr;
        return 1;
    }

    case CPU8_JZ:
    {
        uint16_t addr = fetch16(cpu);
        if (cpu->zf)
        {
            cpu->pc = addr;
        }
        return 1;
    }

    case CPU8_JNZ:
    {
        uint16_t addr = fetch16(cpu);
        if (!cpu->zf)
        {
            cpu->pc = addr;
        }
        return 1;
    }

    case CPU8_OUT:
    {
        uint8_t r = fetch8(cpu);
        uint8_t addr = fetch8(cpu);
        if (r < CPU8_NUM_REGS)
        {
            cpu8_io_write(cpu, addr, cpu->regs[r]);
        }
        return 1;
    }

    default:
        printf("[CPU8] Unknown opcode 0x%02X at PC=0x%04X\n", op, cpu->pc - 1);
        return 0;
    }
}
