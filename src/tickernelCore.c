#include <tickernelCore.h>
#include <stdarg.h>
#include <string.h>
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
#define TKN_PATH_SEPARATOR "/"
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
#define TKN_PATH_SEPARATOR "\\"
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

bool TKNStartsWith(const char *str, const char *prefix)
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

bool TKNEndsWith(const char *str, const char *suffix)
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

void TKNCombinePaths(char *dstPath, size_t dstPathSize, const char *srcPath)
{
    size_t dstPathLength = strlen(dstPath);
    size_t srcPathLength = strlen(srcPath);
    if (TKNEndsWith(dstPath, TKN_PATH_SEPARATOR))
    {
        if (TKNStartsWith(srcPath, TKN_PATH_SEPARATOR))
        {
            size_t separatorLength = strlen(TKN_PATH_SEPARATOR);
            dstPath[dstPathLength - separatorLength] = '\0';
            strcat(dstPath, srcPath);
        }
        else
        {
            strcat(dstPath, srcPath);
        }
    }
    else if (TKNStartsWith(srcPath, TKN_PATH_SEPARATOR))
    {
        strcat(dstPath, srcPath);
    }
    else
    {
        strcat(dstPath, TKN_PATH_SEPARATOR);
        strcat(dstPath, srcPath);
    }
}