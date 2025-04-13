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

typedef struct
{
    size_t dataSize;
    uint32_t maxLength;
    uint32_t length;
    void **array;
} TickernelDynamicArray;

void tickernelError(char const *const _Format, ...);
void tickernelSleep(uint32_t milliseconds);
void *tickernelMalloc(size_t size);
void tickernelFree(void *block);

void tickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t maxLength, size_t dataSize);
void tickernelDestroyDynamicArray(TickernelDynamicArray *pDynamicArray);
void *tickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData, uint32_t index);
void tickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, void* pData);
void tickernelRemoveAtIndexFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index);
void tickernelClearDynamicArray(TickernelDynamicArray *pDynamicArray);
