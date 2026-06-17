#include <sys/stat.h>
#include <errno.h>

int _write(int file, char* ptr, int len)
{
    errno = ENOSYS;
    return -1;
}

int _read(int file, char* ptr, int len)
{
    errno = ENOSYS;
    return -1;
}

int _close(int file)
{
    errno = ENOSYS;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    errno = ENOSYS;
    return -1;
}

int _fstat(int file, struct stat* st)
{
    st->st_mode = 0; // <-- correct for newlib-nano
    return 0;
}

int _isatty(int file) { return 1; }
