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

    // Test tickernelCreateDynamicArray (指定初始容量为4)
    tickernelCreateDynamicArray(&dynamicArray, sizeof(int), 4);
    assert(dynamicArray.maxCount == 4);
    assert(dynamicArray.count == 0);
    assert(dynamicArray.array != NULL);

    // Test tickernelAddToDynamicArray
    int value1 = 10, value2 = 20, value3 = 30;
    tickernelAddToDynamicArray(&dynamicArray, &value1, 0);
    tickernelAddToDynamicArray(&dynamicArray, &value2, 1);
    tickernelAddToDynamicArray(&dynamicArray, &value3, 2);

    assert(dynamicArray.count == 3);
    assert(*(int*)tickernelGetFromDynamicArray(&dynamicArray, 0) == 10);
    assert(*(int*)tickernelGetFromDynamicArray(&dynamicArray, 1) == 20);
    assert(*(int*)tickernelGetFromDynamicArray(&dynamicArray, 2) == 30);

    // Test tickernelRemoveAtIndexFromDynamicArray
    tickernelRemoveAtIndexFromDynamicArray(&dynamicArray, 1);
    assert(dynamicArray.count == 2);
    assert(*(int*)tickernelGetFromDynamicArray(&dynamicArray, 0) == 10);
    assert(*(int*)tickernelGetFromDynamicArray(&dynamicArray, 1) == 30);

    // Test tickernelRemoveFromDynamicArray
    tickernelRemoveFromDynamicArray(&dynamicArray, &value3);
    assert(dynamicArray.count == 1);
    assert(*(int*)tickernelGetFromDynamicArray(&dynamicArray, 0) == 10);

    // Test tickernelClearDynamicArray
    tickernelClearDynamicArray(&dynamicArray);
    assert(dynamicArray.count == 0);

    // Test tickernelDestroyDynamicArray
    tickernelDestroyDynamicArray(dynamicArray); // 修正：传递指针

    // Test resize functionality
    TickernelDynamicArray smallArray;
    tickernelCreateDynamicArray(&smallArray, sizeof(int), 2); // 元素大小为int，初始容量2
    int values[4] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) {
        tickernelAddToDynamicArray(&smallArray, &values[i], i);
    }
    assert(smallArray.maxCount == 4); // 验证扩容
    assert(smallArray.count == 4);
    for (int i = 0; i < 4; i++) {
        assert(*(int*)tickernelGetFromDynamicArray(&smallArray, i) == values[i]);
    }
    tickernelDestroyDynamicArray(smallArray);

    printf("All tests passed!\n");
    return 0;
}