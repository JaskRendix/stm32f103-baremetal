ENTRY(Reset_Handler)

/* STM32F103C8T6
 * RAM   = 20K  (0x20000000 → 0x20005000)
 * FLASH = 128K (0x08000000 → 0x08020000)
 */

_ram_start       = 0x20000000;
_ram_end         = 0x20005000;
_estack          = _ram_end;

_min_heap_size   = 0x0000;
_min_stack_size  = 0x0100;

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    /* Interrupt vector table */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* Code + read‑only data */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
    } > FLASH

    /* Data section (load in FLASH, run in RAM) */
    _sidata = LOADADDR(.data);

    .data : AT(_sidata)
    {
        . = ALIGN(4);
        _sdata = .;

        *(.data)
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } > RAM

    /* Zero‑initialized data */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;

        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    PROVIDE(end = _ebss);
    PROVIDE(_end = _ebss);

    /* Heap + stack reservation */
    ._user_heap_stack :
    {
        . = ALIGN(4);
        . += _min_heap_size;
        . += _min_stack_size;
        . = ALIGN(4);
    } > RAM

    /* Remove unused standard library sections */
    /DISCARD/ :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }
}
