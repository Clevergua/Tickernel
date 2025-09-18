#include "tknCore.h"

static void tknInternalError(const char *prefix, const char *format, va_list args)
{
    if (prefix)
    {
        fprintf(stderr, "%s: ", prefix);
    }
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    abort();
}

void tknWarning(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "WARNING: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void tknError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    tknInternalError("ERROR: ", format, args);
    va_end(args);
}

void tknAssert(bool condition, const char *format, ...)
{
    if (!condition)
    {
        va_list args;
        va_start(args, format);
        tknInternalError("ASSERTION FAILED: ", format, args);
        va_end(args);
    }
}

// static int allocTimes = 0;

void *tknMalloc(size_t size)
{
    return malloc(size);
}

void tknFree(void *ptr)
{
    free(ptr);
}

TknDynamicArray tknCreateDynamicArray(size_t dataSize, uint32_t maxCount)
{
    TknDynamicArray tknDynamicArray = {
        .maxCount = maxCount,
        .count = 0,
        .dataSize = dataSize,
        .array = tknMalloc(dataSize * maxCount)};
    memset(tknDynamicArray.array, 0, dataSize * maxCount);
    return tknDynamicArray;
}
void tknDestroyDynamicArray(TknDynamicArray dynamicArray)
{
    tknFree(dynamicArray.array);
    dynamicArray.array = NULL;
    dynamicArray.count = 0;
    dynamicArray.maxCount = 0;
}
void tknInsertIntoDynamicArray(TknDynamicArray *pDynamicArray, void *pData, uint32_t index)
{
    tknAssert(index >= 0 && index <= pDynamicArray->count, "Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
    if (pDynamicArray->count >= pDynamicArray->maxCount)
    {
        pDynamicArray->maxCount *= 2;
        void *newArray;
        newArray = tknMalloc(pDynamicArray->dataSize * pDynamicArray->maxCount);
        memcpy(newArray, pDynamicArray->array, pDynamicArray->dataSize * pDynamicArray->count);
        tknFree(pDynamicArray->array);
        pDynamicArray->array = newArray;
    }
    void *targetAddress = (char *)pDynamicArray->array + index * pDynamicArray->dataSize;
    if (index < pDynamicArray->count)
    {
        memmove(
            (char *)pDynamicArray->array + (index + 1) * pDynamicArray->dataSize,
            targetAddress,
            (pDynamicArray->count - index) * pDynamicArray->dataSize);
    }
    memcpy(targetAddress, pData, pDynamicArray->dataSize);
    pDynamicArray->count++;
}
void tknAddToDynamicArray(TknDynamicArray *pDynamicArray, void *pData)
{
    tknInsertIntoDynamicArray(pDynamicArray, pData, pDynamicArray->count);
}
void tknRemoveFromDynamicArray(TknDynamicArray *pDynamicArray, void *pData)
{
    for (uint32_t i = 0; i < pDynamicArray->count; i++)
    {
        void *currentElement = (char *)pDynamicArray->array + i * pDynamicArray->dataSize;
        if (memcmp(currentElement, pData, pDynamicArray->dataSize) == 0)
        {
            if (i < pDynamicArray->count - 1)
            {
                memmove(
                    currentElement,
                    (char *)pDynamicArray->array + (i + 1) * pDynamicArray->dataSize,
                    (pDynamicArray->count - i - 1) * pDynamicArray->dataSize);
            }
            pDynamicArray->count--;
            return;
        }
    }
    tknError("Data not found!\n");
}
void tknRemoveAtIndexFromDynamicArray(TknDynamicArray *pDynamicArray, uint32_t index)
{
    tknAssert(index < pDynamicArray->count, "Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
    void *target = (char *)pDynamicArray->array + index * pDynamicArray->dataSize;
    if (index < pDynamicArray->count - 1)
    {
        memmove(
            target,
            (char *)pDynamicArray->array + (index + 1) * pDynamicArray->dataSize,
            (pDynamicArray->count - index - 1) * pDynamicArray->dataSize);
    }
    pDynamicArray->count--;
}
void tknClearDynamicArray(TknDynamicArray *pDynamicArray)
{
    pDynamicArray->count = 0;
    memset(pDynamicArray->array, 0, pDynamicArray->dataSize * pDynamicArray->maxCount);
}
void *tknGetFromDynamicArray(TknDynamicArray *pDynamicArray, uint32_t index)
{
    if (index < pDynamicArray->count)
    {
        void *output = (char *)pDynamicArray->array + index * pDynamicArray->dataSize;
        return output;
    }
    else
    {
        printf("Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
        return NULL;
    }
}
bool tknContainsInDynamicArray(TknDynamicArray *pDynamicArray, void *pData)
{
    if (NULL == pDynamicArray || NULL == pData || 0 == pDynamicArray->count)
    {
        return false;
    }
    uint8_t *arrayData = (uint8_t *)pDynamicArray->array;
    for (uint32_t i = 0; i < pDynamicArray->count; i++)
    {
        void *currentElement = arrayData + (i * pDynamicArray->dataSize);
        if (memcmp(currentElement, pData, pDynamicArray->dataSize) == 0)
        {
            return true;
        }
    }
    return false;
}

TknHashSet tknCreateHashSet(size_t dataSize)
{
    uint32_t capacity = 1u << TKN_DEFAULT_COLLECTION_POWER_OF_TWO;
    TknHashSet tknHashSet = {
        .capacity = capacity,
        .count = 0,
        .dataSize = dataSize,
        .nodePtrs = tknMalloc(sizeof(TknListNode *) * capacity),
    };
    memset(tknHashSet.nodePtrs, 0, sizeof(TknListNode *) * capacity);
    return tknHashSet;
}

void tknDestroyHashSet(TknHashSet tknHashSet)
{
    tknClearHashSet(&tknHashSet);
    tknFree(tknHashSet.nodePtrs);
    tknHashSet.nodePtrs = NULL;
    tknHashSet.capacity = 0;
    tknHashSet.count = 0;
    tknHashSet.dataSize = 0;
}

bool tknAddToHashSet(TknHashSet *pTknHashSet, const void *pData)
{
    size_t index = 0;
    memcpy(&index, pData, sizeof(index) < pTknHashSet->dataSize ? sizeof(index) : pTknHashSet->dataSize);
    index &= (pTknHashSet->capacity - 1);

    TknListNode *node = pTknHashSet->nodePtrs[index];
    while (node)
    {
        if (memcmp(node->data, pData, pTknHashSet->dataSize) == 0)
            return false;
        node = node->pNextNode;
    }
    TknListNode *newNode = tknMalloc(sizeof(TknListNode));
    newNode->data = tknMalloc(pTknHashSet->dataSize);
    memcpy(newNode->data, pData, pTknHashSet->dataSize);
    newNode->pNextNode = pTknHashSet->nodePtrs[index];
    pTknHashSet->nodePtrs[index] = newNode;
    pTknHashSet->count++;
    return true;
}

bool tknContainsInHashSet(TknHashSet *pTknHashSet, const void *pData)
{
    size_t index = 0;
    memcpy(&index, pData, sizeof(index) < pTknHashSet->dataSize ? sizeof(index) : pTknHashSet->dataSize);
    index &= (pTknHashSet->capacity - 1);

    TknListNode *node = pTknHashSet->nodePtrs[index];
    while (node)
    {
        if (memcmp(node->data, pData, pTknHashSet->dataSize) == 0)
            return true;
        node = node->pNextNode;
    }
    return false;
}

void tknRemoveFromHashSet(TknHashSet *pTknHashSet, const void *pData)
{
    size_t index = 0;
    memcpy(&index, pData, sizeof(index) < pTknHashSet->dataSize ? sizeof(index) : pTknHashSet->dataSize);
    index &= (pTknHashSet->capacity - 1);

    TknListNode *node = pTknHashSet->nodePtrs[index];
    TknListNode *prevNode = NULL;
    while (node)
    {
        if (memcmp(node->data, pData, pTknHashSet->dataSize) == 0)
        {
            if (prevNode)
                prevNode->pNextNode = node->pNextNode;
            else
                pTknHashSet->nodePtrs[index] = node->pNextNode;
            tknFree(node->data);
            tknFree(node);
            pTknHashSet->count--;
            return;
        }
        prevNode = node;
        node = node->pNextNode;
    }
}

void tknClearHashSet(TknHashSet *pTknHashSet)
{
    for (size_t i = 0; i < pTknHashSet->capacity; i++)
    {
        TknListNode *node = pTknHashSet->nodePtrs[i];
        while (node)
        {
            TknListNode *nextNode = node->pNextNode;
            tknFree(node->data);
            tknFree(node);
            node = nextNode;
        }
        pTknHashSet->nodePtrs[i] = NULL;
    }
    pTknHashSet->count = 0;
}
