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
void TKNGetCurrentDirectory(char *directory, size_t size);
void TKNCombinePaths(char *dstPath, size_t size, const char *srcPath);
bool TKNStartsWith(const char *str, const char *prefix);
bool TKNEndsWith(const char *str, const char *suffix);
