#include "tknCore.h"

static void tknInternalError(const char *prefix, const char *format, va_list args)
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

void tknError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    tknInternalError("ERROR", format, args);
    va_end(args);
}

void tknAssert(bool condition, const char *format, ...)
{
    if (!condition)
    {
        va_list args;
        va_start(args, format);
        tknInternalError("ASSERTION FAILED", format, args);
        va_end(args);
    }
}

void tknSleep(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}

void tknMalloc(size_t size, void **output)
{
    *output = malloc(size);
}

void tknFree(void *block)
{
    free(block);
}

void tknCreateDynamicArray(size_t dataSize, uint32_t maxCount, TknDynamicArray *pDynamicArray)
{
    pDynamicArray->maxCount = maxCount;
    pDynamicArray->count = 0;
    pDynamicArray->dataSize = dataSize;
    tknMalloc(dataSize * maxCount, &pDynamicArray->array);
    memset(pDynamicArray->array, 0, dataSize * maxCount);
}
void tknDestroyDynamicArray(TknDynamicArray dynamicArray)
{
    tknFree(dynamicArray.array);
    dynamicArray.array = NULL;
    dynamicArray.count = 0;
    dynamicArray.maxCount = 0;
}
void tknAddToDynamicArray(TknDynamicArray *pDynamicArray, void *pInput, uint32_t index)
{
    tknAssert(index <= pDynamicArray->count, "Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
    if (pDynamicArray->count >= pDynamicArray->maxCount)
    {
        pDynamicArray->maxCount *= 2;
        void *newArray;
        tknMalloc(pDynamicArray->dataSize * pDynamicArray->maxCount, &newArray);
        memcpy(newArray, pDynamicArray->array, pDynamicArray->dataSize * pDynamicArray->count);
        tknFree(pDynamicArray->array);
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
void tknRemoveFromDynamicArray(TknDynamicArray *pDynamicArray, void *pData)
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
    tknError("Data not found!\n");
}
void tknRemoveAtIndexFromDynamicArray(TknDynamicArray *pDynamicArray, uint32_t index)
{
    tknAssert(index < pDynamicArray->count, "Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
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
void tknClearDynamicArray(TknDynamicArray *pDynamicArray)
{
    pDynamicArray->count = 0;
    memset(pDynamicArray->array, 0, pDynamicArray->dataSize * pDynamicArray->maxCount);
}
void tknGetFromDynamicArray(TknDynamicArray *pDynamicArray, uint32_t index, void **output)
{
    if (index < pDynamicArray->count)
    {
        *output = (char *)pDynamicArray->array + index * pDynamicArray->dataSize;
        printf("Get from dynamic array at index %u: %p\n", index, *output);
    }
    else
    {
        tknError("Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
    }
}
