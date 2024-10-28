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

void CreateGeometrySubpass(RenderPass *pDeferredRenderPass, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void DestroyGeometrySubpass(RenderPass *pDeferredRenderPass, VkDevice vkDevice);

void AddModelToGeometrySubpass(RenderPass *pDeferredRenderPass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, GeometrySubpassVertex *geometrySubpassVertices, uint32_t *pIndex);
void RemoveModelFromGeometrySubpass(RenderPass *pDeferredRenderPass, VkDevice vkDevice, uint32_t index);
void UpdateModelUniformToGeometrySubpass(RenderPass *pDeferredRenderPass, uint32_t index, GeometrySubpassModelUniformBuffer geometrySubpassModelUniformBuffer);