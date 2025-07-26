#include "tknCore.h"
#include <stdio.h>
#include <stdlib.h>

// 测试初始化与销毁
static void testInitializationAndDestruction() {
    printf("Running testInitializationAndDestruction...\n");
    
    TknHashSet set = tknCreateHashSet(10);
    tknAssert(set.capacity == 10, "Initial capacity mismatch");
    tknAssert(set.count == 0, "Initial count should be 0");
    tknAssert(set.nodePtrs != NULL, "Node pointers array should be allocated");
    
    // 验证所有桶初始化为NULL
    for (size_t i = 0; i < set.capacity; i++) {
        tknAssert(set.nodePtrs[i] == NULL, "Bucket %zu should be NULL", i);
    }
    
    tknDestroyHashSet(set);
    printf("testInitializationAndDestruction passed.\n\n");
}

// 测试添加元素
static void testAddElement() {
    printf("Running testAddElement...\n");
    
    TknHashSet set = tknCreateHashSet(10);
    int value1 = 42;
    int value2 = 99;
    
    // 添加第一个元素
    bool result = tknAddToHashSet(&set, &value1);
    tknAssert(result == true, "Adding first element should succeed");
    tknAssert(set.count == 1, "Count should be 1 after adding one element");
    
    // 添加重复元素（应该失败）
    result = tknAddToHashSet(&set, &value1);
    tknAssert(result == false, "Adding duplicate element should fail");
    tknAssert(set.count == 1, "Count should remain 1 after duplicate add");
    
    // 添加第二个元素
    result = tknAddToHashSet(&set, &value2);
    tknAssert(result == true, "Adding second element should succeed");
    tknAssert(set.count == 2, "Count should be 2 after adding two elements");
    
    tknDestroyHashSet(set);
    printf("testAddElement passed.\n\n");
}

// 测试包含元素
static void testContainsElement() {
    printf("Running testContainsElement...\n");
    
    TknHashSet set = tknCreateHashSet(10);
    int value1 = 42;
    int value2 = 99;
    int value3 = 100;
    
    tknAddToHashSet(&set, &value1);
    tknAddToHashSet(&set, &value2);
    
    // 检查已添加的元素
    tknAssert(tknContainsInHashSet(&set, &value1) == true, "Set should contain value1");
    tknAssert(tknContainsInHashSet(&set, &value2) == true, "Set should contain value2");
    
    // 检查未添加的元素
    tknAssert(tknContainsInHashSet(&set, &value3) == false, "Set should not contain value3");
    
    tknDestroyHashSet(set);
    printf("testContainsElement passed.\n\n");
}

// 测试移除元素
static void testRemoveElement() {
    printf("Running testRemoveElement...\n");
    
    TknHashSet set = tknCreateHashSet(10);
    int value1 = 42;
    int value2 = 99;
    
    tknAddToHashSet(&set, &value1);
    tknAddToHashSet(&set, &value2);
    
    // 移除元素
    tknRemoveFromHashSet(&set, &value1);
    tknAssert(set.count == 1, "Count should be 1 after removal");
    tknAssert(tknContainsInHashSet(&set, &value1) == false, "Set should not contain removed value");
    
    // 尝试移除不存在的元素（应无影响）
    tknRemoveFromHashSet(&set, &value1);  // 重复移除
    tknAssert(set.count == 1, "Count should remain 1 after removing non-existent value");
    
    // 移除剩余元素
    tknRemoveFromHashSet(&set, &value2);
    tknAssert(set.count == 0, "Count should be 0 after removing all elements");
    
    tknDestroyHashSet(set);
    printf("testRemoveElement passed.\n\n");
}

// 测试容量与扩容（需要修改代码支持动态扩容）
static void testCapacityAndResize() {
    printf("Running testCapacityAndResize...\n");
    
    // 注意：当前实现不支持自动扩容，此测试仅验证固定容量
    TknHashSet set = tknCreateHashSet(2);
    int values[3];
    
    // 添加元素直到达到容量上限
    for (int i = 0; i < 2; i++) {
        values[i] = i;
        bool result = tknAddToHashSet(&set, &values[i]);
        tknAssert(result == true, "Adding element %d should succeed", i);
    }
    
    // 验证容量限制（当前实现允许超过容量，仅导致更多冲突）
    values[2] = 2;
    bool result = tknAddToHashSet(&set, &values[2]);
    tknAssert(result == true, "Adding element beyond capacity should still succeed (current implementation)");
    
    tknDestroyHashSet(set);
    printf("testCapacityAndResize passed.\n\n");
}

// 测试哈希冲突处理
static void testCollisionHandling() {
    printf("Running testCollisionHandling...\n");
    
    // 创建一个小容量的哈希集，确保会发生冲突
    TknHashSet set = tknCreateHashSet(1);
    int value1 = 42;
    int value2 = 99;
    
    // 添加两个元素，它们的哈希值相同（因为容量为1）
    tknAddToHashSet(&set, &value1);
    tknAddToHashSet(&set, &value2);
    
    // 验证两个元素都存在
    tknAssert(tknContainsInHashSet(&set, &value1) == true, "Set should contain value1 after collision");
    tknAssert(tknContainsInHashSet(&set, &value2) == true, "Set should contain value2 after collision");
    
    // 验证计数正确
    tknAssert(set.count == 2, "Count should be 2 after handling collision");
    
    // 移除其中一个元素
    tknRemoveFromHashSet(&set, &value1);
    tknAssert(tknContainsInHashSet(&set, &value1) == false, "Set should not contain removed value1");
    tknAssert(tknContainsInHashSet(&set, &value2) == true, "Set should still contain value2");
    
    tknDestroyHashSet(set);
    printf("testCollisionHandling passed.\n\n");
}

// 测试边界条件
static void testEdgeCases() {
    printf("Running testEdgeCases...\n");
    
    // 测试空集合
    TknHashSet set = tknCreateHashSet(10);
    tknAssert(tknContainsInHashSet(&set, NULL) == false, "Empty set should not contain NULL");
    
    // 测试添加NULL
    bool result = tknAddToHashSet(&set, NULL);
    tknAssert(result == true, "Adding NULL should succeed");
    tknAssert(tknContainsInHashSet(&set, NULL) == true, "Set should contain NULL after adding");
    
    // 测试移除NULL
    tknRemoveFromHashSet(&set, NULL);
    tknAssert(tknContainsInHashSet(&set, NULL) == false, "Set should not contain NULL after removal");
    
    // 测试清空集合
    int values[5];
    for (int i = 0; i < 5; i++) {
        values[i] = i;
        tknAddToHashSet(&set, &values[i]);
    }
    tknClearHashSet(&set);
    tknAssert(set.count == 0, "Count should be 0 after clearing");
    for (int i = 0; i < 5; i++) {
        tknAssert(tknContainsInHashSet(&set, &values[i]) == false, "Cleared set should not contain value %d", i);
    }
    
    tknDestroyHashSet(set);
    printf("testEdgeCases passed.\n\n");
}

int main() {
    printf("=== Starting TknHashSet Tests ===\n\n");
    testInitializationAndDestruction();
    testAddElement();
    testContainsElement();
    testRemoveElement();
    testCapacityAndResize();
    testCollisionHandling();
    testEdgeCases();
    
    printf("=== All TknHashSet Tests Passed ===\n");
    return 0;
}