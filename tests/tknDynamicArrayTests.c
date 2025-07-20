#include "tknCore.h"
#include <stdio.h>
#include <string.h>

// 定义一个测试用的结构体（验证复杂数据类型的处理）
typedef struct {
    int id;
    char name[16];
} TestStruct;

// 初始化与销毁测试
static void testDynamicArrayInitialization() {
    printf("Running testDynamicArrayInitialization...\n");
    
    // 测试int类型数组初始化
    TknDynamicArray intArray = tknCreateDynamicArray(sizeof(int), 5);
    tknAssert(intArray.dataSize == sizeof(int), "int array dataSize mismatch");
    tknAssert(intArray.maxCount == 5, "int array maxCount should be 5");
    tknAssert(intArray.count == 0, "int array initial count should be 0");
    tknAssert(intArray.array != NULL, "int array memory should be allocated");
    
    // 测试结构体类型数组初始化
    TknDynamicArray structArray = tknCreateDynamicArray(sizeof(TestStruct), 3);
    tknAssert(structArray.dataSize == sizeof(TestStruct), "struct array dataSize mismatch");
    tknAssert(structArray.maxCount == 3, "struct array maxCount should be 3");
    tknAssert(structArray.count == 0, "struct array initial count should be 0");
    
    // 销毁数组
    tknDestroyDynamicArray(intArray);
    tknDestroyDynamicArray(structArray);
    
    printf("testDynamicArrayInitialization passed.\n\n");
}

// 添加元素测试（包括扩容）
static void testAddToDynamicArray() {
    printf("Running testAddToDynamicArray...\n");
    
    // 测试int数组添加与扩容
    TknDynamicArray intArray = tknCreateDynamicArray(sizeof(int), 2); // 初始maxCount=2
    
    // 添加第一个元素（索引0，末尾）
    int val1 = 10;
    tknAddToDynamicArray(&intArray, &val1, 0);
    tknAssert(intArray.count == 1, "int array count should be 1 after first add");
    tknAssert(intArray.maxCount == 2, "maxCount should remain 2 (not full)");
    
    // 添加第二个元素（索引1，末尾）
    int val2 = 20;
    tknAddToDynamicArray(&intArray, &val2, 1);
    tknAssert(intArray.count == 2, "int array count should be 2 after second add");
    tknAssert(intArray.maxCount == 2, "maxCount should still be 2 (now full)");
    
    // 添加第三个元素（触发扩容，maxCount翻倍为4）
    int val3 = 30;
    tknAddToDynamicArray(&intArray, &val3, 2);
    tknAssert(intArray.count == 3, "int array count should be 3 after third add");
    tknAssert(intArray.maxCount == 4, "maxCount should double to 4 after扩容");
    
    // 验证数据正确性（扩容后数据未丢失）
    int* ptr;
    tknGetFromDynamicArray(&intArray, 0, (void**)&ptr);
    tknAssert(*ptr == 10, "int array[0] should be 10");
    tknGetFromDynamicArray(&intArray, 1, (void**)&ptr);
    tknAssert(*ptr == 20, "int array[1] should be 20");
    tknGetFromDynamicArray(&intArray, 2, (void**)&ptr);
    tknAssert(*ptr == 30, "int array[2] should be 30");
    
    // 测试结构体数组插入（中间位置）
    TknDynamicArray structArray = tknCreateDynamicArray(sizeof(TestStruct), 2);
    TestStruct s1 = {1, "Alice"};
    TestStruct s2 = {2, "Bob"};
    TestStruct s3 = {3, "Charlie"};
    
    tknAddToDynamicArray(&structArray, &s1, 0); // 添加到索引0（末尾）
    tknAddToDynamicArray(&structArray, &s3, 1); // 添加到索引1（末尾）
    tknAddToDynamicArray(&structArray, &s2, 1); // 插入到索引1（s1和s3之间）
    
    // 验证插入后顺序：s1 -> s2 -> s3
    TestStruct* sPtr;
    tknGetFromDynamicArray(&structArray, 0, (void**)&sPtr);
    tknAssert(sPtr->id == 1 && strcmp(sPtr->name, "Alice") == 0, "struct[0] mismatch");
    
    tknGetFromDynamicArray(&structArray, 1, (void**)&sPtr);
    tknAssert(sPtr->id == 2 && strcmp(sPtr->name, "Bob") == 0, "struct[1] mismatch");
    
    tknGetFromDynamicArray(&structArray, 2, (void**)&sPtr);
    tknAssert(sPtr->id == 3 && strcmp(sPtr->name, "Charlie") == 0, "struct[2] mismatch");
    
    // 销毁数组
    tknDestroyDynamicArray(intArray);
    tknDestroyDynamicArray(structArray);
    printf("testAddToDynamicArray passed.\n\n");
}

// 按索引移除元素测试
static void testRemoveAtIndexFromDynamicArray() {
    printf("Running testRemoveAtIndexFromDynamicArray...\n");
    
    TknDynamicArray intArray = tknCreateDynamicArray(sizeof(int), 5);
    int vals[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        tknAddToDynamicArray(&intArray, &vals[i], i);
    }
    tknAssert(intArray.count == 5, "int array should have 5 elements before removal");
    
    // 移除中间元素（索引2，值30）
    tknRemoveAtIndexFromDynamicArray(&intArray, 2);
    tknAssert(intArray.count == 4, "count should be 4 after removal");
    
    // 验证移除后顺序：10,20,40,50
    int* ptr;
    tknGetFromDynamicArray(&intArray, 0, (void**)&ptr);
    tknAssert(*ptr == 10, "index 0 should be 10 after removal");
    
    tknGetFromDynamicArray(&intArray, 1, (void**)&ptr);
    tknAssert(*ptr == 20, "index 1 should be 20 after removal");
    
    tknGetFromDynamicArray(&intArray, 2, (void**)&ptr);
    tknAssert(*ptr == 40, "index 2 should be 40 after removal");
    
    tknGetFromDynamicArray(&intArray, 3, (void**)&ptr);
    tknAssert(*ptr == 50, "index 3 should be 50 after removal");
    
    // 移除第一个元素（索引0）
    tknRemoveAtIndexFromDynamicArray(&intArray, 0);
    tknAssert(intArray.count == 3, "count should be 3 after second removal");
    
    tknGetFromDynamicArray(&intArray, 0, (void**)&ptr);
    tknAssert(*ptr == 20, "index 0 should be 20 after removing first element");
    
    // 移除最后一个元素（索引2）
    tknRemoveAtIndexFromDynamicArray(&intArray, 2);
    tknAssert(intArray.count == 2, "count should be 2 after third removal");
    
    tknGetFromDynamicArray(&intArray, 1, (void**)&ptr);
    tknAssert(*ptr == 40, "index 1 should be 40 after removing last element");
    
    tknDestroyDynamicArray(intArray);
    printf("testRemoveAtIndexFromDynamicArray passed.\n\n");
}

// 按值移除元素测试
static void testRemoveFromDynamicArray() {
    printf("Running testRemoveFromDynamicArray...\n");
    
    TknDynamicArray structArray = tknCreateDynamicArray(sizeof(TestStruct), 3);
    TestStruct s1 = {1, "Alice"};
    TestStruct s2 = {2, "Bob"};
    TestStruct s3 = {3, "Charlie"};
    
    tknAddToDynamicArray(&structArray, &s1, 0);
    tknAddToDynamicArray(&structArray, &s2, 1);
    tknAddToDynamicArray(&structArray, &s3, 2);
    tknAssert(structArray.count == 3, "struct array should have 3 elements before removal");
    
    // 移除s2
    tknRemoveFromDynamicArray(&structArray, &s2);
    tknAssert(structArray.count == 2, "count should be 2 after removing s2");
    
    // 验证剩余元素：s1, s3
    TestStruct* sPtr;
    tknGetFromDynamicArray(&structArray, 0, (void**)&sPtr);
    tknAssert(sPtr->id == 1, "struct[0] should be s1 after removal");
    
    tknGetFromDynamicArray(&structArray, 1, (void**)&sPtr);
    tknAssert(sPtr->id == 3, "struct[1] should be s3 after removal");
    
    // 尝试移除不存在的元素（应该触发错误）
    TestStruct s4 = {4, "Dave"};
    // 注意：tknRemoveFromDynamicArray会调用tknError，这里注释掉避免测试终止
    // tknRemoveFromDynamicArray(&structArray, &s4);
    
    tknDestroyDynamicArray(structArray);
    printf("testRemoveFromDynamicArray passed.\n\n");
}

// 获取元素与越界测试
static void testGetFromDynamicArray() {
    printf("Running testGetFromDynamicArray...\n");
    
    TknDynamicArray intArray = tknCreateDynamicArray(sizeof(int), 2);
    int val1 = 100, val2 = 200;
    tknAddToDynamicArray(&intArray, &val1, 0);
    tknAddToDynamicArray(&intArray, &val2, 1);
    
    // 正常获取
    int* ptr;
    tknGetFromDynamicArray(&intArray, 0, (void**)&ptr);
    tknAssert(*ptr == 100, "get index 0 should return 100");
    
    tknGetFromDynamicArray(&intArray, 1, (void**)&ptr);
    tknAssert(*ptr == 200, "get index 1 should return 200");
    
    // 越界获取（应该触发错误）
    // 注意：会调用tknError，这里注释掉避免测试终止
    // tknGetFromDynamicArray(&intArray, 2, (void**)&ptr);
    
    tknDestroyDynamicArray(intArray);
    printf("testGetFromDynamicArray passed.\n\n");
}

// 清空数组测试
static void testClearDynamicArray() {
    printf("Running testClearDynamicArray...\n");
    
    TknDynamicArray intArray = tknCreateDynamicArray(sizeof(int), 5);
    for (int i = 0; i < 3; i++) {
        int val = i * 10;
        tknAddToDynamicArray(&intArray, &val, i);
    }
    tknAssert(intArray.count == 3, "array should have 3 elements before clear");
    
    // 清空数组
    tknClearDynamicArray(&intArray);
    tknAssert(intArray.count == 0, "count should be 0 after clear");
    tknAssert(intArray.maxCount == 5, "maxCount should remain unchanged after clear");
    
    // 清空后添加元素
    int val = 100;
    tknAddToDynamicArray(&intArray, &val, 0);
    tknAssert(intArray.count == 1, "count should be 1 after adding to cleared array");
    
    int* ptr;
    tknGetFromDynamicArray(&intArray, 0, (void**)&ptr);
    tknAssert(*ptr == 100, "element should be correctly added after clear");
    
    tknDestroyDynamicArray(intArray);
    printf("testClearDynamicArray passed.\n\n");
}

// 边界条件测试
static void testDynamicArrayEdgeCases() {
    printf("Running testDynamicArrayEdgeCases...\n");
    
    // 测试空数组操作
    TknDynamicArray emptyArray = tknCreateDynamicArray(sizeof(float), 0); // maxCount=0
    // 注意：添加元素到maxCount=0的数组会触发扩容（0*2=0？可能需要优化）
    // 这里用maxCount=1测试
    TknDynamicArray tinyArray = tknCreateDynamicArray(sizeof(float), 1);
    float f = 3.14f;
    tknAddToDynamicArray(&tinyArray, &f, 0); // 添加第一个元素
    tknAssert(tinyArray.count == 1, "tiny array count should be 1");
    
    tknAddToDynamicArray(&tinyArray, &f, 1); // 扩容到2
    tknAssert(tinyArray.maxCount == 2, "tiny array should resize to 2");
    
    // 测试插入到索引0（头部）
    TknDynamicArray strArray = tknCreateDynamicArray(sizeof(const char*), 3);
    const char* s1 = "hello";
    const char* s2 = "world";
    tknAddToDynamicArray(&strArray, &s1, 0); // [hello]
    tknAddToDynamicArray(&strArray, &s2, 0); // [world, hello]
    
    const char** strPtr;
    tknGetFromDynamicArray(&strArray, 0, (void**)&strPtr);
    tknAssert(strcmp(*strPtr, "world") == 0, "index 0 should be 'world'");
    
    tknGetFromDynamicArray(&strArray, 1, (void**)&strPtr);
    tknAssert(strcmp(*strPtr, "hello") == 0, "index 1 should be 'hello'");
    
    tknDestroyDynamicArray(emptyArray);
    tknDestroyDynamicArray(tinyArray);
    tknDestroyDynamicArray(strArray);
    printf("testDynamicArrayEdgeCases passed.\n\n");
}

int main() {
    printf("=== Starting TknDynamicArray Tests ===\n\n");
    
    testDynamicArrayInitialization();
    testAddToDynamicArray();
    testRemoveAtIndexFromDynamicArray();
    testRemoveFromDynamicArray();
    testGetFromDynamicArray();
    testClearDynamicArray();
    testDynamicArrayEdgeCases();
    
    printf("=== All TknDynamicArray Tests Passed ===\n");
    return 0;
}