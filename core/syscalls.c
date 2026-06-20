#include <sys/stat.h>
#include <errno.h>
#include "uart.h"

/*
 * _write is routed to USART1 so that printf/puts/etc. work over serial.
 * uart_init() must be called before any code path that triggers stdio
 * output (printf, puts, ...), or this will time out on every call
 * (bounded by UART_DEFAULT_TIMEOUT in uart.h, so it cannot hang forever,
 * but it will silently report a short/failed write until UART is
 * initialized).
 */
int _write(int file, char* ptr, int len)
{
    (void)file;

    for (int i = 0; i < len; i++)
    {
        if (uart_send_char(ptr[i]) != UART_OK)
        {
            return i; /* partial write, matches POSIX _write semantics */
        }
    }

    return len;
}

/*
 * ptr is const here because this stub never writes through it. If a
 * real UART RX implementation is added later, ptr becomes the
 * destination buffer for incoming bytes and this should go back to
 * non-const at that point.
 */
int _read(int file, const char* ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    errno = ENOSYS;
    return -1;
}

int _close(int file)
{
    (void)file;
    errno = ENOSYS;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    errno = ENOSYS;
    return -1;
}

int _fstat(int file, struct stat* st)
{
    (void)file;
    st->st_mode = 0; // <-- correct for newlib-nano
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}