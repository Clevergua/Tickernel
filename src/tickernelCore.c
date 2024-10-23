#include <tickernelCore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#if PLATFORM_POSIX
#include <execinfo.h>
#elif PLATFORM_WINDOWS
#include <dbghelp.h>
#include <windows.h>
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

void TickernelError(char const *const _Format, ...)
{
    va_list args;
    va_start(args, _Format);
    vfprintf(stderr, _Format, args);
    va_end(args);

#if PLATFORM_POSIX
    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **symbols = backtrace_symbols(buffer, nptrs);

    if (symbols == NULL)
    {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nptrs; i++)
    {
        printf("%s\n", symbols[i]);
    }

    free(symbols);
#elif PLATFORM_WINDOWS
    void *stack[100];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD frames = CaptureStackBackTrace(0, 100, stack, NULL);
    SYMBOL_INFO *symbol = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (int i = 0; i < frames; i++)
    {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        printf("%i: %s - 0x%0llX\n", frames - i - 1, symbol->Name, symbol->Address);
    }

    free(symbol);
    SymCleanup(process);
#else
#error "Unknown platform"
#endif
    abort();
}

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