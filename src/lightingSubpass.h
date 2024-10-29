#pragma once
#include <graphicEngineCore.h>

void CreateLightingSubpass(Subpass *pLightingSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t lightingSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);
void DestroyLightingSubpass(Subpass *pLightingSubpass, VkDevice vkDevice);
void RecreateLightingSubpassModel(Subpass *pLightingSubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);