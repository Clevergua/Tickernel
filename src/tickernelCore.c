#include "tickernelCore.h"

#define TICKERNEL_POINTER_SIZE sizeof(void *)

void tickernelError(char const *const _Format, ...)
{
    va_list args;
    va_start(args, _Format);
    vfprintf(stderr, _Format, args);
    va_end(args);

    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **symbols = backtrace_symbols(buffer, nptrs);

    if (symbols == NULL)
    {
        tickernelError("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nptrs; i++)
    {
        printf("%s\n", symbols[i]);
    }

    tickernelFree(symbols);

    abort();
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

void tickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t maxLength, size_t dataSize)
{
    pDynamicArray->maxLength = maxLength;
    pDynamicArray->length = 0;
    pDynamicArray->dataSize = dataSize;
    pDynamicArray->array = (void **)tickernelMalloc(TICKERNEL_POINTER_SIZE * maxLength);
    for (uint32_t i = 0; i < maxLength; i++)
    {
        pDynamicArray->array[i] = NULL;
    }
}
void tickernelDestroyDynamicArray(TickernelDynamicArray dynamicArray)
{
    tickernelClearDynamicArray(&dynamicArray);
    tickernelFree(dynamicArray.array);
}

void tickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pInput, uint32_t index, void *pOutput)
{
    if (pDynamicArray->length >= pDynamicArray->maxLength)
    {
        pDynamicArray->maxLength *= 2;
        void **newArray = (void **)tickernelMalloc(sizeof(void *) * pDynamicArray->maxLength);
        memcpy(newArray, pDynamicArray->array, sizeof(void *) * pDynamicArray->length);
        tickernelFree(pDynamicArray->array);
        pDynamicArray->array = newArray;
    }

    pOutput = tickernelMalloc(pDynamicArray->dataSize);
    if (index < pDynamicArray->length)
    {
        memmove(&pDynamicArray->array[index + 1], &pDynamicArray->array[index], (pDynamicArray->length - index) * sizeof(void *));
        memcpy(pOutput, pInput, pDynamicArray->dataSize);
        pDynamicArray->array[index] = pOutput;
    }
    else if (index == pDynamicArray->length)
    {
        memcpy(pOutput, pInput, pDynamicArray->dataSize);
        pDynamicArray->array[pDynamicArray->length] = pOutput;
    }
    else
    {
        tickernelError("Index out of bounds!\n");
    }
    pDynamicArray->length++;
}

void tickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData)
{
    for (size_t i = 0; i < pDynamicArray->length; i++)
    {
        if (pData == pDynamicArray->array[i])
        {
            tickernelFree(pDynamicArray->array[i]);
            if (i < pDynamicArray->length - 1)
            {
                memmove(&pDynamicArray->array[i], &pDynamicArray->array[i + 1], (pDynamicArray->length - i - 1) * sizeof(void *));
            }
            else
            {
                pDynamicArray->length--;
            }
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

    tickernelFree(pDynamicArray->array[index]);
    if (index < pDynamicArray->length - 1)
    {
        memmove(&pDynamicArray->array[index], &pDynamicArray->array[index + 1], (pDynamicArray->length - index - 1) * sizeof(void *));
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
    for (uint32_t i = 0; i < pDynamicArray->length; i++)
    {
        if (pDynamicArray->array[i] != NULL)
        {
            tickernelFree(pDynamicArray->array[i]);
            pDynamicArray->array[i] = NULL;
        }
    }
    pDynamicArray->length = 0;
}
