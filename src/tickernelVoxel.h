#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
typedef enum
{
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    FLOAT32,
} TickernelVoxelPropertyType;

typedef union
{
    int8_t int8Value;
    uint8_t uint8Value;
    int16_t int16Value;
    uint16_t uint16Value;
    int32_t int32Value;
    uint32_t uint32Value;
    float float32Value;
} TickernelVoxelProperty;

typedef struct
{
    uint32_t propertyCount;
    char **names;
    TickernelVoxelPropertyType **types;
    uint32_t vertexCount;
    TickernelVoxelProperty **indexToProperties;
} TickernelVoxel;

typedef void *(*TickernelVoxelMalloc)(size_t);
typedef void (*TickernelVoxelFree)(void *);

void SerializeTickernelVoxel(const char *filename, TickernelVoxel tickernelVoxel)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Failed to open file with name :%s for writing!\n", filename);
        abort();
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
        result = fwrite(tickernelVoxel.names[i], sizeof(char), length, file);
    }
    for (size_t i = 0; i < tickernelVoxel.propertyCount; i++)
    {
        result = fwrite(tickernelVoxel.types[i], sizeof(int32_t), 1, file);
    }
    result = fwrite(&tickernelVoxel.vertexCount, sizeof(uint32_t), 1, file);
    for (size_t i = 0; i < tickernelVoxel.propertyCount; i++)
    {
        result = fwrite(tickernelVoxel.types[i], sizeof(TickernelVoxelProperty), tickernelVoxel.vertexCount, file);
    }
    fclose(file);
}

void DeserializeTickernelVoxel(const char *filename, TickernelVoxel *pTickernelVoxel, TickernelVoxelMalloc tickernelVoxelMalloc)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Failed to open file for reading");
        return;
    }
    fread(&pTickernelVoxel->propertyCount, sizeof(uint32_t), 1, file);
    pTickernelVoxel->names = (char **)tickernelVoxelMalloc(pTickernelVoxel->propertyCount * sizeof(char *));
    for (size_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        size_t length;
        fread(&length, sizeof(size_t), 1, file);
        pTickernelVoxel->names[i] = (char *)tickernelVoxelMalloc(length * sizeof(char));
        fread(pTickernelVoxel->names[i], sizeof(char), length, file);
    }
    pTickernelVoxel->types = (TickernelVoxelPropertyType **)tickernelVoxelMalloc(pTickernelVoxel->propertyCount * sizeof(TickernelVoxelPropertyType *));
    for (size_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        fread(pTickernelVoxel->types[i], sizeof(int32_t), 1, file);
    }
    fread(&pTickernelVoxel->vertexCount, sizeof(uint32_t), 1, file);
    pTickernelVoxel->indexToProperties = (TickernelVoxelProperty **)tickernelVoxelMalloc(pTickernelVoxel->propertyCount * sizeof(TickernelVoxelProperty *));
    for (size_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        pTickernelVoxel->indexToProperties[i] = (TickernelVoxelProperty *)tickernelVoxelMalloc(pTickernelVoxel->vertexCount * sizeof(TickernelVoxelProperty));
        for (size_t j = 0; j < pTickernelVoxel->vertexCount; j++)
        {
            fread(&pTickernelVoxel->indexToProperties[i][j], sizeof(TickernelVoxelProperty), 1, file);
        }
    }
    fclose(file);
}

void ReleaseTickernelVoxel(TickernelVoxel *pTickernelVoxel)
{
    for (size_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        TickernelVoxelFree(pTickernelVoxel->indexToProperties[i]);
    }
    TickernelVoxelFree(pTickernelVoxel->indexToProperties);
    TickernelVoxelFree(pTickernelVoxel->types);
    for (size_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        TickernelVoxelFree(pTickernelVoxel->names[i]);
    }
    TickernelVoxelFree(pTickernelVoxel->names);
}