#pragma once
// standard
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
// posix
#include <unistd.h>
#include <execinfo.h>

#define TICKERNEL_ARRAY_COUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))

typedef struct
{
    uint32_t maxCount;
    uint32_t count;
    size_t dataSize;
    void *array;
} TickernelDynamicArray;

void tickernelError(char const *const _Format, ...);
void tickernelAssert(bool condition, char const *const _Format, ...);
void tickernelSleep(uint32_t milliseconds);
void *tickernelMalloc(size_t size);
void tickernelFree(void *block);

void tickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, size_t dataSize, uint32_t maxCount);
void tickernelDestroyDynamicArray(TickernelDynamicArray dynamicArray);
void tickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pInput, uint32_t index);
void tickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData);
void tickernelRemoveAtIndexFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index);
void tickernelClearDynamicArray(TickernelDynamicArray *pDynamicArray);
void tickernelGetFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index, void **output);
typedef bool (*TickernelCompareFunction)(const void *, const void *, size_t size);
bool tickernelContainsDynamicArray(TickernelDynamicArray *pDynamicArray, void *pInput, TickernelCompareFunction compareFunction);

#define TICKERNEL_GET_FROM_DYNAMIC_ARRAY(pDynamicArray, index, type)    \
    ({                                                                  \
        void *__tmp;                                                    \
        tickernelGetFromDynamicArray((pDynamicArray), (index), &__tmp); \
        *(type *)(__tmp);                                               \
    })