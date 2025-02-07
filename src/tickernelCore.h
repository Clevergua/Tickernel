#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

typedef enum {
   KEY_CODE_A,
   KEY_CODE_B,
   KEY_CODE_C,
   KEY_CODE_D,
   KEY_CODE_E,
   KEY_CODE_F,
   KEY_CODE_G,
   KEY_CODE_H,
   KEY_CODE_I,
   KEY_CODE_J,
   KEY_CODE_K,
   KEY_CODE_L,
   KEY_CODE_M,
   KEY_CODE_N,
   KEY_CODE_O,
   KEY_CODE_P,
   KEY_CODE_Q,
   KEY_CODE_R,
   KEY_CODE_S,
   KEY_CODE_T,
   KEY_CODE_U,
   KEY_CODE_V,
   KEY_CODE_W,
   KEY_CODE_X,
   KEY_CODE_Y,
   KEY_CODE_Z,
   KEY_CODE_0,
   KEY_CODE_1,
   KEY_CODE_2,
   KEY_CODE_3,
   KEY_CODE_4,
   KEY_CODE_5,
   KEY_CODE_6,
   KEY_CODE_7,
   KEY_CODE_8,
   KEY_CODE_9,
   KEY_CODE_ENTER,
   KEY_CODE_ESCAPE,
   KEY_CODE_BACKSPACE,
   KEY_CODE_TAB,
   KEY_CODE_SPACE,
   KEY_CODE_MINUS,
   KEY_CODE_EQUAL,
   KEY_CODE_LEFT_BRACKET,
   KEY_CODE_RIGHT_BRACKET,
   KEY_CODE_BACKSLASH,
   KEY_CODE_SEMICOLON,
   KEY_CODE_APOSTROPHE,
   KEY_CODE_GRAVE,
   KEY_CODE_COMMA,
   KEY_CODE_PERIOD,
   KEY_CODE_SLASH,
   KEY_CODE_CAPS_LOCK,
   KEY_CODE_F1,
   KEY_CODE_F2,
   KEY_CODE_F3,
   KEY_CODE_F4,
   KEY_CODE_F5,
   KEY_CODE_F6,
   KEY_CODE_F7,
   KEY_CODE_F8,
   KEY_CODE_F9,
   KEY_CODE_F10,
   KEY_CODE_F11,
   KEY_CODE_F12,
   KEY_CODE_PRINT_SCREEN,
   KEY_CODE_SCROLL_LOCK,
   KEY_CODE_PAUSE,
   KEY_CODE_INSERT,
   KEY_CODE_HOME,
   KEY_CODE_PAGE_UP,
   KEY_CODE_DELETE,
   KEY_CODE_END,
   KEY_CODE_PAGE_DOWN,
   KEY_CODE_RIGHT,
   KEY_CODE_LEFT,
   KEY_CODE_DOWN,
   KEY_CODE_UP,
   KEY_CODE_NUM_LOCK,
   KEY_CODE_NUMPAD_0,
   KEY_CODE_NUMPAD_1,
   KEY_CODE_NUMPAD_2,
   KEY_CODE_NUMPAD_3,
   KEY_CODE_NUMPAD_4,
   KEY_CODE_NUMPAD_5,
   KEY_CODE_NUMPAD_6,
   KEY_CODE_NUMPAD_7,
   KEY_CODE_NUMPAD_8,
   KEY_CODE_NUMPAD_9,
   KEY_CODE_NUMPAD_DIVIDE,
   KEY_CODE_NUMPAD_MULTIPLY,
   KEY_CODE_NUMPAD_SUBTRACT,
   KEY_CODE_NUMPAD_ADD,
   KEY_CODE_NUMPAD_ENTER,
   KEY_CODE_NUMPAD_DECIMAL,

   KEY_CODE_MAX_ENUM,
} KeyCode;

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

typedef struct
{
    size_t dataSize;
    uint32_t maxLength;
    uint32_t length;
    void **array;
} TickernelDynamicArray;

void tickernelError(char const *const _Format, ...);
void tickernelSleep(uint32_t milliseconds);
void *tickernelMalloc(size_t size);
void tickernelFree(void *block);
void tickernelCombinePaths(char *dstPath, size_t size, const char *srcPath);
bool tickernelStartsWith(const char *str, const char *prefix);
bool tickernelEndsWith(const char *str, const char *suffix);
const char *tickernelGetPathSeparator(void);

void tickernelCreateLinkedList(TickernelLinkedList *pLinkedList, size_t dataSize);
void tickernelDestroyLinkedList(TickernelLinkedList *pLinkedList);
void tickernelAddToLinkedList(TickernelLinkedList *pLinkedList, void *pData);
void tickernelRemoveFromLinkedList(TickernelLinkedList *pLinkedList);
void tickernelClearLinkedList(TickernelLinkedList *pLinkedList);

void tickernelCreateCollection(TickernelCollection *pCollection, size_t dataSize, uint32_t maxLength);
void tickernelDestroyCollection(TickernelCollection *pCollection);
void tickernelAddToCollection(TickernelCollection *pCollection, void *pData, uint32_t *pIndex);
void tickernelRemoveFromCollection(TickernelCollection *pCollection, uint32_t index);
void tickernelClearCollection(TickernelCollection *pCollection);

void tickernelCreateDynamicArray(TickernelDynamicArray *pDynamicArray, size_t dataSize, uint32_t maxLength);
void tickernelDestroyDynamicArray(TickernelDynamicArray *pDynamicArray);
void tickernelAddToDynamicArray(TickernelDynamicArray *pDynamicArray, void *pData);
void tickernelRemoveFromDynamicArray(TickernelDynamicArray *pDynamicArray, uint32_t index);
void tickernelClearDynamicArray(TickernelDynamicArray *pDynamicArray);
