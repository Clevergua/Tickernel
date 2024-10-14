#include <tickernelCore.h>

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
    const char *pathSeparator = TickernelGetPathSeparator();
    size_t dstPathLength = strlen(dstPath);
    size_t srcPathLength = strlen(srcPath);
    if (TickernelEndsWith(dstPath, pathSeparator))
    {
        if (TickernelStartsWith(srcPath, pathSeparator))
        {
            size_t separatorLength = strlen(pathSeparator);
            dstPath[dstPathLength - separatorLength] = '\0';
            strcat(dstPath, srcPath);
        }
        else
        {
            strcat(dstPath, srcPath);
        }
    }
    else if (TickernelStartsWith(srcPath, pathSeparator))
    {
        strcat(dstPath, srcPath);
    }
    else
    {
        strcat(dstPath, pathSeparator);
        strcat(dstPath, srcPath);
    }
}

const char *TickernelGetPathSeparator()
{
#if PLATFORM_POSIX
    return "/";
#elif PLATFORM_WINDOWS
    return "\\";
#else
#error "Unknown platform"
#endif
}