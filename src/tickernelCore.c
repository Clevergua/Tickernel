#include <tickernelCore.h>
#include <stdarg.h>
#include <string.h>
#if PLATFORM_POSIX
#include <unistd.h>
#define Tickernel_PATH_SEPARATOR "/"
#elif PLATFORM_WINDOWS
#include <windows.h>
#define Tickernel_PATH_SEPARATOR "\\"
#else
#error "Unknown platform"
#endif

void TickernelSleep(uint32_t milliseconds)
{
#if PLATFORM_POSIX
    usleep(milliseconds * 1000);
#elif PLATFORM_WINDOWS
    Sleep(milliseconds);
#else
#error "Unknown platform"
#endif
}

void TickernelGetCurrentDirectory(char *path, size_t size)
{
#if PLATFORM_POSIX
    char *result = getcwd(path, size);
    printf("Current working directory: %s\n", result);
#elif PLATFORM_WINDOWS
    GetCurrentDirectory(size, path);
#else
#error "Unknown platform"
#endif
}

void *TickernelMalloc(size_t size)
{
    return malloc(size);
}

void TickernelFree(void *block)
{
    free(block);
}

bool TickernelStartsWith(const char *str, const char *prefix)
{
    int strLength = strlen(str);
    int prefixLength = strlen(prefix);
    if (strLength < prefixLength)
    {
        return false;
    }
    else
    {
        int result = strncmp(str, prefix, prefixLength);
        return 0 == result;
    }
}

bool TickernelEndsWith(const char *str, const char *suffix)
{
    int strLength = strlen(str);
    int suffixLength = strlen(suffix);
    if (strLength < suffixLength)
    {
        return false;
    }
    else
    {
        for (int i = 0; i < suffixLength; i++)
        {
            if (str[i + strLength - suffixLength] != suffix[i])
            {
                return false;
            }
        }
        return true;
    }
}

void TickernelCombinePaths(char *dstPath, size_t dstPathSize, const char *srcPath)
{
    size_t dstPathLength = strlen(dstPath);
    size_t srcPathLength = strlen(srcPath);
    if (TickernelEndsWith(dstPath, Tickernel_PATH_SEPARATOR))
    {
        if (TickernelStartsWith(srcPath, Tickernel_PATH_SEPARATOR))
        {
            size_t separatorLength = strlen(Tickernel_PATH_SEPARATOR);
            dstPath[dstPathLength - separatorLength] = '\0';
            strcat(dstPath, srcPath);
        }
        else
        {
            strcat(dstPath, srcPath);
        }
    }
    else if (TickernelStartsWith(srcPath, Tickernel_PATH_SEPARATOR))
    {
        strcat(dstPath, srcPath);
    }
    else
    {
        strcat(dstPath, Tickernel_PATH_SEPARATOR);
        strcat(dstPath, srcPath);
    }
}