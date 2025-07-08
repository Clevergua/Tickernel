#include "tickernelCore.h"

static void tickernelInternalError(const char *prefix, const char *format, va_list args)
{
    if (prefix)
    {
        fprintf(stderr, "%s: ", prefix);
    }

    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **symbols = backtrace_symbols(buffer, nptrs);

    if (symbols)
    {
        fprintf(stderr, "Backtrace:\n");
        for (int i = 0; i < nptrs; i++)
        {
            fprintf(stderr, "  %s\n", symbols[i]);
        }
        free(symbols);
    }
    else
    {
        fprintf(stderr, "Failed to get backtrace symbols\n");
    }

    abort();
}

void tickernelError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    tickernelInternalError("ERROR", format, args);
    va_end(args);
}

void tickernelAssert(bool condition, const char *format, ...)
{
    if (!condition)
    {
        va_list args;
        va_start(args, format);
        tickernelInternalError("ASSERTION FAILED", format, args);
        va_end(args);
    }
}

void tickernelSleep(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}

void *tickernelMalloc(size_t size)
{
    return malloc(size);
}

void tickernelFree(void *block)
{
    free(block);
}

void tickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, size_t dataSize, uint32_t maxCount)
{
    pDynamicArray->maxCount = maxCount;
    pDynamicArray->count = 0;
    pDynamicArray->dataSize = dataSize;
    pDynamicArray->array = tickernelMalloc(dataSize * maxCount);
    memset(pDynamicArray->array, 0, dataSize * maxCount);
}
void tickernelDestroyDynamicArray(TickernelDynamicArray dynamicArray)
{
    tickernelFree(dynamicArray.array);
    dynamicArray.array = NULL;
    dynamicArray.count = 0;
    dynamicArray.maxCount = 0;
}

void tickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pInput, uint32_t index)
{
    tickernelAssert(index <= pDynamicArray->count, "Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
    if (pDynamicArray->count >= pDynamicArray->maxCount)
    {
        pDynamicArray->maxCount *= 2;
        void *newArray = tickernelMalloc(pDynamicArray->dataSize * pDynamicArray->maxCount);
        memcpy(newArray, pDynamicArray->array, pDynamicArray->dataSize * pDynamicArray->count);
        tickernelFree(pDynamicArray->array);
        pDynamicArray->array = newArray;
    }
    void *targetAddress = (char *)pDynamicArray->array + index * pDynamicArray->dataSize;
    if (index < pDynamicArray->count)
    {
        memmove(
            (char *)pDynamicArray->array + (index + 1) * pDynamicArray->dataSize,
            targetAddress,
            (pDynamicArray->count - index) * pDynamicArray->dataSize);
    }
    memcpy(targetAddress, pInput, pDynamicArray->dataSize);
    pDynamicArray->count++;
}

void tickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData)
{
    for (uint32_t i = 0; i < pDynamicArray->count; i++)
    {
        void *currentElement = (char *)pDynamicArray->array + i * pDynamicArray->dataSize;
        if (memcmp(currentElement, pData, pDynamicArray->dataSize) == 0)
        {
            if (i < pDynamicArray->count - 1)
            {
                memmove(
                    currentElement,
                    (char *)pDynamicArray->array + (i + 1) * pDynamicArray->dataSize,
                    (pDynamicArray->count - i - 1) * pDynamicArray->dataSize);
            }
            pDynamicArray->count--;
            return;
        }
    }
    tickernelError("Data not found!\n");
}

void tickernelRemoveAtIndexFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index)
{
    tickernelAssert(index < pDynamicArray->count, "Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
    void *target = (char *)pDynamicArray->array + index * pDynamicArray->dataSize;
    if (index < pDynamicArray->count - 1)
    {
        memmove(
            target,
            (char *)pDynamicArray->array + (index + 1) * pDynamicArray->dataSize,
            (pDynamicArray->count - index - 1) * pDynamicArray->dataSize);
    }
    pDynamicArray->count--;
}

void tickernelClearDynamicArray(TickernelDynamicArray *pDynamicArray)
{
    pDynamicArray->count = 0;
    memset(pDynamicArray->array, 0, pDynamicArray->dataSize * pDynamicArray->maxCount);
}

void tickernelGetFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index, void **output)
{
    if (index < pDynamicArray->count)
    {
        *output = (char *)pDynamicArray->array + index * pDynamicArray->dataSize;
        printf("Get from dynamic array at index %u: %p\n", index, *output);
    }
    else
    {
        tickernelError("Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
    }
}

bool tickernelContainsDynamicArray(TickernelDynamicArray *pDynamicArray, const void *pInput, TickernelCompareFunction compareFunction)
{
    for (uint32_t i = 0; i < pDynamicArray->count; i++)
    {
        void *item = (char *)pDynamicArray->array + i * pDynamicArray->dataSize;
        if (compareFunction(item, pInput, pDynamicArray->dataSize))
        {
            return true;
        }
    }
    return false;
}

bool tickernelCompareMemory(const void *a, const void *b, size_t size)
{
    return (memcmp(a, b, size) == 0);
}