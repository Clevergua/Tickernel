#include "tickernelCore.h"

#define TICKERNEL_POINTER_SIZE sizeof(void *)

static void tickernelInternalError(const char *prefix, const char *format, va_list args)
{
    if (prefix) {
        fprintf(stderr, "%s: ", prefix);
    }
    
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **symbols = backtrace_symbols(buffer, nptrs);
    
    if (symbols) {
        fprintf(stderr, "Backtrace:\n");
        for (int i = 0; i < nptrs; i++) {
            fprintf(stderr, "  %s\n", symbols[i]);
        }
        free(symbols);
    } else {
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
    if (!condition) {
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

void tickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t maxLength)
{
    pDynamicArray->maxLength = maxLength;
    pDynamicArray->length = 0;
    pDynamicArray->array = (void **)tickernelMalloc(TICKERNEL_POINTER_SIZE * maxLength);
    for (uint32_t i = 0; i < maxLength; i++)
    {
        pDynamicArray->array[i] = NULL;
    }
}
void tickernelDestroyDynamicArray(TickernelDynamicArray dynamicArray)
{
    tickernelFree(dynamicArray.array);
    dynamicArray.array = NULL;
    dynamicArray.length = 0;
    dynamicArray.maxLength = 0;
}

void tickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pInput, uint32_t index)
{
    if (pDynamicArray->length >= pDynamicArray->maxLength)
    {
        pDynamicArray->maxLength *= 2;
        void **newArray = (void **)tickernelMalloc(sizeof(void *) * pDynamicArray->maxLength);
        memcpy(newArray, pDynamicArray->array, sizeof(void *) * pDynamicArray->length);
        tickernelFree(pDynamicArray->array);
        pDynamicArray->array = newArray;
    }

    if (index < pDynamicArray->length)
    {
        memmove(&pDynamicArray->array[index + 1], &pDynamicArray->array[index], (pDynamicArray->length - index) * sizeof(void *));
        pDynamicArray->array[index] = pInput;
    }
    else if (index == pDynamicArray->length)
    {
        pDynamicArray->array[pDynamicArray->length] = pInput;
    }
    pDynamicArray->length++;
}

void tickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData)
{
    for (size_t i = 0; i < pDynamicArray->length; i++)
    {
        if (pData == pDynamicArray->array[i])
        {
            if (i < pDynamicArray->length - 1)
            {
                memmove(&pDynamicArray->array[i], &pDynamicArray->array[i + 1], (pDynamicArray->length - i - 1) * TICKERNEL_POINTER_SIZE);
            }
            else
            {
                // do nothing, we are removing the last element.
            }
            pDynamicArray->length--;
            pDynamicArray->array[pDynamicArray->length] = NULL;
            return;
        }
    }
    tickernelError("Data not found!\n");
}

void tickernelRemoveAtIndexFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index)
{
    if (index >= pDynamicArray->length)
    {
        tickernelError("Index out of bounds!\n");
    }

    if (index < pDynamicArray->length - 1)
    {
        memmove(&pDynamicArray->array[index], &pDynamicArray->array[index + 1], (pDynamicArray->length - index - 1) * TICKERNEL_POINTER_SIZE);
    }
    else
    {
        // do nothing
    }
    pDynamicArray->length--;
    pDynamicArray->array[pDynamicArray->length] = NULL;
}

void tickernelClearDynamicArray(TickernelDynamicArray *pDynamicArray)
{
    pDynamicArray->length = 0;
}
