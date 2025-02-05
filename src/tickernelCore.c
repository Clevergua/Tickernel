#include "tickernelCore.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <execinfo.h>


void tickernelError(char const *const _Format, ...)
{
    va_list args;
    va_start(args, _Format);
    vfprintf(stderr, _Format, args);
    va_end(args);

    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **symbols = backtrace_symbols(buffer, nptrs);

    if (symbols == NULL)
    {
        tickernelError("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nptrs; i++)
    {
        printf("%s\n", symbols[i]);
    }

    tickernelFree(symbols);

    abort();
}

void tickernelSleep(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}

void *tickernelMalloc(size_t size)
{
    return malloc(size);
}

void tickernelFree(void *block)
{
    free(block);
}

bool tickernelStartsWith(const char *str, const char *prefix)
{
    size_t strLength = strlen(str);
    size_t prefixLength = strlen(prefix);
    if (strLength < prefixLength)
    {
        return false;
    }
    else
    {
        int result = strncmp(str, prefix, prefixLength);
        return 0 == result;
    }
}

bool tickernelEndsWith(const char *str, const char *suffix)
{
    size_t strLength = strlen(str);
    size_t suffixLength = strlen(suffix);
    if (strLength < suffixLength)
    {
        return false;
    }
    else
    {
        for (int i = 0; i < suffixLength; i++)
        {
            if (str[i + strLength - suffixLength] != suffix[i])
            {
                return false;
            }
        }
        return true;
    }
}

void tickernelCombinePaths(char *dstPath, size_t dstPathSize, const char *srcPath)
{
    const char *pathSeparator = tickernelGetPathSeparator();
    size_t dstPathLength = strlen(dstPath);
    if (tickernelEndsWith(dstPath, pathSeparator))
    {
        if (tickernelStartsWith(srcPath, pathSeparator))
        {
            size_t separatorLength = strlen(pathSeparator);
            dstPath[dstPathLength - separatorLength] = '\0';
            strcat(dstPath, srcPath);
        }
        else
        {
            strcat(dstPath, srcPath);
        }
    }
    else if (tickernelStartsWith(srcPath, pathSeparator))
    {
        strcat(dstPath, srcPath);
    }
    else
    {
        strcat(dstPath, pathSeparator);
        strcat(dstPath, srcPath);
    }
}

const char *tickernelGetPathSeparator(void)
{
    return "/";
}

void tickernelCreateLinkedList(TickernelLinkedList *pLinkedList, size_t dataSize)
{
    pLinkedList->dataSize = dataSize;
    pLinkedList->pHead = NULL;
}

void tickernelDestroyLinkedList(TickernelLinkedList *pLinkedList)
{
    tickernelClearLinkedList(pLinkedList);
}

void tickernelAddToLinkedList(TickernelLinkedList *pLinkedList, void *pData)
{
    TickernelNode *pNewNode = (TickernelNode *)tickernelMalloc(sizeof(TickernelNode));
    pNewNode->pData = tickernelMalloc(pLinkedList->dataSize);
    pNewNode->pNext = pLinkedList->pHead;
    memcpy(pNewNode->pData, pData, pLinkedList->dataSize);
    pLinkedList->pHead = pNewNode;
}

void tickernelRemoveFromLinkedList(TickernelLinkedList *pLinkedList)
{
    TickernelNode *current = pLinkedList->pHead;
    pLinkedList->pHead = pLinkedList->pHead->pNext;
    tickernelFree(current->pData);
    tickernelFree(current);
}

void tickernelClearLinkedList(TickernelLinkedList *pLinkedList)
{
    while (NULL != pLinkedList->pHead)
    {
        tickernelRemoveFromLinkedList(pLinkedList);
    }
}

void tickernelCreateCollection(TickernelCollection *pCollection, size_t dataSize, uint32_t maxLength)
{
    pCollection->dataSize = dataSize;
    pCollection->maxLength = maxLength;
    pCollection->length = 0;
    pCollection->array = tickernelMalloc(dataSize * maxLength);
    for (uint32_t i = 0; i < maxLength; i++)
    {
        pCollection->array[i] = NULL;
    }
    tickernelCreateLinkedList(&pCollection->removedIndexLinkedList, sizeof(uint32_t));
}
void tickernelDestroyCollection(TickernelCollection *pCollection)
{
    tickernelClearLinkedList(&pCollection->removedIndexLinkedList);
    tickernelDestroyLinkedList(&pCollection->removedIndexLinkedList);
    tickernelFree(pCollection->array);
}
void tickernelAddToCollection(TickernelCollection *pCollection, void *pData, uint32_t *pIndex)
{
    if (NULL != pCollection->removedIndexLinkedList.pHead)
    {
        uint32_t index = *(uint32_t *)pCollection->removedIndexLinkedList.pHead->pData;
        tickernelRemoveFromLinkedList(&pCollection->removedIndexLinkedList);
        pCollection->array[index] = tickernelMalloc(pCollection->dataSize);
        memcpy(pCollection->array[index], pData, pCollection->dataSize);
        *pIndex = index;
    }
    else if (pCollection->length < pCollection->maxLength)
    {
        pCollection->array[pCollection->length] = tickernelMalloc(pCollection->dataSize);
        memcpy(pCollection->array[pCollection->length], pData, pCollection->dataSize);
        *pIndex = pCollection->length;
        pCollection->length++;
    }
    else
    {
        pCollection->maxLength *= 2;
        void **newArray = tickernelMalloc(pCollection->dataSize * pCollection->maxLength);
        for (uint32_t i = 0; i < pCollection->maxLength; i++)
        {
            newArray[i] = NULL;
        }
        memcpy(newArray, pCollection->array, pCollection->dataSize * pCollection->length);
        tickernelFree(pCollection->array);
        pCollection->array = newArray;

        pCollection->array[pCollection->length] = tickernelMalloc(pCollection->dataSize);
        memcpy(pCollection->array[pCollection->length], pData, pCollection->dataSize);
        *pIndex = pCollection->length;
        pCollection->length++;
    }
}
void tickernelRemoveFromCollection(TickernelCollection *pCollection, uint32_t index)
{
    tickernelFree(pCollection->array[index]);
    pCollection->array[pCollection->length] = NULL;
    if (index == (pCollection->length - 1))
    {
        pCollection->length--;
    }
    else
    {
        tickernelAddToLinkedList(&pCollection->removedIndexLinkedList, &index);
    }
}

void tickernelClearCollection(TickernelCollection *pCollection)
{
    for (uint32_t i = 0; i < pCollection->length; i++)
    {
        if (NULL != pCollection->array[i])
        {
            tickernelFree(pCollection->array[i]);
            pCollection->array[i] = NULL;
        }
        else
        {
            // Skip null
        }
    }
    tickernelClearLinkedList(&pCollection->removedIndexLinkedList);
    pCollection->length = 0;
}

void tickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, size_t dataSize, uint32_t maxLength)
{
    pDynamicArray->dataSize = dataSize;
    pDynamicArray->maxLength = maxLength;
    pDynamicArray->length = 0;
    pDynamicArray->array = (void **)tickernelMalloc(dataSize * maxLength);
    for (uint32_t i = 0; i < maxLength; i++)
    {
        pDynamicArray->array[i] = NULL;
    }
}
void tickernelDestroyDynamicArray(TickernelDynamicArray *pDynamicArray)
{
    tickernelClearDynamicArray(pDynamicArray);
    tickernelFree(pDynamicArray->array);
}

void tickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData)
{
    if (pDynamicArray->length >= pDynamicArray->maxLength)
    {
        pDynamicArray->maxLength *= 2;
        void **newArray = (void **)tickernelMalloc(pDynamicArray->dataSize * pDynamicArray->maxLength);
        memcpy(newArray, pDynamicArray->array, pDynamicArray->dataSize * pDynamicArray->length);
        tickernelFree(pDynamicArray->array);
        pDynamicArray->array = newArray;
    }
    else
    {
        // Do nothing.
    }
    pDynamicArray->array[pDynamicArray->length] = tickernelMalloc(pDynamicArray->dataSize);
    memcpy(pDynamicArray->array[pDynamicArray->length], pData, pDynamicArray->dataSize);
    pDynamicArray->length++;
}

void tickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index)
{
    if (index >= pDynamicArray->length)
    {
        tickernelError("Index out of bounds!\n");
    }

    if (NULL == pDynamicArray->array[index])
    {
        tickernelError("Try to remove NULL!\n");
    }

    tickernelFree(pDynamicArray->array[index]);
    if (index < pDynamicArray->length - 1)
    {
        memmove(&pDynamicArray->array[index], &pDynamicArray->array[index + 1], (pDynamicArray->length - index - 1) * sizeof(void *));
    }
    else
    {
        pDynamicArray->length--;
    }
    pDynamicArray->array[pDynamicArray->length] = NULL;
}

void tickernelClearDynamicArray(TickernelDynamicArray *pDynamicArray)
{
    for (uint32_t i = 0; i < pDynamicArray->length; i++)
    {
        if (pDynamicArray->array[i] != NULL)
        {
            tickernelFree(pDynamicArray->array[i]);
            pDynamicArray->array[i] = NULL;
        }
    }
    pDynamicArray->length = 0;
}
