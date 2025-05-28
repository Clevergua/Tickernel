#pragma once
#include "graphicCore.h"

typedef struct
{
    vec3 position;
    vec4 color;
    vec3 normal;
} GeometrySubpassVertex;

typedef struct
{
    mat4 model;
} GeometrySubpassInstance;

// For graphic engine
void createGeometrySubpass(Subpass *pGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t geometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void destroyGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice);

SubpassModel *addModelToGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, GeometrySubpassVertex *geometrySubpassVertices);
void removeModelFromGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice, SubpassModel *pSubpassModel);
void updateInstancesInGeometrySubpass(Subpass *pGeometrySubpass, SubpassModel *pSubpassModel, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, GeometrySubpassInstance *geometrySubpassInstances, uint32_t instanceCount);
