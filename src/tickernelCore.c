#include <tickernelCore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#if PLATFORM_POSIX
#include <execinfo.h>
#elif PLATFORM_WINDOWS
#include <dbghelp.h>
#include <windows.h>
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

void TickernelError(char const *const _Format, ...)
{
    va_list args;
    va_start(args, _Format);
    vfprintf(stderr, _Format, args);
    va_end(args);

#if PLATFORM_POSIX
    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **symbols = backtrace_symbols(buffer, nptrs);

    if (symbols == NULL)
    {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nptrs; i++)
    {
        printf("%s\n", symbols[i]);
    }

    TickernelFree(symbols);
#elif PLATFORM_WINDOWS
    void *stack[100];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD frames = CaptureStackBackTrace(0, 100, stack, NULL);
    SYMBOL_INFO *symbol = (SYMBOL_INFO *)TickernelMalloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (int i = 0; i < frames; i++)
    {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        printf("%i: %s - 0x%0llX\n", frames - i - 1, symbol->Name, symbol->Address);
    }

    TickernelFree(symbol);
    SymCleanup(process);
#else
#error "Unknown platform"
#endif
    abort();
}

void TickernelSleep(uint32_t milliseconds)
{
#if PLATFORM_POSIX
    usleep(milliseconds * 1000);
#elif PLATFORM_WINDOWS
    Sleep(milliseconds);
#else
#error "Unknown platform"
#endif
}

void *TickernelMalloc(size_t size)
{
    return malloc(size);
}

void TickernelFree(void *block)
{
    free(block);
}

bool TickernelStartsWith(const char *str, const char *prefix)
{
    int strLength = strlen(str);
    int prefixLength = strlen(prefix);
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

bool TickernelEndsWith(const char *str, const char *suffix)
{
    int strLength = strlen(str);
    int suffixLength = strlen(suffix);
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

void TickernelCombinePaths(char *dstPath, size_t dstPathSize, const char *srcPath)
{
    const char *pathSeparator = TickernelGetPathSeparator();
    size_t dstPathLength = strlen(dstPath);
    size_t srcPathLength = strlen(srcPath);
    if (TickernelEndsWith(dstPath, pathSeparator))
    {
        if (TickernelStartsWith(srcPath, pathSeparator))
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
    else if (TickernelStartsWith(srcPath, pathSeparator))
    {
        strcat(dstPath, srcPath);
    }
    else
    {
        strcat(dstPath, pathSeparator);
        strcat(dstPath, srcPath);
    }
}

const char *TickernelGetPathSeparator()
{
#if PLATFORM_POSIX
    return "/";
#elif PLATFORM_WINDOWS
    return "\\";
#else
#error "Unknown platform"
#endif
}

void TickernelCreateLinkedList(TickernelLinkedList *pLinkedList, size_t dataSize)
{
    pLinkedList->dataSize = dataSize;
    pLinkedList->pHead = NULL;
}

void TickernelDestroyLinkedList(TickernelLinkedList *pLinkedList)
{
    TickernelClearLinkedList(pLinkedList);
}

void TickernelAddToLinkedList(TickernelLinkedList *pLinkedList, void *pData)
{
    TickernelNode *pNewNode = (TickernelNode *)TickernelMalloc(sizeof(TickernelNode));
    pNewNode->pData = TickernelMalloc(pLinkedList->dataSize);
    pNewNode->pNext = pLinkedList->pHead;
    memcpy(pNewNode->pData, pData, pLinkedList->dataSize);
    pLinkedList->pHead = pNewNode;
}

void TickernelRemoveFromLinkedList(TickernelLinkedList *pLinkedList)
{
    TickernelNode *current = pLinkedList->pHead;
    pLinkedList->pHead = pLinkedList->pHead->pNext;
    TickernelFree(current->pData);
    TickernelFree(current);
}

void TickernelClearLinkedList(TickernelLinkedList *pLinkedList)
{
    while (NULL != pLinkedList->pHead)
    {
        TickernelRemoveFromLinkedList(pLinkedList);
    }
}

void TickernelCreateCollection(TickernelCollection *pCollection, size_t dataSize, uint32_t maxLength)
{
    pCollection->dataSize = dataSize;
    pCollection->maxLength = maxLength;
    pCollection->length = 0;
    pCollection->array = TickernelMalloc(dataSize * maxLength);
    for (uint32_t i = 0; i < maxLength; i++)
    {
        pCollection->array[i] = NULL;
    }
    TickernelCreateLinkedList(&pCollection->removedIndexLinkedList, sizeof(uint32_t));
}
void TickernelDestroyCollection(TickernelCollection *pCollection)
{
    TickernelClearLinkedList(&pCollection->removedIndexLinkedList);
    TickernelDestroyLinkedList(&pCollection->removedIndexLinkedList);
    TickernelFree(pCollection->array);
}
void TickernelAddToCollection(TickernelCollection *pCollection, void *pData, uint32_t *pOutputIndex)
{
    if (NULL != pCollection->removedIndexLinkedList.pHead)
    {
        uint32_t index = *(uint32_t *)pCollection->removedIndexLinkedList.pHead->pData;
        TickernelRemoveFromLinkedList(&pCollection->removedIndexLinkedList);
        pCollection->array[index] = TickernelMalloc(pCollection->dataSize);
        memcpy(pCollection->array[index], pData, pCollection->dataSize);
        *pOutputIndex = index;
    }
    else if (pCollection->length < pCollection->maxLength)
    {
        pCollection->array[pCollection->length] = TickernelMalloc(pCollection->dataSize);
        memcpy(pCollection->array[pCollection->length], pData, pCollection->dataSize);
        *pOutputIndex = pCollection->length;
        pCollection->length++;
    }
    else
    {
        pCollection->maxLength *= 2;
        void **newArray = TickernelMalloc(pCollection->dataSize * pCollection->maxLength);
        for (uint32_t i = 0; i < pCollection->maxLength; i++)
        {
            newArray[i] = NULL;
        }
        memcpy(newArray, pCollection->array, pCollection->dataSize * pCollection->length);
        TickernelFree(pCollection->array);
        pCollection->array = newArray;

        pCollection->array[pCollection->length] = TickernelMalloc(pCollection->dataSize);
        memcpy(pCollection->array[pCollection->length], pData, pCollection->dataSize);
        *pOutputIndex = pCollection->length;
        pCollection->length++;
    }
}
void TickernelRemoveFromCollection(TickernelCollection *pCollection, uint32_t index)
{
    TickernelFree(pCollection->array[index]);
    pCollection->array[pCollection->length] = NULL;
    if (index == (pCollection->length - 1))
    {
        pCollection->length--;
    }
    else
    {
        TickernelAddToLinkedList(&pCollection->removedIndexLinkedList, &index);
    }
}

void TickernelClearCollection(TickernelCollection *pCollection)
{
    for (uint32_t i = 0; i < pCollection->length; i++)
    {
        if (NULL != pCollection->array[i])
        {
            TickernelFree(pCollection->array[i]);
            pCollection->array[i] = NULL;
        }
        else
        {
            // Skip null
        }
    }
    TickernelClearLinkedList(&pCollection->removedIndexLinkedList);
    pCollection->length = 0;
}

void TickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, size_t dataSize, uint32_t maxLength)
{
    pDynamicArray->dataSize = dataSize;
    pDynamicArray->maxLength = maxLength;
    pDynamicArray->length = 0;
    pDynamicArray->array = (void **)TickernelMalloc(dataSize * maxLength);
    for (uint32_t i = 0; i < maxLength; i++)
    {
        pDynamicArray->array[i] = NULL;
    }
}
void TickernelDestroyDynamicArray(TickernelDynamicArray *pDynamicArray)
{
    TickernelClearDynamicArray(pDynamicArray);
    TickernelFree(pDynamicArray->array);
}

void TickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData)
{
    if (pDynamicArray->length >= pDynamicArray->maxLength)
    {
        pDynamicArray->maxLength *= 2;
        void **newArray = (void **)TickernelMalloc(pDynamicArray->dataSize * pDynamicArray->maxLength);
        memcpy(newArray, pDynamicArray->array, pDynamicArray->dataSize * pDynamicArray->length);
        TickernelFree(pDynamicArray->array);
        pDynamicArray->array = newArray;
    }
    else
    {
        // Do nothing.
    }
    pDynamicArray->array[pDynamicArray->length] = TickernelMalloc(pDynamicArray->dataSize);
    memcpy(pDynamicArray->array[pDynamicArray->length], pData, pDynamicArray->dataSize);
    pDynamicArray->length++;
}

void TickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index)
{
    if (index >= pDynamicArray->length)
    {
        TickernelError("Index out of bounds!\n");
    }

    if (NULL == pDynamicArray->array[index])
    {
        TickernelError("Try to remove NULL!\n");
    }

    TickernelFree(pDynamicArray->array[index]);
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

void TickernelClearDynamicArray(TickernelDynamicArray *pDynamicArray)
{
    for (uint32_t i = 0; i < pDynamicArray->length; i++)
    {
        if (pDynamicArray->array[i] != NULL)
        {
            TickernelFree(pDynamicArray->array[i]);
            pDynamicArray->array[i] = NULL;
        }
    }
    pDynamicArray->length = 0;
}
