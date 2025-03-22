#pragma once
#include "graphicCore.h"

#define UI_SHADER_PATH_LENGTH 256

typedef struct
{
    vec2 position;
    vec2 texCoord;
    vec4 color;
} UISubpassVertex;

void createUISubpass(Subpass *pUISubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t subpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer);
void destroyUISubpass(Subpass *pUISubpass, VkDevice vkDevice);
void updateUISubpass(Subpass *pUISubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer);
void recordUISubpass(Subpass *pUISubpass, VkCommandBuffer vkCommandBuffer, VkDevice vkDevice);

SubpassModel *addModelToUISubpass(Subpass *pUISubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, UISubpassVertex *uiSubpassVertices);
void removeModelFromUISubpass(Subpass *pUISubpass, VkDevice vkDevice, SubpassModel *pSubpassModel);