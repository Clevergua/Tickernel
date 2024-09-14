#pragma once
#include <graphicEngineCore.h>
typedef struct GeometrySubpassVertexStruct
{
    vec3 position;
    vec3 color;
} GeometrySubpassVertex;

typedef struct GeometrySubpassModelUniformBufferStruct
{
    mat4 model;
} GeometrySubpassModelUniformBuffer;

void CreateGeometrySubpass(GraphicEngine *pGraphicEngine);
void DestroyGeometrySubpass(GraphicEngine *pGraphicEngine);
void AddModelToGeometrySubpass(GraphicEngine *pGraphicEngine, GeometrySubpassVertex *geometrySubpassVertices, uint32_t *outputIndex);
void RemoveModelFromGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t inputIndex);