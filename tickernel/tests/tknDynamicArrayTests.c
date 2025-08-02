#include "tknCore.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 测试用结构体（验证复杂类型处理）
typedef struct {
    int id;
    char name[32];
    float score;
} Student;

// 辅助函数：创建测试用Student
static Student createStudent(int id, const char* name, float score) {
    Student s;
    s.id = id;
    strncpy(s.name, name, sizeof(s.name)-1);
    s.name[sizeof(s.name)-1] = '\0'; // 确保字符串结束
    s.score = score;
    return s;
}

// 1. 初始化与销毁测试
static void testInitialization() {
    printf("Running testInitialization...\n");

    // 测试int类型数组
    TknDynamicArray intArr = tknCreateDynamicArray(sizeof(int), 5);
    tknAssert(intArr.dataSize == sizeof(int), "intArr dataSize mismatch");
    tknAssert(intArr.maxCount == 5, "intArr maxCount should be 5");
    tknAssert(intArr.count == 0, "intArr initial count should be 0");
    tknAssert(intArr.array != NULL, "intArr should allocate memory");

    // 测试结构体类型数组
    TknDynamicArray studentArr = tknCreateDynamicArray(sizeof(Student), 3);
    tknAssert(studentArr.dataSize == sizeof(Student), "studentArr dataSize mismatch");
    tknAssert(studentArr.maxCount == 3, "studentArr maxCount should be 3");
    tknAssert(studentArr.count == 0, "studentArr initial count should be 0");

    // 销毁数组
    tknDestroyDynamicArray(intArr);
    tknDestroyDynamicArray(studentArr);

    printf("testInitialization passed.\n\n");
}

// 2. 添加元素与扩容测试
static void testAddAndResize() {
    printf("Running testAddAndResize...\n");

    // 测试int数组（初始容量2，触发扩容）
    TknDynamicArray intArr = tknCreateDynamicArray(sizeof(int), 2);
    int vals[] = {10, 20, 30, 40};

    // 添加元素0（索引0）
    tknAddToDynamicArray(&intArr, &vals[0], 0);
    tknAssert(intArr.count == 1, "intArr count should be 1 after first add");
    int* intPtr = (int*)tknGetFromDynamicArray(&intArr, 0);
    tknAssert(intPtr != NULL && *intPtr == 10, "intArr[0] should be 10");

    // 添加元素1（索引1，末尾）
    tknAddToDynamicArray(&intArr, &vals[1], 1);
    tknAssert(intArr.count == 2, "intArr count should be 2 after second add");
    intPtr = (int*)tknGetFromDynamicArray(&intArr, 1);
    tknAssert(intPtr != NULL && *intPtr == 20, "intArr[1] should be 20");

    // 添加元素2（容量满，触发扩容至4）
    tknAddToDynamicArray(&intArr, &vals[2], 2);
    tknAssert(intArr.maxCount == 4, "intArr should resize to 4");
    tknAssert(intArr.count == 3, "intArr count should be 3 after third add");
    intPtr = (int*)tknGetFromDynamicArray(&intArr, 2);
    tknAssert(intPtr != NULL && *intPtr == 30, "intArr[2] should be 30");

    // 测试结构体数组（中间插入）
    TknDynamicArray studentArr = tknCreateDynamicArray(sizeof(Student), 2);
    Student s1 = createStudent(1, "Alice", 90.5f);
    Student s2 = createStudent(2, "Bob", 85.0f);
    Student s3 = createStudent(3, "Charlie", 95.5f);

    tknAddToDynamicArray(&studentArr, &s1, 0); // [s1]
    tknAddToDynamicArray(&studentArr, &s3, 1); // [s1, s3]
    tknAddToDynamicArray(&studentArr, &s2, 1); // [s1, s2, s3]（扩容至4）

    Student* stuPtr = (Student*)tknGetFromDynamicArray(&studentArr, 0);
    tknAssert(stuPtr != NULL && stuPtr->id == 1, "studentArr[0] should be s1");

    stuPtr = (Student*)tknGetFromDynamicArray(&studentArr, 1);
    tknAssert(stuPtr != NULL && stuPtr->id == 2, "studentArr[1] should be s2");

    stuPtr = (Student*)tknGetFromDynamicArray(&studentArr, 2);
    tknAssert(stuPtr != NULL && stuPtr->id == 3, "studentArr[2] should be s3");

    // 销毁数组
    tknDestroyDynamicArray(intArr);
    tknDestroyDynamicArray(studentArr);

    printf("testAddAndResize passed.\n\n");
}

// 3. 按索引移除元素测试
static void testRemoveAtIndex() {
    printf("Running testRemoveAtIndex...\n");

    TknDynamicArray strArr = tknCreateDynamicArray(sizeof(const char*), 5);
    const char* strs[] = {"apple", "banana", "cherry", "date"};

    // 初始化数组：[apple, banana, cherry, date]
    for (int i = 0; i < 4; i++) {
        tknAddToDynamicArray(&strArr, &strs[i], i);
    }
    tknAssert(strArr.count == 4, "strArr should have 4 elements initially");

    // 移除索引1（banana）
    tknRemoveAtIndexFromDynamicArray(&strArr, 1);
    tknAssert(strArr.count == 3, "strArr count should be 3 after removal");
    
    const char** strPtr = (const char**)tknGetFromDynamicArray(&strArr, 0);
    tknAssert(strPtr != NULL && strcmp(*strPtr, "apple") == 0, "strArr[0] should be 'apple'");
    
    strPtr = (const char**)tknGetFromDynamicArray(&strArr, 1);
    tknAssert(strPtr != NULL && strcmp(*strPtr, "cherry") == 0, "strArr[1] should be 'cherry'");

    // 移除索引2（date）
    tknRemoveAtIndexFromDynamicArray(&strArr, 2);
    tknAssert(strArr.count == 2, "strArr count should be 2 after second removal");
    
    strPtr = (const char**)tknGetFromDynamicArray(&strArr, 1);
    tknAssert(strPtr != NULL && strcmp(*strPtr, "cherry") == 0, "strArr[1] should remain 'cherry'");

    // 移除索引0（apple）
    tknRemoveAtIndexFromDynamicArray(&strArr, 0);
    tknAssert(strArr.count == 1, "strArr count should be 1 after third removal");
    
    strPtr = (const char**)tknGetFromDynamicArray(&strArr, 0);
    tknAssert(strPtr != NULL && strcmp(*strPtr, "cherry") == 0, "strArr[0] should be 'cherry'");

    tknDestroyDynamicArray(strArr);
    printf("testRemoveAtIndex passed.\n\n");
}

// 4. 按值移除元素测试
static void testRemoveByValue() {
    printf("Running testRemoveByValue...\n");

    TknDynamicArray studentArr = tknCreateDynamicArray(sizeof(Student), 3);
    Student s1 = createStudent(1, "Alice", 90.5f);
    Student s2 = createStudent(2, "Bob", 85.0f);
    Student s3 = createStudent(3, "Charlie", 95.5f);

    tknAddToDynamicArray(&studentArr, &s1, 0);
    tknAddToDynamicArray(&studentArr, &s2, 1);
    tknAddToDynamicArray(&studentArr, &s3, 2);
    tknAssert(studentArr.count == 3, "studentArr should have 3 elements initially");

    // 移除s2
    tknRemoveFromDynamicArray(&studentArr, &s2);
    tknAssert(studentArr.count == 2, "studentArr count should be 2 after removing s2");
    
    Student* stuPtr = (Student*)tknGetFromDynamicArray(&studentArr, 0);
    tknAssert(stuPtr != NULL && stuPtr->id == 1, "studentArr[0] should be s1");
    
    stuPtr = (Student*)tknGetFromDynamicArray(&studentArr, 1);
    tknAssert(stuPtr != NULL && stuPtr->id == 3, "studentArr[1] should be s3");

    // 尝试移除不存在的元素（应触发tknError）
    Student s4 = createStudent(4, "Dave", 70.0f);
    // 注意：取消注释会触发错误终止
    // tknRemoveFromDynamicArray(&studentArr, &s4);

    tknDestroyDynamicArray(studentArr);
    printf("testRemoveByValue passed.\n\n");
}

// 5. 获取元素与越界测试
static void testGetElement() {
    printf("Running testGetElement...\n");

    TknDynamicArray intArr = tknCreateDynamicArray(sizeof(int), 3);
    int vals[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        tknAddToDynamicArray(&intArr, &vals[i], i);
    }

    // 正常获取
    int* intPtr = (int*)tknGetFromDynamicArray(&intArr, 0);
    tknAssert(intPtr != NULL && *intPtr == 100, "intArr[0] should be 100");
    
    intPtr = (int*)tknGetFromDynamicArray(&intArr, 2);
    tknAssert(intPtr != NULL && *intPtr == 300, "intArr[2] should be 300");

    // 越界获取（索引3，count=3，应返回NULL）
    intPtr = (int*)tknGetFromDynamicArray(&intArr, 3);
    tknAssert(intPtr == NULL, "Getting index 3 should return NULL");

    // 负数索引（通过断言拦截，因为uint32_t无负数，实际会是大正数）
    // 注意：uint32_t index=-1会被转换为4294967295，必然越界
    intPtr = (int*)tknGetFromDynamicArray(&intArr, (uint32_t)-1);
    tknAssert(intPtr == NULL, "Negative index (as uint32_t) should return NULL");

    tknDestroyDynamicArray(intArr);
    printf("testGetElement passed.\n\n");
}

// 6. 清空数组测试
static void testClearArray() {
    printf("Running testClearArray...\n");

    TknDynamicArray floatArr = tknCreateDynamicArray(sizeof(float), 4);
    float floats[] = {1.1f, 2.2f, 3.3f};
    for (int i = 0; i < 3; i++) {
        tknAddToDynamicArray(&floatArr, &floats[i], i);
    }
    tknAssert(floatArr.count == 3, "floatArr should have 3 elements before clear");

    // 清空数组
    tknClearDynamicArray(&floatArr);
    tknAssert(floatArr.count == 0, "floatArr count should be 0 after clear");
    tknAssert(floatArr.maxCount == 4, "floatArr maxCount should remain 4 after clear");

    // 清空后获取元素（应返回NULL）
    float* floatPtr = (float*)tknGetFromDynamicArray(&floatArr, 0);
    tknAssert(floatPtr == NULL, "Getting from cleared array should return NULL");

    // 清空后添加元素
    float newVal = 4.4f;
    tknAddToDynamicArray(&floatArr, &newVal, 0);
    tknAssert(floatArr.count == 1, "floatArr count should be 1 after adding to cleared array");
    
    floatPtr = (float*)tknGetFromDynamicArray(&floatArr, 0);
    tknAssert(floatPtr != NULL && *floatPtr == 4.4f, "floatArr[0] should be 4.4f");

    tknDestroyDynamicArray(floatArr);
    printf("testClearArray passed.\n\n");
}

// 7. 边界条件测试（空数组、容量0等）
static void testEdgeCases() {
    printf("Running testEdgeCases...\n");

    // 测试容量为0的数组（添加时会扩容）
    TknDynamicArray emptyArr = tknCreateDynamicArray(sizeof(int), 0);
    int val = 5;
    tknAddToDynamicArray(&emptyArr, &val, 0); // 容量0*2=0？实际会分配内存（依赖实现）
    tknAssert(emptyArr.count == 1, "emptyArr should have 1 element after add");
    
    int* ptr = (int*)tknGetFromDynamicArray(&emptyArr, 0);
    tknAssert(ptr != NULL && *ptr == 5, "emptyArr[0] should be 5");

    // 测试头部插入（频繁移动元素）
    TknDynamicArray queue = tknCreateDynamicArray(sizeof(int), 2);
    for (int i = 0; i < 4; i++) {
        tknAddToDynamicArray(&queue, &i, 0); // 每次插入到头部
    }
    // 预期顺序：3,2,1,0
    tknAssert(*(int*)tknGetFromDynamicArray(&queue, 0) == 3, "queue[0] should be 3");
    tknAssert(*(int*)tknGetFromDynamicArray(&queue, 3) == 0, "queue[3] should be 0");

    tknDestroyDynamicArray(emptyArr);
    tknDestroyDynamicArray(queue);
    printf("testEdgeCases passed.\n\n");
}

int main() {
    printf("=== Starting TknDynamicArray Tests (Updated) ===\n\n");

    testInitialization();
    testAddAndResize();
    testRemoveAtIndex();
    testRemoveByValue();
    testGetElement();
    testClearArray();
    testEdgeCases();

    printf("=== All TknDynamicArray Tests Passed ===\n");
    return 0;
}