#include <tickernelCore.h>
#include <stdarg.h>

#if PLATFORM_POSIX
#include <unistd.h>
void TKNSleep(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}

void TKNGetCurrentDirectory(char *directroy, size_t size)
{
    getcwd(directroy, size);
}
#define PATH_SEPARATOR "/"
#elif PLATFORM_WINDOWS
#include <windows.h>
void TKNSleep(uint32_t milliseconds)
{
    Sleep(milliseconds);
}

void TKNGetCurrentDirectory(char *directroy, size_t size)
{
    GetCurrentDirectory(size, directroy);
}
#define PATH_SEPARATOR "\\"
#else
#error "Unknown platform"
#endif

void *TKNMalloc(size_t size)
{
    return malloc(size);
}

void TKNFree(void *block)
{
    free(block);
}

bool TKNEndsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return false;
    size_t strLength = strlen(str);
    size_t suffixLength = strlen(suffix);
    if (suffixLength > strLength)
        return false;
    return strncmp(str + strLength - suffixLength, suffix, suffixLength) == 0;
}

void TKNCombinePaths(char *dstPath, rsize_t size, const char *srcPath, ...)
{
    va_list args;
    va_start(args, srcPath);
    const char *currentPath = srcPath;
    while ((NULL != currentPath))
    {
        if (TKNEndsWith(dstPath, PATH_SEPARATOR))
        {
            // continue.
        }
        else
        {
            errno_t result = strcat_s(dstPath, size, PATH_SEPARATOR);
        }
        errno_t result = strcat_s(dstPath, size, currentPath);
        if (0 == result)
        {
            // continue.
        }
        else
        {
            perror("strcat_s error");
        }
        currentPath = va_arg(args, const char *);
    }
    va_end(args);
}
