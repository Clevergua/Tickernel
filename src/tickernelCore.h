#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <tickernelPlatform.h>

void TKNSleep(uint32_t milliseconds);
void TKNLog(char const *const _Format, ...);
void *TKNMalloc(size_t size);
void TKNFree(void *block);
void TKNCombinePaths(char *dstPath, rsize_t size, const char *srcPath, ...);
void TKNGetCurrentDirectory(char *directory, size_t size);