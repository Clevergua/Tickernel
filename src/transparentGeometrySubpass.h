#pragma once
#include <graphicEngineCore.h>

typedef struct
{
    vec3 position;
    vec4 color;
    vec3 normal;
} TransparentGeometrySubpassVertex;

typedef struct
{
    mat4 model;
} TransparentGeometrySubpassInstance;

typedef struct
{
    uint32_t count;
    TransparentGeometrySubpassVertex *transparentGeometrySubpassVertices;

} TransparentGeometrySubpassModel;

// For graphic engine
void CreateTransparentGeometrySubpass(Subpass *pTransparentGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t transparentGeometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void DestroyTransparentGeometrySubpass(Subpass *pTransparentGeometrySubpass, VkDevice vkDevice);

void AddModelToTransparentGeometrySubpass(Subpass *pTransparentGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, TransparentGeometrySubpassVertex *transparentGeometrySubpassVertices, uint32_t *pIndex);
void RemoveModelFromTransparentGeometrySubpass(Subpass *pTransparentGeometrySubpass, VkDevice vkDevice, uint32_t index);
void UpdateInstancesToTransparentGeometrySubpass(Subpass *pTransparentGeometrySubpass, uint32_t modelIndex, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, TransparentGeometrySubpassInstance *transparentGeometrySubpassInstances, uint32_t instanceCount);