#include "tickernelCore.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct aabb_storage
{
    float center[3];
    float extents[3];
};

struct PreparedRendererInfo
{
    bool isOptimizedMesh;
    bool removeScaleFromWorldMatrix;
    bool hasNonUniformScaleTransform;
    bool calculateAnimatedPosesReturnValue;
    bool calculateBoneBasedBounds;
    void *invalidationRoot;
    void *hierarchy;
    uint32_t rendererTransformIndex;
    uint32_t rootBoneTransformIndex;
    uint32_t tempTransformCount;
    uint32_t boneTransformCount;
    uint32_t boundsCount;
    uint32_t *tempTransformParentIndices;
    uint32_t *tempTransformHierarchyIndices;
    uint32_t *boneTransformIndices;
    uint32_t *boundsTransformIndices;
    void *boundsAABBs;
    struct aabb_storage localAABB;
    void *calcSkinMatricesTask;
};
int main()
{
    TickernelDynamicArray dynamicArray;

    // Test tickernelCreateDynamicArray
    tickernelCreateDynamicArray(&dynamicArray, 4);
    assert(dynamicArray.maxLength == 4);
    assert(dynamicArray.length == 0);
    assert(dynamicArray.array != NULL);

    // Verify all elements are initialized to NULL
    for (uint32_t i = 0; i < dynamicArray.maxLength; i++) {
        assert(dynamicArray.array[i] == NULL);
    }

    // Test tickernelAddToDynamicArray
    int value1 = 10, value2 = 20, value3 = 30;
    tickernelAddToDynamicArray(&dynamicArray, &value1, 0);
    tickernelAddToDynamicArray(&dynamicArray, &value2, 1);
    tickernelAddToDynamicArray(&dynamicArray, &value3, 2);
    
    assert(dynamicArray.length == 3);
    assert(*(int *)dynamicArray.array[0] == 10);
    assert(*(int *)dynamicArray.array[1] == 20);
    assert(*(int *)dynamicArray.array[2] == 30);

    // Test tickernelRemoveAtIndexFromDynamicArray
    tickernelRemoveAtIndexFromDynamicArray(&dynamicArray, 1);
    assert(dynamicArray.length == 2);
    assert(*(int *)dynamicArray.array[0] == 10);
    assert(*(int *)dynamicArray.array[1] == 30);
    assert(dynamicArray.array[2] == NULL); // Verify the last element was set to NULL

    // Test tickernelRemoveFromDynamicArray
    tickernelRemoveFromDynamicArray(&dynamicArray, &value3);
    assert(dynamicArray.length == 1);
    assert(*(int *)dynamicArray.array[0] == 10);
    assert(dynamicArray.array[1] == NULL);

    // Test tickernelClearDynamicArray
    tickernelClearDynamicArray(&dynamicArray);
    assert(dynamicArray.length == 0);
    // Elements might still contain pointers - your implementation doesn't clear them

    // Test tickernelDestroyDynamicArray
    tickernelDestroyDynamicArray(dynamicArray);
    
    // Test resize functionality
    TickernelDynamicArray smallArray;
    tickernelCreateDynamicArray(&smallArray, 2);
    int values[4] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) {
        tickernelAddToDynamicArray(&smallArray, &values[i], i);
    }
    assert(smallArray.maxLength == 4); // Should have doubled from 2
    assert(smallArray.length == 4);
    tickernelDestroyDynamicArray(smallArray);

    printf("All tests passed!\n");
    return 0;
}