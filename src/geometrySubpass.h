#pragma once
#include <graphicEngineCore.h>

typedef struct GeometrySubpassVertexStruct
{
    vec3 position;
    vec4 color;
    vec3 normal;
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

void CreateGeometrySubpass(RenderPass *pDeferredRenderPass);
void DestroyGeometrySubpass(RenderPass *pDeferredRenderPass);
void AddModelToGeometrySubpass(RenderPass *pDeferredRenderPass, uint32_t vertexCount, GeometrySubpassVertex *geometrySubpassVertices, uint32_t *pIndex);
void RemoveModelFromGeometrySubpass(RenderPass *pDeferredRenderPass, uint32_t index);
void UpdateModelUniformToGeometrySubpass(RenderPass *pDeferredRenderPass, uint32_t index, GeometrySubpassModelUniformBuffer geometrySubpassModelUniformBuffer);