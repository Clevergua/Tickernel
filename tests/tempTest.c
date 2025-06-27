#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tickernelCore.h"

// 测试用结构体类型
typedef struct {
    int id;
    char name[20];
} TestStruct;

// 填充测试结构体
void fillStruct(TestStruct* s, int id, const char* name) {
    s->id = id;
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->name[sizeof(s->name) - 1] = '\0';
}

// 比较两个结构体是否相等
int structsEqual(const TestStruct* a, const TestStruct* b) {
    return a->id == b->id && strcmp(a->name, b->name) == 0;
}

int main() {
    printf("=== TickernelDynamicArray 简化测试 ===\n\n");

    /*---------------------
     * 指针存储测试
     *---------------------*/
    printf("--- 指针存储测试 ---\n");
    TickernelDynamicArray ptrArray;
    
    // 1. 创建存储int*的动态数组
    tickernelCreateDynamicArray(&ptrArray, sizeof(int*), 2);
    assert(ptrArray.maxCount == 2);
    assert(ptrArray.count == 0);
    printf("  动态数组创建 - 通过\n");

    // 2. 添加指针元素
    int num1 = 100, num2 = 200, num3 = 300;
    int* ptr1 = &num1, *ptr2 = &num2, *ptr3 = &num3;
    
    tickernelAddToDynamicArray(&ptrArray, &ptr1, 0);
    tickernelAddToDynamicArray(&ptrArray, &ptr2, 1);
    
    int** retrievedPtr1, **retrievedPtr2;
    tickernelGetFromDynamicArray(&ptrArray, 0, (void**)&retrievedPtr1);
    tickernelGetFromDynamicArray(&ptrArray, 1, (void**)&retrievedPtr2);
    
    assert(ptrArray.count == 2);
    assert(*retrievedPtr1 == ptr1);
    assert(*retrievedPtr2 == ptr2);
    printf("  指针添加与获取 - 通过\n");

    // 3. 扩容测试
    tickernelAddToDynamicArray(&ptrArray, &ptr3, 2);  // 触发扩容
    assert(ptrArray.maxCount == 4);
    assert(ptrArray.count == 3);
    
    int** retrievedPtr3;
    tickernelGetFromDynamicArray(&ptrArray, 2, (void**)&retrievedPtr3);
    assert(*retrievedPtr3 == ptr3);
    printf("  指针数组扩容 - 通过\n");

    // 4. 删除测试
    tickernelRemoveAtIndexFromDynamicArray(&ptrArray, 1);
    assert(ptrArray.count == 2);
    
    tickernelGetFromDynamicArray(&ptrArray, 0, (void**)&retrievedPtr1);
    tickernelGetFromDynamicArray(&ptrArray, 1, (void**)&retrievedPtr3);
    
    assert(*retrievedPtr1 == ptr1);
    assert(*retrievedPtr3 == ptr3);
    printf("  指针删除 - 通过\n");

    tickernelDestroyDynamicArray(ptrArray);
    printf("  指针数组销毁 - 通过\n");


    /*-------------------------
     * 结构体存储测试
     *-------------------------*/
    printf("\n--- 结构体存储测试 ---\n");
    TickernelDynamicArray structArray;
    tickernelCreateDynamicArray(&structArray, sizeof(TestStruct), 2);
    
    // 1. 创建并添加结构体
    TestStruct struct1, struct2, struct3;
    fillStruct(&struct1, 1, "One");
    fillStruct(&struct2, 2, "Two");
    fillStruct(&struct3, 3, "Three");
    
    tickernelAddToDynamicArray(&structArray, &struct1, 0);
    tickernelAddToDynamicArray(&structArray, &struct2, 1);
    
    TestStruct* retrieved1, *retrieved2;
    tickernelGetFromDynamicArray(&structArray, 0, (void**)&retrieved1);
    tickernelGetFromDynamicArray(&structArray, 1, (void**)&retrieved2);
    
    assert(structArray.count == 2);
    assert(structsEqual(retrieved1, &struct1));
    assert(structsEqual(retrieved2, &struct2));
    printf("  结构体添加与获取 - 通过\n");

    // 2. 扩容测试
    tickernelAddToDynamicArray(&structArray, &struct3, 2);  // 触发扩容
    assert(structArray.maxCount == 4);
    assert(structArray.count == 3);
    
    TestStruct* retrieved3;
    tickernelGetFromDynamicArray(&structArray, 2, (void**)&retrieved3);
    assert(structsEqual(retrieved3, &struct3));
    printf("  结构体数组扩容 - 通过\n");

    // 3. 删除测试
    tickernelRemoveFromDynamicArray(&structArray, &struct2);
    assert(structArray.count == 2);
    
    tickernelGetFromDynamicArray(&structArray, 0, (void**)&retrieved1);
    tickernelGetFromDynamicArray(&structArray, 1, (void**)&retrieved3);
    
    assert(structsEqual(retrieved1, &struct1));
    assert(structsEqual(retrieved3, &struct3));
    printf("  结构体删除 - 通过\n");

    tickernelDestroyDynamicArray(structArray);
    printf("  结构体数组销毁 - 通过\n");


    printf("\n=== 简化测试完成 ===\n");
    return 0;
}