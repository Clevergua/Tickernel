#pragma once
#include "graphicCore.h"

void createLightingSubpass(Subpass *pLightingSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t lightingSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);
void destroyLightingSubpass(Subpass *pLightingSubpass, VkDevice vkDevice);
void recreateLightingSubpassModel(Subpass *pLightingSubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);
