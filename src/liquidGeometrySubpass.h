#pragma once
#include <graphicEngineCore.h>

typedef struct LiquidGeometrySubpassVertexStruct
{
    vec3 position;
    vec4 color;
    vec3 normal;
} LiquidGeometrySubpassVertex;

typedef struct LiquidGeometrySubpassModelStruct
{
    uint32_t count;
    LiquidGeometrySubpassVertex *geometrySubpassVertices;

} LiquidGeometrySubpassModel;

typedef struct LiquidGeometrySubpassModelUniformBufferStruct
{
    mat4 model;
} LiquidGeometrySubpassModelUniformBuffer;

void CreateLiquidGeometrySubpass(GraphicEngine *pGraphicEngine);
void DestroyLiquidGeometrySubpass(GraphicEngine *pGraphicEngine);
void AddModelToLiquidGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t vertexCount, LiquidGeometrySubpassVertex *liquidGeometrySubpassVertices, uint32_t *pIndex);
void RemoveModelFromLiquidGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t index);
void UpdateModelUniformToLiquidGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t index, LiquidGeometrySubpassModelUniformBuffer liquidGeometrySubpassModelUniformBuffer);