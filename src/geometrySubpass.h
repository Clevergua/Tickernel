#pragma once
#include <graphicEngineCore.h>

typedef struct GeometrySubpassVertexStruct
{
    vec3 position;
    vec3 color;
} GeometrySubpassVertex;

typedef struct GeometrySubpassModelStruct
{
    uint32_t count;
    GeometrySubpassVertex *geometrySubpassVertices;

} GeometrySubpassModel;

typedef struct GeometrySubpassModelUniformBufferStruct
{
    mat4 model;
} GeometrySubpassModelUniformBuffer;

void CreateGeometrySubpass(GraphicEngine *pGraphicEngine);
void DestroyGeometrySubpass(GraphicEngine *pGraphicEngine);
void AddModelToGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t vertexCount, GeometrySubpassVertex *geometrySubpassVertices, uint32_t *pGroupIndex, uint32_t *pModelIndex);
void RemoveModelFromLightingSubpass(GraphicEngine *pGraphicEngine, uint32_t groupIndex, uint32_t modelIndex);