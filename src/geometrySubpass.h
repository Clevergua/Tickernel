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

void CreateGeometrySubpass(Subpass *pGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t geometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void DestroyGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice);
void AddModelToGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, GeometrySubpassVertex *geometrySubpassVertices, uint32_t *pIndex);
void RemoveModelFromGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice, uint32_t index);
void UpdateModelUniformToGeometrySubpass(Subpass *pGeometrySubpass, uint32_t index, GeometrySubpassModelUniformBuffer geometrySubpassModelUniformBuffer);