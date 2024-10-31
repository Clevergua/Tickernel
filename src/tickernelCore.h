#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <tickernelPlatform.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#if PLATFORM_POSIX
#include <unistd.h>
#elif PLATFORM_WINDOWS
#include <windows.h>
#else
#error "Unknown platform"
#endif

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

void TickernelError(char const *const _Format, ...);
void TickernelSleep(uint32_t milliseconds);
void *TickernelMalloc(size_t size);
void TickernelFree(void *block);
void TickernelCombinePaths(char *dstPath, size_t size, const char *srcPath);
bool TickernelStartsWith(const char *str, const char *prefix);
bool TickernelEndsWith(const char *str, const char *suffix);
const char *TickernelGetPathSeparator();

void TickernelCreateLinkedList(TickernelLinkedList *pLinkedList, size_t dataSize);
void TickernelDestroyLinkedList(TickernelLinkedList *pLinkedList);
void TickernelAddToLinkedList(TickernelLinkedList *pLinkedList, void *pData);
void TickernelRemoveFromLinkedList(TickernelLinkedList *pLinkedList);
void TickernelClearLinkedList(TickernelLinkedList *pLinkedList);

void TickernelCreateCollection(TickernelCollection *pCollection, size_t dataSize, uint32_t maxLength);
void TickernelDestroyCollection(TickernelCollection *pCollection);
void TickernelAddToCollection(TickernelCollection *pCollection, void *pData, uint32_t *pOutputIndex);
void TickernelRemoveFromCollection(TickernelCollection *pCollection, uint32_t index);
void TickernelClearCollection(TickernelCollection *pCollection);