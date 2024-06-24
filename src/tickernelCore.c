#include <tickernelCore.h>
#include <stdarg.h>
static FILE *const stream;

#ifdef PLATFORM_POSIX
#include <unistd.h>
void TickernelSleep(uint32_t milliseconds)
{
    int result = usleep(milliseconds * 1000);
    if (EXIT_SUCCESS == result)
    {
        // Do nothing.
    }
    else
    {
        abort();
    }
}
#elif defined(PLATFORM_WINDOWS)
#include <windows.h>
void TickernelSleep(uint32_t milliseconds)
{
    Sleep(milliseconds);
}
#else
#error "Unknown platform"
#endif

void TickernelLog(char const *const format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}
