#pragma once
#include "tickernelCore.h"

typedef enum
{
    TICKERNEL_VOXEL_INT8,
    TICKERNEL_VOXEL_UINT8,
    TICKERNEL_VOXEL_INT16,
    TICKERNEL_VOXEL_UINT16,
    TICKERNEL_VOXEL_INT32,
    TICKERNEL_VOXEL_UINT32,
    TICKERNEL_VOXEL_FLOAT32,
} TickernelVoxelPropertyType;

static size_t tickernelVoxelPropertyTypeToBytes[] =
    {
        1,
        1,
        2,
        2,
        4,
        4,
        4,
};

typedef struct
{
    uint32_t propertyCount;
    char **names;
    TickernelVoxelPropertyType *types;
    uint32_t vertexCount;
    void **indexToProperties;
} TickernelVoxel;

typedef void *(*TickernelVoxelMalloc)(size_t);
typedef void (*TickernelVoxelFree)(void *);

void serializeTickernelVoxel(const char *filename, TickernelVoxel tickernelVoxel);
void deserializeTickernelVoxel(const char *filename, TickernelVoxel *pTickernelVoxel, TickernelVoxelMalloc tickernelVoxelMalloc);
void releaseTickernelVoxel(TickernelVoxel *pTickernelVoxel, TickernelVoxelFree tickernelFree);
