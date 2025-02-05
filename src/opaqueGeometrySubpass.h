#pragma once
#include "graphicCore.h"

typedef struct
{
    vec3 position;
    vec4 color;
    vec3 normal;
} OpaqueGeometrySubpassVertex;

typedef struct
{
    mat4 model;
} OpaqueGeometrySubpassInstance;

typedef struct
{
    uint32_t count;
    OpaqueGeometrySubpassVertex *opaqueGeometrySubpassVertices;

} OpaqueGeometrySubpassModel;

// For graphic engine
void createOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t opaqueGeometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void destroyOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, VkDevice vkDevice);

void addModelToOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, OpaqueGeometrySubpassVertex *opaqueGeometrySubpassVertices, uint32_t *pIndex);
void removeModelFromOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, VkDevice vkDevice, uint32_t index);
void updateInstancesInOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, uint32_t modelIndex, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, OpaqueGeometrySubpassInstance *opaqueGeometrySubpassInstances, uint32_t instanceCount);
