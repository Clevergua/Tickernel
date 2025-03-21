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

// For graphic engine
void createWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t waterGeometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void destroyWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice);

SubpassModel* addModelToWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, WaterGeometrySubpassVertex *waterGeometrySubpassVertices);
void removeModelFromWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice, SubpassModel *pSubpassModel);
void updateInstancesInWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, SubpassModel* pSubpassModel, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, WaterGeometrySubpassInstance *waterGeometrySubpassInstances, uint32_t instanceCount);
