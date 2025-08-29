#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "tkn.h"
#define TKN_CLAMP(x, min, max) \
    ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define TKN_DEFAULT_COLLECTION_SIZE 8
#define TKN_MIN_COLLECTION_SIZE 1

typedef struct
{
    uint32_t maxCount;
    uint32_t count;
    size_t dataSize;
    void *array;
} TknDynamicArray;

typedef struct TknListNode
{
    void *pointer;
    struct TknListNode *nextNodePtr;
} TknListNode;

typedef struct
{
    uint32_t capacity;
    uint32_t count;
    TknListNode **nodePtrs;
} TknHashSet;

TknDynamicArray tknCreateDynamicArray(size_t dataSize, uint32_t maxCount);
void tknDestroyDynamicArray(TknDynamicArray dynamicArray);
void tknInsertIntoDynamicArray(TknDynamicArray *pDynamicArray, void *pData, uint32_t index);
void tknAddToDynamicArray(TknDynamicArray *pDynamicArray, void *pData);
void tknRemoveFromDynamicArray(TknDynamicArray *pDynamicArray, void *pData);
void tknRemoveAtIndexFromDynamicArray(TknDynamicArray *pDynamicArray, uint32_t index);
void tknClearDynamicArray(TknDynamicArray *pDynamicArray);
void *tknGetFromDynamicArray(TknDynamicArray *pDynamicArray, uint32_t index);
bool tknContainsInDynamicArray(TknDynamicArray *pDynamicArray, void *pData);

TknHashSet tknCreateHashSet(uint32_t capacity);
void tknDestroyHashSet(TknHashSet tknHashSet);
bool tknAddToHashSet(TknHashSet *pTknHashSet, void *pointer);
bool tknContainsInHashSet(TknHashSet *pTknHashSet, void *pointer);
void tknRemoveFromHashSet(TknHashSet *pTknHashSet, void *pointer);
void tknClearHashSet(TknHashSet *pTknHashSet);
