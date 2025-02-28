#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <execinfo.h>

typedef struct TickernelNodeStruct
{
    void *pData;
    struct TickernelNodeStruct *pNext;
} TickernelNode;

typedef struct
{
    size_t dataSize;
    TickernelNode *pHead;
} TickernelLinkedList;

typedef struct
{
    size_t dataSize;
    uint32_t maxLength;
    uint32_t length;
    void **array;
    TickernelLinkedList removedIndexLinkedList;
} TickernelCollection;

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
void tickernelCombinePaths(char *dstPath, size_t size, const char *srcPath);
bool tickernelStartsWith(const char *str, const char *prefix);
bool tickernelEndsWith(const char *str, const char *suffix);
const char *tickernelGetPathSeparator(void);

void tickernelCreateLinkedList(TickernelLinkedList *pLinkedList, size_t dataSize);
void tickernelDestroyLinkedList(TickernelLinkedList *pLinkedList);
void tickernelAddToLinkedList(TickernelLinkedList *pLinkedList, void *pData);
void tickernelRemoveFromLinkedList(TickernelLinkedList *pLinkedList);
void tickernelClearLinkedList(TickernelLinkedList *pLinkedList);

void tickernelCreateCollection(TickernelCollection *pCollection, size_t dataSize, uint32_t maxLength);
void tickernelDestroyCollection(TickernelCollection *pCollection);
void tickernelAddToCollection(TickernelCollection *pCollection, void *pData, uint32_t *pIndex);
void tickernelRemoveFromCollection(TickernelCollection *pCollection, uint32_t index);
void tickernelClearCollection(TickernelCollection *pCollection);

void tickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, size_t dataSize, uint32_t maxLength);
void tickernelDestroyDynamicArray(TickernelDynamicArray *pDynamicArray);
void tickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData);
void tickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index);
void tickernelClearDynamicArray(TickernelDynamicArray *pDynamicArray);
