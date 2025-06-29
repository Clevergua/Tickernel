#include <assert.h>
#include "graphics.h"

// 测试用结构体类型
typedef struct
{
    int id;
    char name[20];
} TestStruct;

// 填充测试结构体
void fillStruct(TestStruct *s, int id, const char *name)
{
    s->id = id;
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->name[sizeof(s->name) - 1] = '\0';
}

// 比较两个结构体是否相等
int structsEqual(const TestStruct *a, const TestStruct *b)
{
    return a->id == b->id && strcmp(a->name, b->name) == 0;
}

int main()
{
    printf("=== TickernelDynamicArray 简化测试 ===\n\n");
    TickernelDynamicArray ptrArray;
    tickernelCreateDynamicArray(&ptrArray, sizeof(int *), 4);
    // 添加指针到数组
    int *ptr = tickernelMalloc(sizeof(int));
    *ptr = 100;
    tickernelAddToDynamicArray(&ptrArray, &ptr, 0); // 注意：传入指针的地址
    // 从数组中获取指针
    void *ptrAddr = NULL;
    tickernelGetFromDynamicArray(&ptrArray, 0, &ptrAddr);

    int *ptrResult = TICKERNEL_GET_FROM_DYNAMIC_ARRAY(&ptrArray, 0, int *);
    // 使用指针
    printf("Value: %d\n", **(int **)ptrAddr); // 输出: 100
    tickernelAssert(*(int **)ptrAddr == ptr, "Pointer address mismatch!");

    // 创建存储FieldLayout的数组（注意：元素大小是结构体本身的大小）
    TickernelDynamicArray layoutArray;
    tickernelCreateDynamicArray(&layoutArray, sizeof(FieldLayout), 4);

    // 创建并初始化结构体（栈上或堆上均可）
    FieldLayout layout = {
        .name = "position",
        .size = 12, // 3 floats
        .offset = 213};

    // 直接存储结构体（而非指针）
    tickernelAddToDynamicArray(&layoutArray, &layout, 0); // 存储结构体的副本

    // 从数组中获取结构体（返回值类型是FieldLayout，而非指针）
    FieldLayout retrievedLayout = TICKERNEL_GET_FROM_DYNAMIC_ARRAY(&layoutArray, 0, FieldLayout);

    // 使用获取的结构体（直接访问，无需指针解引用）
    printf("Field: %s\n", retrievedLayout.name);          // 输出: position
    printf("Size: %u bytes\n", retrievedLayout.size);     // 输出: 12
    printf("Offset: %u bytes\n", retrievedLayout.offset); // 输出: 0

    // 验证内存地址不同（因为存储的是副本）
    printf("原始结构体地址: %p\n", (void *)&layout);
    printf("数组中结构体地址: %p\n", (void *)&retrievedLayout);
    // 使用 assert 验证结构体内容
    assert(strcmp(retrievedLayout.name, "position") == 0);
    assert(retrievedLayout.size == layout.size);
    assert(retrievedLayout.offset == layout.offset);

    // 验证内存地址不同（因为存储的是副本）
    assert(&layout != &retrievedLayout);
    // 清理数组（无需释放单个元素，因为存储的是值而非指针）
    tickernelDestroyDynamicArray(layoutArray);

    return 0;

    // printf("=== TickernelDynamicArray 简化测试 ===\n\n");
    // TickernelDynamicArray rwqq;
    // tickernelCreateDynamicArray(&rwqq, sizeof(int), 4);
    // // 添加指针到数组
    // int qqq = 999;
    // tickernelAddToDynamicArray(&rwqq, &qqq, 0); // 注意：传入指针的地址
    // // 从数组中获取指针
    // int wwww;
    // tickernelGetFromDynamicArray(&rwqq, 0, (void **)&wwww);
    // // 使用指针
    // printf("Value: %d\n", *ptrAddr); // 输出: 999
    // tickernelAssert(ptrAddr == ptr, "Pointer address mismatch!");
    return 0;
}