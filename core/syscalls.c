#include <stddef.h>
#include <sys/stat.h>
#include <errno.h>
#include "uart.h"

/*
 * _write is routed to USART1 so that printf/puts/etc. work over serial.
 * uart_init() must be called before any code path that triggers stdio
 * output (printf, puts, ...), or this will time out on every call
 * (bounded by UART_DEFAULT_TIMEOUT in uart.h, so it cannot hang forever,
 * but it will report a short write until UART is initialized).
 */
int _write(int file, char* ptr, int len)
{
    (void)file;

    for (int i = 0; i < len; i++)
    {
        if (uart_send_char(ptr[i]) != UART_OK)
        {
            /* Partial write, consistent with POSIX semantics. */
            return i;
        }
    }

    return len;
}

/*
 * Read is not currently implemented.
 *
 * A future UART RX implementation could read up to len bytes from the
 * UART into ptr and return the number of bytes received.
 */
int _read(int file, char* ptr, int len)
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

    if (st == NULL)
    {
        errno = EFAULT;
        return -1;
    }

    /*
     * Minimal implementation required by newlib/newlib-nano.
     * The value itself is not used by this project.
     */
    st->st_mode = 0;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}
