#include "tickernelCore.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main() {
    TickernelDynamicArray dynamicArray;

    // Test tickernelCreateDynamicArray
    tickernelCreateDynamicArray(&dynamicArray, 4, sizeof(int));
    assert(dynamicArray.maxLength == 4);
    assert(dynamicArray.dataSize == sizeof(int));
    assert(dynamicArray.length == 0);
    assert(dynamicArray.array != NULL);

    // Test tickernelAddToDynamicArray
    int value1 = 10, value2 = 20, value3 = 30;
    tickernelAddToDynamicArray(&dynamicArray, &value1, 0);
    tickernelAddToDynamicArray(&dynamicArray, &value2, 1);
    void *addedValue3 = tickernelAddToDynamicArray(&dynamicArray, &value3, 2); // Capture the returned pointer
    assert(dynamicArray.length == 3);
    assert(*(int *)dynamicArray.array[0] == 10);
    assert(*(int *)dynamicArray.array[1] == 20);
    assert(*(int *)dynamicArray.array[2] == 30);

    // Test tickernelRemoveAtIndexFromDynamicArray
    tickernelRemoveAtIndexFromDynamicArray(&dynamicArray, 1);
    assert(dynamicArray.length == 2);
    assert(*(int *)dynamicArray.array[0] == 10);
    assert(*(int *)dynamicArray.array[1] == 30);

    // Test tickernelRemoveFromDynamicArray
    tickernelRemoveFromDynamicArray(&dynamicArray, addedValue3); // Use the returned pointer
    assert(dynamicArray.length == 1);
    assert(*(int *)dynamicArray.array[0] == 10);

    // Test tickernelClearDynamicArray
    tickernelClearDynamicArray(&dynamicArray);
    assert(dynamicArray.length == 0);

    // Test tickernelDestroyDynamicArray
    tickernelDestroyDynamicArray(&dynamicArray);
    assert(dynamicArray.array == NULL);

    printf("All tests passed!\n");
    return 0;
}