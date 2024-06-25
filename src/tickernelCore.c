#include <tickernelCore.h>
#include <stdarg.h>
static FILE *const stream;

#if PLATFORM_POSIX
#include <unistd.h>
void TickernelSleep(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}
#elif PLATFORM_WINDOWS
#include <windows.h>
void TickernelSleep(uint32_t milliseconds)
{
    Sleep(milliseconds);
}
#else
#error "Unknown platform"
#endif