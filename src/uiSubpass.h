#pragma once

#include "graphicCore.h"

typedef struct {
    vec2 position;
    vec2 texCoord;
    vec4 color;
} UISubpassVertex;

// For graphic engine
void createUISubpass(Subpass *pUISubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t uiSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void destroyUISubpass(Subpass *pUISubpass, VkDevice vkDevice);

void addModelToUISubpass(Subpass *pUISubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, UISubpassVertex *uiSubpassVertices, uint32_t *pIndex);
void removeModelFromUISubpass(Subpass *pUISubpass, VkDevice vkDevice, uint32_t index);
void updateModelInUISubpass(Subpass *pUISubpass, uint32_t modelIndex, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, UISubpassVertex *uiSubpassVertices, uint32_t vertexCount); 