#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef union PLYPropertyUnion
{
    char charValue;
    unsigned char ucharValue;
    short shortValue;
    unsigned short ushortValue;
    int intValue;
    unsigned int uintValue;
    float floatValue;
    double doubleValue;
    int8_t int8Value;
    uint8_t uint8Value;
    int16_t int16Value;
    uint16_t uint16Value;
    int32_t int32Value;
    uint32_t uint32Value;
    float float32Value;
    double float64Value;
} PLYProperty;

typedef struct
{
    uint32_t propertyCount;
    char **names;
    char **types;
    uint32_t vertexCount;
    PLYProperty **indexToProperties;
} PLYModel;

typedef void *(*PLYMalloc)(size_t);
typedef void (*PLYFree)(void *);

void DeserializePLYModel(const char *filename, PLYModel *pPLYModel, PLYMalloc plyMalloc)
{
    if (NULL == pPLYModel)
    {
        printf("pPLYModel cant be null!\n");
        abort();
    }
    else
    {
        // continue;
    }

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Unable to open file %s\n", filename);
        return;
    }
    else
    {
        // continue;
    }

    char line[128];
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "element vertex", 14) == 0)
        {
            sscanf(line, "element vertex %d", &pPLYModel->vertexCount);
            break;
        }
        else
        {
            // continue;
        }
    }

    pPLYModel->propertyCount = 0;
    uint32_t maxPropertyCount = 32;
    uint32_t maxTypeName = 64;
    uint32_t maxPropertyName = 64;
    char **types = (char **)plyMalloc(maxPropertyCount * sizeof(char *));
    char **names = (char **)plyMalloc(maxPropertyCount * sizeof(char *));
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "property", 8) == 0)
        {
            char type[maxTypeName], name[maxPropertyName];
            sscanf(line, "property %s %s", type, name);

            types[pPLYModel->propertyCount] = (char *)plyMalloc((strlen(type) + 1) * sizeof(char));
            names[pPLYModel->propertyCount] = (char *)plyMalloc((strlen(name) + 1) * sizeof(char));

            strcpy(types[pPLYModel->propertyCount], type);
            strcpy(names[pPLYModel->propertyCount], name);

            pPLYModel->propertyCount++;
        }
        else if (strncmp(line, "end_header", 10) == 0)
        {
            break;
        }
    }

    // Assign allocated arrays to the PLYModel structure
    pPLYModel->types = types;
    pPLYModel->names = names;

    pPLYModel->indexToProperties = (PLYProperty **)plyMalloc(sizeof(PLYProperty *) * pPLYModel->propertyCount);
    for (uint32_t i = 0; i < pPLYModel->propertyCount; i++)
    {
        pPLYModel->indexToProperties[i] = (PLYProperty *)plyMalloc(sizeof(PLYProperty) * pPLYModel->vertexCount);
    }

    for (uint32_t i = 0; i < pPLYModel->vertexCount; i++)
    {
        for (uint32_t j = 0; j < pPLYModel->propertyCount; j++)
        {
            char *typeName = pPLYModel->types[j];
            if (strcmp(typeName, "int32") == 0)
            {
                fscanf(file, "%d", &pPLYModel->indexToProperties[j][i].int32Value);
            }
            else if (strcmp(typeName, "uchar") == 0)
            {
                fscanf(file, "%u", (unsigned int *)&pPLYModel->indexToProperties[j][i].ucharValue);
            }
            else
            {
                abort();
            }
        }
    }
    fclose(file);
}

void DestroyPLYModel(PLYModel *pPLYModel, PLYFree plyFree)
{
    for (uint32_t i = 0; i < pPLYModel->propertyCount; i++)
    {
        plyFree(pPLYModel->indexToProperties[i]);
        plyFree(pPLYModel->types[i]);
        plyFree(pPLYModel->names[i]);
    }

    plyFree(pPLYModel->indexToProperties);
    plyFree(pPLYModel->types);
    plyFree(pPLYModel->names);
}