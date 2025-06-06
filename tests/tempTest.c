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
    tickernelCreateDynamicArray(&dynamicArray, 4, sizeof(int));
    assert(dynamicArray.maxLength == 4);
    assert(dynamicArray.dataSize == sizeof(int));
    assert(dynamicArray.length == 0);
    assert(dynamicArray.array != NULL);

    // Test tickernelAddToDynamicArray
    int value1 = 10, value2 = 20, value3 = 30;
    int *p1, *p2;
    tickernelAddToDynamicArray(&dynamicArray, &value1, 0, p1);
    tickernelAddToDynamicArray(&dynamicArray, &value2, 1, p2);
    void *addedValue3;
    tickernelAddToDynamicArray(&dynamicArray, &value3, 2, addedValue3); // Capture the returned pointer
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
    tickernelDestroyDynamicArray(dynamicArray);
    assert(dynamicArray.array == NULL);
    return 0;
}