#pragma once
#include "graphicCore.h"

typedef struct
{
    vec3 position;
    vec4 color;
    vec3 normal;
} WaterGeometrySubpassVertex;

typedef struct
{
    mat4 model;
} WaterGeometrySubpassInstance;

typedef struct
{
    uint32_t count;
    WaterGeometrySubpassVertex *waterGeometrySubpassVertices;

} WaterGeometrySubpassModel;

// For graphic engine
void CreateWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t waterGeometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void DestroyWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice);

void AddModelToWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, WaterGeometrySubpassVertex *waterGeometrySubpassVertices, uint32_t *pIndex);
void RemoveModelFromWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice, uint32_t index);
void UpdateInstancesInWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, uint32_t modelIndex, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, WaterGeometrySubpassInstance *waterGeometrySubpassInstances, uint32_t instanceCount);
