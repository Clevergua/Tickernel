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
    uint32_t maxArrayLength;
    uint32_t arrayLength;
    void **array;
    TickernelLinkedList removedIndexLinkedList;
} TickernelDynamicArray;

void TickernelError(char const *const _Format, ...);
void TickernelSleep(uint32_t milliseconds);
void *TickernelMalloc(size_t size);
void TickernelFree(void *block);
void TickernelCombinePaths(char *dstPath, size_t size, const char *srcPath);
bool TickernelStartsWith(const char *str, const char *prefix);
bool TickernelEndsWith(const char *str, const char *suffix);
const char *TickernelGetPathSeparator();
void TickernelAddNode(TickernelLinkedList *pList, void *pData);
void TickernelRemoveNode(TickernelLinkedList *pList);
void TickernelClearLinkedList(TickernelLinkedList *pList);