#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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

void serializeTickernelVoxel(const char *filename, TickernelVoxel tickernelVoxel)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        tickernelError("Failed to open file with name :%s for writing!\n", filename);
    }
    else
    {
        // continue;
    }

    size_t result = fwrite(&tickernelVoxel.propertyCount, sizeof(uint32_t), 1, file);
    for (size_t i = 0; i < tickernelVoxel.propertyCount; i++)
    {
        size_t length = strlen(tickernelVoxel.names[i]) + 1;
        result = fwrite(&length, sizeof(size_t), 1, file);
        result = fwrite(&tickernelVoxel.names[i], sizeof(char), length, file);
    }
    for (size_t i = 0; i < tickernelVoxel.propertyCount; i++)
    {
        result = fwrite(&tickernelVoxel.types[i], sizeof(int32_t), 1, file);
    }
    result = fwrite(&tickernelVoxel.vertexCount, sizeof(uint32_t), 1, file);
    for (size_t i = 0; i < tickernelVoxel.propertyCount; i++)
    {
        result = fwrite(&tickernelVoxel.indexToProperties[i], tickernelVoxelPropertyTypeToBytes[i], tickernelVoxel.vertexCount, file);
    }
    fclose(file);
}

void deserializeTickernelVoxel(const char *filename, TickernelVoxel *pTickernelVoxel, TickernelVoxelMalloc tickernelVoxelMalloc)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        tickernelError("Failed to open file for reading");
        return;
    }
    size_t freadResult;
    freadResult = fread(&pTickernelVoxel->propertyCount, sizeof(uint32_t), 1, file);
    pTickernelVoxel->names = (char **)tickernelVoxelMalloc(pTickernelVoxel->propertyCount * sizeof(char *));
    for (uint32_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        uint32_t length;
        freadResult = fread(&length, sizeof(uint32_t), 1, file);
        pTickernelVoxel->names[i] = (char *)tickernelVoxelMalloc(length * sizeof(char));
        freadResult = fread(pTickernelVoxel->names[i], sizeof(char), length, file);
    }
    pTickernelVoxel->types = (TickernelVoxelPropertyType *)tickernelVoxelMalloc(pTickernelVoxel->propertyCount * sizeof(TickernelVoxelPropertyType));
    for (uint32_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        freadResult = fread(&pTickernelVoxel->types[i], sizeof(int32_t), 1, file);
    }
    freadResult = fread(&pTickernelVoxel->vertexCount, sizeof(uint32_t), 1, file);
    pTickernelVoxel->indexToProperties = (void **)tickernelVoxelMalloc(pTickernelVoxel->propertyCount * sizeof(void *));
    for (uint32_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        size_t btyes = tickernelVoxelPropertyTypeToBytes[pTickernelVoxel->types[i]];
        pTickernelVoxel->indexToProperties[i] = tickernelVoxelMalloc(pTickernelVoxel->vertexCount * btyes);
        long position = ftell(file);
        if (pTickernelVoxel->types[i] == TICKERNEL_VOXEL_INT8)
        {
            freadResult = fread((int8_t *)pTickernelVoxel->indexToProperties[i], btyes, pTickernelVoxel->vertexCount, file);
        }
        else if (pTickernelVoxel->types[i] == TICKERNEL_VOXEL_UINT8)
        {
            freadResult = fread((uint8_t *)pTickernelVoxel->indexToProperties[i], btyes, pTickernelVoxel->vertexCount, file);
            position = ftell(file);
        }
        else if (pTickernelVoxel->types[i] == TICKERNEL_VOXEL_INT16)
        {
            freadResult = fread((int16_t *)pTickernelVoxel->indexToProperties[i], btyes, pTickernelVoxel->vertexCount, file);
        }
        else if (pTickernelVoxel->types[i] == TICKERNEL_VOXEL_UINT16)
        {
            freadResult = fread((uint16_t *)pTickernelVoxel->indexToProperties[i], btyes, pTickernelVoxel->vertexCount, file);
        }
        else if (pTickernelVoxel->types[i] == TICKERNEL_VOXEL_INT32)
        {
            freadResult = fread((int32_t *)pTickernelVoxel->indexToProperties[i], btyes, pTickernelVoxel->vertexCount, file);
        }
        else if (pTickernelVoxel->types[i] == TICKERNEL_VOXEL_UINT32)
        {
            freadResult = fread((uint32_t *)pTickernelVoxel->indexToProperties[i], btyes, pTickernelVoxel->vertexCount, file);
        }
        else if (pTickernelVoxel->types[i] == TICKERNEL_VOXEL_FLOAT32)
        {
            freadResult = fread((float *)pTickernelVoxel->indexToProperties[i], btyes, pTickernelVoxel->vertexCount, file);
        }
        else
        {
            tickernelError("Unknown type: %d\n", pTickernelVoxel->types[i]);
        }
    }
    fclose(file);
}

void releaseTickernelVoxel(TickernelVoxel *pTickernelVoxel, TickernelVoxelFree tickernelFree)
{
    for (size_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        tickernelFree(pTickernelVoxel->indexToProperties[i]);
    }
    tickernelFree(pTickernelVoxel->indexToProperties);
    tickernelFree(pTickernelVoxel->types);
    for (size_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        tickernelFree(pTickernelVoxel->names[i]);
    }
    tickernelFree(pTickernelVoxel->names);
}