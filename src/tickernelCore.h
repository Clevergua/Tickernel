#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <tickernelPlatform.h>
typedef struct Uint32NodeStruct
{
    struct Uint32NodeStruct *pNext;
    uint32_t data;
} Uint32Node;

void TickernelSleep(uint32_t milliseconds);
void TickernelLog(char const *const _Format, ...);
void *TickernelMalloc(size_t size);
void TickernelFree(void *block);
void TickernelGetCurrentDirectory(char *path, size_t size);
void TickernelCombinePaths(char *dstPath, size_t size, const char *srcPath);
bool TickernelStartsWith(const char *str, const char *prefix);
bool TickernelEndsWith(const char *str, const char *suffix);
