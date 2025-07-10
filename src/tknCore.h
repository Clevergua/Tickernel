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

#define TKN_ARRAY_COUNT(array) (NULL == array) ? 0 : (sizeof(array) / sizeof(array[0]))
#define TKN_CLAMP(x, min, max) \
    ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

typedef struct
{
    uint32_t maxCount;
    uint32_t count;
    size_t dataSize;
    void *array;
} TknDynamicArray;

void tknError(char const *const _Format, ...);
void tknAssert(bool condition, char const *const _Format, ...);
void tknSleep(uint32_t milliseconds);
void *tknMalloc(size_t size);
void tknFree(void *ptr);

void tknCreateDynamicArray(size_t dataSize, uint32_t maxCount, TknDynamicArray *pDynamicArray);
void tknDestroyDynamicArray(TknDynamicArray dynamicArray);
void tknAddToDynamicArray(TknDynamicArray *pDynamicArray, void *pInput, uint32_t index);
void tknRemoveFromDynamicArray(TknDynamicArray *pDynamicArray, void *pData);
void tknRemoveAtIndexFromDynamicArray(TknDynamicArray *pDynamicArray, uint32_t index);
void tknClearDynamicArray(TknDynamicArray *pDynamicArray);
void tknGetFromDynamicArray(TknDynamicArray *pDynamicArray, uint32_t index, void **output);