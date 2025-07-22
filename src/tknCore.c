#include "tknCore.h"

static void tknInternalError(const char *prefix, const char *format, va_list args)
{
    if (prefix)
    {
        fprintf(stderr, "%s: ", prefix);
    }

    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **symbols = backtrace_symbols(buffer, nptrs);

    if (symbols)
    {
        fprintf(stderr, "Backtrace:\n");
        for (int i = 0; i < nptrs; i++)
        {
            fprintf(stderr, "  %s\n", symbols[i]);
        }
        free(symbols);
    }
    else
    {
        fprintf(stderr, "Failed to get backtrace symbols\n");
    }

    abort();
}

void tknError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    tknInternalError("ERROR", format, args);
    va_end(args);
}

void tknAssert(bool condition, const char *format, ...)
{
    if (!condition)
    {
        va_list args;
        va_start(args, format);
        tknInternalError("ASSERTION FAILED", format, args);
        va_end(args);
    }
}

void tknSleep(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}

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
void tknAddToDynamicArray(TknDynamicArray *pDynamicArray, void *pInput, uint32_t index)
{
    tknAssert(index <= pDynamicArray->count, "Index %u is out of bounds for count %u\n", index, pDynamicArray->count);
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
    memcpy(targetAddress, pInput, pDynamicArray->dataSize);
    pDynamicArray->count++;
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

TknHashSet tknCreateHashSet(size_t capacity)
{
    TknHashSet tknHashSet = {
        .capacity = capacity,
        .count = 0,
        .nodePtrs = tknMalloc(sizeof(TknListNode *) * capacity),
    };
    memset(tknHashSet.nodePtrs, 0, sizeof(TknListNode *) * capacity);
    return tknHashSet;
}
void tknDestroyHashSet(TknHashSet *pTknHashSet)
{
    tknClearHashSet(pTknHashSet);
    tknFree(pTknHashSet->nodePtrs);
}
bool tknAddToHashSet(TknHashSet *pTknHashSet, void *value)
{
    if (tknContainsInHashSet(pTknHashSet, value))
    {
        return false;
    }
    else
    {
        if (pTknHashSet->count * 10 >= pTknHashSet->capacity * 8)
        {
            // Resize the hash set
            size_t newCapacity = pTknHashSet->capacity * 2;
            TknListNode **newNodePtrs = tknMalloc(sizeof(TknListNode *) * newCapacity);
            memset(newNodePtrs, 0, sizeof(TknListNode *) * newCapacity);
            for (size_t i = 0; i < pTknHashSet->capacity; i++)
            {
                TknListNode *node = pTknHashSet->nodePtrs[i];
                while (node)
                {
                    size_t newIndex = (size_t)node->value % newCapacity;
                    TknListNode *nextNode = node->nextNodePtr;
                    node->nextNodePtr = newNodePtrs[newIndex];
                    newNodePtrs[newIndex] = node;
                    node = nextNode;
                }
            }
            tknFree(pTknHashSet->nodePtrs);
            pTknHashSet->nodePtrs = newNodePtrs;
            pTknHashSet->capacity = newCapacity;
        }
        else
        {
            // nothing
        }
        size_t index = (size_t)value % pTknHashSet->capacity;
        TknListNode *newNode = tknMalloc(sizeof(TknListNode));
        newNode->value = value;
        newNode->nextNodePtr = pTknHashSet->nodePtrs[index];
        pTknHashSet->nodePtrs[index] = newNode;
        pTknHashSet->count++;
        return true;
    }
}
bool tknContainsInHashSet(TknHashSet *pTknHashSet, void *value)
{
    size_t index = (size_t)value % pTknHashSet->capacity;
    TknListNode *node = pTknHashSet->nodePtrs[index];
    while (node)
    {
        if (node->value == value)
        {
            return true;
        }
        node = node->nextNodePtr;
    }
    return false;
}
void tknRemoveFromHashSet(TknHashSet *pTknHashSet, void *value)
{
    size_t index = (size_t)value % pTknHashSet->capacity;
    TknListNode *node = pTknHashSet->nodePtrs[index];
    TknListNode *prevNode = NULL;
    while (node)
    {
        if (node->value == value)
        {
            if (prevNode)
            {
                prevNode->nextNodePtr = node->nextNodePtr;
            }
            else
            {
                pTknHashSet->nodePtrs[index] = node->nextNodePtr;
            }
            tknFree(node);
            pTknHashSet->count--;
            return;
        }
        prevNode = node;
        node = node->nextNodePtr;
    }
}
void tknClearHashSet(TknHashSet *pTknHashSet)
{
    for (size_t i = 0; i < pTknHashSet->capacity; i++)
    {
        TknListNode *node = pTknHashSet->nodePtrs[i];
        while (node)
        {
            TknListNode *nextNode = node->nextNodePtr;
            tknFree(node);
            node = nextNode;
        }
        pTknHashSet->nodePtrs[i] = NULL;
    }
    pTknHashSet->count = 0;
}